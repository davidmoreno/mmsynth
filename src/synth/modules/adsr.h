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

#ifndef ADSR_H
#define ADSR_H

#include <QList>

class Synth;

/**
 * @short Basic ADSR envelope parameters
 */
struct ADSRParams{
public:
	ADSRParams(){
		a=0.1;
		d=0.1;
		s=1.0;
		r=0.1;
		sl=0.5;
	}
	float a;
	float d;
	float s;
	float sl;
	float r;
};

class ADSR{
public:
	ADSR(Synth *synth, ADSRParams *_params=NULL);
	~ADSR();
	
	void process(float *inout, unsigned int nsamples);

	void noteOn();
	void noteOff();
	
	void setAttack(float a){ params->a=a; }
	void setDecay(float a){ params->d=a; }
	void setSustain(float a){ params->s=a; }
	void setSustainLevel(float a){ params->sl=a; }
	void setRelease(float a){ params->r=a; }

	float attack(){ return params->a; }
	float decay(){ return params->d; }
	float sustain(){ return params->s; }
	float sustainLevel(){ return params->sl; }
	float release(){ return params->r; }

	bool hasSound(){ return _phase!=0; }

	char phase(){ return _phase; }
	float level(){ return currentLevel; }
	void panic(){ _phase=0; currentLevel=0.0; } 
protected:
	ADSRParams *params;
	Synth *synth;
	
	float time;
	float currentLevel;
	char _phase; // 0 none, 1 A, 2 D, 3 S, 4 R.
	bool myparams;  // true if params is mine, and should be deleted 
};


class ADSRGroup{
public:
	ADSRGroup(unsigned int nvoices, Synth *_synth);
	~ADSRGroup();

	ADSR *operator[](unsigned int i){ return list.at(i); }
	
	int getFreeVoice();
	
	void setAttack(float a){ params.a=a; }
	void setDecay(float a){ params.d=a; }
	void setSustain(float a){ params.s=a; }
	void setSustainLevel(float a){ params.sl=a; }
	void setRelease(float a){ params.r=a; }

	float attack(){ return params.a; }
	float decay(){ return params.d; }
	float sustain(){ return params.s; }
	float sustainLevel(){ return params.sl; }
	float release(){ return params.r; }

protected:
	QList<ADSR*> list;
	ADSRParams params;
};

#endif
