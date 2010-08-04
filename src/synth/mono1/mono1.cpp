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

#include "mono1.h"
#include "../../common/mdebug.h"

#define NVOICES 16

#define CC_GAIN 9
#define GAIN_MULT 16.0

#define MONO1_NOSC 4

Synth *create_synth_mono1(QObject *parent){
	return new Mono1(parent);
}

static char oscDelta[MONO1_NOSC]={ 0, -12, 7, 12, };


Mono1::Mono1(QObject *parent): Synth(parent), 
								adsr(NVOICES, this),
								filter(this),
								lfoFreq(this),
								lfoQ(this){
	int i;
	osc=new OscillatorGroup*[MONO1_NOSC];
	for (i=0;i<MONO1_NOSC;i++)
		osc[i]=new OscillatorGroup(NVOICES, this);
	
	adsr.setSustain(10000.0);
	adsr.setSustainLevel(0.5);
	
	temp=NULL;
	_gain=3.0;
	velocity=new float[NVOICES];
	lfoQ.setFrequency(0.5);
	lfoQ.setLevel(0.2);
	lfoQ.setDelta(0.2);
	lfoFreq.setFrequency(0.25);
	lfoFreq.setLevel(1000.0);
	lfoFreq.setDelta(500.0);
	
	fullLevel=false;
}


void Mono1::getAudio(float *audio[2], unsigned int nsamples, const MidiEventList& events){
	if (temp==NULL){
		temp=new float[nsamples];
		dataLFOFreq=new float[nsamples];
		// for Q we reuse temp
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

/**
 * @short Processes a data chunk.
 */
void Mono1::process(float *data, unsigned int nsamples){
	memset(data,0,sizeof(float)*nsamples);
	for (int no=0;no<MONO1_NOSC;no++){
		for (int i=1;i<NVOICES;i++){
			if (adsr[i]->hasSound()){
				osc[no]->at(i)->process(temp, nsamples);
				adsr[i]->process(temp,nsamples);
			}
			else
				memset(temp,0,sizeof(float)*nsamples);

			if (fullLevel){
				for (unsigned int j=0;j<nsamples;j++){
					data[j]+=temp[j];
				}
			}
			else{
				for (unsigned int j=0;j<nsamples;j++){
					data[j]+=temp[j]*velocity[i];
				}
			}
		}
	}
	float gain=_gain/NVOICES;
	float s;
	for (unsigned int j=0;j<nsamples;j++){
		data[j]*=gain;
		s=data[j];
		if (s>1.0 || s<-1.0){
			WARNING("Lowering volume %f to %f, clipped at %f",_gain, _gain*0.95,s);
			if (s<0.0)
				data[j]=-1.0;
			else
				data[j]=1.0;
			_gain*=0.95;
			emit sendController(CC_GAIN, int(_gain*GAIN_MULT) ); // it is enqueued for another thread
			gain=_gain/NVOICES;
		}
	}

	lfoFreq.process(dataLFOFreq,nsamples);
	lfoQ.process(temp,nsamples);

	filter.process(data,nsamples,dataLFOFreq,temp);
}

void Mono1::noteOn(unsigned char note, unsigned char _velocity){
	int v=adsr.getFreeVoice();
	if (v==-1){
		DEBUG("Failed allocate note %d",note);
		return;
	}
	for (int no=0;no<MONO1_NOSC;no++)
		osc[no]->at(v)->playNote(note+oscDelta[no]);
	adsr[v]->noteOn();
	velocity[v]=_velocity/127.0;
}

void Mono1::noteOff(unsigned char note, unsigned char velocity){
	int v=osc[0]->findOscillatorByNote(note);
	if (v==-1){
		DEBUG("Failed deallocate note %d",note);
		return;
	}
	for (int no=0;no<MONO1_NOSC;no++)
		osc[no]->at(v)->playNote(0);
	adsr[v]->noteOff();
}



void Mono1::sendStatus(){
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
}


void Mono1::controller(unsigned char controllerId, unsigned char value){
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
}
