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

#include <stdlib.h>
#include <string.h>

#include "../synth.h"
#include "../../common/mdebug.h"

#include "adsr.h"

ADSR::ADSR(Synth *_synth, ADSRParams *_params){
	synth=_synth;
	if (NULL==_params){
		myparams=true;
		_params=new ADSRParams;
	}
	else
		myparams=false;
	params=_params;
	_phase=0;
	time=0.0;
	currentLevel=0.0;
}

ADSR::~ADSR(){
	if (myparams)
		delete params;
}

/**
 * @short Process a chunk of data 
 */
void ADSR::process(float *inout, unsigned int nsamples){
	float isr=1.0/synth->samplerate();
	float delta;
	unsigned int i;
	switch(_phase){
		case 0:
			memset(inout,0,sizeof(float)*nsamples);
			currentLevel=0.0;
			//DEBUG("adsr %f",currentLevel);
		break;
		case 1:
			if (params->a==0.0){
				_phase++;
				process(inout, nsamples);
				return;
			}
			delta=isr/params->a;
			for (i=0;i<nsamples;i++){
				currentLevel+=delta;
				if (currentLevel>1.0){ // should use antialiasing here.. it does not...
					currentLevel=1.0;
					_phase++;
					//DEBUG("phase %d",phase);
					process(&inout[i], nsamples-i);
					return;
				}
				inout[i]*=currentLevel;
				//DEBUG("adsr %f",currentLevel);
			}
		break;
		case 2:
			if (params->d==0.0){
				_phase++;
				process(inout, nsamples);
				return;
			}
			delta=-(isr/params->d)*(1.0-params->sl);
			for (i=0;i<nsamples;i++){
				currentLevel+=delta;
				if (currentLevel<params->sl){
					currentLevel=params->sl;
					time=0.0f; // time is only used at sustain
					_phase++;
					//DEBUG("phase %d",phase);
					process(&inout[i], nsamples-i);
					return;
				}
				inout[i]*=currentLevel;
				//DEBUG("adsr %f",currentLevel);
			}
		break;
		case 3:
			currentLevel=params->sl;
			for (i=0;i<nsamples;i++){
				time+=isr;
				if (time>params->s){
					_phase++;
					//DEBUG("phase %d",phase);
					process(&inout[i], nsamples-i);
					return;
				}
				inout[i]*=currentLevel;
				//DEBUG("adsr %f",currentLevel);
			}
		break;
		case 4:
			if (params->r==0.0){
				_phase=0;
				process(inout, nsamples);
				return;
			}
			delta=-(isr/params->r)*params->sl;
			for (i=0;i<nsamples;i++){
				currentLevel+=delta;
				if (currentLevel<0.0){
					currentLevel=0.0;
					_phase=0;
					//DEBUG("phase %d",phase);
					process(&inout[i],nsamples-i);
					return;
				}
				inout[i]*=currentLevel;
				//DEBUG("adsr %f",currentLevel);
			}
		break;
	}
}

/**
 * @short Go to attack phase.
 */
void ADSR::noteOn(){
	_phase=1;
	//DEBUG("phase %d",phase);
}

/**
 * @short If running, go to phase 4, decay
 */
void ADSR::noteOff(){
	if (_phase)
		_phase=4;
	//DEBUG("phase %d",phase);
}

ADSRGroup::ADSRGroup(unsigned int nvoices, Synth *_synth){
	unsigned int i;
	for (i=0;i<nvoices;i++){
		list.append(new ADSR(_synth, &params));
	}
}

ADSRGroup::~ADSRGroup(){
	foreach(ADSR *adsr, list){
		delete adsr;
	}
}

/**
 * @short Checks all voices and returns one that is free (phase=0), or -1
 */
int ADSRGroup::getFreeVoice(){
	int i=0;
	foreach(ADSR *adsr, list){
		if (adsr->phase()==0)
			return i;
		i++;
	}
	return -1;
}
