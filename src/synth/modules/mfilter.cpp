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

#include "mfilter.h"

MFilter::MFilter(Synth *_synth){
	synth=_synth;
	
	panic();
}

void MFilter::panic(){
	y[0]=y[1]=y[2]=y[3]=0.0;
	old[0]=old[1]=old[2]=old[3]=0.0;
	out=0.0;
	freq=800.0;
	res=0.1;
}

void MFilter::process(float *inout, unsigned int nsamples){
	unsigned int i;
	float f,p,k,scale,r,x;

	f=2.0*(freq)/synth->samplerate();
	k=3.6f*f - 1.6f*f*f - 1.0f; /* empirical tunning */
	p=(k+1)*0.5f;
	scale=exp((1.0f-p)*1.386249f);
	r=(res)*scale;

	for(i=0;i<nsamples;i++){


		/* real loop */
		/* Inverted feedback for corner peaking */
		x=(inout[i])-r*y[3];
		/* four cascaded onepole filters (bilinear transform) */
		y[0]=(x*p) + (old[3]*p) - (k*y[0]);
		y[1]=(y[0]*p) + (old[0]*p) - (k*y[1]);
		y[2]=(y[1]*p) + (old[1]*p) - (k*y[2]);
		y[3]=(y[2]*p) + (old[2]*p) - (k*y[3]);

		y[3]=y[3] - (pow(y[3],3.0f)/6.0f);
		old[3]=x;
		old[0]=y[0];
		old[1]=y[1];
		old[2]=y[2];
		/* output */
		inout[i]=y[3];
	}
}
