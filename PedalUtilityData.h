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
#include <stdlib.h>
#include <algorithm>

#include "utilityFunctions.h"
#include "structs.h"
#include "ProcessingControl.h"
#include "HostUiInt.h"
using namespace std;



class PedalUtilityData
{
private:

	Json::Reader pedalUtilityJsonReader;
	Json::Value pedalUtilityJson;
	Json::FastWriter pedalUtilityJsonWriter;

	bool usbEnableMode;
	int hostOs;
	JackUtility jackUtil;
	ProcessingUtility processingUtil;

	/**************************** Validating data from JSON file and PedalUiInt updates********************/
	bool getValidatedJsonFileBoolData(string utility, bool defaultValue);
	bool getValidatedJsonFileBoolData(string utility, string utilityParameter, bool defaultValue);
	int getValidatedJsonFileUIntData(string utility, int min, int max, int defaultValue);
	int getValidatedJsonFileUIntData(string utility, string utilityParameter, int min, int max, int defaultValue);
	double getValidatedJsonFileDoubleData(string utility, double min, double max, double defaultValue);
	double getValidatedJsonFileDoubleData(string utility, string utilityParameter, double min, double max, double defaultValue);
	string getValidatedJsonFileStringData(string utility, string defaultValue);
	string getValidatedJsonFileStringData(string utility, string utilityParameter, string defaultValue);

	bool getValidatedUpdateBoolData(string value, bool defaultValue);
	int getValidatedUpdateUIntData(string value, int min, int max, int defaultValue);
	double getValidatedUpdateDoubleData(string value, double min, double max, double defaultValue);
	string getValidatedUpdateStringData(string value, vector<string> validStrings, string defaultStr);


public:
	PedalUtilityData();
	~PedalUtilityData();


	int readUtilityDataFromFile();
	int writeUtilityDataToFile();

	Json::Value getUtilityDataForPedalUi();

	int updateUtilityValue(string utilityParameterName, string utilityParameterValue);
	bool getUsbEnableMode();
	int getAntiAliasingNumber();
	int getInputCouplingMode();
	int getWaveshaperMode();
	int getHostOs();
	int getJack_Period();
	int getBufferSize();
	int getJack_Buffer();
	double getNoiseGate_CloseThres();
	double getNoiseGate_OpenThres();
	double getNoiseGate_Gain();
	double getTrigger_HighThres();
	double getTrigger_LowThres();
	ProcessingUtility getProcessingUtility();
	ProcessUtility getProcessUtility();
	JackUtility getJackUtility();
	int processPedalUtilityChange(string utilityParameterName, string utilityParameterValue,
								  PedalUtilityData pedalUtilData, HostUiInt& hostUi, ProcessingControl& procCont);

};

#endif /* PEDALUTILITYDATA_H_ */
