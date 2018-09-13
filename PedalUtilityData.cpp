/*
 * PedalUtilityData.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: buildrooteclipse
 */

#include "PedalUtilityData.h"

extern bool debugOutput;
extern unsigned int bufferSize;
extern ProcessingControl procCont;

PedalUtilityData::PedalUtilityData() {
	// TODO Auto-generated constructor stub

}

PedalUtilityData::~PedalUtilityData() {
	// TODO Auto-generated destructor stub
}

#define dbg 1
int PedalUtilityData::readUtilityDataFromFile()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::readUtilityDataFromFile" << endl;
#endif


	int status = 0;
	char pedalUtilityString[500];
	/******* Get JACK initialization data ***************/
	cout << "getting ofxParams.txt." << endl;
	int ofxParamsFD = open("/home/ofxParams.txt", O_CREAT, O_RDONLY);
	/* read file into temp string */
	read(ofxParamsFD, pedalUtilityString, 500);
	close(ofxParamsFD);
	this->pedalUtilityJson.clear();

	if(this->pedalUtilityJsonReader.parse(pedalUtilityString, this->pedalUtilityJson) == false)
	{
		if(debugOutput) cout << "failed to read JACK initialization data file.\n" << endl;
		/************ Should use default initialization data here ****************/
		status = -1;
	}

	if(status >= 0)
	{
		this->usbEnableMode = this->pedalUtilityJson["usbEnable"].asBool();
		this->antiAliasingNumber = atoi(this->pedalUtilityJson["antiAliasingNumber"].asString().c_str());
		if(this->antiAliasingNumber > 1)
		{
			if(debugOutput) cout << "anti-aliasing: on." << endl;
		}
		else
		{
			if(debugOutput) cout << "anti-aliasing: off." << endl;
		}


		if(this->pedalUtilityJson["inputCoupling"].asString().compare("offset") == 0)
		{
			this->inputCouplingMode = 0;
			if(debugOutput) cout << "input coupling mode: averaging offset." << endl;
		}
		else if(this->pedalUtilityJson["inputCoupling"].asString().compare("filter") == 0)
		{
			this->inputCouplingMode = 1;
			if(debugOutput) cout << "input coupling mode: highpass filter." << endl;
		}
		else
		{
			this->inputCouplingMode = 2;
			if(debugOutput) cout << "input coupling mode: none." << endl;
		}


		if(this->pedalUtilityJson["waveshaperMode"].asString().compare("circuitModel") == 0)
		{
			this->waveshaperMode = 0;
			if(debugOutput) cout << "waveshaper mode: circuit model." << endl;
		}
		else
		{
			this->waveshaperMode = 1;
			if(debugOutput) cout << "waveshaper mode: slope/intercept." << endl;
		}

		this->jack.period = atoi(this->pedalUtilityJson["jack"]["period"].asString().c_str());
		this->jack.buffer = atoi(this->pedalUtilityJson["jack"]["buffer"].asString().c_str());
		if(debugOutput) cout << "JACK period: " << this->jack.period << "\tJACK buffer: " << this->jack.buffer << endl;

		this->noiseGate.openThres = atof(this->pedalUtilityJson["noiseGate"]["openThres"].asString().c_str());
		this->noiseGate.closeThres = atof(this->pedalUtilityJson["noiseGate"]["closeThres"].asString().c_str());
		this->noiseGate.gain = atof(this->pedalUtilityJson["noiseGate"]["gain"].asString().c_str());
		if(debugOutput) cout << "Noise gate close threshold: " << this->noiseGate.closeThres << "\tNoise gate open threshold: " << this->noiseGate.openThres << endl;
		this->trigger.highThres = atof(this->pedalUtilityJson["trigger"]["highThres"].asString().c_str());
		this->trigger.lowThres = atof(this->pedalUtilityJson["trigger"]["lowThres"].asString().c_str());
		if(debugOutput) cout << "Trigger low threshold: " << this->trigger.lowThres << "\tTrigger high threshold: " << this->trigger.highThres << endl;
		this->delayFineDivisor = atoi(this->pedalUtilityJson["delayFineDivisor"].asString().c_str());
		if(debugOutput) cout << "Delay Fine Divisor: " << this->delayFineDivisor << endl;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::readUtilityDataFromFile: " << status << endl;
#endif


	return status;
}

