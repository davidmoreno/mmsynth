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
#ifndef MSURFACE_H
#define MSURFACE_H

#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>

/**
  * @short MIDI Surface that receives and sends midi signals
  *
  * On these surface usually you move some slider or dial and
  * send a midi event (send a signal to the sendMIDINote signal),
  * or receive a signal (from the receiveMIDINote signal).
  *
  * These are usually connected to the custom widgets that almost
  * all of them allow to have custom 'id', for example on
  * controllers they use sendControl with the predefined id as first
  * value (controller id), and the slider value as second. Similary
  * when a controller sends a value, the widget checks if its for
  * itself.
  *
  * The custom widgets are: MLed, MButton, MDial, MSlider.
  */
class QDESIGNER_WIDGET_EXPORT MSurface : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString synthName READ synthName WRITE setSynthName);
public:
    MSurface(QWidget *parent = 0);
    ~MSurface();

    QString synthName() {
        return _synthName;
    }

public slots:
    void setSynthName(const QString &s) {
        _synthName=s;
    }

	void autoconnect();

    /// @{ @name from widget to world, proxy for signal
    void sendNoteOnSlot(unsigned char midiNote, unsigned char pressure) {
        sendNoteOn(midiNote,pressure);
    }
    void sendNoteOffSlot(unsigned char midiNote, unsigned char pressure) {
        sendNoteOff(midiNote,pressure);
    }
    void sendControllerSlot(unsigned char controllerId, unsigned char value) {
        sendController(controllerId,value);
    }
    /// @}

signals:
    /// @{ @name from world to widget
    void receiveNoteOn(unsigned char midiNote, unsigned char pressure);
    void receiveNoteOff(unsigned char midiNote, unsigned char pressure);
    void receiveController(unsigned char controllerId, unsigned char value);
    /// @}

    /// @{ @name from widget to world
    void sendNoteOn(unsigned char midiNote, unsigned char pressure);
    void sendNoteOff(unsigned char midiNote, unsigned char pressure);
    void sendController(unsigned char controllerId, unsigned char value);
    /// @}


private:
    /// Used to join this interface to a synth, so the engine fires it up and joins.
    QString _synthName;
};


#endif // MSURFACE_H
