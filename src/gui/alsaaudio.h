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


#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H

#include <sys/time.h>
#include <QThread>
#include <alsa/asoundlib.h>

#include "audio.h"

/**
 * @short Alsa PCM audio class
 *
 * Just open a connection to the default interface and send them the processed audio
 */
class AlsaAudio : public Audio
{
	Q_OBJECT
public:
	AlsaAudio(const QString &options, QObject *parent=NULL);
	~AlsaAudio();
	
	virtual bool init();

	/// On proper midi devices, returns the current sample offset, ie, how many samples have happened since last getAudio on synths.
	/// It is usefull to set midi offseted events, which are more accurate than begining-of-block events.
	unsigned int getSampleOffset();
public slots:
	void run();

protected:
	/// Number of block on the queue
	int periods;
	/// Size of the buffer (latency is periods*frames)
	unsigned int frames;
	
	/// PCM handle
	snd_pcm_t *pcm;
	/// Block sizes, as needed by the alsa API
	unsigned int blockSize;
	/// Semaphore like, 0 is keep on the run loop, 1 is quit.
	QAtomicInt wantToQuit;
	/// Usefull to get the sample offset
	struct timeval lastGetAudioTime;
};

#endif // ALSAAUDIO_H
