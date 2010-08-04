#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class Midi;
class MSurface;
class Audio;
class Synth;

namespace Ui
{
class MainWindow;
}

/**
 * @short The main window for this msynth
 *
 * It coordinates all high level communication: between soundcard 
 * and UIS, saving presets...
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void setMidi(Midi *);
	void setAudio(Audio *);
	
	static QString lookForFile(const QString &filename, const QString &optionalsubdir);
public slots:
	void on_actionSavePreset_triggered();
	void on_actionLoadPreset_triggered();
	void on_actionMIDILearn_triggered();
	
	void addSynth(const QString &synthname);
	
	void receivedController(unsigned char controllerId, unsigned char value);
signals:
	void sendController(unsigned char controllerId, unsigned char value);
	
private:
	void connectGUI(MSurface *ui);
	Synth *createSynth(const QString &name);
	
private:
	char controllerValue[128];
	
	Ui::MainWindow *ui;
	QList<MSurface *> msynthuis;
	QList<Synth *> synths;
	Midi *midi;
	Audio *audio;
};

#endif // MAINWINDOW_H
