/*
 * Processing.h
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */

#ifndef PROCESSINGCONTROL_H_
#define PROCESSINGCONTROL_H_
#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <jack/jack.h>
#include "config.h"

//#include "Process.h"
//#include "Effects.h"
#include "GPIOClass.h"
#include "ComboDataInt.h"
#include "Processing.h"


class ProcessingControl
{
private:
	Processing *processing;
	//bool inputsSwitched;
	//double inputLevel[4][2];
	bool footSwitchPressed[2];
	bool footswitchStatus[FOOTSWITCH_COUNT] = {false, false};

public:
	//GPIOClass portConSwitch[3];
	GPIOClass footswitchLed[2];
	GPIOClass footswitchPin[2];
	ProcessingControl();
	~ProcessingControl();
	// portConSwitch and areInputsSwitched have to be public to access sysfs (?)
	//bool areInputsSwitched(void); // see if JACK has connected input jacks backwards

	/*int load(vector<Process> processesStruct, vector<Json::Value> connectionsJson,
			vector<Control> controlsStruct, vector<ControlConnection> controlConnectionsStruct);*/
	//int load(int comboIndex);
	int load(string comboName);
	int start();
	int stop();
	int getProcessData(int index, double *data);
	//int clearProcessData(int index, double *data);
	int enableEffects();
	int disableEffects();
	int enableAudioOutput();
	int disableAudioOutput();
	int updateProcessParameter(int parameterIndex, int parameterValue);
	int updateControlParameter(int parameterIndex, int parameterValue);
	double getOutputAmplitudes(void);

	int readFootswitches(void);
	int setNoiseGateCloseThreshold(float closeThres);
	int setNoiseGateOpenThreshold(float openThres);
	int setNoiseGateGain(float gain);
	int setTriggerLowThreshold(float lowThres);
	int setTriggerHighThreshold(float lowThres);

	//int updateFootswitch(int *footswitchStatus);
};

#endif /* PROCESSINGCONTROL_H_ */
