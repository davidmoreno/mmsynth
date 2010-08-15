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

#include <QStringList>
#include <QStringListIterator>

#include <math.h>

#include <synth.h>

#include "midi.h"
#include "alsaaudio.h"
#include "mdebug.h"

AlsaAudio::AlsaAudio(const QString &options, QObject *parent) : Audio(parent){
	periods=2;
	frames=1024;
	device="default";
	
	if (!options.isEmpty()){
		DEBUG("Options %s",options.toAscii().constData());
		
		QString opt;
		QStringList optlist=options.split(",");
		QStringListIterator I(optlist);
		while(I.hasNext()){
			DEBUG("Option %s",opt.toAscii().constData());
			opt=I.next();
			QString key=opt.section("=",0,0);
			QString value=opt.section("=",1);
			if (key=="periods"){
				periods=value.toInt();
				DEBUG("Set periods to %d",periods);
			}
			else if (key=="frames"){
				frames=value.toInt();
				DEBUG("Set fames to %d",periods);
			}
			else if (key=="device"){
				device=value;
				if (I.hasNext()){
					value=I.next();
					if (value.indexOf("=")>0)
						I.previous();
					else
						device+=","+value;
				}
				
				DEBUG("Set device to %s",device.toAscii().constData());
			}
			else{
				WARNING("Unknown alsa audio option: %s=%s",key.toAscii().constData(), value.toAscii().constData());
			}
		}
	}
	pcm=NULL;
}

/**
 * @short Initializes the alsa PCM, with all the default parameters.
 */
bool AlsaAudio::init(){
	int err;
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_stream_t stream=SND_PCM_STREAM_PLAYBACK;
	
	int nbuffers=periods;
	
	err=snd_pcm_open(&pcm,device.toAscii().constData(), stream, 0);
	if (err<0){ WARNING("Cant open audio device. Set with --audio=alsa,device=hw:0");  return false; }
	
	err=snd_pcm_hw_params_malloc(&hwparams);
	if (err<0){ WARNING("Cant malloc hw parameters memory");  return false; }

	err=snd_pcm_hw_params_any(pcm, hwparams)<0;
	if (err){ WARNING("Cant set any hw parameters"); return false; }

	err=snd_pcm_hw_params_set_access(pcm,hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)<0;
	if (err){ WARNING("Cant set hw parameters"); return false; }
	err|= snd_pcm_hw_params_set_format (pcm, hwparams, SND_PCM_FORMAT_S16)<0;
	if (err){ WARNING("Cant set hw parameters"); return false; }
	unsigned int rate=44100;
	int dir=0;
	err|= snd_pcm_hw_params_set_rate_near(pcm, hwparams, &rate, &dir)<0;
	_samplerate=rate;
	if (err){ WARNING("Cant set hw parameters"); return false; }
	err|= snd_pcm_hw_params_set_channels(pcm, hwparams, 2)<0;
	if (err){ WARNING("Cant set hw parameters"); return false; }
	err|=snd_pcm_hw_params_set_periods(pcm, hwparams, nbuffers, 0)<0;
	if (err){ WARNING("Cant set hw parameters, bad period number: %d, set with --audio=alsa,periods=3",nbuffers); return false; }
	snd_pcm_uframes_t minframes=0;
	snd_pcm_uframes_t maxframes=0;
	err|=snd_pcm_hw_params_get_buffer_size_min(hwparams, &minframes )<0;
	if (err){ WARNING("Cant set hw parameters"); return false; }
	err|=snd_pcm_hw_params_get_buffer_size_max(hwparams, &maxframes )<0;
	if (err){ WARNING("Cant set hw parameters"); return false; }

	if (minframes==maxframes && minframes!=frames){
		WARNING("With this period size you can only have %ld frames, setting it automatically", minframes);
		frames=minframes;
	}

	if (frames<minframes || frames>maxframes){
		WARNING("Selected frames (%d) not allowed, should be between %ld and %ld. Select with --audio=alsa,frames=%ld. Setting to minimum.",
					frames, minframes, maxframes, minframes);
		frames=minframes;
	}


	err|=snd_pcm_hw_params_set_buffer_size(pcm, hwparams, frames);
	if (err){ WARNING("Cant set hw parameters"); return false; }

	err|= snd_pcm_hw_params (pcm, hwparams)<0;

	unsigned int channels;
	snd_pcm_hw_params_get_channels(hwparams, &channels);
	DEBUG("%d channels",channels);
	
	snd_pcm_hw_params_free(hwparams);
	if (err){ WARNING("Cant set hw parameters"); return false; }


	err=snd_pcm_prepare(pcm);
	if (err<0){ WARNING("Cant set hw parameters on real hw"); return false; }

	blockSize=frames;
	
	float latency=(nbuffers*blockSize/_samplerate);
	
	DEBUG("Block size %d samples, %f secs of latency",blockSize,latency);
	// Initialize processing!
	wantToQuit=false;
	gettimeofday(&lastGetAudioTime,NULL);
	start();
	
	return true;
}

