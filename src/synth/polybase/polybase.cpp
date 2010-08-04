/*
    MSynth - Monkey Synth - Modular synthetiser with nice UI
    Copyright (C) 2009 David Moreno Montero

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>

#include <mdebug.h>

#include "polybase.h"

#define NVOICES 10

Synth *create_synth_polybase(QObject *parent){
	return new Polybase(parent);
}

Polybase::Polybase(QObject *parent) : Synth(parent), filter(this){
	nvoices=10;

	oscillators=new Oscillator[nvoices];
	chunk=NULL;
	chunksize=0;
	
	unsigned int i;
	for (i=0;i<nvoices;i++){
		oscillators[i].poly=this;
	}
	volume=0.4;

	oscillatorForm[0]=0;
	oscillatorForm[1]=32;
	oscillatorForm[2]=64;
	oscillatorForm[3]=92;

	oscillatorLevel[0]=32;
	oscillatorLevel[1]=32;
	oscillatorLevel[2]=32;
	oscillatorLevel[3]=32;
	
	filter.freq=0.0;
	filter.tofreq=10000.0;
	filter.q=0.0;
	filter.dist=0.0;
}

Polybase::~Polybase(){
	delete[] oscillators;
	if (chunk)
		delete[] chunk;
}

/**
 * @short Gets the audio signals for both channels (actually a copy)
 */
void Polybase::getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &list){
	if (nsamples>chunksize){
		if (chunk){
			delete chunk;
		}
		chunk=new float[nsamples];
	}

	unsigned int uptoSample=0;
	unsigned int i;

	foreach(MidiEvent ev, list){
		if (ev.sampleOffset>uptoSample){
			calculateChunk(&audio[0][uptoSample], ev.sampleOffset-uptoSample);
			uptoSample=ev.sampleOffset;
		}
		switch (ev.type){
			case MidiEvent::noteOn:
				noteOn(ev.data[0], ev.data[1]);
				break;
			case MidiEvent::noteOff:
				noteOff(ev.data[0], ev.data[1]);
				break;
			case MidiEvent::controller:
				controller(ev.data[0], ev.data[1]);
				break;
			default:
				break;
		}
	}
	calculateChunk(&audio[0][uptoSample], nsamples-uptoSample);
	
	for (i=0;i<nsamples;i++){
		float s=audio[0][i]*volume;
		if (s>1.0 || s<-1.0){
			volume*=0.9;
			if (s<0.0)
				s=-1.0;
			else
				s=1.0;

			WARNING("Clip %f! lowering volume to %f", s, volume);
			emit sendController(0, volume*127);
		}
		audio[0][i]=s;
	}
	
	
	// no stereo by the moment
	memcpy(audio[1],audio[0],sizeof(float)*nsamples);
}

/**
 * @short Calculates a small chunk (may be all) of data.
 */
void Polybase::calculateChunk(float *data, unsigned int nsamples){
	// gain + volume check
	unsigned int i,v;
	oscillators[0].oscillatorChunk(data, nsamples);
	for (v=1;v<nvoices;v++){
		oscillators[v].oscillatorChunk(chunk, nsamples);
		for(i=0;i<nsamples;i++){
			data[i]+=chunk[i];
		}
	}
	filter.filterChunk(data, nsamples);
}


void Polybase::sendStatus(){
	DEBUG("Sending initial status");
	emit sendController(0,volume*127);
	emit sendController(1,oscillatorForm[0]);
	emit sendController(2,oscillatorLevel[0]);
	emit sendController(3,oscillatorForm[1]);
	emit sendController(4,oscillatorLevel[1]);
	emit sendController(5,oscillatorForm[2]);
	emit sendController(6,oscillatorLevel[2]);
	emit sendController(7,oscillatorForm[3]);
	emit sendController(8,oscillatorLevel[3]);

	emit sendController(9,filter.tofreq/10.0);
	emit sendController(10,filter.q*64.0);
	emit sendController(11,filter.dist*64.0);

}

void Polybase::noteOn(unsigned char note, unsigned char ){
	for (unsigned int v=0;v<nvoices;v++){
		if (oscillators[v].note==0){
			//DEBUG("Note on %d, allocated on %d",note,v);
			oscillators[v].note=note;
			oscillators[v].freq[0]=freqOfNote(note-12);
			oscillators[v].freq[1]=freqOfNote(note);
			oscillators[v].freq[2]=freqOfNote(note+7);
			oscillators[v].freq[3]=freqOfNote(note+12);
			return;
		}
	}
	DEBUG("Failed allocate note %d",note);
}

