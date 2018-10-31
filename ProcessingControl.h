/*
 * ProcessingControl.h
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */

#ifndef PROCESSINGCONTROL_H_
#define PROCESSINGCONTROL_H_
#include <iostream>
#include <fcntl.h>

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <jack/jack.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "config.h"
#include "structs.h"
#include "GPIOClass.h"
#include "Processing.h"


class ProcessingControl
{
private:
	Processing *processing;
	ProcessingUtility processingUtil;
	JackUtility jackUtil;
	bool justPoweredUp;
	bool inputsSwitched;
	bool footSwitchPressed[2];
	bool footswitchStatus[FOOTSWITCH_COUNT] = {false, false};
	GPIOClass footswitchLed[2];
	GPIOClass footswitchPin[2];
public:
	ProcessingControl();
	~ProcessingControl();

	int loadComboStruct(ComboStruct comboStruct);
	ComboStruct getComboStruct();
	int startJack(void);
	int stopJack(void);
	int startComboProcessing();
	int stopComboProcessing();
	void enableEffects();
	void disableEffects();
	void enableAudioOutput();
	void disableAudioOutput();
	void updateProcessParameter(string parentProcess, string parameter, int parameterValue);

	void updateControlParameter(string parentControl, string parameter, int parameterValue);

	void readFootswitches(void);
	void setNoiseGateCloseThreshold(double closeThres);
	void setNoiseGateOpenThreshold(double openThres);
	void setNoiseGateGain(double gain);
	void setTriggerLowThreshold(double lowThres);
	void setTriggerHighThreshold(double lowThres);
	void setProcessingUtility(ProcessingUtility gateTrigUtil);
	void setJackUtility(JackUtility jackUtil);
	ProcessingUtility getProcessingUtility();

};

int loadComponentSymbols(void);
int loadControlSymbols(void);

#endif /* PROCESSINGCONTROL_H_ */
