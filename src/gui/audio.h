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

#ifndef AUDIO_H
#define AUDIO_H

#include <QThread>
#include <QMutex>

class Synth;
class Midi;

/**
 * @short Audio PCM interface
 *
 * Audio interface so we can have several audio backends. This only relates to 
 * PCM interface, but it has a method, setMidi (that sets the protected member midi)
 * that is necesary to be able to use midi events at the synth.
 *
 * The interface must be processing the audio by its own means, usually another thread, 
 * and if it needs it it might use RT scheduling and so on.
 *
 * The MIDI and PCM interfaces are separeted so you can have them apart, but if you
 * have them together (like jack with midi), you can set the base midi as midi interface
 * and implement the Audio so it gets the signals from UI and sends from MIDI.
 *
 * It is a QThread, so that shoul dbe used to generate new threads and controlling them.
 */
class Audio : public QThread 
{
Q_OBJECT
public:
	Audio(QObject *parent=NULL);
	virtual ~Audio();

	/// Reimplement to initialize the audio system. Its here so it can fail, and can have delayed init.
	virtual bool init()=0;

	/// On proper midi devices, returns the current sample offset, ie, how many samples have happened since last getAudio on synths.
	/// It is usefull to set midi offseted events, which are more accurate than begining-of-block events.
	virtual unsigned int getSampleOffset(){ return 0; };

	/// Gets teh current samplerate
	float getSamplerate(){ return _samplerate; }
	
	static Audio *createAudio(const QString &audioname, QObject *parent);
public slots:
	/// Sets the midi interface, might be reimplemented if further action with midi is needed from audio
	virtual void setMidi(Midi *m);
	
	/// Adds a synth to the list
	void addSynth(Synth *);
	/// Remove a synth
	void removeSynth(Synth *);
protected:
	/// The midi interface, needed to send events correctly to Synth::getAudio from Midi::getEvents
	Midi *midi;
	/// This lock must be used always that the synths list is used, as synths may appear and dissapear at runtime
	QMutex synthlock;
	/// The synths list
	QList<Synth*> synths;
	/// The current samplerate
	float _samplerate;
	
};

#endif // AUDIO_H

