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
#ifndef MSLIDER_H
#define MSLIDER_H

#include <QSlider>
#include <QtDesigner/QDesignerExportWidget>

/**
* @short A custom slider that is usefull for MSurface
*/
class QDESIGNER_WIDGET_EXPORT MSlider : public QSlider
{
	Q_OBJECT
	Q_PROPERTY(int number READ number WRITE setNumber);
public:
	MSlider(unsigned char number, Qt::Orientation, QWidget *parent=NULL);

	unsigned char number() {
		return _number;
	}

public slots:
	void setNumber(unsigned char n) {
		_number=n;
	}
	void setValue(unsigned char number, unsigned char value);
	void onValueChange(int val);
signals:
	void valueChanged(unsigned char, unsigned char);

protected:
	void paintEvent ( QPaintEvent * event );
	void mousePressEvent ( QMouseEvent * event );
	void mouseMoveEvent ( QMouseEvent * event );
private:
	unsigned char _number;
	unsigned char _value;
};


#endif // MSLIDER_H