void Polybase::noteOff(unsigned char note, unsigned char ){
	bool ok=false;
	for (unsigned int v=0;v<nvoices;v++){
		if (oscillators[v].note==note){
			//DEBUG("Note off %d, deallocated from %d",note,v);
			oscillators[v].note=0;
			ok=true;
		}
	}
	if (!ok)
		DEBUG("Failed deallocate note %d",note);
}

void Polybase::controller(unsigned char id, unsigned char value){
	switch(id){
		case 0:
			volume=value/127.0;
			break;
		case 9:
			filter.tofreq=value*10.0;
			DEBUG("Frecuency set at %f",filter.freq);
			break;
		case 10:
			filter.q=value/64.0;
			DEBUG("Q set at %f",filter.q);
			break;
		case 11:
			filter.dist=value/64.0;
			DEBUG("Dist set at %f",filter.dist);
			break;
	}
	if (id>=1 && id<=8){
		if (0==(id%2)){
			oscillatorLevel[id/2 - 1]=value;
		}
		else{
			oscillatorForm[id/2]=value;
		}
	}
	//DEBUG("Set controller %d %d",id/2 -1, id/2);
}

/**
 * @short Calculates the chunk for just an oscillator (actually 4), its is more like a voice.
 */
void Polybase::Oscillator::oscillatorChunk(float *data, unsigned int nsamples){
	if (note==0){ // freq 0 == stopped
		memset(data,0,sizeof(float)*nsamples);
		return;
	}

#define NWAVES 5
#define NWAVESBLOCK (128/NWAVES)

#define SIN(x) (sin(x*2.0*M_PI))
#define SAW(x) (fmod(1.0f+(x*4.0f),2.0)-1.0)
#define TRIANGLE(x) ((x>0.25 && x<0.75) ? - SAW(x) : SAW(x))
#define SQUARE(x) (x < 0.5 ? 1.0 : -1.0);
#define NOISE(x) (((float(rand())/RAND_MAX)*2.0) - 1.0)

	unsigned int i;
	float *temp=data;
	float os,ds,v;
	float sr=poly->samplerate();
	float pstep[4]={freq[0]/sr, freq[1]/sr, freq[2]/sr, freq[3]/sr};
	
	for (i=0;i<nsamples;i++){
		float r=0;
		for (int o=0;o<4;o++){
			phase[o]+=pstep[o];
			
			float form=(poly->oscillatorForm[o]%NWAVESBLOCK)/float(NWAVESBLOCK);
			int nform=poly->oscillatorForm[o]/NWAVESBLOCK;
			float mphase=fmod(phase[o],1.0);
			
			//DEBUG("nform %d",nform);
			
			switch(nform){
				case 0:
					os=SIN(mphase);
					ds=SAW(mphase);
					break;
				case 1:
					os=SAW(mphase);
					ds=TRIANGLE(mphase);
					break;
				case 2:
					os=TRIANGLE(mphase);
					ds=SQUARE(mphase);
					break;
				case 3:
					os=SQUARE(mphase);
					ds=SIN(mphase);
					break;
				default:
					//DEBUG("NOISE");
					os=NOISE(mphase);
					ds=NOISE(mphase);
			}

			v=(os*(1.0-form)) + (ds*form);
			
			r+=v*poly->oscillatorLevel[o]/64.0;
		}
		*temp=r;
		temp++;
	}
	phase[0]=fmod(phase[0],1.0);
	phase[1]=fmod(phase[1],1.0);
	phase[2]=fmod(phase[2],1.0);
	phase[3]=fmod(phase[3],1.0);
}

float mmin(float x, float y){
	return (x<y) ? x : y;
}

void Polybase::Filter::filterChunk(float *data, unsigned int nsamples){
	float ret, notch, high, mfreq, damp;
	unsigned int n;
	register int i;
	float in;

	mfreq=mmin(0.25f, 2.0f*sin(M_PI*(freq)/(poly->samplerate()*2.0f)));
	damp=mmin(2.0f*(1.0f - pow(q, 0.25f)), mmin(2.0f, 2.0f/mfreq - mfreq*0.5f));


	for (n=0;n<nsamples;n++){
		in=data[n];
		
		freq=freq*0.9999+tofreq*0.0001;
		
		for (i=0;i<3;i++){
			notch=(in)-(damp*band);
			low=low + (mfreq*band);
			high=notch-low;
			band  = mfreq*high + band - (dist)*band*band*band;
			ret=0.5f*(low);
		}

		if (fabs(band)>1000.0)
			band=0.0f;
		if (fabs(low)>1000.0)
			low=0.0f;
		
		data[n]=ret;
	}
	//DEBUG("%f %f %f %f %f",low,band,freq,q,dist);
}