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
#include <QtGui/QApplication>

#include "mainwindow.h"

#include "alsamidi.h" // Later will use a factory... by the moment just alsa.
#include "alsaaudio.h" // Later will use a factory... by the moment just alsa.
#include "mdebug.h"

static void help(const char *msynth);
static void audioHelp();
static void license();

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QString audiosystem="alsa";
	
	QStringList synths;
	QStringList args=a.arguments();
	args.removeAt(0);
	// Load just synth4k or as many synths as passed as args.
	foreach(QString opt, args){
		if (opt=="--help"){
			help(argv[0]);
			exit(1);
		}
		else if (opt=="--license"){
			license();
			exit(1);
		}
		else if (opt.startsWith("--midi=")){
			WARNING("--midi Not yet");
			exit(1);
		}
		else if (opt.startsWith("--audio=")){
			audiosystem=opt.section("=",1);
			if (audiosystem=="help"){
				audioHelp();
				exit(1);
			}
		}
		else if (opt.startsWith("--")){
			WARNING("Unknown option");
			help(argv[0]);
			exit(2);
		}
		else
			synths.append(opt);
	}
	
	if (synths.count()==0)
		synths.append("synth4k");
	
	MainWindow w;
	
	bool ok;
	AlsaMidi alsamidi(&w);
	ok=alsamidi.init();
	if (!ok){
		ERROR("Cant initialize MIDI device!");
		exit(1);
	}
	
	
	Audio *audio=Audio::createAudio(audiosystem, &w);
	ok=audio && audio->init();
	if (!ok){
		ERROR("Cant initialize AUDIO device!");
		exit(1);
	}
	
	w.setMidi(&alsamidi);
	w.setAudio(audio);
	
	foreach(QString s, synths)
		w.addSynth(s);
	
	w.show();
	return a.exec();
}

static void help(const char *msynth){
	printf("%s -- Monkey Synth\n"
		   "\tA sound synthesizer. (C) 2009 David Moreno Montero. Under GPLv3 license or above.\n"
		   "\n"
		   "Command line options:\n"
		   "\tmsynth [--license|--help|--midi=MMM|--audio=AAA] [synth1] [synth2] [...]\n"
		   "\n"
		   "\t--help       -- Shows this help\n"
		   "\t--license    -- Shows the license information\n"
		   "\t--midi=MMM   -- Uses that MIDI engine, by default alsa, 'help' shows more help\n"
		   "\t--audio=AAA  -- Uses that AUDIO engine, by default alsa, 'help' shows more help\n"
		   "\n\t[synth1] [synth2]... -- Initializes with that synths, to consecutive channels, channel 0 is all channels. 'help' shows more help\n"
		   "\n"
		   "\nAdditionally you can set the DEBUG environment variable to see debug traces, " 
		   "and MSYNTH to set the place where all synths and skins are installed (synth and uis directories)\n"
		   "\n",
		   msynth);
}

static void license(){
	printf("\n"
    "MSynth - Monkey Synth - Modular synthetiser with nice UI\n"
    "Copyright (C) 2009 David Moreno Montero\n"
	"\n"
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
	"\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
	"\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
    "\n");
}

static void audioHelp(){
	printf("\n"
	"Available audio engines: alsa jack\n\n"
	"Options by engine:\n\n"
	" --audio=alsa,periods=3,frames=128,device=hw:0,0\n"
	"     periods -- Number of periods\n"
	"     frames  -- Number of frames, latency is (frames*periods/channels)/samplerate\n"
	"     device  -- Alsa device to use\n"
	"\n");
}