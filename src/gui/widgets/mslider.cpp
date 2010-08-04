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

#include <QPainter>
#include <QMouseEvent>
#include <QVariant>

#include <mdebug.h>

#include "mslider.h"

MSlider::MSlider(unsigned char number, Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent) {
    _number=number;
    setRange(0,127);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChange(int)));
}

void MSlider::onValueChange(int val) {
    emit valueChanged(_number, val);
}

void MSlider::setValue(unsigned char number, unsigned char value) {
    if (_number==number)
        setProperty("value",QVariant((int)value));
}

void MSlider::paintEvent ( QPaintEvent * event ){
	QPainter p(this);

	QPalette pal=palette();
	QPen pen(pal.color(QPalette::Active, hasFocus() ? QPalette::Highlight : QPalette::Text));
	p.setPen(pen);
	

	QBrush brush(pal.color(QPalette::Active, QPalette::Midlight));
	p.setBrush(brush);

	p.drawRect(0,0,width()-1, height());
	
	brush=QBrush(hasFocus() ? Qt::yellow : pal.color(QPalette::Active, QPalette::Highlight));
	p.setBrush(brush);
		
	int h=(height()*(127-value()))/127;
	p.drawRect(0,h,width()-1, height()-h);
	
	QString val=QString::number(value());
	QFontMetrics metrics(font());
	if (hasFocus()){
		QFont f=font();
		f.setBold(true);
		p.setFont(f);
	}
	p.drawText((width()-metrics.width(val))/2,metrics.height(),val);
}

void MSlider::mousePressEvent ( QMouseEvent * event ){
	int val=127-((event->y()*127)/height());
	setProperty("value",QVariant((int)val));
	update();
}

void MSlider::mouseMoveEvent ( QMouseEvent * event ){
	mousePressEvent(event);
}
