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

#ifndef __MDEBUG__
#define __MDEBUG__

#include <stdlib.h>
#include <QDateTime>

static int __doDebug=getenv("DEBUG")!=NULL;

#define DEBUG(...) if(__doDebug){ fprintf(stderr,"\033[01;32m%s %s:%d @%s \033[00m", \
                                         QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toAscii().data(), \
                                         basename(__FILE__),__LINE__,__FUNCTION__); \
                                         qDebug(__VA_ARGS__); }

#define WARNING(...) { fprintf(stderr,"\033[40;33;01m%s %s:%d @%s \033[00m", \
                                         QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toAscii().data(), \
                                         basename(__FILE__),__LINE__,__FUNCTION__); \
                                         qDebug(__VA_ARGS__); }

#define ERROR(...) { fprintf(stderr,"\033[01;35;01m%s %s:%d @%s \033[00m", \
                                         QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toAscii().data(), \
                                         basename(__FILE__),__LINE__,__FUNCTION__); \
                                         qDebug(__VA_ARGS__); }


#endif
