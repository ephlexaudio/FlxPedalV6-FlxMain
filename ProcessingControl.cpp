/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */


#include "config.h"
#include "ProcessingControl.h"


/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1*/

using namespace std;

extern bool debugOutput;

extern ComboStruct combo;
extern int currentComboStructIndex;
extern int oldComboStructIndex;





#define FS1_LED	30
#define FS2_LED	31
#define FS1_PIN	33 // 33 and 32 are switch around
#define FS2_PIN	32

/**************************************
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::" << endl;
	if(debugOutput) cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::: " << status << endl;
#endif

#if(dbg >=2)
#endif
********************************************/
extern int globalComboIndex;
extern bool justPoweredUp;
extern bool inputsSwitched;

#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43

#if(COMBO_DATA_VECTOR == 1)
	extern vector<ComboDataInt> comboDataVector;
#elif(COMBO_DATA_ARRAY == 1)
	extern ComboDataInt comboDataArray[15];
#elif(COMBO_DATA_MAP == 1)
	extern map<string, ComboDataInt> comboDataMap;
#endif


ProcessingControl::ProcessingControl()
{

	//this->footswitchStatus = {false, false};
	//right/control port signal switches (audio/control_voltage/ground)
	char gpioStr[5];
	strcpy(gpioStr,"out");
	//this->processing = new Combo; /
	/*this->portConSwitch[0] = GPIOClass(CONTROL_PIN_0_NUMBER);
	this->portConSwitch[0].export_gpio();
	this->portConSwitch[0].setdir_gpio(gpioStr);
	//this->portConSwitch[0].setdir_gpio("out");
	this->portConSwitch[0].setval_gpio(1);

	this->portConSwitch[1] = GPIOClass(CONTROL_PIN_1_NUMBER);
	this->portConSwitch[1].export_gpio();
	this->portConSwitch[1].setdir_gpio(gpioStr);
	this->portConSwitch[1].setval_gpio(0);

	this->portConSwitch[2] = GPIOClass(CONTROL_PIN_2_NUMBER);
	this->portConSwitch[2].export_gpio();
	this->portConSwitch[2].setdir_gpio(gpioStr);
	this->portConSwitch[2].setval_gpio(0);*/

	this->footswitchLed[0] = GPIOClass(FS1_LED/*"4"*/);//, "in");
	this->footswitchLed[0].export_gpio();
	this->footswitchLed[0].setdir_gpio(gpioStr);
	this->footswitchLed[0].setval_gpio(1);

	this->footswitchLed[1] = GPIOClass(FS2_LED/*"5"*/);//, "out");
	this->footswitchLed[1].export_gpio();
	this->footswitchLed[1].setdir_gpio(gpioStr);
	this->footswitchLed[1].setval_gpio(1);

	strcpy(gpioStr,"in");
	this->footswitchPin[0] = GPIOClass(FS1_PIN/*"4"*/);//, "in");
	this->footswitchPin[0].export_gpio();
	this->footswitchPin[0].setdir_gpio(gpioStr);

	this->footswitchPin[1] = GPIOClass(FS2_PIN/*"5"*/);//, "out");
	this->footswitchPin[1].export_gpio();
	this->footswitchPin[1].setdir_gpio(gpioStr);


}


ProcessingControl::~ProcessingControl()
{

}

int audioCallbackComboIndex = 100;
#define dbg 0


