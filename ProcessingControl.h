/*
 * ProcessingControl.h
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

#include "GPIOClass.h"
#include "ComboDataInt.h"
#include "Processing.h"


class ProcessingControl
{
private:
	Processing *processing;
	bool footSwitchPressed[2];
	bool footswitchStatus[FOOTSWITCH_COUNT] = {false, false};

public:
	ProcessingControl();
	~ProcessingControl();

	GPIOClass footswitchLed[2];
	GPIOClass footswitchPin[2];
	int load(string comboName);
	int start();
	int stop();
	int enableEffects();
	int disableEffects();
	int enableAudioInput();
	int disableAudioInput();
	int enableAudioOutput();
	int disableAudioOutput();
	int updateProcessParameter(int parameterIndex, int parameterValue);
	int updateControlParameter(int parameterIndex, int parameterValue);

	int readFootswitches(void);
	int setNoiseGateCloseThreshold(float closeThres);
	int setNoiseGateOpenThreshold(float openThres);
	int setNoiseGateGain(float gain);
	int setTriggerLowThreshold(float lowThres);
	int setTriggerHighThreshold(float lowThres);
};

#endif /* PROCESSINGCONTROL_H_ */
