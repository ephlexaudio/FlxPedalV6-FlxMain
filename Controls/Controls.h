/*
 * Controls.h
 *
 *  Created on: Mar 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef CONTROLS_CONTROLS_H_
#define CONTROLS_CONTROLS_H_


#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>

#include "../structs.h"

using std::array;


class Controls {
protected:
	ControlObjectData controlData;
	array<ProcessParameterControlBuffer,60> *processParamControlBufferArray;
	string name;
	int type;
	vector<int> outputToParamControlBufferIndex;
	vector<int> outputInvToParamControlBufferIndex;
	double output;
	double outputInv;
	int int_output;
	int int_outputInv;

public:
	Controls();
	Controls(ControlObjectData controlData);
	virtual ~Controls();

	string getName();
	int getType();
	void setParameter(int paramIndex, int valueIndex);
	int getParameter(int paramIndex);
	string getParameterName(int paramIndex);
	void setProcessParameterControlBufferArray(array<ProcessParameterControlBuffer,60> &procParamControlBufferArray);
	virtual void load() = 0;
	virtual void run(int controlVoltageIndex, bool envTrigger) = 0;
	virtual void stop() = 0;
};

class Normal : public Controls {
private:

public:
	Normal();
	Normal(ControlObjectData controlData);
	~Normal();

	void load();
	void run(int controlVoltageIndex, bool envTrigger);
	void stop();
};



class LFO : public Controls
{
private:
	int  cycleTimeValueIndex;
	double cyclePositionValue;
	int int_cyclePositionValue;
	double waveValue;

public:
	LFO();
	LFO(ControlObjectData controlData);
	~LFO();

	void load();
	void run(int controlVoltageIndex, bool envTrigger);
	void stop();
};

class EnvGen : public Controls
{
private:
	int envStage; //0:attack, 1:decay, 2:sustain, 3:release
	int stageTimeValue;
	double slewRate;
	bool envTrigger;
public:
	EnvGen();
	EnvGen(ControlObjectData controlData);
	~EnvGen();

	void load();

	void run(int controlVoltageIndex, bool envTrigger);
	void stop();
};




#endif /* CONTROLS_CONTROLS_H_ */
