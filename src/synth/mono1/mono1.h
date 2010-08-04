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

#ifndef MONO1_H
#define MONO1_H

#include "../synth.h"
#include "../modules/oscillator.h"
#include "../modules/adsr.h"
#include "../modules/mfiltertim.h"
#include "../modules/lfo.h"

class Mono1 : public Synth
{
public:
	Mono1(QObject *parent);
	
	virtual void getAudio(float *audio[2], unsigned int nsamples, const MidiEventList& events);
	virtual void sendStatus();

protected:
	void process(float *data, unsigned int nsamples);

	void noteOn(unsigned char note, unsigned char velocity);
	void noteOff(unsigned char note, unsigned char velocity);
	void controller(unsigned char controllerId, unsigned char value);
protected:
	OscillatorGroup **osc;
	ADSRGroup adsr;
	MFilterTim filter;
	LFO lfoFreq;
	LFO lfoQ;
	
	bool fullLevel;
	
	float *velocity;
	
	float _gain;
	
	float *temp;
	//float *dataLFOQ; // we reuse temp
	float *dataLFOFreq;
};

#endif // MONOI_H
