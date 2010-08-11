/*
    MSynth - Monkey Synth - Modular synthetiser with nice UI
    Copyright (C) 2009-2010 David Moreno Montero

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

#include <sndfile.h>
#include <math.h>

#include "sampler.h"
#include "../../common/mdebug.h"

Synth *create_synth_sampler(QObject *parent){
	return new Sampler(parent);
}

#define DEFAULT_SAMPLE "/tmp/test.wav"
#define DEFAULT_TONE   60

Sampler::Sampler(QObject *parent): Synth(parent){
	DEBUG("Creating the sampler. Loading %s",DEFAULT_SAMPLE);
	temp=NULL;
	memset(pnote,0,SAMPLER_POLYPHONY);
	
	loadSample(DEFAULT_SAMPLE);
}

/**
 * @short Loads a sample
 */
void Sampler::loadSample(const QString &name){
	SF_INFO fileInfo;
	SNDFILE *file;

	fileInfo.format=0;
 	file=sf_open(name.toAscii().constData(), SFM_READ, &fileInfo);
	
	frames=fileInfo.frames;
	
	sample=new float[frames*fileInfo.channels];
	sf_read_float(file, sample, frames*fileInfo.channels);

	/// De-interlace to the first part of the sample
	unsigned long f;
	int i;
	for (f=0;f<frames;f++){
		double s=0;
		for (i=0;i<fileInfo.channels;i++){
			s+=sample[f*fileInfo.channels + i];
		}
		s/=fileInfo.channels;
		sample[f]=s;
	}

	sampleFreq=freqOfNote(DEFAULT_TONE)*samplerate()/fileInfo.samplerate;

	DEBUG("Sample is %ld frames long, %d channels, samplerate %d", fileInfo.frames, fileInfo.channels, fileInfo.samplerate);

	sf_close(file);
}

/**
 * @short Performs a data chunk. 
 */
