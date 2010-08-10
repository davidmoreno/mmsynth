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

#ifndef ALSASOUND_H
#define ALSASOUND_H

#include <alsa/asoundlib.h>

#include <midi.h>


class AlsaMidi: public Midi
{
Q_OBJECT
public:
	AlsaMidi(MainWindow *parent=NULL);
    virtual ~AlsaMidi();
    virtual int init();
public slots:
	virtual void GUINoteOn(unsigned char note, unsigned char pressure);
	virtual void GUINoteOff(unsigned char note, unsigned char pressure);
	virtual void GUIController(unsigned char controllerId, unsigned char value);
	
	void run();
protected:
	void autoconnect();
	
private:
	/// Alsa sequencer handle
	snd_seq_t *handle;
	/// Alsa port number.
	int allPort;
};

#endif // ALSASOUND_H
