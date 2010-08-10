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

#include <QStatusBar>

#include "mainwindow.h"
#include "mdebug.h"
#include "midi.h"
#include "audio.h"

Midi::Midi(MainWindow *parent):QThread(parent){
	mainwindow=parent;
	for (int i=0;i<128;i++)
		controllerMapping[i]=i;
	noteOnController=255;
	lastControllerUsed=255;
}

Midi::~Midi(){
}

/**
 * @short Returns the events gathers since last call to this.
 */
MidiEventList Midi::getEvents(){
	eventLock.lock();
	MidiEventList ret=events;
	events=MidiEventList();
	eventLock.unlock();
	return ret;
}

/// Default implementation, just adds the event
void Midi::GUINoteOn(unsigned char note, unsigned char pressure){
	MidiEvent event={ audio->getSampleOffset(), pressure==0 ? MidiEvent::noteOff : MidiEvent::noteOn, {note, pressure } };
	eventLock.lock();
	events.append(event);
	eventLock.unlock();
}

/// Default implementation, just adds the event
void Midi::GUINoteOff(unsigned char note, unsigned char pressure){
	MidiEvent event={ audio->getSampleOffset(), MidiEvent::noteOff, { note, pressure } };
	eventLock.lock();
	events.append(event);
	eventLock.unlock();
}

/// Default implementation, just adds the event
void Midi::GUIController(unsigned char controllerId, unsigned char value){
	if (lastControllerUsed==128){
		lastControllerUsed=controllerId;
		mainwindow->statusBar()->showMessage(tr("Now move the midi controller to complete the mapping."));
	}
	
	MidiEvent event={ audio->getSampleOffset(), MidiEvent::controller, {controllerId, value } };
	eventLock.lock();
	events.append(event);
	eventLock.unlock();
}
