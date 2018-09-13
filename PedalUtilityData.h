/*
 * PedalUtilityData.h
 *
 *  Created on: Nov 24, 2017
 *      Author: buildrooteclipse
 */

#ifndef PEDALUTILITYDATA_H_
#define PEDALUTILITYDATA_H_

#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>
#include <json/json.h>
#include <algorithm>

#include "ProcessingControl.h"

using namespace std;

class PedalUtilityData {
private:
	Json::Reader pedalUtilityJsonReader;
	Json::Value pedalUtilityJson;
	Json::FastWriter pedalUtilityJsonWriter;

	bool usbEnableMode;
	int antiAliasingNumber;
	int inputCouplingMode;
	int waveshaperMode;

	struct {
		int period;
		int buffer;
	}jack;

	struct {
		float closeThres;
		float openThres;
		float gain;
	}noiseGate;

	struct {
		float highThres;
		float lowThres;
	}trigger;
	
	int delayFineDivisor;

public:
	PedalUtilityData();
	virtual ~PedalUtilityData();

	int readUtilityDataFromFile();
	int writeUtilityDataToFile();

	Json::Value getUtilityDataForPedalUi();

	int updateUtilityValue(string utilityParameterName, string utilityParameterValue);
	bool getUsbEnableMode();
	int getAntiAliasingNumber();
	int getInputCouplingMode();
	int getWaveshaperMode();
	int getJack_Period();
	int getBufferSize();
	int getJack_Buffer();
	float getNoiseGate_CloseThres();
	float getNoiseGate_OpenThres();
	float getNoiseGate_Gain();
	float getTrigger_HighThres();
	float getTrigger_LowThres();
	int getDelayFineDivisor();
};

#endif /* PEDALUTILITYDATA_H_ */
