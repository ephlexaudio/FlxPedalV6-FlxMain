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
#include <array>
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>
#include <jack/jack.h>
#include <algorithm>
#include "GPIOClass.h"
#include "Controls.h"
#include "structs.h"
#include "utilityFunctions.h"
#include "jackaudioio.h"
#include "Processes.h"

using namespace std;

class Processing: public JackCpp::AudioIO{
private:
	int bufferSize;
	bool inputsSwitched = false;
	double gateCloseThreshold = 0.01;
	double gateOpenThreshold = 0.30;
	double gateClosedGain = 0.002;
	double gateGain;
	double triggerHighThreshold;
	double triggerLowThreshold;
	double noiseGateBuffer[1024];
	bool gateOpen;
	int gateStatus;
	int comboTime;
	bool envTrigger;
	int envTriggerStatus;
	double inPosPeak[2];
	double inNegPeak[2];
	double inMaxAmp[2];

	double inSignalLevel;
	double inPrevSignalLevel;
	double inSignalDeltaFilterOut;
	bool processingEnabled;
	string comboName;
	bool footswitchStatus[10];
	/********* data structures from ComboStruct ***************/
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int paramControlBufferCount;
	bool controlVoltageEnabled;
	array<ProcessEvent,20> processSequence; // array of Process child classes
	array<ControlEvent,20> controlSequence; // array of Control child classes
	array<ProcessSignalBuffer,60> processSignalBufferArray; // keep this as struct
	array<ProcessParameterControlBuffer,60> processParamControlBufferArray; // keep this as struct
	map<string, ProcessIndexing>  processIndexMap;
	map<string, ControlIndexing>  controlIndexMap;
	//******** Arrays need to be cleared between combo changes. These will be used for that via the array::fill function
	ProcessEvent emptyProcEvent;
	ControlEvent emptyContEvent;
	ProcessSignalBuffer emptyProcSigBuffer;
	ProcessParameterControlBuffer emptyProcParamContBuffer;

	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	GPIOClass portConSwitch[3];
	GPIOClass audioOutputEnable;

	void triggerInputSignalFiltering();
	void noiseGate(double* bufferIn, double *bufferOut);
	void envelopeTrigger();
	void printProcessParameterControlBuffer();

public:

	Processing(ProcessingUtility processingUtil);
	~Processing();
	string getComboName();
	void setComboName(string comboName);

	bool areInputsSwitched(); // needs to be public to access sysfs (?)
	int loadComboStructData(ComboStruct comboStruct);
	ComboStruct getComboStruct();
	/******audioCallback is where the processing and parameter control takes place ******/
	int audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs);

	int getPitch(bool activate, double *signal);
	int updateFootswitch(bool footswitchStatus[]);
	void enableProcessing();
	void disableProcessing();
	void enableAudioOutput();
	void disableAudioOutput();
	int updateProcessParameter(string processName, string parameter, int parameterValue);
	int getProcessParameter(string processName, string parameter);
	int updateControlParameter(string controlName, string parameter, int parameterValue);
	int getControlParameter(string controlName, string parameter);
	void setNoiseGateCloseThreshold(double closeThres);
	void setNoiseGateOpenThreshold(double openThres);
	void setNoiseGateGain(double gain);
	void setTriggerLowThreshold(double lowThres);
	void setTriggerHighThreshold(double lowThres);
	void printIndexMappedProcessData();
	void printIndexMappedControlData();
};


#endif /* PROCESSING_H_ */
