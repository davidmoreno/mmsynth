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

#include "../synth.h"

#include "lfo.h"

LFO::LFO(Synth *parent) : synth(parent){
	panic();
}

void LFO::panic(){
	_type=LFO::sine;
	phase=0;
	freq=0.5;
}

#define SIN(x) (sin(x*2.0*M_PI))
#define SAW(x) (fmod(1.0f+(x*4.0f),2.0)-1.0)

void LFO::process(float *data, unsigned int nsamples){
	unsigned int i;

	float d=freq/synth->samplerate();
	switch(_type){
	case saw:
		for (i=0;i<nsamples;i++){
			data[i]=SAW(phase)*pivotDelta + pivotLevel;
			phase+=d;
		}
	break;
	case sine:
		for (i=0;i<nsamples;i++){
			data[i]=SIN(phase)*pivotDelta + pivotLevel;
			phase+=d;
		}
	break;
	}
	phase=fmod(phase,1.0);
}
