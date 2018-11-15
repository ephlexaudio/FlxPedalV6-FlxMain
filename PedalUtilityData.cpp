/*
 * PedalUtilityData.cpp
 *
 *  Created on: Nov 24, 2017
 *      Author: buildrooteclipse
 */

#include "PedalUtilityData.h"
#include "utilityFunctions.h"


namespace std
{

PedalUtilityData::PedalUtilityData()
{
	// Set current value/option, min, and max values or valid options
	this->gateUtil.closeThres = {0.04,0.01,1.0};
	this->gateUtil.gain = {0.01,0.001,0.1};
	this->gateUtil.openThres = {0.2,0.1,1.5};

	this->hostUtil.os = {"Lin",vector<string>{"Win","Mac","Lin"}};

	this->jackUtil.buffer = {3,2,3};
	this->jackUtil.period = {256,128,1024};

	this->pedalUtil.usbEnable = true;

	this->triggerUtil.highThres = {0.2,0.05,1.0};
	this->triggerUtil.lowThres = {0.059,0.1,0.01};

	this->processUtil.antiAliasingNumber = {1,1,5};
	this->processUtil.bufferSize = {256, 128, 1024};
	this->processUtil.inputCouplingMode = {"filter",vector<string>{"filter","none"}};
	this->processUtil.waveshaperMode = {"circuitModel",vector<string>{"circuitModel","slopeIntercept"}};


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



#define dbg 0
int PedalUtilityData::validateIntData(int utilityParameterValue, UtilIntValue utilInt)
{
	int intValue;
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::validateIntData" << endl;
#endif

	try
	{
		if(utilInt.minimum <= utilityParameterValue && utilityParameterValue <= utilInt.maximum)  intValue = utilityParameterValue;
		else intValue = (utilInt.minimum + utilInt.maximum)/2;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::validateIntData: " << e.what() << endl;
	}
#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::validateIntData: " << status << endl;
#endif

	return intValue;
}

double PedalUtilityData::validateDoubleData(double utilityParameterValue, UtilDoubleValue utilDouble)
{
	double doubleValue;
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::validateDoubleData" << endl;
#endif

	try
	{

		if(utilDouble.minimum <= utilityParameterValue && utilityParameterValue <= utilDouble.maximum)  doubleValue = utilityParameterValue;
		else doubleValue = (utilDouble.minimum + utilDouble.maximum)/2;
	}

	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::validateDoubleData: " << e.what() << endl;
	}
#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::validateDoubleData: " << status << endl;
#endif

	return doubleValue;

}


string PedalUtilityData::validateOptionData(string utilityParameterOption, UtilOption utilOption)
{
	string optionValue;
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::validateOptionData" << endl;
#endif

	try
	{
		for(auto & option : utilOption.validOptions)
		{
			if(utilityParameterOption.compare(option) == 0)
			{
				optionValue = utilityParameterOption; // utilityParameterOption is a valid option
				break;
			}
			optionValue = utilOption.validOptions[0]; //utilityParameterOption wasn't valid, so replacing with
														// first valid option
		}
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::validateOptionData: " << e.what() << endl;
	}
#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::validateOptionData: " << status << endl;
#endif

	return optionValue;

}

/*************************************************************************************************************/



#define dbg 0
int PedalUtilityData::readUtilityDataFromFile()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::readUtilityDataFromFile" << endl;
#endif

	int status = 0;

	this->pedalUtilityJson = fsInt.loadPedalUtilityJsonDataFromFileSystem();

