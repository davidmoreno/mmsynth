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

#include "../synth.h"
#include "../../common/mdebug.h"
#include "oscillator.h"

#include <math.h>

Oscillator::Oscillator(Synth *parent){
	synth=parent;
	phase=0;
	_type=saw;
	_note=0;
	freq=0.0;
}

#define SIN(x) (sin(x*2.0*M_PI))
#define SAW(x) (fmod(1.0f+(x*4.0f),2.0)-1.0)
#define TRIANGLE(x) ((x>0.25 && x<0.75) ? - SAW(x) : SAW(x))
#define SQUARE(x) (x < 0.5 ? 1.0 : -1.0);

void Oscillator::process(float *data, unsigned int nsamples){
	float step=freq/synth->samplerate();
	
	/*
	if (_note==0){
		memset(data,0,sizeof(float)*nsamples);
		return;
	}
	*/
	
	unsigned int i;
	// switch outside as this way the inner loop has fewer branches
	switch(_type){
		case saw:
			for (i=0;i<nsamples;i++){
				*data++=SAW(phase);
				phase+=step;
			}
			break;
		case triangle:
			for (i=0;i<nsamples;i++){
				*data++=TRIANGLE(phase);
				phase=fmod(phase+step,1.0f);
			}
			break;
		case square:
			for (i=0;i<nsamples;i++){
				*data++=SQUARE(phase);
				phase=fmod(phase+step,1.0f);
			}
			break;
		case sine:
			for (i=0;i<nsamples;i++){
				*data++=SIN(phase);
				phase+=step;
			}
			break;
		case isaw:
			for (i=0;i<nsamples;i++){
				*data++=-SAW(phase);
				phase+=step;
			}
			break;
	}
	
	// "phase ⊂ ℝ" , but usually on low numbers. At the end/begining of the cycle always "phase ⊂ (0,1]".
	phase=fmod(phase,1.0);
}

/**
 * @short Sets the oscillator to play at this tune
 */
void Oscillator::playNote(char note){
	_note=note;
	if (note)
		freq=Synth::freqOfNote(note);
}



/**
 * @short Creates an oscillator group, of count members and with parent as Synth parent
 */
OscillatorGroup::OscillatorGroup(unsigned int count, Synth *parent){
	DEBUG("Creating oscillator group %p",this);
	for (unsigned int i=0;i<count;i++){
		list.append(new Oscillator(parent));
	}
}

/// Deletes the group, and all its oscillators.
OscillatorGroup::~OscillatorGroup(){
	DEBUG("Removing oscillator group %p",this);
	foreach(Oscillator *o, list){
		delete o;
	}
}

/// Sets the type to all oscillators.
void OscillatorGroup::setType(Oscillator::Type t){
	foreach(Oscillator *o, list){
		o->setType(t);
	}
}

/**
 * @short Returns the type of the group.
 *
 * Actually returns the oscillator of the first member, but should be equivalent.
 */
Oscillator::Type OscillatorGroup::type(){
	return list.at(0)->type();
}

/**
 * @short Looks for an oscillator with the given note. 
 *
 * Usually note 0 means free, and all oscillators are set to 0 at start.
 *
 * @returns the oscillator number (0,count()) or -1 if none
 */
int OscillatorGroup::findOscillatorByNote(char note){
	unsigned int i=0;
	foreach(Oscillator *o, list){
		if (o->note()==note){
			return i;
		}
		i++;
	}
	return -1;
}
