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

#ifndef JACKAUDIO_H
#define JACKAUDIO_H

#include <sys/time.h>
#include <jack/jack.h>

#include "audio.h"


class JackAudio : public Audio
{
public:
	JackAudio(QObject *parent);
	~JackAudio();
	
    bool init();
	int processData(jack_nframes_t nframes);

	/// On proper midi devices, returns the current sample offset, ie, how many samples have happened since last getAudio on synths.
	/// It is usefull to set midi offseted events, which are more accurate than begining-of-block events.
	unsigned int getSampleOffset();

protected:
	jack_client_t *jack;
	jack_port_t *outputR, *outputL;
	float *synthaudio[2];
	unsigned int blockSize;
	/// Usefull to get the sample offset
	struct timeval lastGetAudioTime;
};

#endif // JACKAUDIO_H
