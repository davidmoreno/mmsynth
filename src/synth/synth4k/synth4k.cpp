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

#include "synth4k.h"

//MSYNTH_REGISTER("synth4k",Synth4k);

Synth *create_synth_synth4k(QObject *parent){
	return new Synth4k(parent);
}

#include <malloc.h>
#include <math.h>

#define WITH_GLIDE 1

/**************************/
typedef struct mzk_Oscillator{
#ifdef WITH_GLIDE
  float glide;
  float gtime;
  float ifreq;
#endif
  float phase;
  float freq;
}mzk_Oscillator;
typedef struct mzk_Envelope{
  float attack;
  float decay;
  float sustain;
  float release;
  float level;
  float time;
  char  phase;
}mzk_Envelope;
typedef struct mzk_Filter{
  float low;
  float band;
  float freq;
  float q;
  float dist;
}mzk_Filter;
#define LOW 0
#define BAND 1
typedef struct mzk_Tom{
  float phase;
  float freq;
  float speed;
}mzk_Tom;
typedef struct mzk_Bass{
  float flfo1;
  float flfo2;
  float flfo3;
  mzk_Envelope env;
  mzk_Filter filter;
  mzk_Oscillator osc;
  float lfo1;
  float lfo2;
  float lfo3;
}mzk_Bass;
typedef struct mzk_Charles{
  mzk_Envelope env;
  float time_left;
}mzk_Charles;



#ifdef WITH_GLIDE
#ifdef USE_BASS
static mzk_Bass bass={0.33f, 0.20f, 0.11f,  // flfos
		      {0.3f, 0.1f, 0.3f, 0.01f, 0.6}, // env
		      {0.0f, 0.0f},{0.3f}}; // filter, osc(glide)
#endif

static mzk_Bass solo={0.2f, 0.3f, 0.0f, 
		      {0.8f, 0.1f, 0.4f, 0.1f, 0.8,0.0,0},
		      {0.0f, 0.0f,0.0,0.0,0.0},{0.05f,0.0,0.0,0.0,0.0},0.0,0.0,0.0};

#else
#ifdef USE_BASS
static mzk_Bass bass={0.33f, 0.20f, 0.11f, 
		      {0.3f, 0.1f, 0.3f, 0.05f, 0.6},
		      {0.0f, 0.0f}};
#endif
static mzk_Bass solo={0.1f, 0.3f, 0.0f, 
		      {0.8f, 0.1f, 0.8f, 0.8f, 0.8},
		      {0.0f, 0.0f}};
#endif
#ifdef USE_BASS
static mzk_Tom tom={0.0f, 0.0f, 1.0e3f};
static mzk_Tom colchon={0.0f, 0.0f, 7.0e1f};
static mzk_Charles charles={{0.005f, 0.01f, 0.03f, 0.001f, 0.7f}, 0.0f};

static mzk_Bass *pbass=&bass;
static mzk_Bass *psolo=&solo;
static mzk_Tom *ptom=&tom;
static mzk_Tom *pcolchon=&colchon;
//static mzk_Charles *pcharles=&charles;

static float do_tom(mzk_Tom *tom);
static float do_charles(mzk_Charles *charles);
#endif

static float do_bass(mzk_Bass *bass, float samplerate);
static float do_filter(float in, mzk_Filter *filter, float samplerate);
static float do_envelope(mzk_Envelope *env, float samplerate);
static void end_envelope(mzk_Envelope *env);

static void start_envelope(mzk_Envelope *env);

#define SINE(x) (sin(x*2.0*M_PI))
#define SAW(x) (fmod((x*2.0)-1.0, 1.0))
#define NOISE(x) (frand())
#define SQUARE(x) (((x<0.5)*2)-1)
#define SAW2(x) (SAW(x)*0.26+SAW(x*2.0f)*0.21+SQUARE(x*4.0f)*0.52)

#define FMOD fmod

#include <QMutexLocker>


Synth4k::Synth4k(QObject *parent) : Synth(parent){
	DEBUG("Created synth4k");
	volume=1.0;
	mysynth=new char[sizeof(::solo)];
	memcpy(mysynth,(char*)&::solo,sizeof(::solo));
	
	receiveNoteOn(60);
}

Synth4k::~Synth4k()
{
	delete[] mysynth;
}


