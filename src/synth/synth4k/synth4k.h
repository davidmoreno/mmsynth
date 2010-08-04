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

#ifndef SYNTH4K_H
#define SYNTH4K_H

#include "../synth.h"

/**
 * @short Synth based on solo/bass sound from 4kwave by Sertrem/RGBA
 *
 * This synth is quite a lot spagetti code as it comes from the original 4k, 
 * (optimized for size, quite strong time constraints at the moment), and
 * i wanted to modify as little as possible, so the sound is the original.
 *
 * The original, but with controllers so you can modify the sound, and with
 * note off (in the original i never gave enought time between notes to check).
 */
class Synth4k : public Synth
{
public:
	Synth4k(QObject *parent);
	~Synth4k();
	
    virtual void getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &list);
	virtual void sendStatus();
protected:
	void receiveController(unsigned char controllerId, unsigned char value);
	void receiveNoteOn(unsigned char note);
	void receiveNoteOff(unsigned char note);
	
protected:
	/// Last played note - its a monophonic synth.
	int lastnote;
	float volume;
	char *mysynth;
};

#endif // SYNTH4K_H
