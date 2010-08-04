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
#include <mdebug.h>

#include "synth.h"

QMap<QString,Synth::synth_creator_func> Synth::synths;

Synth::Synth(QObject *parent) : QObject(parent){
	DEBUG("Initialiting synth");
	_samplerate=44100.0; // a safe guess, by the moment.
	position=0;
}

/**
 * @short Sample implementation, just a sine oscillator at C.
 */
void Synth::getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &events){
	unsigned int i;
	float freq=freqOfNote(60);
	float s;
	for (i=0;i<nsamples;i++){
		s=position/_samplerate;
		audio[0][i]=audio[1][i]=sin(2*M_PI*(s*freq))*0.5;
		
		position++;
	}
	//WARNING("%f sec",s);
}

/**
 * @short Sends the sendController so that the GUI is in tune with the synth.
 */
void Synth::sendStatus(){
	WARNING("Reimplement with your own send status!");
}

/**
 * @short For a given note, returns the normal freq it represents: 69 is central A is 440Hz
 */
float Synth::freqOfNote(unsigned char note){
	return 440.0*pow(2,(note-69)/12.0);
}
