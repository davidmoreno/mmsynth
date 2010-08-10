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

#ifndef __MFACTORY_H__
#define __MFACTORY_H__

#include <QWidget>
#include <QtUiTools/QUiLoader>

class MUiLoader : public QUiLoader{
	virtual QWidget *createWidget(const QString &name, QWidget *parent, const QString &objectName);
};



#endif