/// Close the PCM
AlsaAudio::~AlsaAudio(){
	wantToQuit=true;
	while(wantToQuit){ msleep(100); }
	if (pcm)
		snd_pcm_close(pcm);
	DEBUG("Closed alsa audio");
}

int mmin(int a, int b){
	if (a<b) return a;
	return b;
}

/**
 * @short Thread that processes the synth audio, and sends it to the soundcard.
 */
void AlsaAudio::run(){
	float *synthaudio[2];
	short *finalaudio;
	short *taudio;
	synthaudio[0]=new float[blockSize];
	synthaudio[1]=new float[blockSize];

	finalaudio=new short[blockSize*2];

	unsigned int i;
	int m=0;
	while(!wantToQuit){
		synthlock.lock();
		if (synths.count())
			m=0x7FFF/synths.count();
		
		memset(finalaudio,0,blockSize*sizeof(finalaudio[0])*2);
		MidiEventList events=midi ? midi->getEvents() : MidiEventList();
		foreach(Synth *synth, synths){
			synth->getAudio(synthaudio, blockSize, events );
			
			taudio=finalaudio;
			
			for(i=0;i<blockSize;i++){
				*taudio++=synthaudio[0][i]*m;
				*taudio++=synthaudio[1][i]*m;
			}
		}
		synthlock.unlock();
		gettimeofday(&lastGetAudioTime,NULL);
		int pendingwrite=blockSize;
		while (pendingwrite){
			int wrote=snd_pcm_writei(pcm, &finalaudio[(blockSize-pendingwrite)*2], pendingwrite);
			//DEBUG("Wrote %d samples, asked %d",wrote,mmin(pendingwrite,blockSize/100));
			if (wrote==-EPIPE){
				WARNING("Buffer underrun. Please try setting higher buffer size.");
				snd_pcm_prepare(pcm);
				pendingwrite=0;
				//DEBUG("Wrote %d samples, asked %d, pending %d",wrote,mmin(pendingwrite,blockSize/10), pendingwrite);
			}
			else if (wrote<0){
				ERROR("Error writing to buffer");
				perror("");
				wantToQuit=true;
			}
			else{
				if (wrote<pendingwrite){
					//DEBUG("Wrote less, retrying with %d frames",pendingwrite-wrote);
				}
				pendingwrite-=wrote;
				//DEBUG("%d pending",pendingwrite);
			}
		}
		//DEBUG("Block wrote");
	}
	
	delete[] synthaudio[0];
	delete[] synthaudio[1];

	delete[] finalaudio;
	
	wantToQuit=false;
	DEBUG("Exit from alsa loop");
}

/**
 * @short Current sample offset calculated since last buffer commit start
 */
unsigned int AlsaAudio::getSampleOffset(){
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

