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
#include <sstream>

#include "FileSystemInt.h"
#include "structs.h"



class PedalUtilityData
{
private:

	Json::Reader pedalUtilityJsonReader;
	Json::Value pedalUtilityJson;
	Json::FastWriter pedalUtilityJsonWriter;
	FileSystemInt fsInt;
	//struct FlxUtilityData {
		ProcessUtility processUtil;
		JackUtility jackUtil;
		NoiseGateUtility gateUtil;
		EnvTriggerUtility triggerUtil;
		PedalUtility pedalUtil;
		HostPcUtility hostUtil;
	//} flxUtil;

	bool validateBoolData(bool utilityParameterValue, bool defaultValue);
	int validateIntData(int utilityParameterValue, UtilIntValue utilInt);
	double validateDoubleData(double utilityParameterValue, UtilDoubleValue utilDouble);
	string validateOptionData(string utilityParameterOption, UtilOption utilOption);


public:
	PedalUtilityData();
	~PedalUtilityData();


	int readUtilityDataFromFile();
	int writeUtilityDataToFile();

	Json::Value getUtilityDataForPedalUi();

	int updateUtilityValue(string utilityParameterName, string utilityParameterValue);
	ProcessUtility getProcessUtility();
	JackUtility getJackUtility();
	NoiseGateUtility getNoiseGateUtility();
	EnvTriggerUtility getEnvTriggerUtility();
	PedalUtility getPedalUtility();
	HostPcUtility getHostUtility();


};

#endif /* PEDALUTILITYDATA_H_ */
