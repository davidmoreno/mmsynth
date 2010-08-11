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

#define SAMPLER_POLYPHONY 64

/**
 *  @short Sample sampler
 */
class Sampler : public Synth
{
public:
	Sampler(QObject *parent);
	
	virtual void getAudio(float *audio[2], unsigned int nsamples, const MidiEventList& events);
	virtual void sendStatus();
protected:
	void process(float *data, unsigned int nsamples);
	
	void loadSample(const QString &name);
	
	void noteOn(unsigned char note, unsigned char velocity);
	void noteOff(unsigned char note, unsigned char velocity);
	void controller(unsigned char controllerId, unsigned char value);
protected:
	float *temp;

	char vel;	

	float *sample;
	float sampleFreq; // freq to make it be a 440Hz note (A) at current samplerate
	unsigned long frames;

	char pnote[SAMPLER_POLYPHONY];
	double delta[SAMPLER_POLYPHONY]; // delta for position that it should advance in the sample itself each new frame.
	double samplePosition[SAMPLER_POLYPHONY];
};

#endif 

