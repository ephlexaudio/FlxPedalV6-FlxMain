/*
 * Combo.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#ifndef COMBO_H_
#define COMBO_H_

#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>
#include <jack/jack.h>
//#include "Process.h"
#include "Effects2.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"
#include "jackaudioio.h"
#define BUFFER_SIZE 1024

/*typedef struct _processingParams{
	float lowGateThres;
	float highGateThres;
	float gatedGain;
};*/

struct _noiseGate {
	float lowThres;
	float highThres;
	float gain;
};

struct _trigger {
	float lowThres;
	float highThres;
};

struct _processingParams{
	struct _noiseGate noiseGate;
	struct _trigger trigger;
};



class Combo: public JackCpp::AudioIO {
private:
	//Process *process[20];

	ProcessBuffer outProcBuffer;
	int switchedStatus = 0;
	float gateOnThreshold = 0.01;
	float gateOffThreshold = 0.30;
	float posPeak[2];
	float negPeak[2];
	float maxAmp[2];
	float prevMaxAmp[2];
	float posPeakArray[4][2];
	float negPeakArray[4][2];
	int chan1GndCount;
	int chan2GndCount;
	float comboInputBuffer[2][BUFFER_SIZE];
	float comboOutputBuffer[2][BUFFER_SIZE];
	float inputLevel = 0.01;
	bool gateStatus;
	float envGenTriggerMultiple = 5.0;
	float triggerHighThreshold;
	float triggerLowThreshold;
	bool envTrigger;
	int envTriggerPeriods;
	float maxAmpFilter[16];
	float maxAmpFilterOut;
	float prevMaxAmpFilterOut;
	int maxAmpFilterIndex;
	int gateEnvStatus;
	float signalLevel;
	float prevSignalLevel;
	float signalLevelHighPeak;
	float signalLevelLowPeak;
	float signalDeltaFilter[16];
	float signalDeltaFilterOut;
	int signalDeltaFilterIndex;
	int signalDeltaPositiveCount;
	int signalDeltaNegativeCount;
	int aveArrayIndex;

public:
	/*struct */ProcessEvent processSequence[20]; // do these 5 variables/structs need to be public ??
	ControlEvent controlSequence[20];
	ProcessBuffer procBufferArray[60];
	int footswitchStatus[10];
	int inputProcBufferIndex[2];
	int outputProcBufferIndex[2];
	int processCount;
	int controlCount;
	int bufferCount;
	Combo();
	~Combo();

	/*int setCheckInputs();
	int clearCheckInputs();*/
	bool areInputsSwitched(); // needs to be public to access sysfs (?)

	int initProcBufferArray(struct ProcessBuffer *bufferArray, vector<Json::Value> connectionsJson);
	int loadEffects();
	//int start();
	//void stop();
	int stopEffects();
	int audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs);
	int getProcessData(int index, float *data);
	int clearProcessData(int index, float *data);
	int updateFootswitch(int footswitchStatus[]);
	int bypassAll();
	int updateProcessParameter(string processName, int parameterIndex, int parameterValue);
	int updateControlParameter(string controlName, int parameterIndex, int parameterValue);
};

#endif /* COMBO_H_ */
