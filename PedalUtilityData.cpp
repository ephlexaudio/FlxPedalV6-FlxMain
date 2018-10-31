/*
 * PedalUtilityData.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: buildrooteclipse
 */

#include "PedalUtilityData.h"

extern bool debugOutput;
extern ProcessingControl procCont;
extern HostUiInt hostUi;
using namespace std;

PedalUtilityData::PedalUtilityData()
{
	// TODO Auto-generated constructor stub
	this->processingUtil.procUtil.antiAliasingNumber = 4;
	this->hostOs = 0;
	this->processingUtil.procUtil.inputCouplingMode = 1;
	this->usbEnableMode = true;
	this->processingUtil.procUtil.waveshaperMode = 0;
}

PedalUtilityData::~PedalUtilityData() {
	// TODO Auto-generated destructor stub
}

bool string2bool(string boolStr)
{
	bool value;

	if(boolStr.compare("true")) value = true;
	else  value = false;

	return value;
}

bool PedalUtilityData::getValidatedJsonFileBoolData(string utility, bool defaultValue)
{
	bool boolValue = defaultValue;

	try
	{
		boolValue = this->pedalUtilityJson[utility].asBool();
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << e.what() << endl;
	}

	return boolValue;
}

bool PedalUtilityData::getValidatedJsonFileBoolData(string utility, string utilityParameter, bool defaultValue)
{
	bool boolValue = defaultValue;

	try
	{
		boolValue = this->pedalUtilityJson[utility][utilityParameter].asBool();
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << "." << utilityParameter << e.what() << endl;
	}

	return boolValue;
}

int PedalUtilityData::getValidatedJsonFileUIntData(string utility, int min, int max, int defaultValue)
{
	int IntValue = defaultValue;

	try
	{
		int tempValue = this->pedalUtilityJson[utility].asUInt();
		if(min <= tempValue && tempValue <= max)  IntValue = tempValue;
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility <<  e.what() << endl;
	}

	return IntValue;
}
int PedalUtilityData::getValidatedJsonFileUIntData(string utility, string utilityParameter, int min, int max, int defaultValue)
{
	int uIntValue = defaultValue;

	try
	{
		int tempValue = this->pedalUtilityJson[utility][utilityParameter].asUInt();
		if(min <= tempValue && tempValue <= max)  uIntValue = tempValue;
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << ":" << utilityParameter << e.what() << endl;
	}

	return uIntValue;
}
double PedalUtilityData::getValidatedJsonFileDoubleData(string utility, double min, double max, double defaultValue)
{
	double doubleValue = defaultValue;

	try
	{
		double tempValue = this->pedalUtilityJson[utility].asDouble();
		if(min <= tempValue && tempValue <= max)  doubleValue = tempValue;
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility <<  e.what() << endl;
	}

	return doubleValue;

}
double PedalUtilityData::getValidatedJsonFileDoubleData(string utility, string utilityParameter, double min, double max, double defaultValue)
{
	double doubleValue = defaultValue;

	try
	{
		double tempValue = this->pedalUtilityJson[utility][utilityParameter].asDouble();
		if(min <= tempValue && tempValue <= max)  doubleValue = tempValue;
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << ":" << utilityParameter << e.what() << endl;
	}

	return doubleValue;

}


string PedalUtilityData::getValidatedJsonFileStringData(string utility, string defaultValue)
{
	string stringValue = defaultValue;

	try
	{
		string tempValue = this->pedalUtilityJson[utility].asString();
		stringValue = tempValue;  // if an exception occurs, this line will not be reached
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << e.what() << endl;
	}

	return stringValue;
}
string PedalUtilityData::getValidatedJsonFileStringData(string utility, string utilityParameter, string defaultValue)
{
	string stringValue = defaultValue;

	try
	{
		string tempValue = this->pedalUtilityJson[utility][utilityParameter].asString();
		stringValue = tempValue;  // if an exception occurs, this line will not be reached
	}
	catch(exception &e)
	{
		cout << "exception getting " << utility << ":" << utilityParameter << e.what() << endl;
	}

	return stringValue;

}

/*************************************************************************************************************/



