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

#ifndef POLYBASE_H
#define POLYBASE_H

#include "../synth.h"

/**
 * @short simple polyphonic synthetiser with 4 oscilators * nvoices voices.
 *
 * It is just a simple synth with not too much control, but enough to play with
 * polyphony and farfisa like sounds.
 */
class Polybase : public Synth{
public:
	Polybase(QObject *parent);
	~Polybase();
	
	virtual void getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &list);
	virtual void sendStatus();
protected:
	void noteOn(unsigned char note, unsigned char pressure);
	void noteOff(unsigned char note, unsigned char pressure);
	void controller(unsigned char id, unsigned char value);
	
protected:
	struct Oscillator{
		Oscillator(){ 
			phase[0]=0.0f; phase[1]=0.0f; phase[2]=0.0f; phase[3]=0.0f; 
			note=0; 
			poly=NULL; 
		}
		
		float freq[4];
		float phase[4];
		unsigned char note;
		Polybase *poly;
		
		void oscillatorChunk(float *data, unsigned int nsamples);
	};

	struct Filter{
		Filter(Polybase *_poly){ low=0; band=0; freq=0; q=0; dist=0; poly=_poly; }
		
		float low;
		float band;
		float freq;
		float tofreq;
		float q;
		float dist;
		Polybase *poly;

		void filterChunk(float *data, unsigned int nsamples);
	};

	void calculateChunk(float *data, unsigned int nsamples);
	
protected:
	float *chunk;
	unsigned int chunksize;
	unsigned char nvoices;
	float oscillatorLevel[4];
	unsigned char oscillatorForm[4];
	Oscillator *oscillators;
	
	Filter filter; // just one filter.
	
	float volume;
	
	friend class Oscillator;
};

#endif