void Synth4k::getAudio(float *audio[2], unsigned int nsamples, const MidiEventList &list){
	unsigned int i;
	float s;
	int nevent=0;
	
	for (i=0;i<nsamples;i++){
		while (list.count()>nevent && list[nevent].sampleOffset<=i){
			//DEBUG("Processing event %d at sample %d, %f %%",nevent,i,i*100.0/nsamples);
			switch(list[nevent].type){
				case MidiEvent::controller:
					receiveController(list[nevent].data[0],list[nevent].data[1]);
					break;
				case MidiEvent::noteOn:
					receiveNoteOn(list[nevent].data[0]);
					break;
				case MidiEvent::noteOff:
					receiveNoteOff(list[nevent].data[0]);
					break;
				default:
					break;
			}
			nevent++;
		}
		s=do_bass((mzk_Bass*)mysynth,_samplerate)*volume;
		if (s>1.0 || s<-1.0){
			WARNING("Lowering volume %f to %f, clipped at %f",volume, volume*0.95,s);
			if (s<0.0)
				s=-1.0;
			else
				s=1.0;
			volume*=0.95;
			emit sendController(9, int(sqrt(volume)*127/2.4) ); // it is enqueued for another thread
		}
		audio[0][i]=s;
		audio[1][i]=s;
	}
	if (nevent<list.count())
		WARNING("Some events left, with offset over block size: %d>%d",list[nevent].sampleOffset,i);
}

void Synth4k::sendStatus(){
	DEBUG("Sending current status");
	
	static mzk_Bass *self=(mzk_Bass*)mysynth;

	emit sendController(0, int(sqrt(self->flfo1)*127/5.47) );
	emit sendController(1, int(sqrt(self->flfo2)*127/5.47) );
	emit sendController(2, int(sqrt(self->flfo3)*127/5.47) );
	emit sendController(3, int(sqrt(self->env.attack)*127/3.0) );
	emit sendController(4, int(sqrt(self->env.decay)*127/3.0) );
	emit sendController(5, int(sqrt(self->env.sustain)*127/3.0) );
	emit sendController(6, int(sqrt(self->env.release)*127/3.0) );
	emit sendController(7, int(self->env.level*127) );
	emit sendController(8, int(sqrt(self->osc.glide)*127/1.4) );
	emit sendController(9, int(sqrt(volume)*127/2.4) );
}

void Synth4k::receiveNoteOn(unsigned char note){
	static mzk_Bass *self=(mzk_Bass*)mysynth;
	
	lastnote=note;

	self->osc.ifreq=self->osc.freq;
	self->osc.gtime=0.0f;
	self->osc.freq=freqOfNote(lastnote);
	if (self->osc.ifreq>self->osc.freq)
	  self->osc.ifreq=self->osc.freq;
	start_envelope(&self->env);
}

void Synth4k::receiveNoteOff(unsigned char note){
	if (note!=lastnote)
		return;
	static mzk_Bass *self=(mzk_Bass*)mysynth;

	end_envelope(&self->env);
}

/**
 * @short Synth4k controllers
 *
 * The controllers are
 *
 * * 0 - 2 -- Freq of the 3 LFOs, its exponential 0-30
 * * 3 - 7 -- Envelope (attack, decay, sustain, release, sustain level) 
 * * 8     -- Glide time 0-2
 * 
 */
void Synth4k::receiveController(unsigned char controllerId, unsigned char value){
	float fvalue=value/127.0;
	static mzk_Bass *self=(mzk_Bass*)mysynth;

	switch(controllerId){
		case 0:
			self->flfo1=(fvalue*5.47)*(fvalue*5.47);
			DEBUG("flfo1 %f",self->flfo1);
			break;
		case 1:
			self->flfo2=(fvalue*5.47)*(fvalue*5.47);
			DEBUG("flfo2 %f",self->flfo2);
			break;
		case 2:
			self->flfo3=(fvalue*5.47)*(fvalue*5.47);
			DEBUG("flfo3 %f",self->flfo3);
			break;
		case 3:
			self->env.attack=(fvalue*5.47)*(fvalue*3.0);
			DEBUG("attack %f",self->env.attack);
			break;
		case 4:
			self->env.decay=(fvalue*5.47)*(fvalue*3.00);
			DEBUG("decay %f",self->env.decay);
			break;
		case 5:
			self->env.sustain=(fvalue*5.47)*(fvalue*3.00);
			DEBUG("sustain %f",self->env.sustain);
			break;
		case 6:
			self->env.release=(fvalue*5.47)*(fvalue*3.00);
			DEBUG("release %f",self->env.release);
			break;
		case 7:
			self->env.level=(value/127.0);
			DEBUG("level %f",self->env.level);
			break;
		case 8:
			self->osc.glide=(fvalue*1.4)*(fvalue*1.4);
			DEBUG("glide %f",self->osc.glide);
			break;
		case 9:
			volume=(fvalue*2.4)*(fvalue*2.4);
			DEBUG("general volume %f",volume);
			break;
	}
}