#define dbg 0
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
		try
		{
			this->usbEnableMode = this->getValidatedJsonFileBoolData("pedal","usbEnable",true);

			this->processingUtil.procUtil.bufferSize = this->getValidatedJsonFileUIntData("process","bufferSize",128,1024,256);
			this->processingUtil.procUtil.antiAliasingNumber = this->getValidatedJsonFileUIntData("process","antiAliasingNumber",1,5,1);
			if(this->processingUtil.procUtil.antiAliasingNumber > 1)
			{
				if(debugOutput) cout << "anti-aliasing: on." << endl;
			}
			else
			{
				if(debugOutput) cout << "anti-aliasing: off." << endl;
			}


			string mode = this->getValidatedJsonFileStringData("process","inputCoupling","filter");
			if(mode.compare("none") == 0)
			{
				this->processingUtil.procUtil.inputCouplingMode = 0;
				if(debugOutput) cout << "input coupling mode: none." << endl;
			}
			else if(mode.compare("filter") == 0)
			{
				this->processingUtil.procUtil.inputCouplingMode = 1;
				if(debugOutput) cout << "input coupling mode: highpass filter." << endl;
			}

			string hostOs = this->getValidatedJsonFileStringData("hostPc","os","Lin");
			if(hostOs.compare("Win") == 0)
			{
				this->hostOs = 1;
				if(debugOutput) cout << "Host OS: Windows" << endl;
			}
			else if(hostOs.compare("Mac") == 0)
			{
				this->hostOs = 2;
				if(debugOutput) cout << "Host OS: Mac" << endl;
			}
			else if(hostOs.compare("Lin") == 0)
			{
				this->hostOs = 3;
				if(debugOutput) cout << "Host OS: Linux" << endl;
			}

			mode = this->getValidatedJsonFileStringData("process","waveshaperMode","circuitModel");
			if(mode.compare("circuitModel") == 0)
			{
				this->processingUtil.procUtil.waveshaperMode = 0;
				if(debugOutput) cout << "waveshaper mode: circuit model." << endl;
			}
			else
			{
				this->processingUtil.procUtil.waveshaperMode = 1;
				if(debugOutput) cout << "waveshaper mode: slope/intercept." << endl;
			}

			this->jackUtil.period = this->getValidatedJsonFileUIntData("jack","period",128,1024,256);
			this->jackUtil.buffer = this->getValidatedJsonFileUIntData("jack","buffer",2,3,3);
			if(debugOutput) cout << "JACK period: " << this->jackUtil.period << "\tJACK buffer: " << this->jackUtil.buffer << endl;

			this->processingUtil.noiseGateUtil.openThres = this->getValidatedJsonFileDoubleData("noiseGate","openThres",0.005, 2.00, 0.095);
			this->processingUtil.noiseGateUtil.closeThres = this->getValidatedJsonFileDoubleData("noiseGate","closeThres",0.005, 2.00, 0.04);
			this->processingUtil.noiseGateUtil.gain = this->getValidatedJsonFileDoubleData("noiseGate","gain",0.0001, 0.05, 0.0002);
			if(debugOutput) cout << "Noise gate close threshold: " << this->processingUtil.noiseGateUtil.closeThres << "\tNoise gate open threshold: " << this->processingUtil.noiseGateUtil.openThres << endl;
			this->processingUtil.triggerUtil.highThres = this->getValidatedJsonFileDoubleData("trigger","highThres",0.1, 1.00, 0.45);
			this->processingUtil.triggerUtil.lowThres = this->getValidatedJsonFileDoubleData("trigger","lowThres",0.1, 1.00, 0.059);
			if(debugOutput) cout << "Trigger low threshold: " << this->processingUtil.triggerUtil.lowThres << "\tTrigger high threshold: " << this->processingUtil.triggerUtil.highThres << endl;


		}
		catch(exception &e)
		{
			cout << "exception with PedalUtilityData::readUtilityDataFromFile: " << e.what() << endl;
			status = -1;

		}
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::readUtilityDataFromFile: " << status << endl;
#endif


	return status;
}

#define dbg 0
Json::Value PedalUtilityData::getUtilityDataForPedalUi()
{

	Json::Value pedalUiUtilityDataJson;
	string pedalUiUtilityDataString;


	pedalUiUtilityDataJson["noiseGate"]["openThres"] = this->processingUtil.noiseGateUtil.openThres;
	pedalUiUtilityDataJson["noiseGate"]["closeThres"] = this->processingUtil.noiseGateUtil.closeThres;
	pedalUiUtilityDataJson["noiseGate"]["gain"] = this->processingUtil.noiseGateUtil.gain;
	pedalUiUtilityDataJson["trigger"]["highThres"] = this->processingUtil.triggerUtil.highThres;
	pedalUiUtilityDataJson["trigger"]["lowThres"] = this->processingUtil.triggerUtil.lowThres;
	pedalUiUtilityDataJson["hostPc"]["os"] = this->hostOs;

	return pedalUiUtilityDataJson;
}