void Sampler::getAudio(float *audio[2], unsigned int nsamples, const MidiEventList& events){
	if (temp==NULL){
		temp=new float[nsamples];
	}
	
	unsigned int csample=0;
	foreach(MidiEvent ev, events){
		if (ev.sampleOffset>csample){ // calculate a chunk
			process(&audio[0][csample], ev.sampleOffset-csample);
			csample=ev.sampleOffset;
		}
		/// process the MIDI events.
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
	process(&audio[0][csample], nsamples-csample);
	memcpy(audio[1],audio[0],sizeof(float)*nsamples);
}

void Sampler::process(float *data, unsigned int nsamples){
	memset(data,0,sizeof(data[0])*nsamples);
	//DEBUG("get %d samples",nsamples);
	for (int p=0;p<SAMPLER_POLYPHONY;p++){
		if (pnote[p]==0)
			continue;
		for (unsigned int i=0;i<nsamples;i++){
			data[i]+=sample[int(samplePosition[p])];
			//DEBUG("%.2f %f",samplePosition, data[i]);
			samplePosition[p]+=delta[p];
			if (samplePosition[p]>frames){
				samplePosition[p]=frames;
				delta[p]=0;
			}
		}
	}
	for (unsigned int i=0;i<nsamples;i++){
		data[i]*=0.30;
	}
}


void Sampler::noteOn(unsigned char note, unsigned char velocity){
	//DEBUG("Note on %d %d %f %f",note,velocity, sampleFreq, delta);
	for (int p=0;p<SAMPLER_POLYPHONY;p++){
		if (pnote[p]==0){
			DEBUG("Give note on %d slot (note %d)",p,note);
			samplePosition[p]=0;
			delta[p]=freqOfNote(note)/sampleFreq;
			pnote[p]=note;
			break;
		}
	}
	vel=velocity;
}

void Sampler::noteOff(unsigned char note, unsigned char velocity){
	for (int p=0;p<SAMPLER_POLYPHONY;p++){
		if (pnote[p]==note){
			DEBUG("Take note off %d slot (note %d)",p,note);
			samplePosition[p]=0;
			delta[p]=0.0;
			pnote[p]=0;
			break;
		}
	}
	//DEBUG("Note off %d %d",note,velocity);
}



void Sampler::sendStatus(){
	/*
	sendController(15,osc[0]->type()*127/5);
	sendController(16,osc[1]->type()*127/5);
	sendController(17,osc[2]->type()*127/5);
	sendController(18,osc[3]->type()*127/5);
	sendController(2,adsr.attack()*64.0);
	sendController(3,adsr.decay()*64.0);
	sendController(4,adsr.sustain()*12.7);
	sendController(5,adsr.release()*64.0);
	sendController(6,adsr.sustainLevel()*12.7);
	
	sendController(7,lfoFreq.frequency()*127.0/10.0);
	sendController(8,lfoQ.frequency()*127.0/10.0);

	sendController(11,lfoFreq.level()*127.0/15000.0);
	sendController(12,lfoFreq.delta()*127.0/15000.0);
	sendController(13,lfoQ.level()*1.5/127.0);
	sendController(14,lfoQ.delta()*1.5/127.0);
	
	sendController(CC_GAIN,_gain*GAIN_MULT);
	*/
}


void Sampler::controller(unsigned char controllerId, unsigned char value){
	DEBUG("Controller %d %d",controllerId, value);
		/*
	switch(controllerId){
		case CC_GAIN:
			_gain=value/GAIN_MULT;
			break;
		case 15:
		case 16:
		case 17:
		case 18:
			{
			int n=controllerId-15;
			switch(value*5/127){
				case 0:
					osc[n]->setType(Oscillator::saw);
					DEBUG("Osc %d to SAW",n);
				break;
				case 1:
					osc[n]->setType(Oscillator::triangle);
					DEBUG("Osc %d to TRI",n);
				break;
				case 2:
					osc[n]->setType(Oscillator::square);
					DEBUG("Osc %d to SQU",n);
				break;
				case 3:
					osc[n]->setType(Oscillator::sine);
					DEBUG("Osc %d to SIN",n);
				break;
				case 4:
					osc[n]->setType(Oscillator::isaw);
					DEBUG("Osc %d to ISAW",n);
				break;
				default:
					WARNING("Out of limits oscillator type");
				break;
			}
			}
			break;
		case 2:
			adsr.setAttack(value/64.0);
			break;
		case 3:
			adsr.setDecay(value/64.0);
			break;
		case 4:
			if (value>=127){
				DEBUG("Set long sustain, so its there until released");
				adsr.setSustain(1000.0); // 16 minutes
			}
			else
				adsr.setSustain(value/12.7);
			break;
		case 5:
			adsr.setRelease(value/64.0);
			break;
		case 6:
			adsr.setSustainLevel(value/127.0);
			break;
		case 7:
			lfoFreq.setFrequency(value*10.0/127.0);
			break;
		case 8:
			lfoQ.setFrequency(value*10.0/127.0);
			break;

		case 11:
			lfoFreq.setLevel(value*15000.0/127.0);
			break;
		case 12:
			lfoFreq.setDelta(value*15000.0/127.0);
			break;
		case 13:
			lfoQ.setLevel(value*1.5/127.0);
			break;
		case 14:
			lfoQ.setDelta(value*1.5/127.0);
			break;
		
		case 21:
			fullLevel=(value>64);
			DEBUG("Set full level %s",fullLevel ? "true" : "false");
			break;
			
		case 40: // panic!
		{
			unsigned int i;
			for(i=0;i<NVOICES;i++){
				DEBUG("Voice %d. Note %d, ADSR phase %d, ADSR level %f",i,osc[0]->at(i)->note(), adsr[i]->phase(), adsr[i]->level());
				adsr[i]->panic();
				for (int no=0;no<MONO1_NOSC;no++)
					osc[no]->at(i)->playNote(0);
			}
			filter.panic();
		}
		break;
		default:
			WARNING("Unknown controller %d",controllerId);
	}
	*/
}
