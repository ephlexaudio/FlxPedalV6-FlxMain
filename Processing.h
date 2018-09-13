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
#include "GPIOClass.h"
#include "Controls.h"
#include "structs.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"
#include "jackaudioio.h"
#include "Processes.h"



class Processing: public JackCpp::AudioIO {
private:
	int bufferSize = 256;
	int switchedStatus = 0;
	float gateCloseThreshold = 0.01;
	float gateOpenThreshold = 0.30;
	float gateClosedGain = 0.002;
	float gateGain;
	double noiseGateBuffer[2][256];
	double inPosPeak[2];
	double inNegPeak[2];
	double inMaxAmp[2];
	double inPrevMaxAmp[2];
	double inPosPeakArray[4][2];
	double inNegPeakArray[4][2];
	int chan1GndCount;
	int chan2GndCount;
	double inputGain = 0.01;
	bool gateOpen;
	double envGenTriggerMultiple = 5.0;
	double triggerHighThreshold;
	double triggerLowThreshold;
	bool envTrigger;

	double inMaxAmpFilter[16];
	double inMaxAmpFilterOut;
	double inPrevMaxAmpFilterOut;
	int inMaxAmpFilterIndex;
	int gateStatus;
	double intermediaryGainBuffer[2][256];
	int envTriggerStatus;
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
	float sampleGain = 0;
	bool processingEnabled;
	int gatePosition = 0;
	bool cutSignal = false;
	string comboName;
	bool footswitchStatus[10];
	void triggerInputSignalFiltering();
	void noiseGate(double* bufferIn, double *bufferOut);
	void envelopeTrigger();

public:
	Processing();
	~Processing();
	string getComboName();
	void setComboName(string comboName);
	GPIOClass portConSwitch[3];
	GPIOClass audioOutputEnable;

	bool areInputsSwitched(); // needs to be public to access sysfs (?)

	int loadCombo(void);
	int audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs);
	int getPitch(bool activate, double *signal);
	int updateFootswitch(bool footswitchStatus[]);
	int enableProcessing();
	int disableProcessing();
	int enableAudioInput();
	int disableAudioInput();
	int enableAudioOutput();
	int disableAudioOutput();
	int updateProcessParameter(string processName, int parameterIndex, int parameterValue);
	int updateControlParameter(string controlName, int parameterIndex, int parameterValue);
	int setNoiseGateCloseThreshold(float closeThres);
	int setNoiseGateOpenThreshold(float openThres);
	int setNoiseGateGain(float gain);
	int setTriggerLowThreshold(float lowThres);
	int setTriggerHighThreshold(float lowThres);
};

int loadComponentSymbols(void);
int loadControlTypeSymbols(void);
#endif /* PROCESSING_H_ */