/************************************************************/
/// @{ @name Real synth code, got straigth from 4kwave     
/************************************************************/


static float mmin(float a, float b){
	return (a<b) ? a : b;
}
#if USE_BASS
static float frand(){
	return ((rand()-(RAND_MAX/2))/(RAND_MAX/2.0));
}
static float do_tom(mzk_Tom *tom){
	if ((tom->freq<=10.0f)&&(tom->phase<=0.001)){
		tom->freq=tom->phase=0.0f;
	}
	else{
		tom->phase=FMOD(tom->phase+(tom->freq/MZK_SAMPLERATE), 1.0f);
		tom->freq-=tom->speed/MZK_SAMPLERATE;
	}
	return SINE(tom->phase);
}
#endif

static float do_filter(float in, mzk_Filter *filter, float samplerate){
	float ret, notch, high, freq, damp;
	register int i;

	freq=mmin(0.25f, 2.0f*sin(M_PI*(filter->freq)/(samplerate*2.0f)));
	damp=mmin(2.0f*(1.0f - pow(filter->q, 0.25f)), mmin(2.0f, 2.0f/freq - freq*0.5f));

	for (i=0;i<3;i++){
		notch=(in)-(damp*filter->band);
		filter->low=filter->low + (freq*filter->band);
		high=notch-filter->low;
		filter->band  = freq*high + filter->band - (filter->dist)*filter->band*filter->band*filter->band;
		ret=0.5f*(filter->low);
	}

	if (isnan(filter->band))
		filter->band=0.0f;
	if (isnan(filter->low))
		filter->low=0.0f;

	return ret;
}

static float lfo(float *lfo, float freq, float range, float mid, float samplerate){
	*lfo=fmod(*lfo + (freq/samplerate), 1.0f);
	return SINE(*lfo)*range + mid;
}

static float do_bass(mzk_Bass *bass, float samplerate){
	register float ret;
	#ifdef WITH_GLIDE
	register float tfreq=(bass->osc.gtime>=bass->osc.glide) ? bass->osc.freq
							: bass->osc.ifreq+(((bass->osc.freq-bass->osc.ifreq)*bass->osc.gtime)/(bass->osc.glide));
	bass->osc.gtime+=1.0f/samplerate;
	bass->osc.phase=fmod(bass->osc.phase+(tfreq/samplerate), 1.0f);
	#else
	bass->osc.phase=fmod(bass->osc.phase+(bass->osc.freq/samplerate), 1.0f);
	#endif

	bass->filter.freq=lfo((&bass->lfo1), bass->flfo1, 400.0f, 700.0f, samplerate);
	bass->filter.q=lfo(&(bass->lfo2), bass->flfo2, 0.3f, 0.6f, samplerate);
	bass->filter.dist=SINE(bass->lfo2)*0.2f+ 0.4f;

	ret=SAW2(bass->osc.phase);

	if (bass->flfo3!=0.0f){
		if (ret<=lfo(&(bass->lfo3), bass->flfo3, 0.7f, 1.0f, samplerate))
			ret=SINE(ret);
	}
	ret=do_filter(ret*do_envelope(&bass->env, samplerate), &(bass->filter), samplerate);

	return ret;
}

static void start_envelope(mzk_Envelope *env){
	env->time=(env->phase==0) ? 0.0f : env->level*env->attack;
	env->phase=1;
}

static void end_envelope(mzk_Envelope *env){
	if (env->phase && (env->phase<4)){
		env->time=0.0f;
		env->phase=4;
	}
}

static float do_envelope(mzk_Envelope *env, float samplerate){
	if (env->phase!=0){
		float ret=0.0f;
		env->time+=1.0f/samplerate;
		switch(env->phase){
			case 3: /* Most probable, sustain */
				ret=env->level;
				if (env->time>=env->sustain){
					env->phase=4; env->time=0.0f;
				}
				break;
			case 1: /* attack */
				ret=env->time/env->attack;
				if (env->time>=env->attack){
					env->phase=2; env->time=0.0f;
				}
				break;
			case 2: /* decay */
				ret=1.0f-((1.0f-env->level)*(env->time/env->decay));
				if (env->time>=env->decay){
					env->phase=3; env->time=0.0f;
				}
				break;
			case 4: // release 
				ret=env->level-(env->level*(env->time/env->release));
				if (env->time>=env->release)
					env->phase=0;
				break;
		}
		if (isnan(ret)) // division by 0 or similar
			ret=0.0f;
		return ret;
	}
	else
		return 0.0f;
}

/// @}
