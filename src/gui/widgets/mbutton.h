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
#ifndef MBUTTON_H
#define MBUTTON_H

#include <QPushButton>
#include <QtDesigner/QDesignerExportWidget>

/**
 * @short A push button to be used with the msurface
 *
 * This button also have some extra information as the controller/note number
 * and value, so when clicked it emits the clicked(unsigned char, unsigned char) signal.
 */
class QDESIGNER_WIDGET_EXPORT MButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int number READ number WRITE setNumber);
    Q_PROPERTY(int value READ value WRITE setValue);
    Q_PROPERTY(int buttonType READ buttonType WRITE setButtonType);
public:
    MButton(unsigned char number, unsigned char value, QWidget *parent=NULL);

    unsigned char number() {
        return _number;
    }
    unsigned char value() {
        return _value;
    }
	int buttonType(){
		return (int)_buttonType;
	}

public slots:
    void setNumber(unsigned char n) {
        _number=n;
    }
    void setValue(unsigned char v) {
        _value=v;
    }
    void setButtonType(int v) {
        _buttonType=v;
    }

    void onClick();
signals:
    void clicked(unsigned char, unsigned char);

private:
    unsigned char _number;
    unsigned char _value;
	int _buttonType; /// 0 controller, 1 noteon, 2 noteoff
};

#endif // MBUTTON_H
