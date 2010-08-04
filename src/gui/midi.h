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

#ifndef SOUNDCARD_H
#define SOUNDCARD_H

#include <QMutex>
#include <QThread>

#include <midievent.h>

class Audio;
class MainWindow;

/**
 * @short Base for midi communication with external sources. It can finally use a diferent backend.
 *
 * Backends could use ALSA, Jack, OSS... Even other propietary solutions.
 *
 * It is very important the getEvents and events list as they contein the info sent
 * to the synths.
 */
class Midi : public QThread
{
Q_OBJECT
public:
	Midi(MainWindow *parent=NULL);
	virtual ~Midi();
	
	/// Reimplement to initialize the midi system. Its here so it can fail, and can have delayed init.
	virtual int init(){ return true; };
	virtual MidiEventList getEvents();
	
	void setAudio(Audio *_audio){ audio=_audio; };
signals:
	/// @{ @name Events received from the external midi, sent to GUI
	void MIDINoteOn(unsigned char note, unsigned char pressure);
	void MIDINoteOff(unsigned char note, unsigned char pressure);
	void MIDIController(unsigned char note, unsigned char pressure);
	/// @}
public slots:
	/// @{ @name Events received from the GUI, sent to synth and midi out
	virtual void GUINoteOn(unsigned char note, unsigned char pressure);
	virtual void GUINoteOff(unsigned char note, unsigned char pressure);
	virtual void GUIController(unsigned char controllerId, unsigned char value);
	/// @}
	void setLearnMode(){ lastControllerUsed=128; }
protected:
	/// The mainwindow pointer, so we can use some GUI, like status bar.
	MainWindow *mainwindow;
	/// The lock for the events list
	QMutex eventLock;
	/// The events gathered by this midi interface, as passed to the Audio and finally to the Synth
	MidiEventList events;
	/// Paired Audio deice, needed for sync.
	Audio *audio;
	/**
	 * @short Last controller used, when in learn mode. If not in learn, its 255, if should learn, its 128, if selected a GUI control, its that controller.
	 *
	 * Being an atomic int i can assure that it will have a consistent value. Checking all uses I dont care if the value is invalid for the current state
	 * as the worst it can happend is that it sets the controller number to 128 or 255, which should then be ignored. And those cases are on non 
	 * normal use of the midi mapping feature, so it should not be important. All this applies too to controllerMapping.
	 *
	 * If all this proves wrong, we can set a lock around it and controllerMapping.
	 */
	QAtomicInt lastControllerUsed;
	/// Controller remapping, from midi learn; maps the midi controllers to the gui/synth ones.
	QAtomicInt controllerMapping[128];
	/// Maps the note on pressure to a controller.
	unsigned char noteOnController;
};

#endif // SOUNDCARD_H
