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

#ifndef LFO_H
#define LFO_H

class Synth;

class LFO{
public:
	enum Type{
		sine=0,
		saw=1,
	};
	
	LFO(Synth *synth);

	void panic();
	
	void setType(Type t){ _type=t; }
	void setFrequency(float _freq){ freq=_freq; }
	void setLevel(float level){ pivotLevel=level; }
	void setDelta(float delta){ pivotDelta=delta; }
	
	Type type(){ return _type; }
	float frequency(){ return freq; }
	float level(){ return pivotLevel; }
	float delta(){ return pivotDelta; }
	
	void process(float *data, unsigned int nsamples);
protected:
	Type _type;
	float phase;

	float freq;
	float pivotLevel;
	float pivotDelta;
	
	Synth *synth;
};

#endif