#if(COMBO_DATA_MAP == 1)
	int ProcessingControl::load(string comboName)
	{
		int status = 0;
		int loadIndex = 0;
	#if(dbg >= 1)
		if(debugOutput) cout << "ENTERING: ProcessingControl::load (using Map of ComboDataInts)" << endl;
		if(debugOutput) cout << "comboName: " << comboName << endl;
	#endif

		this->processing->comboName = comboName;
#if(dbg >= 2)
		if(debugOutput) cout << "comboName: " << comboName << endl;

		if(debugOutput) cout << "stopping combo." << endl;
#endif

		/*oldComboStructIndex = currentComboStructIndex;
		currentComboStructIndex ^= 1;*/
#if(dbg >= 2)
		if(debugOutput) cout << "oldComboStructIndex: " << oldComboStructIndex << "\tcurrentComboStructIndex: " << currentComboStructIndex << endl;
#endif

		combo = getComboStructFromComboName(this->processing->comboName);
		if(combo.name.empty() == false)
		{
			if(debugOutput) cout << "loading combo: " << this->processing->comboName  << endl;
			this->processing->loadCombo();
			this->processing->processingUpdated = true;
			//this->processing->updateProcessing = false;
			this->processing->processingContextAllocationError = false;
			if(debugOutput) cout << "combo loaded." << endl;
		}
		else
		{
			if(debugOutput) cout << "combo struct is blank or invalid." << endl;
			status = -1;
		}


	#if(dbg >= 1)
		if(debugOutput) cout << "EXIT: ProcessingControl::load (using Map of ComboDataInts): " << status << endl;
	#endif
		return status;
	}
#endif



#define dbg 0
int ProcessingControl::start() // start clients and connect them
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::start" << endl;
#endif


#if(dbg >= 1)
	if(debugOutput) cout << "starting combo." << endl;
#endif
	this->processing = new Processing;// initial ports from constructor created here.
#if(dbg >= 1)
	if(debugOutput) cout << "processing Constructor successful." << endl;
#endif
	this->processing->start();	// activate the client
#if(dbg >= 1)
	if(debugOutput) cout << "process started." << endl;
#endif
	// reporting some client info
#if(dbg >= 1)
	if(debugOutput) cout << "my name: " << this->processing->getName() << endl;
#endif

	// test to see if it is real time
	if (this->processing->isRealTime())
#if(dbg >= 1)
#endif
		if(debugOutput) cout << "is realtime " << endl;
	else
#if(dbg >= 1)
#endif
		if(debugOutput) cout << "is not realtime " << endl;

	//connect our ports to physical ports
	this->processing->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
	this->processing->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1

	//char connToString[25];
#if(dbg >= 1)
	if(debugOutput) cout << "connecting processes." << endl;
#endif

	this->processing->connectToPhysical(0,0);		// connects this client out port 0 to physical destination port 0
#if(dbg >= 1)
	if(debugOutput) cout << "connecting processes: " << this->processing->getOutputPortName(0) << endl;
#endif
	this->processing->connectToPhysical(1,1);		// connects this client out port 1 to physical destination port 1
#if(dbg >= 1)
	if(debugOutput) cout << "connecting processes: " << this->processing->getOutputPortName(1) << endl;
#endif


	if(justPoweredUp)
	{
		inputsSwitched = this->processing->areInputsSwitched();
		justPoweredUp = false;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::start: " << status << endl;
#endif

	return status;

}

#define dbg 0
int ProcessingControl::stop() // stop clients and disconnect them
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::stop" << endl;
#endif

	this->processing->disconnectInPort(0);	// Disconnecting ports.
#if(dbg >= 1)
	if(debugOutput) cout << "this->disconnectInPort(0)" << endl;
#endif
	this->processing->disconnectOutPort(1);
#if(dbg >= 1)
	if(debugOutput) cout << "this->processing->disconnectOutPort(1)" << endl;
#endif
	this->processing->stop();
	this->processing->close();	// stop client.
#if(dbg >= 1)
	if(debugOutput) cout << "this->processing->close()" << endl;
#endif
	delete this->processing;	// always clean up after yourself.
#if(dbg >= 1)
	if(debugOutput) cout << "delete this->processing" << endl;
#endif
	//exit(0);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::stop: " << status << endl;
#endif

	return status;
}

/*int ProcessingControl::getProcessData(int index, double *data)
{
	int status = 0;

	this->processing->getProcessData(index, data);

	return status;
}*/

/*int ProcessingControl::clearProcessData(int index, double *data)
{
	int status = 0;

	this->processing->clearProcessData(index, data);

	return status;
}*/