	if(status >= 0)
	{
		// Keep data in human-readable form (i.e. string when appropriate) for as long
		// as is practical. As data is validated at UI, re-validation shouldn't be needed
		// when reading from file.
		try

		{
			string option = this->pedalUtilityJson["hostPc"]["os"].asString();
			this->hostUtil.os.option = this->validateOptionData(option, this->hostUtil.os);

			int jackPeriod = this->pedalUtilityJson["jack"]["period"].asInt();
			this->jackUtil.period.value = this->validateIntData(jackPeriod, this->jackUtil.period);

			int jackBuffer = this->pedalUtilityJson["jack"]["buffer"].asInt();
			this->jackUtil.buffer.value = this->validateIntData(jackBuffer, this->jackUtil.buffer);

			int processAntiAliasNumber = this->pedalUtilityJson["process"]["antiAliasingNumber"].asInt();
			this->processUtil.antiAliasingNumber.value =
							this->validateIntData(processAntiAliasNumber, this->processUtil.antiAliasingNumber);

			string processInputCouplingMode = this->pedalUtilityJson["process"]["inputCouplingMode"].asString();
			this->processUtil.inputCouplingMode.option =
							this->validateOptionData(processInputCouplingMode,this->processUtil.inputCouplingMode);

			string processWaveShapeMode = this->pedalUtilityJson["process"]["waveshaperMode"].asString();
			this->processUtil.waveshaperMode.option =
							this->validateOptionData(processWaveShapeMode,this->processUtil.waveshaperMode);


			this->processUtil.bufferSize.value = this->jackUtil.period.value;

			double gateCloseThres = this->pedalUtilityJson["noiseGate"]["closeThres"].asDouble();
			this->gateUtil.closeThres.value = this->validateDoubleData(gateCloseThres,this->gateUtil.closeThres);

			double gateOpenThres = this->pedalUtilityJson["noiseGate"]["openThres"].asDouble();
			this->gateUtil.openThres.value = this->validateDoubleData(gateOpenThres,this->gateUtil.openThres);

			double gateClosedGain = this->pedalUtilityJson["noiseGate"]["gain"].asDouble();
			this->gateUtil.gain.value = this->validateDoubleData(gateClosedGain,this->gateUtil.gain);

			double triggerHighThres = this->pedalUtilityJson["trigger"]["highThres"].asDouble();
			this->triggerUtil.highThres.value = this->validateDoubleData(triggerHighThres,this->triggerUtil.highThres);

			double triggerLowThres = this->pedalUtilityJson["trigger"]["lowThres"].asDouble();
			this->triggerUtil.lowThres.value = this->validateDoubleData(triggerLowThres,this->triggerUtil.lowThres);

			if(this->processUtil.antiAliasingNumber.value > 1)
			{
				 cout << "anti-aliasing: on." << endl;
			}
			else
			{
				 cout << "anti-aliasing: off." << endl;
			}
			 cout << "JACK period: " << this->jackUtil.period.value << "\tJACK buffer: " << this->jackUtil.buffer.value << endl;
			 cout << "Noise gate close threshold: " << this->gateUtil.closeThres.value << "\tNoise gate open threshold: " << this->gateUtil.openThres.value << endl;
			 cout << "Trigger low threshold: " << this->triggerUtil.lowThres.value << "\tTrigger high threshold: " << this->triggerUtil.highThres.value << endl;

		}
		catch(exception &e)
		{
			cout << "exception with PedalUtilityData::readUtilityDataFromFile: " << e.what() << endl;
			status = -1;

		}
	}
#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::readUtilityDataFromFile: " << status << endl;
#endif


	return status;
}

#define dbg 0
Json::Value PedalUtilityData::getUtilityDataForPedalUi()
{

	Json::Value pedalUiUtilityDataJson;
	string pedalUiUtilityDataString;


	pedalUiUtilityDataJson["noiseGate"]["openThres"] = this->gateUtil.openThres.value;
	pedalUiUtilityDataJson["noiseGate"]["closeThres"] = this->gateUtil.closeThres.value;
	pedalUiUtilityDataJson["noiseGate"]["gain"] = this->gateUtil.gain.value;
	pedalUiUtilityDataJson["trigger"]["highThres"] = this->triggerUtil.highThres.value;
	pedalUiUtilityDataJson["trigger"]["lowThres"] = this->triggerUtil.lowThres.value;
	pedalUiUtilityDataJson["hostPc"]["os"] = this->hostUtil.os.option;

	return pedalUiUtilityDataJson;
}

