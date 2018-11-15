/*
 * ProcessingControl.h
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */

#ifndef PROCESSINGCONTROL_H_
#define PROCESSINGCONTROL_H_
#include <iostream>
#include <map>
#include <fcntl.h>

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <jack/jack.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "Processing.h"

namespace std
{

class ProcessingControl
{
private:
	Processing *processing;
	int bufferSize;
	NoiseGateUtility gateUtil;
	EnvTriggerUtility triggerUtil;
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
	void stopComboProcessing();
	void enableEffects();
	void disableEffects();
	void enableAudioOutput();
	void disableAudioOutput();
	void updateProcessParameter(string parentProcess, string parameter, int parameterValue);

	void updateControlParameter(string parentControl, string parameter, int parameterValue);

	void readFootswitches(void);
	void setNoiseGateUtility(NoiseGateUtility gateTrigUtil);
	void updateNoiseGateUtility(NoiseGateUtility gateTrigUtil);
	NoiseGateUtility getNoiseGateUtility();
	void setEnvTriggerUtility(EnvTriggerUtility gateTrigUtil);
	void updateEnvTriggerUtility(EnvTriggerUtility gateTrigUtil);
	EnvTriggerUtility getEnvTriggerUtility();
	void setBufferSize(int bufferSize);
	int getBufferSize();
	void setJackUtility(JackUtility jackUtil);
	void loadSymbols();
	vector<string> getComponentSymbols();
	vector<string> getControlSymbols();
	//int processUtilityChange(PedalUtilityChange utilChange);  // "process" meaning "do something with this"
};
}


#endif /* PROCESSINGCONTROL_H_ */
