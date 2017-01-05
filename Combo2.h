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
	double lowGateThres;
	double highGateThres;
	double gatedGain;
};*/

struct _noiseGate {
	double lowThres;
	double highThres;
	double gain;
};

struct _trigger {
	double lowThres;
	double highThres;
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
	double gateOnThreshold = 0.01;
	double gateOffThreshold = 0.30;
	double posPeak[2];
	double negPeak[2];
	double maxAmp[2];
	double prevMaxAmp[2];
	double posPeakArray[4][2];
	double negPeakArray[4][2];
	int chan1GndCount;
	int chan2GndCount;
	double comboInputBuffer[2][BUFFER_SIZE];
	double comboOutputBuffer[2][BUFFER_SIZE];
	double inputLevel = 0.01;
	bool gateStatus;
	double envGenTriggerMultiple = 5.0;
	double triggerHighThreshold;
	double triggerLowThreshold;
	bool envTrigger;
	int envTriggerPeriods;
	double maxAmpFilter[16];
	double maxAmpFilterOut;
	double prevMaxAmpFilterOut;
	int maxAmpFilterIndex;
	int gateEnvStatus;
	double signalLevel;
	double prevSignalLevel;
	double signalLevelHighPeak;
	double signalLevelLowPeak;
	double signalDeltaFilter[16];
	double signalDeltaFilterOut;
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
	int getProcessData(int index, double *data);
	int clearProcessData(int index, double *data);
	int updateFootswitch(int footswitchStatus[]);
	int bypassAll();
	int updateProcessParameter(string processName, int parameterIndex, int parameterValue);
	int updateControlParameter(string controlName, int parameterIndex, int parameterValue);
};

#endif /* COMBO_H_ */