#define dbg 0
Json::Value PedalUtilityData::getUtilityDataForPedalUi()
{
	int status = 0;
	Json::Value pedalUiUtilityDataJson;
	string pedalUiUtilityDataString;


	pedalUiUtilityDataJson["noiseGate"]["openThres"] = this->noiseGate.openThres;
	pedalUiUtilityDataJson["noiseGate"]["closeThres"] = this->noiseGate.closeThres;
	pedalUiUtilityDataJson["noiseGate"]["gain"] = this->noiseGate.gain;
	pedalUiUtilityDataJson["trigger"]["highThres"] = this->trigger.highThres;
	pedalUiUtilityDataJson["trigger"]["lowThres"] = this->trigger.lowThres;

	return pedalUiUtilityDataJson;
}

#define dbg 1
int PedalUtilityData::writeUtilityDataToFile()
{
	int status = 0;
	int size = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::writeUtilityDataToFile" << endl;
#endif
	Json::FastWriter pedalUiUtilityDataStringWriter;
	string pedalUiUtilityDataString;
	int ofxParamsFD = open("/home/ofxParams.txt", O_WRONLY);

	this->pedalUtilityJson["noiseGate"]["openThres"] = this->noiseGate.openThres;
	this->pedalUtilityJson["noiseGate"]["closeThres"] = this->noiseGate.closeThres;
	this->pedalUtilityJson["noiseGate"]["gain"] = this->noiseGate.gain;
	this->pedalUtilityJson["trigger"]["highThres"] = this->trigger.highThres;
	this->pedalUtilityJson["trigger"]["lowThres"] = this->trigger.lowThres;

	pedalUiUtilityDataString = pedalUiUtilityDataStringWriter.write(this->pedalUtilityJson);

	if((size = write(ofxParamsFD, pedalUiUtilityDataString.c_str(), pedalUiUtilityDataString.size())) >= 100)
	{
		status = 0;
	}
	else
	{
		status = -1;
		cout << "failed to write to ofxParams.txt" << endl;
	}
	close(ofxParamsFD);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::writeUtilityDataToFile: " << status << endl;
#endif
#if(dbg >= 2)
	if(debugOutput) cout << "pedalUiUtilityDataString: " << pedalUiUtilityDataString << endl;
#endif
	return status;
}


