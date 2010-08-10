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

#include <QVariant>
#include <QPainter>

#include <mdebug.h>

#include "mdial.h"

MDial::MDial(unsigned char number, QWidget *parent) : QDial(parent) {
	_number=number;
	setRange(0,127);
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChange(int)));
}

void MDial::onValueChange(int val) {
	emit valueChanged(_number, val);
}

void MDial::setValue(unsigned char number, unsigned char value) {
	if (_number==number)
		setProperty("value",QVariant((int)value));
}

void MDial::paintEvent ( QPaintEvent * event ){
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	
	QPalette pal=palette();
	
	QBrush brush(pal.color(QPalette::Active, QPalette::Midlight));
	p.setBrush(brush);

	QPen pen(pal.color(QPalette::Active, hasFocus() ? QPalette::Highlight : QPalette::Text));
	pen.setWidth(1);
	p.setPen(pen);

	int dim=width()>height() ? height() : width();
	int x=(width()-dim)/2;
	int y=1+(height()-dim)/2;

	
	p.drawPie(x,y, dim, dim, (270+30)*16, (360-60)*16);

	brush=QBrush(hasFocus() ? Qt::yellow : pal.color(QPalette::Active, QPalette::Highlight));
	p.setBrush(brush);
	
	int a=(360-60)*value()*16/127;
	p.drawPie(x,y, dim, dim, (270-30)*16-a, a);

	brush=QBrush(pal.color(QPalette::Active, QPalette::Window));
	p.setBrush(brush);
	
	int margin=dim*40/100;
	p.drawPie(x+margin/2, y+margin/2, dim-margin, dim-margin, 270*16, 360*16);

	pen=QPen(pal.color(QPalette::Active, QPalette::Window));
	pen.setWidth(3);
	p.setPen(pen);

	p.drawPie(x+margin/2, y+margin/2, dim-margin, dim-margin, (270-29)*16, 58*16);

	pen=QPen(pal.color(QPalette::Active, hasFocus() ? QPalette::Highlight : QPalette::Text));
	pen.setWidth(1);
	p.setPen(pen);

	QString val=QString::number(value());
	QFontMetrics metrics(font());
	if (hasFocus()){
		QFont f=font();
		f.setBold(true);
		p.setFont(f);
	}
	p.drawText((width()-metrics.width(val))/2, (height()+(metrics.height()/2))/2, val);
}
