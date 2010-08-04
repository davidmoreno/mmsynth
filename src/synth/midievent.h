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

#ifndef MIDIEVENT_H
#define MIDIEVENT_H

#include <QList>

class MidiEvent
{
public:
	enum Type{
		none=0,
		noteOn=1,
		noteOff=2,
		controller=3,
	};
	
	/// Offset inside the petition, so the related synths can react.
	unsigned int sampleOffset;
	/// Type of the event
	Type type;
	/// Some data, by the moment no more then 2 bytes should be needed.
	char data[2];
};

typedef QList<MidiEvent> MidiEventList;


#endif // MIDIEVENT_H