#define dbg 0

int ProcessingControl::updateProcessParameter(int parameterIndex, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::updateProcessParameter" << endl;
	if(debugOutput) cout << "parameterIndex: " << parameterIndex << "\tparameterValue: "  << parameterValue << endl;
#endif

#if(COMBO_DATA_VECTOR == 1)
	IndexedProcessParameter parameter = comboDataVector[this->processing->comboIndex].sortedParameterArray[parameterIndex];
#elif(COMBO_DATA_ARRAY == 1)
	IndexedProcessParameter parameter = comboDataArray[this->processing->comboIndex].sortedParameterArray[parameterIndex];
#elif(COMBO_DATA_MAP == 1)
	IndexedProcessParameter parameter = comboDataMap[this->processing->comboName].sortedParameterArray[parameterIndex];
#endif
	string processName = parameter.processName; // need to use process name instead of index because process index
											// in parameterArray does not correspond to process index in processSequence

#if(dbg >= 2)
	if(debugOutput) cout << "processName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.processName
			<< "\t\tabsProcessIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.absProcessIndex
			<< "\t\tparamName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramName
			<< "\t\tprocessParamIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.processParamIndex
			<< "\t\tabsParamIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.absParamIndex
			<< "\t\tparamName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramName
			<< "\t\tparamValue: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramValue << endl;
#endif

	int processParameterIndex = parameter.processParamIndex;
#if(dbg >= 2)
	if(debugOutput) cout << "process: " << processName << "\t\tparameter: " << parameter.paramName << endl;
#endif
	this->processing->updateProcessParameter(processName, processParameterIndex, parameterValue);

#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: ProcessingControl::updateProcessParameter: " << status << endl;
#endif

	return status;
}

#define dbg 0
int ProcessingControl::updateControlParameter(int parameterIndex, int parameterValue)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
	if(debugOutput) cout << "parameterIndex: " << parameterIndex << "\tparameterValue: "  << parameterValue << endl;
#endif

#if(dbg >= 2)
	if(debugOutput) cout << "parameterIndex: " << parameterIndex << endl;
	if(debugOutput) cout << "parameterValue: " << parameterValue << endl;
	if(debugOutput) cout << "comboDataArray[this->processing->comboIndex].controlParameterArray[parameterIndex]" << endl;
	if(debugOutput) cout << "this->processing->comboIndex: " << this->processing->comboIndex << endl;
#endif

#if(COMBO_DATA_VECTOR == 1)
	IndexedControlParameter parameter = comboDataVector[this->processing->comboIndex].controlParameterArray[parameterIndex];
#elif(COMBO_DATA_ARRAY == 1)
	IndexedControlParameter parameter = comboDataArray[this->processing->comboIndex].controlParameterArray[parameterIndex];
#elif(COMBO_DATA_MAP == 1)
	string tempString;
	IndexedControlParameter parameter = comboDataMap[this->processing->comboName].controlParameterArray[parameterIndex];
#endif
	string controlName = parameter.controlName; // need to use control name instead of index because control index
											// in parameterArray does not correspond to control index in controlSequence

#if(dbg >= 2)
	//int procParamIndex = comboData.controlParameterArray[parameterIndex].absProcessParamIndex;
	if(debugOutput) cout << "controlName: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlName
			<< "\tabsControlIndex: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.absControlIndex
			<< "\tcontrolParamName: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamName
			<< "\tcontrolParamIndex: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamIndex
			<< "\tcontrolParamValue: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamValue << endl;
#endif

	int controlParameterIndex = parameter.controlParamIndex;
	this->processing->updateControlParameter(controlName, controlParameterIndex, parameterValue);

#if(dbg >= 1)
	if(debugOutput) cout << "control: " << controlName << "\t\tparameter: " << parameter.controlParamName << endl;
	if(debugOutput) cout << "EXITING: ProcessingControl::updateControlParameter: " << status << endl;
