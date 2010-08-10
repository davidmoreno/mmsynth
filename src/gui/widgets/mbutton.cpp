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
#include "mbutton.h"

MButton::MButton(unsigned char number, unsigned char value, QWidget *parent): QPushButton(parent) {
    _number=number;
    _value=value;
	_buttonType=0;
    connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void MButton::onClick() {
    emit clicked(_number, _value);
}
