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

#include <dlfcn.h>
#include <stdio.h> 

#include <QtUiTools/QtUiTools>
#include <QFileDialog>
#include <QMessageBox>
 
#include <synth.h>
#include <mdebug.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "midi.h"
#include "audio.h"
#include "widgets/msurface.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	midi=NULL;
	audio=NULL;
	memset(controllerValue,0,sizeof(controllerValue));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @short Sets the soundcard to use on this msynth window.
 */
void MainWindow::setMidi(Midi *sc){
	midi=sc;
	if (audio)
		audio->setMidi(midi);
	connect(this, SIGNAL(sendController(unsigned char,unsigned char)),
			midi, SLOT(GUIController(unsigned char,unsigned char)));
	connect(midi, SIGNAL(MIDIController(unsigned char,unsigned char)),
			this, SLOT(receivedController(unsigned char,unsigned char)));
}

/**
 * @short Sets the audio to use to process the synth audio and output the sound
 */
void MainWindow::setAudio(Audio *au){
	audio=au;
	
	DEBUG("Setting the audio %p",au);
	
	if (midi)
		audio->setMidi(midi);
}


/**
 * @short Saves the preset file for the current synth. Just a list of values for all controllers.
 *
 * The preset itself its just a list of human readable controller values, from controller 0 to crller 127,
 * separated by spaces.
 */
void MainWindow::on_actionSavePreset_triggered(){
	QString output=QFileDialog::getSaveFileName(this,tr("Select preset file"),".","*.mmsynth");
	
	if (output.isEmpty())
		return;
	
	DEBUG("Creating file %s",output.toAscii().data());
	
	QFile f(output);
	if (!f.open(QIODevice::WriteOnly)){
		QMessageBox::critical(this,tr("Error saving file"),tr("Error saving the preset file. Could not open the file for writing."));
		return;
	}
	int i;
	for (i=0;i<128;i++){
		f.write(QString::number(controllerValue[i]).toAscii().data());
		f.write(" ");
	}
	f.close();
}

/**
 * @short Loads the presets from a given file. See savePreset for format description.
 */
void MainWindow::on_actionLoadPreset_triggered(){
	QString input=QFileDialog::getOpenFileName(this,tr("Select preset file"),".","*.mmsynth");
	
	if (input.isEmpty())
		return;
	
	DEBUG("Loading file %s",input.toAscii().data());
	
	QFile f(input);
	if (!f.open(QIODevice::ReadOnly)){
		QMessageBox::critical(this,tr("Error loading file"),tr("Error loading the preset file. Could not open the file for reading."));
		return;
	}
	QTextStream stream(&f);
	int i;
	int val;
	for (i=0;i<128;i++){
		stream>>val;
		//DEBUG("For controller %d send %d",i,val);
		emit sendController(i,val);
	}
	f.close();
}


/**
 * @short Controller changes because of midi, synth or GUI are stored here, for later saving if necesary.
 */
void MainWindow::receivedController(unsigned char controllerId, unsigned char value){
	if (controllerId>127)
		return;
	controllerValue[controllerId]=value;
}

/**
 * @short Start the midi learn mode
 */
void MainWindow::on_actionMIDILearn_triggered(){
	statusBar()->showMessage(tr("Move the GUI control with the mouse where you want to map, and then the MIDI control"));
	midi->setLearnMode();
}


/**
 * @short Adds a synth to the system
 */
void MainWindow::addSynth(const QString &synthname){
	QUiLoader loader;

	QString uifile=lookForFile(QString("%1.ui").arg(synthname),"uis");
	
	if (uifile.isEmpty())
		uifile=lookForFile(QString("default.ui").arg(synthname),"uis");
	QFile file(uifile);
	file.open(QFile::ReadOnly);
	MSurface *msynthui = (MSurface*)(loader.load(&file, this));
	file.close();
	
	if (msynthui){
		msynthui->autoconnect();
		setCentralWidget(msynthui);
		setWindowTitle(msynthui->windowTitle());
		connectGUI(msynthui);
		msynthuis.append(msynthui);
	}
	else{
		ERROR("Cant create GUI for %s",synthname.toAscii().data());
	}
	
	Synth *synth=createSynth(synthname);
	if (!synth){
		ERROR("Cant create synth %s",synthname.toAscii().data());
	}
	if (msynthui){
		//DEBUG("Connecting synth to ui (default params)");
		connect(synth, SIGNAL(sendController(unsigned char,unsigned char)), msynthui, SIGNAL(receiveController(unsigned char,unsigned char)));
		connect(synth, SIGNAL(sendController(unsigned char,unsigned char)),
				this, SLOT(receivedController(unsigned char,unsigned char)));
	}
	if (synth){
		audio->addSynth(synth);
		synth->sendStatus();
	}
	synths.append(synth);
}