#define dbg 0
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

	this->pedalUtilityJson["noiseGate"]["openThres"] = this->processingUtil.noiseGateUtil.openThres;
	this->pedalUtilityJson["noiseGate"]["closeThres"] = this->processingUtil.noiseGateUtil.closeThres;
	this->pedalUtilityJson["noiseGate"]["gain"] = this->processingUtil.noiseGateUtil.gain;
	this->pedalUtilityJson["trigger"]["highThres"] = this->processingUtil.triggerUtil.highThres;
	this->pedalUtilityJson["trigger"]["lowThres"] = this->processingUtil.triggerUtil.lowThres;
	switch(this->hostOs)
	{
		case 1:
			this->pedalUtilityJson["hostPc"]["os"] = "Win";
			break;
		case 2:
			this->pedalUtilityJson["hostPc"]["os"] = "Mac";
			break;
		case 3:
			this->pedalUtilityJson["hostPc"]["os"] = "Lin";
			break;
		default:
			this->pedalUtilityJson["hostPc"]["os"] = "Lin";
	}

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


#define dbg 1
int PedalUtilityData::updateUtilityValue(string utilityParameterString, string utilityParameterValue)
{

	int status = 0;
	string utility;
	string utilityParameter;
	int  underscoreIndex = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::updateUtilityValue" << endl;
	if(debugOutput) cout << utilityParameterString << ": " << utilityParameterValue << endl;
#endif

	try
	{
		if((underscoreIndex = utilityParameterString.find("_")) > 0)
		{
			utility = utilityParameterString.substr(0,underscoreIndex);
			utilityParameter = utilityParameterString.substr(underscoreIndex+1);
		}
		else
		{
			utility = utilityParameterString;
			utilityParameter.clear();
		}

	#if(dbg >=2)
		if(debugOutput) cout << "underscoreIndex: " << underscoreIndex<< "\t utility: " << utility << "\tutilityParameter: " << utilityParameter << endl;
	#endif

    if(utility.compare("hostPc") == 0)
		{
			if(utilityParameter.compare("os") == 0)
			{
				if(utilityParameterValue.compare("Win") == 0)
				{
					this->hostOs = 1;
					if(debugOutput) cout << "Host OS: Windows" << endl;
				}
				else if(utilityParameterValue.compare("Mac") == 0)
				{
					this->hostOs = 2;
					if(debugOutput) cout << "Host OS: Mac" << endl;
				}
				else if(utilityParameterValue.compare("Lin") == 0)
				{
					this->hostOs = 3;
					if(debugOutput) cout << "Host OS: Linux" << endl;
				}
			}
		}
		else if(utility.compare("process") == 0)
		{
			if(utilityParameter.compare("antiAliasingNumber") == 0)
			{
				this->processingUtil.procUtil.antiAliasingNumber = (int)strtol(utilityParameterValue.c_str(),NULL,10);

	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.procUtil.antiAliasingNumber: " << this->processingUtil.procUtil.antiAliasingNumber << endl;
	#endif
			}
			else if(utilityParameter.compare("inputCouplingMode") == 0)
			{
				this->processingUtil.procUtil.inputCouplingMode = (int)strtol(utilityParameterValue.c_str(),NULL,10);
	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.procUtil.inputCouplingMode: " << this->processingUtil.procUtil.inputCouplingMode << endl;
	#endif
			}
			else if(utilityParameter.compare("waveshaperMode") == 0)
			{
				this->processingUtil.procUtil.waveshaperMode = (int)strtol(utilityParameterValue.c_str(),NULL,10);
	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.procUtil.waveshaperMode: " << this->processingUtil.procUtil.waveshaperMode << endl;
	#endif
			}

		}
		else if(utility.compare("pedal") == 0)
		{
			if(utilityParameter.compare("usbEnableMode") == 0)
			{
				if(utilityParameterValue.compare("true")) this->usbEnableMode = true;
				else  this->usbEnableMode = false;
	#if(dbg >=2)
		if(debugOutput) cout << "this->usbEnableMode: " << this->usbEnableMode << endl;
	#endif
			}
		}
		else if(utility.compare("jack") == 0)
		{
			if(utilityParameter.compare("period") == 0)
			{
				this->jackUtil.period = (int)strtol(utilityParameterValue.c_str(),NULL,10);
	#if(dbg >=2)
		if(debugOutput) cout << "this->jackUtil.period: " << this->jackUtil.period << endl;
	#endif
			}
			else if(utilityParameter.compare("buffer") == 0)
			{
				this->jackUtil.buffer = (int)strtol(utilityParameterValue.c_str(),NULL,10);
	#if(dbg >=2)
		if(debugOutput) cout << "this->jackUtil.buffer: " << this->jackUtil.buffer << endl;
	#endif
			}
		}
		else if(utility.compare("noiseGate") == 0)
		{
			if(utilityParameter.compare("closeThres") == 0)
			{
				this->processingUtil.noiseGateUtil.closeThres = strtod(utilityParameterValue.c_str(),NULL);
	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.noiseGateUtil.closeThres: " << this->processingUtil.noiseGateUtil.closeThres << endl;
	#endif
			}
			else if(utilityParameter.compare("openThres") == 0)
			{
				this->processingUtil.noiseGateUtil.openThres = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.noiseGateUtil.openThres: " << this->processingUtil.noiseGateUtil.openThres << endl;
	#endif
			}
			else if(utilityParameter.compare("gain") == 0)
			{
				this->processingUtil.noiseGateUtil.gain = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.noiseGateUtil.gain: " << this->processingUtil.noiseGateUtil.gain << endl;
	#endif
			}
		}
		else if(utility.compare("trigger") == 0)
		{
			if(utilityParameter.compare("highThres") == 0)
			{
				this->processingUtil.triggerUtil.highThres = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.triggerUtil.highThres: " << this->processingUtil.triggerUtil.highThres << endl;
	#endif
			}
			else if(utilityParameter.compare("lowThres") == 0)
			{
				this->processingUtil.triggerUtil.lowThres = strtod(utilityParameterValue.c_str(),NULL);
	#if(dbg >=2)
		if(debugOutput) cout << "this->processingUtil.triggerUtil.lowThres: " << this->processingUtil.triggerUtil.lowThres << endl;
	#endif
			}
		}

	}
	catch(exception &e)
	{
		cout << "exception with PedalUtilityData::updateUtilityValue: " << e.what() << endl;
		status = -1;
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
	return this->processingUtil.procUtil.antiAliasingNumber;
}

int PedalUtilityData::getInputCouplingMode()
{
	return this->processingUtil.procUtil.inputCouplingMode;
}

int PedalUtilityData::getWaveshaperMode()
{
	return this->processingUtil.procUtil.waveshaperMode;
}

int PedalUtilityData::getHostOs()
{
	return this->hostOs;
}


int PedalUtilityData::getJack_Period()
{
	return this->jackUtil.period;
}

int PedalUtilityData::getJack_Buffer()
{
	return this->jackUtil.buffer;
}

int PedalUtilityData::getBufferSize()
{
	return this->jackUtil.period;
}
double PedalUtilityData::getNoiseGate_CloseThres()
{
	return this->processingUtil.noiseGateUtil.closeThres;
}

double PedalUtilityData::getNoiseGate_OpenThres()
{
	return this->processingUtil.noiseGateUtil.openThres;
}

double PedalUtilityData::getNoiseGate_Gain()
{
	return this->processingUtil.noiseGateUtil.gain;
}

double PedalUtilityData::getTrigger_HighThres()
{
	return this->processingUtil.triggerUtil.highThres;
}

double PedalUtilityData::getTrigger_LowThres()
{
	return this->processingUtil.triggerUtil.lowThres;
}


#define dbg 1
ProcessingUtility PedalUtilityData::getProcessingUtility()
{
	ProcessingUtility processingUtil;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::getProcessingUtility" << endl;
#endif

	try
	{
		processingUtil.noiseGateUtil.closeThres = this->processingUtil.noiseGateUtil.closeThres;
		processingUtil.noiseGateUtil.gain = this->processingUtil.noiseGateUtil.gain;
		processingUtil.noiseGateUtil.openThres = this->processingUtil.noiseGateUtil.openThres;
		processingUtil.triggerUtil.highThres = this->processingUtil.triggerUtil.highThres;
		processingUtil.triggerUtil.lowThres = this->processingUtil.triggerUtil.lowThres;
		processingUtil.procUtil.bufferSize = this->processingUtil.procUtil.bufferSize;
		processingUtil.procUtil.inputCouplingMode = this->processingUtil.procUtil.inputCouplingMode;
		processingUtil.procUtil.antiAliasingNumber = this->processingUtil.procUtil.antiAliasingNumber;
		processingUtil.procUtil.waveshaperMode = this->processingUtil.procUtil.waveshaperMode;
		cout << "processingUtil.procUtil.bufferSize: " << processingUtil.procUtil.bufferSize << endl;
		cout << "processingUtil.procUtil.inputCouplingMode: " << processingUtil.procUtil.inputCouplingMode << endl;
		cout << "processingUtil.procUtil.antiAliasingNumber: " << processingUtil.procUtil.antiAliasingNumber << endl;
		cout << "processingUtil.procUtil.waveshaperMode: " << processingUtil.procUtil.waveshaperMode << endl;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getProcessingUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::getProcessingUtility" << endl;
#endif

	return processingUtil;
}

ProcessUtility PedalUtilityData::getProcessUtility()
{
	ProcessUtility procUtil;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::getProcessUtility" << endl;
#endif

	try
	{
		procUtil.bufferSize = this->processingUtil.procUtil.bufferSize;
		procUtil.inputCouplingMode = this->processingUtil.procUtil.inputCouplingMode;
		procUtil.antiAliasingNumber = this->processingUtil.procUtil.antiAliasingNumber;
		procUtil.waveshaperMode = this->processingUtil.procUtil.waveshaperMode;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getProcessUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::getProcessUtility" << endl;
#endif

	return procUtil;

}

JackUtility PedalUtilityData::getJackUtility()
{
	JackUtility jackUtil;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUtilityData::getJackUtility" << endl;
#endif

	try
	{
		jackUtil.buffer = this->jackUtil.buffer;
		jackUtil.period = this->jackUtil.period;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getJackUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUtilityData::getJackUtility" << endl;
#endif

	return jackUtil;
}

#define dbg 2
int PedalUtilityData::processPedalUtilityChange(string utilityParameterName, string utilityParameterValue,
							  PedalUtilityData pedalUtilData, HostUiInt &hostUi, ProcessingControl &procCont)
{
	int status = 0;
	string utility;
	string utilityParameter;
	int  underscoreIndex = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING PedalUtilityData::processPedalUtilityChange" << endl;
#endif
	try
	{
		if((underscoreIndex = utilityParameterName.find("_")) > 0)
		{
			utility = utilityParameterName.substr(0,underscoreIndex);
			utilityParameter = utilityParameterName.substr(underscoreIndex+1);
		}
		else
		{
			utility = utilityParameterName;
			utilityParameter.clear();
		}


		if(utility.compare("hostPc") == 0)
		{
			if(utilityParameter.compare("os") == 0)
			{
					hostUi.configureUsbGadgetForOs(pedalUtilData.getHostOs());
			}
		}
		else if(utility.compare("pedal") == 0)
		{
			if(utilityParameter.compare("usbEnableMode") == 0)
			{
				pedalUtilData.getUsbEnableMode();
	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.usbEnableMode: " << pedalUtilData.getUsbEnableMode() << endl;
	#endif
			}
		}
		else if(utility.compare("noiseGate") == 0)
		{
			if(utilityParameter.compare("closeThres") == 0)
			{
				procCont.setNoiseGateCloseThreshold(pedalUtilData.getNoiseGate_CloseThres());
	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.noiseGateUtil.closeThres: " << pedalUtilData.getNoiseGate_CloseThres() << endl;
	#endif
			}
			else if(utilityParameter.compare("openThres") == 0)
			{
				procCont.setNoiseGateOpenThreshold(pedalUtilData.getNoiseGate_OpenThres());

	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.noiseGateUtil.openThres: " << pedalUtilData.getNoiseGate_OpenThres() << endl;
	#endif
			}
			else if(utilityParameter.compare("gain") == 0)
			{
				procCont.setNoiseGateGain(pedalUtilData.getNoiseGate_Gain());

	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.noiseGateUtil.gain: " << pedalUtilData.getNoiseGate_Gain() << endl;
	#endif
			}
		}
		else if(utility.compare("trigger") == 0)
		{
			if(utilityParameter.compare("highThres") == 0)
			{
				procCont.setTriggerHighThreshold(pedalUtilData.getTrigger_HighThres());

	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.triggerUtil.highThres: " << pedalUtilData.getTrigger_HighThres() << endl;
	#endif
			}
			else if(utilityParameter.compare("lowThres") == 0)
			{
				procCont.setTriggerLowThreshold(pedalUtilData.getTrigger_LowThres());
	#if(dbg >=2)
		if(debugOutput) cout << "pedalUtilData.triggerUtil.lowThres: " << pedalUtilData.getTrigger_LowThres() << endl;
	#endif
			}
		}
	}
	catch(exception &e)
	{

	}


#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING PedalUtilityData::processPedalUtilityChange: " << endl;
#endif
	return status;
}
