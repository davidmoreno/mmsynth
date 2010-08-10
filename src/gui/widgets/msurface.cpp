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
#include <QPalette>
#include <QStack>

#include <mdebug.h>

#include "msurface.h"
#include "mbutton.h"

MSurface::MSurface(QWidget *parent)
        : QWidget(parent)
{
}

MSurface::~MSurface()
{

}

/**
 * @short Looks for known controller/note widgets, check the type, and automatically connect them
 *
 * This way creation of interfaces is even simpler.
 *
 * It looks for children of children too as when grouping they are set childs of that widget.
 */
void MSurface::autoconnect(){
	QStack<QObject *> stack;
	foreach(QObject *o,children()) stack.push(o);
	
	int n=0;
	while (!stack.isEmpty()){
		QObject *child=stack.pop();

		//DEBUG("Child %s, class %s", child->objectName().toAscii().data(), child->metaObject()->className());
		if (child->inherits("MDial") || child->inherits("MSlider")){
			DEBUG("Connecting object %s, type MDial|MSlider",child->objectName().toAscii().constData());
			connect(this, SIGNAL(receiveController(unsigned char,unsigned char)), child, SLOT(setValue(unsigned char, unsigned char)));
			connect(child, SIGNAL(valueChanged(unsigned char,unsigned char)), this, SIGNAL(sendController(unsigned char,unsigned char)));
			n++;
		}
		else if (child->inherits("MButton")){
			MButton *b=qobject_cast< MButton* >(child);
			DEBUG("Connecting object %s, type MButton, subtype %d",child->objectName().toAscii().constData(), b->buttonType());
			switch(b->buttonType()){
				case 0: // controller
					connect(this, SIGNAL(receiveController(unsigned char,unsigned char)), child, SLOT(setValue(unsigned char, unsigned char)));
					connect(child, SIGNAL(clicked(unsigned char,unsigned char)), this, SIGNAL(sendController(unsigned char,unsigned char)));
					n++;
					break;
				case 1: // note on
					connect(this, SIGNAL(receiveNoteOn(unsigned char,unsigned char)), child, SLOT(setValue(unsigned char, unsigned char)));
					connect(child, SIGNAL(clicked(unsigned char,unsigned char)), this, SIGNAL(sendNoteOn(unsigned char,unsigned char)));
					n++;
					break;
				case 2: // note off
					connect(this, SIGNAL(receiveNoteOff(unsigned char,unsigned char)), child, SLOT(setValue(unsigned char, unsigned char)));
					connect(child, SIGNAL(clicked(unsigned char,unsigned char)), this, SIGNAL(sendNoteOff(unsigned char,unsigned char)));
					n++;
					break;
				default:
					break;
			}
		}
		foreach(QObject *o,child->children()) stack.push(o);
	}
	DEBUG("Connected %d objects to surface",n);
}