#endif

	return status;
}

#define dbg 0
int ProcessingControl::enableEffects()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::enableEffects" << endl;
#endif


	status = this->processing->enableProcessing();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::enableEffects: " << status << endl;
#endif
	return status;
}

int ProcessingControl::disableEffects()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::disableEffects" << endl;
#endif


	status = this->processing->disableProcessing();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::disableEffects: " << status << endl;
#endif
	return status;
}

#define dbg 1
int ProcessingControl::enableAudioOutput()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::enableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x07");
	//status = this->processing->enableAudioOutput();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::enableAudioOutput: " << status << endl;
#endif
	return status;
}


int ProcessingControl::disableAudioOutput()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x0F");
	//status = this->processing->disableAudioOutput();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::disableAudioOutput: " << status << endl;
#endif
	return status;
}

#define dbg 0
double ProcessingControl::getOutputAmplitudes(void)
{
	double amplitude = 0.000;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::getOutputAmplitudes" << endl;
#endif

	//amplitude = this->processing->getOutputAmplitudes();

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: ProcessingControl::getOutputAmplitudes: " << amplitude << endl;
#endif
	return amplitude;
}



#define dbg 0
int ProcessingControl::readFootswitches(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::readFootswitches" << endl;
#endif

	int footSwitchState0;
	int footSwitchState1;


	for(int i = 0; i < FOOTSWITCH_COUNT; i++)
	{
		// debounce switches first
		this->footswitchPin[i].getval_gpio(footSwitchState0);
		usleep(5000);
		this->footswitchPin[i].getval_gpio(footSwitchState1);

		if((footSwitchState0 == footSwitchState1))
		{
			if((footSwitchState0 == 0) && (this->footSwitchPressed[i] == true))
			{
				this->footSwitchPressed[i] = false;
				if(debugOutput) cout << "footswitch[" << i << "] = 0: " << this->footswitchStatus[i] << endl;
			}
			else if((footSwitchState0 == 1) && (this->footSwitchPressed[i] == false))
			{
				this->footswitchStatus[i] ^= true; // footswitches are active low
				if(debugOutput) cout << "footswitch[" << i << "] = 1: " << this->footswitchStatus[i] << endl;
				this->footswitchLed[i].setval_gpio((footswitchStatus[i]?0:1));
				this->footSwitchPressed[i] = true;
			}
		}
	}


	this->processing->updateFootswitch(this->footswitchStatus);
	//if(debugOutput) cout << "footswitchStatusArray: " << footswitchStatusArray[0] << "," << footswitchStatusArray[1] << "," << footswitchStatusArray[2] << "," << footswitchStatusArray[3] << endl;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::readFootswitches:" << this->footswitchStatus[0] << "," << this->footswitchStatus[1] << endl;
#endif

	return status;
}

/*#define dbg 0
int ProcessingControl::updateFootswitch(int *footswitchStatus)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::updateFootswitch" << endl;
#endif
	int status = 0;

	this->processing->updateFootswitch(footswitchStatus);

#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: ProcessingControl::updateFootswitch" << endl;
#endif
	return status;
}*/

int ProcessingControl::setNoiseGateCloseThreshold(float closeThres)
{
	int status = 0;

	this->processing->setNoiseGateCloseThreshold(closeThres);

	return status;
}

int ProcessingControl::setNoiseGateOpenThreshold(float openThres)
{
	int status = 0;

	this->processing->setNoiseGateOpenThreshold(openThres);

	return status;
}

int ProcessingControl::setNoiseGateGain(float gain)
{
	int status = 0;

	this->processing->setNoiseGateGain(gain);

	return status;
}

int ProcessingControl::setTriggerLowThreshold(float lowThres)
{
	int status = 0;

	this->processing->setTriggerLowThreshold(lowThres);

	return status;
}

int ProcessingControl::setTriggerHighThreshold(float highThres)
{
	int status = 0;

	this->processing->setTriggerHighThreshold(highThres);

	return status;
}