/**
 * @short Factory of synths
 *
 * It tries on several ways: from the dict of known creators and dynamic loading libs.
 */
Synth *MainWindow::createSynth(const QString &synthname){
	DEBUG("Tying to create '%s' synth",synthname.toAscii().data());

	Synth::synth_creator_func func;
	DEBUG("Loading via dlsym");

	// System and ldpreload 
	QString libfile=lookForFile(QString("lib%1.so").arg(synthname),"synth");
	if (libfile.isEmpty()){
		libfile=lookForFile(QString("%1/lib%1.so").arg(synthname),"synth");
		if (libfile.isEmpty()){
			WARNING("Cant find a proper synth library. Failing synth load.");
			return NULL;
		}
	}
	void *lib=dlopen(libfile.toAscii().data(),RTLD_NOW|RTLD_GLOBAL|RTLD_NODELETE);
	DEBUG("Loading %s, %s",libfile.toAscii().data(), lib ? "ok" : "nok");
	if (!lib){
		ERROR("Library found, but cant be loaded. Synth '%s' not created.",synthname.toAscii().data());
		WARNING(dlerror());
	}
	else{
		dlclose(lib);
	}

	char temp[256];
	// Create C mode
	sprintf(temp,"create_synth_%s",synthname.toAscii().data());
	func=(Synth::synth_creator_func)dlsym(NULL,temp); // try C mode
	if (func)
		return func(this);
	// create C++ mode
	sprintf(temp,"_Z%dcreate_synth_%sP7QObject",13+synthname.length(), synthname.toAscii().data()); // try C++ mode
	func=(Synth::synth_creator_func)dlsym(NULL,temp);
	if (func)
		return func(this);

	ERROR("Synth '%s' not found. Not creating it.",synthname.toAscii().data());
	WARNING(dlerror());
	return new Synth(this);
}


/**
 * @short Connects the needed signals to the newly created ui.
 */
void MainWindow::connectGUI(MSurface *ui){
	if (ui && midi){
		// First connect from world to widgets
		connect(ui,   SIGNAL(sendNoteOn(unsigned char,unsigned char)),
				midi, SLOT(GUINoteOn(unsigned char,unsigned char)));
		connect(ui,   SIGNAL(sendNoteOff(unsigned char,unsigned char)),
				midi, SLOT(GUINoteOff(unsigned char,unsigned char)));
		connect(ui,   SIGNAL(sendController(unsigned char,unsigned char)),
				midi, SLOT(GUIController(unsigned char,unsigned char)));
				
		// Now from widgets to world
		connect(midi, SIGNAL(MIDINoteOn(unsigned char,unsigned char)),
				ui,   SIGNAL(receiveNoteOn(unsigned char,unsigned char)));
		connect(midi, SIGNAL(MIDINoteOff(unsigned char,unsigned char)),
				ui,   SIGNAL(receiveNoteOff(unsigned char,unsigned char)));
		connect(midi, SIGNAL(MIDIController(unsigned char,unsigned char)),
				ui,   SIGNAL(receiveController(unsigned char,unsigned char)));

		connect(this, SIGNAL(sendController(unsigned char,unsigned char)),
				ui,   SIGNAL(receiveController(unsigned char,unsigned char)));
		connect(ui,   SIGNAL(sendController(unsigned char,unsigned char)),
				this, SLOT(receivedController(unsigned char,unsigned char)));
	}
}

/**
 * @short Looks for a given file at all the possible install directories
 *
 * This dirs are:
 * * <filename>
 * * <optionaldir>/<filename>
 * * $MSYNTH/<filename>
 * * $MSYNTH/<optionaldir>/<filename>
 * * INSTALL_PREFIX/<filename>
 * * INSTALL_PREFIX/<optionaldir>/<filename>
 *
 * As can be seen the prefrences are always a given normal dir, then the optional part, 
 * and then on current, on session indicated with MSYNTH environment variable, and 
 * finally on installed place. Although the last is usually the good.
 */
QString MainWindow::lookForFile(const QString &filename, const QString &optionaldir){
	QStringList options;
	const char *mdir=getenv("MSYNTH");
	
	options<<filename;
	options<<QString("%1/%2").arg(optionaldir).arg(filename);
	if (mdir){
		options<<QString("%1/%2").arg(mdir).arg(filename);
		options<<QString("%1/%2/%3").arg(mdir).arg(optionaldir).arg(filename);
	}
	options<<QString("%1/%2").arg(INSTALL_PREFIX).arg(filename);
	options<<QString("%1/%2/%3").arg(INSTALL_PREFIX).arg(optionaldir).arg(filename);
	
	foreach(QString file, options){
		DEBUG("Checking if %s exists",file.toAscii().data());
		if (QFile::exists(file))
			return file;
	}
	DEBUG("Cant find file %s at known paths. Maybe I need MSYNTH env. var?",filename.toAscii().data());
	return QString::null;
}
