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

#ifndef SYNTH_H
#define SYNTH_H

#include <QObject>
#include <QList>
#include <QMap>

#include "midievent.h"

/**
 * @short Synth interface that must be implemented on new synths.
 *
 * The more important method to reimplement is getAudio, but also sendStatus is
 * quite important, as helps to set up the GUI.
 * 
 * The plugins mush also contain a function (C or C++, does not mather) of signature
 * Synth *create_synth_<synthname>(QObject *parent) at the library to call. The lib should
 * be called the same as the create_synth function. The function should create a Synth 
 * object that is the synth itself.
 */
class Synth : public QObject
{
	Q_OBJECT
public:
	/// Signature for the crator func that should be at the loaded shared lib
	typedef Synth *(*synth_creator_func)(QObject *parent);
	
	Synth(QObject *parent=NULL);
	/// Process a chunk of audio and sets it at audio. Size if nsamples and has the midi event list.
	virtual void getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &events);
	/// Sends the needed midi signals, usually controllers.
	virtual void sendStatus();
	
	/// Current samplerate
	float samplerate(){ return _samplerate; }
	
	static float freqOfNote(unsigned char note);
	static Synth *create(const QString &synthname, QObject *parent=NULL);
	static bool registerSynthCreator(const QString &synthname, synth_creator_func synth);
public slots:
	/// Sets the samplerate
	void setSamplerate(float samplerate){ _samplerate=samplerate; }
	
signals:
	void sendNoteOn(unsigned char note, unsigned char vel);
	void sendNoteOff(unsigned char note, unsigned char vel);
	void sendController(unsigned char controllerId, unsigned char value);
protected:
	/// Dict of known synths an its creators
	static QMap<QString,synth_creator_func> synths;
	/// Samples since the begining, should last between 1.12 days (32 bits) and 13 264 000 years (64 bits), at 96khz.
	unsigned int position; 
	/// Current samplerate
	float _samplerate;
};

#endif // SYNTH_H
