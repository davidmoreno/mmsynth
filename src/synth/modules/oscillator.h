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

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <QList>

class Synth;

/**
 * @short A single oscillator, with basic oscillator operations.
 */ 
class Oscillator
{
	public:
		typedef enum Type_t{
			saw=0,
			triangle=1,
			square=2,
			sine=3,
			isaw=4
		}Type;
		
		Oscillator(Synth *parent=NULL);
		void setSynth(Synth *s){ synth=s; }
		
		void setType(Type t){ _type=t; }
		Type type(){ return _type; }
		void setFrequency(float _freq){ freq=_freq; }
		
		void playNote(char note);
		void process(float *data, unsigned int nsamples);
		char note(){ return _note; }
		
	protected:
		Synth *synth;
		Type _type;

		char _note;
		float freq;
		float phase;
};

/**
 * @short A group of oscillators, with common operations so they are transmitted to all oscillators.
 *
 * Normally you create a group, and then access to each individual term with foreach or with .at()
 */
class OscillatorGroup{
public:
	OscillatorGroup(unsigned int count, Synth *parent);
	~OscillatorGroup();

	void setType(Oscillator::Type t);
	Oscillator::Type type();
	
	int findOscillatorByNote(char note);

	/// Returns the oscillator at some position
	Oscillator *operator[](unsigned int i){ return list.at(i); }
	Oscillator *at(unsigned int i){ return list.at(i); }
protected:
	/// The real list of oscillators.
	QList<Oscillator *> list;
};

#endif // OSCILLATOR_H
