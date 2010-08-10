/*
    MSynth - Monkey Synth - Modular synthetiser with nice UI
    Copyright (C) 2010 David Moreno Montero

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

#include "muiloader.h"
#include "mbutton.h"
#include "mdial.h"
#include "mslider.h"
#include "msurface.h"

QWidget *MUiLoader::createWidget(const QString &name, QWidget *parent, const QString &oname){
	QWidget *r=NULL;
	if (name=="MButton")
		r=new MButton(0,0,parent);
	if (name=="MDial")
		r=new MDial(0,parent);
	if (name=="MSlider")
		r=new MSlider(0,Qt::Vertical,parent);
	if (name=="MSurface")
		r=new MSurface(parent);

	if (r){
		r->setObjectName(oname);
		return r;
	}
	return QUiLoader::createWidget(name,parent,oname);
}
