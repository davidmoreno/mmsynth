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

#include <synth.h>

#include "audio.h"
#include "alsaaudio.h"
#include "jackaudio.h"

#include "midi.h"


Audio::Audio(QObject *parent) : QThread(parent){
	midi=NULL;
}
Audio::~Audio(){
}

void Audio::addSynth(Synth *s){
	synthlock.lock();
	s->setSamplerate(_samplerate);
	synths.append(s);
	synthlock.unlock();
}

void Audio::removeSynth(Synth *s){
	synthlock.lock();
	synths.removeAll(s);
	synthlock.unlock();
}

void Audio::setMidi(Midi *m){
	midi=m; 
	midi->setAudio(this); 
}

/**
 * @short Factory for known audio engines.
 *
 * should be more dynamic (like allow the use of plugins), but not by the moment as its unnecesary.
 */
Audio *Audio::createAudio(const QString &audioname, QObject *parent){
	if (audioname=="alsa")
		return new AlsaAudio(parent);
	else if (audioname=="jack")
		return new JackAudio(parent);
	return NULL;
}