#define dbg 0
int PedalUtilityData::updateUtilityValue(string utilityParameterName, string utilityParameterValue)
{

	int status = 0;
	string utilityParameterNameParsed[2];
	int underscoreIndex = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::updateUtilityValue" << endl;
	if(debugOutput) cout << utilityParameterName << ": " << utilityParameterValue << endl;
#endif

	if((underscoreIndex = utilityParameterName.find("_")) > 0)
	{
		utilityParameterNameParsed[0] = utilityParameterName.substr(0,underscoreIndex);
		utilityParameterNameParsed[1] = utilityParameterName.substr(underscoreIndex+1);
	}
	else
	{
		utilityParameterNameParsed[0] = utilityParameterName;
		utilityParameterNameParsed[1].clear();
	}

	char value[8];
	strncpy(value, utilityParameterValue.c_str(),7);

#if(dbg >=2)
	if(debugOutput) cout << "underscoreIndex: " << underscoreIndex<< "\t utilityParameterNameParsed[0]: " << utilityParameterNameParsed[0] << "\tutilityParameterNameParsed[1]: " << utilityParameterNameParsed[1] << endl;
#endif

	if(utilityParameterNameParsed[1].empty()) // base parameter, no sub parameter
	{
		if(utilityParameterNameParsed[0].compare("usbEnableMode") == 0)
		{
			for(int i = 0; i<5; i++) tolower(value[i]);
			if(strncmp(value,"true",4) == 0) this->usbEnableMode = "true";
			else this->usbEnableMode = "false";
#if(dbg >=2)
	if(debugOutput) cout << "this->usbEnableMode: " << this->usbEnableMode << endl;
#endif
		}
		else if(utilityParameterNameParsed[0].compare("antiAliasingNumber") == 0)
		{
			this->antiAliasingNumber = atoi(value);
#if(dbg >=2)
	if(debugOutput) cout << "this->antiAliasingNumber: " << this->antiAliasingNumber << endl;
#endif
		}
		else if(utilityParameterNameParsed[0].compare("inputCouplingMode") == 0)
		{
			this->inputCouplingMode = atoi(value);
#if(dbg >=2)
	if(debugOutput) cout << "this->inputCouplingMode: " << this->inputCouplingMode << endl;
#endif
		}
		else if(utilityParameterNameParsed[0].compare("waveshaperMode") == 0)
		{
			this->waveshaperMode = atoi(value);
#if(dbg >=2)
	if(debugOutput) cout << "this->waveshaperMode: " << this->waveshaperMode << endl;
#endif
		}
	}
	else if(utilityParameterNameParsed[0].compare("jack") == 0)
	{
		if(utilityParameterNameParsed[1].compare("period") == 0)
		{
			this->jack.period = atoi(value);
#if(dbg >=2)
	if(debugOutput) cout << "this->jack.period: " << this->jack.period << endl;
#endif
		}
		else if(utilityParameterNameParsed[1].compare("buffer") == 0)
		{
			this->jack.buffer = atoi(value);
#if(dbg >=2)
	if(debugOutput) cout << "this->jack.buffer: " << this->jack.buffer << endl;
#endif
		}
	}
	else if(utilityParameterNameParsed[0].compare("noiseGate") == 0)
	{
		if(utilityParameterNameParsed[1].compare("closeThres") == 0)
		{
			this->noiseGate.closeThres = atof(value);
			procCont.setNoiseGateCloseThreshold(this->noiseGate.closeThres);
#if(dbg >=2)
	if(debugOutput) cout << "this->noiseGate.closeThres: " << this->noiseGate.closeThres << endl;
#endif
		}
		else if(utilityParameterNameParsed[1].compare("openThres") == 0)
		{
			this->noiseGate.openThres = atof(value);
			procCont.setNoiseGateOpenThreshold(this->noiseGate.openThres);
#if(dbg >=2)
	if(debugOutput) cout << "this->noiseGate.openThres: " << this->noiseGate.openThres << endl;
#endif
		}
		else if(utilityParameterNameParsed[1].compare("gain") == 0)
		{
			this->noiseGate.gain = atof(value);
			procCont.setNoiseGateGain(this->noiseGate.gain);
#if(dbg >=2)
	if(debugOutput) cout << "this->noiseGate.gain: " << this->noiseGate.gain << endl;
#endif
		}
	}
	else if(utilityParameterNameParsed[0].compare("trigger") == 0)
	{
		if(utilityParameterNameParsed[1].compare("highThres") == 0)
		{
			this->trigger.highThres = atof(value);
			procCont.setTriggerHighThreshold(this->trigger.highThres);
#if(dbg >=2)
	if(debugOutput) cout << "this->trigger.highThres: " << this->trigger.highThres << endl;
#endif
		}
		else if(utilityParameterNameParsed[1].compare("lowThres") == 0)
		{
			this->trigger.lowThres = atof(value);
			procCont.setTriggerLowThreshold(this->trigger.lowThres);
#if(dbg >=2)
	if(debugOutput) cout << "this->trigger.lowThres: " << this->trigger.lowThres << endl;
#endif
		}
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::updateUtilityValue: " << status << endl;
#endif


	return status;
}

bool PedalUtilityData::getUsbEnableMode()
{
	return this->usbEnableMode;
}

int PedalUtilityData::getAntiAliasingNumber()
{
	return this->antiAliasingNumber;
}

int PedalUtilityData::getInputCouplingMode()
{
	return this->inputCouplingMode;
}

int PedalUtilityData::getWaveshaperMode()
{
	return this->waveshaperMode;
}

int PedalUtilityData::getJack_Period()
{
	return this->jack.period;
}

int PedalUtilityData::getJack_Buffer()
{
	return this->jack.buffer;
}

int PedalUtilityData::getBufferSize()
{
	return this->jack.period;
}
float PedalUtilityData::getNoiseGate_CloseThres()
{
	return this->noiseGate.closeThres;
}

float PedalUtilityData::getNoiseGate_OpenThres()
{
	return this->noiseGate.openThres;
}

float PedalUtilityData::getNoiseGate_Gain()
{
	return this->noiseGate.gain;
}

float PedalUtilityData::getTrigger_HighThres()
{
	return this->trigger.highThres;
}

float PedalUtilityData::getTrigger_LowThres()
{
	return this->trigger.lowThres;
}

int PedalUtilityData::getDelayFineDivisor()
{
	if(this->delayFineDivisor < 1) this->delayFineDivisor = 50;
	return this->delayFineDivisor;
}