#define dbg 0
int PedalUtilityData::writeUtilityDataToFile()
{
	int status = 0;


	#if(dbg >= 1)
	cout << "***** ENTERING: PedalUtilityData::writeUtilityDataToFile" << endl;
#endif
	Json::FastWriter pedalUiUtilityDataStringWriter;
	string pedalUiUtilityDataString;

	try
	{
		this->pedalUtilityJson["envTrigger"]["highThres"] = this->triggerUtil.highThres.value;
		this->pedalUtilityJson["envTrigger"]["lowThres"] = this->triggerUtil.lowThres.value;
		this->pedalUtilityJson["hostPc"]["os"] = this->hostUtil.os.option;
		this->pedalUtilityJson["jack"]["buffer"] = this->jackUtil.buffer.value;
		this->pedalUtilityJson["jack"]["period"] = this->jackUtil.period.value;
		this->pedalUtilityJson["noiseGate"]["openThres"] =this->gateUtil.openThres.value;
		this->pedalUtilityJson["noiseGate"]["closeThres"] = this->gateUtil.closeThres.value;
		this->pedalUtilityJson["noiseGate"]["gain"] = this->gateUtil.gain.value;
		this->pedalUtilityJson["pedal"]["usbEnable"] = this->pedalUtil.usbEnable;
		this->pedalUtilityJson["process"]["antiAliasingNumber"] = this->processUtil.antiAliasingNumber.value;
		this->pedalUtilityJson["process"]["bufferSize"] = this->processUtil.bufferSize.value;
		this->pedalUtilityJson["process"]["inputCouplingMode"] = this->processUtil.inputCouplingMode.option;
		this->pedalUtilityJson["process"]["waveshaperMode"] = this->processUtil.waveshaperMode.option;



		fsInt.savePedalUtilityJsonDataToFileSystem(this->pedalUtilityJson);




	}
	catch (exception &e)
	{
		cout << "exception in PedalUtilityData::writeUtilityDataToFile: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	cout << "***** EXITING: PedalUtilityData::writeUtilityDataToFile: " << status << endl;
#endif
#if(dbg >= 2)
	cout << "pedalUiUtilityDataString: " << pedalUiUtilityDataString << endl;
#endif
	return status;
}


#define dbg 1
int PedalUtilityData::updateUtilityValue(string utilityParameterString, string utilityParameterValue)
{

	int utilType = 0;
	string utility;
	string utilityParameter;
	int  underscoreIndex = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::updateUtilityValue" << endl;
	 cout << utilityParameterString << ": " << utilityParameterValue << endl;
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
		 cout << "underscoreIndex: " << underscoreIndex<< "\t utility: " << utility << "\tutilityParameter: " << utilityParameter << endl;
	#endif

		if(utility.compare("process") == 0)
		{
			utilType = 0;
			if(utilityParameter.compare("antiAliasingNumber") == 0)
			{
				this->processUtil.antiAliasingNumber.value = static_cast<int>(strtol(utilityParameterValue.c_str(),NULL,10));

	#if(dbg >=2)
		 cout << "this->processingUtil.procUtil.antiAliasingNumber: " << this->processingUtil.procUtil.antiAliasingNumber << endl;
	#endif
			}
			else if(utilityParameter.compare("inputCouplingMode") == 0)
			{
				this->processUtil.inputCouplingMode.option = utilityParameterValue;
	#if(dbg >=2)
		 cout << "this->processingUtil.procUtil.inputCouplingMode: " << this->processingUtil.procUtil.inputCouplingMode << endl;
	#endif
			}
			else if(utilityParameter.compare("waveshaperMode") == 0)
			{
				this->processUtil.waveshaperMode.option = utilityParameterValue;
	#if(dbg >=2)
		 cout << "this->processingUtil.procUtil.waveshaperMode: " << this->processingUtil.procUtil.waveshaperMode << endl;
	#endif
			}

		}
		else if(utility.compare("jack") == 0)
		{
			utilType = 1;
			if(utilityParameter.compare("period") == 0)
			{
				this->jackUtil.period.value = static_cast<int>(strtol(utilityParameterValue.c_str(),NULL,10));
	#if(dbg >=2)
		 cout << "this->jackUtil.period: " << this->jackUtil.period << endl;
	#endif
			}
			else if(utilityParameter.compare("buffer") == 0)
			{
				this->jackUtil.buffer.value = static_cast<int>(strtol(utilityParameterValue.c_str(),NULL,10));
	#if(dbg >=2)
		 cout << "this->jackUtil.buffer: " << this->jackUtil.buffer << endl;
	#endif
			}
		}
		else if(utility.compare("noiseGate") == 0)
		{
			utilType = 2;
			if(utilityParameter.compare("closeThres") == 0)
			{
				this->gateUtil.closeThres.value = strtod(utilityParameterValue.c_str(),NULL);
	#if(dbg >=2)
		 cout << "this->processingUtil.noiseGateUtil.closeThres: " << this->processingUtil.noiseGateUtil.closeThres << endl;
	#endif
			}
			else if(utilityParameter.compare("openThres") == 0)
			{
				this->gateUtil.openThres.value = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		 cout << "this->processingUtil.noiseGateUtil.openThres: " << this->processingUtil.noiseGateUtil.openThres << endl;
	#endif
			}
			else if(utilityParameter.compare("gain") == 0)
			{
				this->gateUtil.gain.value = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		 cout << "this->processingUtil.noiseGateUtil.gain: " << this->processingUtil.noiseGateUtil.gain << endl;
	#endif
			}
		}
		else if(utility.compare("trigger") == 0)
		{
			utilType = 3;
			if(utilityParameter.compare("highThres") == 0)
			{
				this->triggerUtil.highThres.value = strtod(utilityParameterValue.c_str(),NULL);

	#if(dbg >=2)
		 cout << "this->processingUtil.triggerUtil.highThres: " << this->processingUtil.triggerUtil.highThres << endl;
	#endif
			}
			else if(utilityParameter.compare("lowThres") == 0)
			{
				this->triggerUtil.lowThres.value = strtod(utilityParameterValue.c_str(),NULL);
	#if(dbg >=2)
		 cout << "this->processingUtil.triggerUtil.lowThres: " << this->processingUtil.triggerUtil.lowThres << endl;
	#endif
			}
		}
		else if(utility.compare("hostPc") == 0)
		{
			utilType = 4;
			if(utilityParameter.compare("os") == 0)
			{
				this->hostUtil.os.option = utilityParameterValue;
			}
		}
	}
	catch(exception &e)
	{
		cout << "exception with PedalUtilityData::updateUtilityValue: " << e.what() << endl;
		utilType = -1;

	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::updateUtilityValue: " << utilType << endl;
#endif


	return utilType;
}


ProcessUtility PedalUtilityData::getProcessUtility()
{
	ProcessUtility procUtil;
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getProcessUtility" << endl;
#endif

	try
	{
		procUtil.bufferSize.value = this->processUtil.bufferSize.value;
		procUtil.inputCouplingMode.option = this->processUtil.inputCouplingMode.option;
		procUtil.antiAliasingNumber.value = this->processUtil.antiAliasingNumber.value;
		procUtil.waveshaperMode.option = this->processUtil.waveshaperMode.option;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getProcessUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getProcessUtility" << endl;
#endif

	return procUtil;

}

JackUtility PedalUtilityData::getJackUtility()
{
	JackUtility jackUtil;

#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getJackUtility" << endl;
#endif

	try
	{
		jackUtil.buffer.value = this->jackUtil.buffer.value;
		jackUtil.period.value = this->jackUtil.period.value;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getJackUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getJackUtility" << endl;
#endif

	return jackUtil;
}


NoiseGateUtility PedalUtilityData::getNoiseGateUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getNoiseGateUtil" << endl;
#endif
	NoiseGateUtility gateUtil;

	try
	{
		gateUtil.closeThres.value = this->gateUtil.closeThres.value;
		gateUtil.openThres.value = this->gateUtil.openThres.value;
		gateUtil.gain.value = this->gateUtil.gain.value;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getNoiseGateUtil: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getNoiseGateUtil" << endl;
#endif
	 return gateUtil;
}
EnvTriggerUtility PedalUtilityData::getEnvTriggerUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getEnvTriggerUtil" << endl;
#endif
	 EnvTriggerUtility triggerUtil;
	try
	{
		triggerUtil.highThres.value = this->triggerUtil.highThres.value;
		triggerUtil.lowThres.value = this->triggerUtil.lowThres.value;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getEnvTriggerUtil: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getEnvTriggerUtil" << endl;
#endif
	 return triggerUtil;
}
PedalUtility PedalUtilityData::getPedalUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getPedalUtil" << endl;
#endif
	 PedalUtility pedalUtil;
	try
	{
		pedalUtil.usbEnable = this->pedalUtil.usbEnable;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getPedalUtil: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getPedalUtil" << endl;
#endif
	 return pedalUtil;
}
HostPcUtility PedalUtilityData::getHostUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: PedalUtilityData::getHostUtil" << endl;
#endif
	 HostPcUtility hostUtil;
	try
	{
		hostUtil.os.option = this->hostUtil.os.option;
	}
	catch(exception &e)
	{
		cout << "exception in PedalUtilityData::getHostUtil: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: PedalUtilityData::getHostUtil" << endl;
#endif
	 return hostUtil;
}

}
