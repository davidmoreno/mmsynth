/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include <alsa/asoundlib.h>

#include <QStatusBar>
#include <QTimer>

#include "mainwindow.h"
#include "alsamidi.h"
#include "audio.h"
#include "mdebug.h"

AlsaMidi::AlsaMidi(MainWindow *parent) : Midi(parent)
{
	handle=NULL;
}

AlsaMidi::~AlsaMidi(){
	if (handle){
		terminate(); // not very subtle, but just what i need
		wait(); // wait, its cortesy
		
		DEBUG("Removed Alsa object");
		snd_seq_close(handle);
	}
}

/**
 * @short Initializes the Alsa sequencer connection.
 */
int AlsaMidi::init()
{
	DEBUG("Creating Alsa object");

	int err;
	// default is the soundcard... but actually goes to the patch bay.
	err = snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0);
	if (err < 0)
		return false;
	snd_seq_set_client_name(handle, "msynth"); // client name
	allPort=snd_seq_create_simple_port(handle, "all_synths",
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ
					|SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE
					,SND_SEQ_PORT_TYPE_MIDI_GENERIC);

	autoconnect();

	start();
	DEBUG("Created Alsa object");
	return true;
}

/// Sends the event to the midi out, and calls default impl (store event for later use)
void AlsaMidi::GUINoteOn(unsigned char note, unsigned char pressure){
	snd_seq_event ev;
	
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, allPort);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	if (pressure!=0)
		snd_seq_ev_set_noteon(&ev, 0, note, pressure);
	else
		snd_seq_ev_set_noteoff(&ev, 0, note, 0);
	snd_seq_event_output(handle, &ev);
	snd_seq_drain_output(handle);

	Midi::GUINoteOn(note,pressure);
}

/// Sends the event to the midi out, and calls default impl (store event for later use)
void AlsaMidi::GUINoteOff(unsigned char note, unsigned char pressure){
	snd_seq_event ev;
	
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, allPort);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	snd_seq_ev_set_noteoff(&ev, 0, note, pressure);
	snd_seq_event_output(handle, &ev);
	snd_seq_drain_output(handle);

	Midi::GUINoteOff(note,pressure);
}

/// Sends the event to the midi out, and calls default impl (store event for later use)
void AlsaMidi::GUIController(unsigned char controllerId, unsigned char value){
	snd_seq_event ev;
	
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, allPort);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	snd_seq_ev_set_controller(&ev, 0, controllerId, value);
	snd_seq_event_output(handle, &ev);
	snd_seq_drain_output(handle);

	
	Midi::GUIController(controllerId,value);
}


/**
 * @short Thread to wait for midi events, and deliver them as needed.
 *
 * It emits singals, and Qt delivers them to the right thread.
 *
 * Also events are appended to the event list.
 */
void AlsaMidi::run(){
	snd_seq_event_t *ev;
	
	MidiEvent event;
	while(true){
		snd_seq_event_input(handle, &ev);
		event.type=MidiEvent::none;
		
		switch(ev->type){
			case SND_SEQ_EVENT_NOTEON:
				if (lastControllerUsed<128){
					noteOnController=lastControllerUsed;
					DEBUG("Mapped internal controller %d to note velocity",int(lastControllerUsed));
					mainwindow->statusBar()->showMessage(tr("Mapping complete. Mapped to key velocity."),1000);
					lastControllerUsed=255;
				}
				if (ev->data.note.velocity!=0){
					if (noteOnController<128){ // There is a mapping between midi note on veolcity and a controller
						MidiEvent event2={audio->getSampleOffset(), MidiEvent::controller, { noteOnController,ev->data.note.velocity} };
						eventLock.lock();
						events.append(event2);
						eventLock.unlock();
						event=event2;
						emit MIDIController(noteOnController, ev->data.note.velocity);
					}
					MidiEvent event2={audio->getSampleOffset(), MidiEvent::noteOn, { ev->data.note.note,ev->data.note.velocity } };
					event=event2;
					emit MIDINoteOn(ev->data.note.note,ev->data.note.velocity);
				}
				else{
					MidiEvent event2={audio->getSampleOffset(), MidiEvent::noteOff, { ev->data.note.note,ev->data.note.velocity } };
					event=event2;
					emit MIDINoteOff(ev->data.note.note, 0);
				}
				break;
			case SND_SEQ_EVENT_NOTEOFF:
			{
				MidiEvent event2={audio->getSampleOffset(), MidiEvent::noteOff, { ev->data.note.note,ev->data.note.velocity } };
				event=event2;
				emit MIDINoteOff(ev->data.note.note, 0);
			}
				break;
			case SND_SEQ_EVENT_CONTROLLER:
			{
				if (lastControllerUsed<128){
					controllerMapping[ev->data.control.param]=lastControllerUsed;
					mainwindow->statusBar()->showMessage(tr("Mapping complete."),1000);
					DEBUG("Mapped MIDI controller %d to internal controller %d",ev->data.control.param, int(lastControllerUsed));
					lastControllerUsed=255;
				}
				unsigned char controllerId=controllerMapping[ev->data.control.param];
				MidiEvent event2={audio->getSampleOffset(), MidiEvent::controller, { controllerId,ev->data.control.value} };
				event=event2;
				emit MIDIController(controllerId, ev->data.control.value);
			}
				break;
		}
		if (event.type){
			//DEBUG("New midi event %d",event.type);
			eventLock.lock();
			events.append(event);
			eventLock.unlock();
		}
	}
}

/**
 * @short Performs the autoconnection from current client to all midi outputs, so it works out of the box.
 *
 * Sometimes this is not welcome, but by the moment, for testing, its nice.
 */
void AlsaMidi::autoconnect(){
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;
	int count;

	// First, I get the current list
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(handle, cinfo) >= 0) {
		/* reset query info */
		snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
		snd_seq_port_info_set_port(pinfo, -1);
		count = 0;
		while (snd_seq_query_next_port(handle, pinfo) >= 0) {
			count++;
			unsigned int bits=SND_SEQ_PORT_CAP_READ;//|SND_SEQ_PORT_CAP_SUBS_WRITE;
			if (((snd_seq_port_info_get_capability(pinfo) & (bits)) == bits) &&
				(snd_seq_client_info_get_client(cinfo)!=snd_seq_client_id(handle))){
				
					/// The connection!
					// Mainly taken from ALSA documentation
					snd_seq_addr_t sender, dest;
					snd_seq_port_subscribe_t *subs;

					snd_seq_port_subscribe_alloca(&subs);
					sender.client = snd_seq_client_info_get_client(cinfo);
					sender.port = snd_seq_port_info_get_port(pinfo);
					snd_seq_port_subscribe_set_sender(subs, &sender);

					dest.client = snd_seq_client_id(handle);
					dest.port = allPort;
					snd_seq_port_subscribe_set_dest(subs, &dest);
					snd_seq_subscribe_port(handle, subs);
			}
		}
	}
}


