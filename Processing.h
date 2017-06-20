/*
 * Combo.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_

#include <iostream>
#include "config.h"
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
#include "Controls.h"
#include "Effects2.h"
//#include "Combo.h"
#include "structs.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"
#include "jackaudioio.h"



extern ComboDataInt comboData[15];
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


class Processing: public JackCpp::AudioIO {
private:
	//Combo *combo;

	//ProcessBuffer outProcBuffer;
	int switchedStatus = 0;
	double gateOnThreshold = 0.01;
	double gateOffThreshold = 0.30;
	double inPosPeak[2];
	double inNegPeak[2];
	double inMaxAmp[2];
	double inPrevMaxAmp[2];
	double inPosPeakArray[4][2];
	double inNegPeakArray[4][2];
	int chan1GndCount;
	int chan2GndCount;
	//double comboInputBuffer[2][BUFFER_SIZE];
	//double comboOutputBuffer[2][BUFFER_SIZE];
	double inputLevel = 0.01;
	bool gateStatus;
	double envGenTriggerMultiple = 5.0;
	double triggerHighThreshold;
	double triggerLowThreshold;
	bool envTrigger;
	int envTriggerPeriods;
	double inMaxAmpFilter[16];
	double inMaxAmpFilterOut;
	double inPrevMaxAmpFilterOut;
	int inMaxAmpFilterIndex;
	int gateEnvStatus;
	double inSignalLevel;
	double inPrevSignalLevel;
	double inSignalLevelHighPeak;
	double inSignalLevelLowPeak;
	double inSignalDeltaFilter[16];
	double inSignalDeltaFilterOut;
	int inSignalDeltaFilterIndex;
	int inSignalDeltaPositiveCount;
	int inSignalDeltaNegativeCount;
	int aveArrayIndex;
	double outPosPeak[2];
	double outNegPeak[2];
	double outMaxAmp[2][2];
	double outGain;
	bool audioCallbackRunning;
	bool processingEnabled;

public:
	Processing();
	~Processing();
	int comboIndex;
	string comboName;
	int footswitchStatus[10];
	bool processingUpdated;
	bool updateProcessing;
	bool processingContextAllocationError;
	/*int setCheckInputs();
	int clearCheckInputs();*/
	bool areInputsSwitched(); // needs to be public to access sysfs (?)

	//int initProcBufferArray(struct ProcessBuffer *bufferArray, vector<Json::Value> connectionsJson);
	int loadCombo(int comboIndex);
	int loadCombo(void);
	int enableComboBypass();
	int disableComboBypass();
	//int start();
	//void stop();
	int stopCombo(int loadComboIndex);
	int stopCombo(void);
	int audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs);
	int getProcessData(int index, double *data);
	int getPitch(bool activate, double *signal);
	int clearProcessData(int index, double *data);
	int updateFootswitch(int footswitchStatus[]);
	int enableProcessing();
	int disableProcessing();
	int bypassAll();
	int updateProcessParameter(string processName, int parameterIndex, int parameterValue);
	int updateControlParameter(string controlName, int parameterIndex, int parameterValue);
	double getOutputAmplitudes(void);
};

int loadComponentSymbols(void);
int loadControlTypeSymbols(void);
#endif /* PROCESSING_H_ */
