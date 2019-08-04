/*
 * Combo.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_

#include <iostream>
#include <array>
#include <map>
#include <vector>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>
#include <jack/jack.h>

#include "Controls/Controls.h"
#include "Processes/Processes.h"
#include "GPIOClass.h"
#include "jackaudioio.h"
#include "structs.h"


using std::array;


#if(DISABLE_AUDIO_THREAD == 0)
	class Processing: public JackCpp::AudioIO{
#else
		class Processing {
#endif
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
	array<bool,10> footswitchStatusArray;
	/********* data structures from ComboStruct ***************/
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int paramControlBufferCount;
	bool controlVoltageEnabled;
	Processes** processSequence;
	Controls** controlSequence;
	array<ProcessSignalBuffer,60> processSignalBufferArray;
	array<ProcessParameterControlBuffer,60> processParamControlBufferArray;
	map<string, ProcessIndexing>  processIndexMap;
	map<string, ControlIndexing>  controlIndexMap;
	vector<string> componentSymbolVector;
	vector<string> controlSymbolVector;
	bool comboLoaded;

	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	GPIOClass portConSwitch[3];
	GPIOClass audioOutputEnable;
	void triggerInputSignalFiltering();
	void noiseGate(double* bufferIn, double *bufferOut);
	void envelopeTrigger();
	void printProcessParameterControlBuffer();
	void loadComponentSymbolVector();
	void loadControlSymbolVector();

public:
	Processing();
	Processing(NoiseGateUtility gateUtil, EnvTriggerUtility triggerUtil, int bufferSize);
	~Processing();
	string getComboName();
	void setComboName(string comboName);

	bool areInputsSwitched();
	int loadCombo(ComboStruct comboStruct);
	void unloadCombo();
	ComboStruct getComboStruct();
	/******audioCallback is where the processing and parameter control takes place ******/
#if(DISABLE_AUDIO_THREAD == 0)
	int audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs);

#endif



	int updateFootswitch(bool footswitchStatus[]);
	void enableProcessing();
	void disableProcessing();
	void enableAudioOutput();
	void disableAudioOutput();
	int updateProcessParameter(string processName, string parameter, int parameterValue);
	int getProcessParameter(string processName, string parameter);
	int updateControlParameter(string controlName, string parameter, int parameterValue);
	int getControlParameter(string controlName, string parameter);
	void setNoiseGateUtility(NoiseGateUtility gateUtil);
	void setEnvTriggerUtility(EnvTriggerUtility triggerUtil);
	void setBufferSize(int bufferSize);

	void loadSymbols();
	vector<string> getComponentSymbols();
	vector<string> getControlSymbols();

};


#endif /* PROCESSING_H_ */
