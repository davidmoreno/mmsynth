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

#ifndef MFILTER_H
#define MFILTER_H

class Synth;

/**
 * @short Resonant filter based on the moog filter at http://www.musicdsp.org/archive.php?classid=3#24 (Moog VCF)
 *
 * Credited there as from "CSound source code, Stilson/Smith CCRMA paper."
 */
class MFilter{
public:
	MFilter(Synth *parent);
	
	void process(float *data, unsigned int nsamples);
	
	void setFrequency(float f){ freq=f; }
	void setResonance(float r){ res=r; }
	
	float frequency(){ return freq; }
	float resonance(){ return res; }
	
	void panic();
protected:
	float y[4];
	float old[4];
	float out;
	
	float freq;
	float res;
	
	Synth *synth;
};

#endif
