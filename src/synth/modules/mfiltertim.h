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

#ifndef MFILTER_TIM
#define MFILTER_TIM

class Synth;

/**
 * @short Moog-like resonant filter, based on Tim Stilson's moog filter code
 *
 * I took the code from http://www-ccrma.stanford.edu/~dfl/pd/index.htm
 * and adapted where necesary
 */
class MFilterTim{
public:
	MFilterTim(Synth *parent);
	
	void setResonance(float q);
	void setFrequency(float f);
	
	float frequency(){ return x_cutoff; }
	float resonance(){ return x_resonance; }
	
	void process(float *inout, unsigned int nsamples);
	void process(float *inout, unsigned int nsamples, float *lfoFreq, float *lfoQ);
	
	void panic();
protected:
	Synth *synth;
	
	
	float x_resonance;
	float x_cutoff;

	// Internals
	float p, Q;
	
	float output; // sets the pointer to last output
};

#endif
