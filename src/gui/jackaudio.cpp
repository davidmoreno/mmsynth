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


#include <mdebug.h>
#include <midievent.h>
#include <synth.h>
#include <QMutexLocker>

#include "midi.h"
#include "jackaudio.h"

int jack_callback(jack_nframes_t nframes, void *jack){
	return ((JackAudio*)jack)->processData(nframes);
}


JackAudio::JackAudio(QObject* parent): Audio(parent){
	jack=NULL;
	DEBUG("Added jack object");
}


JackAudio::~JackAudio(){
	if (jack){
		jack_client_close(jack);
		
		delete[] synthaudio[0];
		delete[] synthaudio[1];
	}
	DEBUG("Removed jack object");
}


bool JackAudio::init(){
	DEBUG("Initializing Jack audio");
	jack_status_t status;
	
	jack=jack_client_open("mmsynth", JackNullOption, &status);
	if (jack==NULL || status&JackInitFailure){
		ERROR("Error initializing jack. Failing miserablily");
		if (jack)
			jack_client_close(jack);
		return false;
	}
	
	jack_set_process_callback(jack, jack_callback, (void*)this);
	outputR = jack_port_register (jack, "right", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	outputL = jack_port_register (jack, "left", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	int buffsize=jack_get_buffer_size(jack);
	synthaudio[0]=new float[buffsize];
	synthaudio[1]=new float[buffsize];
	blockSize=buffsize;
	
	_samplerate=jack_get_sample_rate(jack);
	
	
	gettimeofday(&lastGetAudioTime,NULL);
	jack_activate(jack);

	const char **ports;

	
	// Connect to all physical output ports... so it sounds for sure. 
	if ((ports = jack_get_ports (jack, NULL, NULL, JackPortIsPhysical|JackPortIsInput)) == NULL) {
		WARNING("Jack cannot find any physical playback ports");
		return true;
	}
	
	int n=0;
	while (ports[n]!=NULL){
		if (jack_connect (jack, jack_port_name ((n%2) ? outputR : outputL), ports[n])) {
			WARNING("Jack cannot connect output port %d",n);
		}
		n++;
	}

	free (ports);
	
	DEBUG("Jack initialized: %f samplerate, %d buffer size (%f latency)", _samplerate, buffsize, float(buffsize)/_samplerate);

	return true;
}

int JackAudio::processData(jack_nframes_t nframes){
	jack_default_audio_sample_t *outR = (jack_default_audio_sample_t *) jack_port_get_buffer (outputR, nframes);
	jack_default_audio_sample_t *outL = (jack_default_audio_sample_t *) jack_port_get_buffer (outputL, nframes);

	memset(outL,0,sizeof(outL[0])*nframes);
	memset(outR,0,sizeof(outR[0])*nframes);
	// If no synths, just silence.
	QMutexLocker l(&synthlock);
	if (synths.count()==0){
		return 0;
	}
	
	float gain=1.0/synths.count();
	
	MidiEventList events=midi ? midi->getEvents() : MidiEventList();
	jack_nframes_t i;
	foreach(Synth *synth, synths){
		synth->getAudio(synthaudio, nframes, events );
		
		jack_default_audio_sample_t *outRt=outR;
		jack_default_audio_sample_t *outLt=outL;
		
		for(i=0;i<nframes;i++){
			*outRt++=synthaudio[0][i]*gain;
			*outLt++=synthaudio[1][i]*gain;
		}
		
	}

	gettimeofday(&lastGetAudioTime,NULL); // start counting samples again.. more or less.
	
	return 0;
}


/**
 * @short Current sample offset calculated since last buffer commit start
 */
unsigned int JackAudio::getSampleOffset(){
	struct timeval current;
	gettimeofday(&current,NULL);
	//DEBUG("Current %d + %d",current.tv_sec,current.tv_usec);
	//DEBUG("Last %d + %d",lastGetAudioTime.tv_sec,lastGetAudioTime.tv_usec);
	
	float usec=((current.tv_sec-lastGetAudioTime.tv_sec)*1000000.0)+(current.tv_usec-lastGetAudioTime.tv_usec);
	unsigned int soffset=int((usec/1000000.0)*_samplerate);
	if (soffset>=blockSize){
		//WARNING("Set the event after the end of offset, moving it so its performed. Maybe because of an xrun, timing will not be perfect.");
		soffset=blockSize-1;
	}
	//DEBUG("Sample offset is %d, blockSize is %d, elapsed %f secs (%f %%)",soffset,blockSize, usec/1e6f, soffset*100.0/blockSize);
	return soffset;
}

