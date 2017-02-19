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


extern ComboStruct combo;
extern int currentComboStructIndex;
extern int oldComboStructIndex;


/**************************************
#if(dbg >= 1)
	cout << "***** ENTERING: ProcessingControl::" << endl;
	cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ProcessingControl::: " << status << endl;
#endif

#if(dbg >=2)
#endif
********************************************/
extern int globalComboIndex;
extern bool justPoweredUp;
extern bool inputsSwitched;
using std::cout;
using std::endl;

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
	//right/control port signal switches (audio/control_voltage/ground)
	char tempStr[5];
	//this->processing = new Combo; /
	this->portConSwitch[0] = GPIOClass(CONTROL_PIN_0_NUMBER);
	this->portConSwitch[0].export_gpio();
	strcpy(tempStr,"out");
	this->portConSwitch[0].setdir_gpio(tempStr);
	//this->portConSwitch[0].setdir_gpio("out");
	this->portConSwitch[0].setval_gpio(1);

	this->portConSwitch[1] = GPIOClass(CONTROL_PIN_1_NUMBER);
	this->portConSwitch[1].export_gpio();
	this->portConSwitch[1].setdir_gpio(tempStr);
	this->portConSwitch[1].setval_gpio(0);

	this->portConSwitch[2] = GPIOClass(CONTROL_PIN_2_NUMBER);
	this->portConSwitch[2].export_gpio();
	this->portConSwitch[2].setdir_gpio(tempStr);
	this->portConSwitch[2].setval_gpio(0);
}


ProcessingControl::~ProcessingControl()
{

}

int audioCallbackComboIndex = 100;

#if(COMBO_DATA_VECTOR == 1 || COMBO_DATA_ARRAY == 1)
	#define dbg 1
	int ProcessingControl::load(int comboIndex)
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "ENTERING: ProcessingControl::load (using Array or Vector of ComboDataInts)" << endl;
	#endif

		this->processing->comboIndex = comboIndex;
		cout << "comboIndex: " << comboIndex << endl;
		combo = getComboStructFromComboIndex(this->processing->comboIndex);
		usleep(100000);
		this->processing->updateProcessing = true;
		this->processing->processingUpdated = false;


	#if(dbg >= 1)
		cout << "EXIT: ProcessingControl::load  (using Array or Vector of ComboDataInts)" << endl;
	#endif
		return status;
	}
#elif(COMBO_DATA_MAP == 1)
	#define dbg 1
	int ProcessingControl::load(string comboName)
	{
		int status = 0;
		int loadIndex = 0;
	#if(dbg >= 1)
		cout << "ENTERING: ProcessingControl::load (using Map of ComboDataInts)" << endl;
		cout << "comboName: " << comboName << endl;
	#endif

		this->processing->comboName = comboName;
		cout << "comboName: " << comboName << endl;
		//oldCombo = transferComboStruct(combo);
		cout << "stopping combo." << endl;
		this->processing->stopCombo();

		/*oldComboStructIndex = currentComboStructIndex;
		currentComboStructIndex ^= 1;*/
		cout << "oldComboStructIndex: " << oldComboStructIndex << "\tcurrentComboStructIndex: " << currentComboStructIndex << endl;
		combo = getComboStructFromComboName(this->processing->comboName);

		cout << "loading combo." << endl;
		this->processing->loadCombo();
		this->processing->processingUpdated = true;
		this->processing->updateProcessing = false;
		this->processing->processingContextAllocationError = false;
		cout << "combo loaded." << endl;


	#if(dbg >= 1)
		cout << "EXIT: ProcessingControl::load (using Map of ComboDataInts): " << status << endl;
	#endif
		return status;
	}
#endif



#define dbg 1
int ProcessingControl::start() // start clients and connect them
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ProcessingControl::start" << endl;
#endif


#if(dbg >= 1)
	cout << "starting combo." << endl;
#endif
	this->processing = new Processing;// initial ports from constructor created here.
	//this->processing->load();
#if(dbg >= 1)
	cout << "processing Constructor successful." << endl;
#endif
	this->processing->start();	// activate the client
#if(dbg >= 1)
	cout << "process started." << endl;
#endif
	// reporting some client info
#if(dbg >= 1)
	cout << endl << "my name: " << this->processing->getName() << endl;
#endif

	// test to see if it is real time
	if (this->processing->isRealTime())
#if(dbg >= 1)
#endif
		cout << "is realtime " << endl;
	else
#if(dbg >= 1)
#endif
		cout << "is not realtime " << endl;

	//connect our ports to physical ports
	this->processing->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
	this->processing->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1

	//this->connectToPhysical(0,0);	// connects this client in port 0 to physical source port 0
	//this->connectToPhysical(1,1);	// connects this client in port 1 to physical source port 1
	//char connToString[25];
#if(dbg >= 1)
	cout << "connecting processes." << endl;
#endif

	this->processing->connectToPhysical(0,0);		// connects this client out port 0 to physical destination port 0
#if(dbg >= 1)
	cout << "connecting processes: " << this->processing->getOutputPortName(0) << endl;
#endif
	this->processing->connectToPhysical(1,1);		// connects this client out port 1 to physical destination port 1
#if(dbg >= 1)
	cout << "connecting processes: " << this->processing->getOutputPortName(1) << endl;
#endif


	if(justPoweredUp)
	{
		inputsSwitched = this->processing->areInputsSwitched();
		justPoweredUp = false;
	}

#if(dbg >= 1)
	cout << "***** EXITING: ProcessingControl::start: " << status << endl;
#endif

	return status;

}

#define dbg 0
int ProcessingControl::stop() // stop clients and disconnect them
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ProcessingControl::stop" << endl;
#endif

	//this->processing->bypassAll(); // set to straight-thru to avoid XRun
	//this->processing->stopEffects();
	//sleep(2);
	this->processing->disconnectInPort(0);	// Disconnecting ports.
#if(dbg == 1)
	cout << "this->disconnectInPort(0)" << endl;
#endif
	this->processing->disconnectOutPort(1);
#if(dbg == 1)
	cout << "this->processing->disconnectOutPort(1)" << endl;
#endif
	this->processing->stop();
	this->processing->close();	// stop client.
#if(dbg == 1)
	cout << "this->processing->close()" << endl;
#endif
	delete this->processing;	// always clean up after yourself.
#if(dbg == 1)
	cout << "delete this->processing" << endl;
#endif
	//exit(0);
#if(dbg >= 1)
	cout << "***** EXITING: ProcessingControl::stop: " << status << endl;
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
int ProcessingControl::updateFootswitch(int *footswitchStatus)
{
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateFootswitch" << endl;
#endif
	int status = 0;

	this->processing->updateFootswitch(footswitchStatus);

#if(dbg >= 1)
	cout << "EXIT: ProcessingControl::updateFootswitch" << endl;
#endif
	return status;
}

#define dbg 1

int ProcessingControl::updateProcessParameter(int parameterIndex, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateProcessParameter" << endl;
	cout << "parameterIndex: " << parameterIndex << "\tparameterValue: "  << parameterValue << endl;
#endif

#if(COMBO_DATA_VECTOR == 1)
	IndexedParameter parameter = comboDataVector[this->processing->comboIndex].sortedParameterArray[parameterIndex];
#elif(COMBO_DATA_ARRAY == 1)
	IndexedParameter parameter = comboDataArray[this->processing->comboIndex].sortedParameterArray[parameterIndex];
#elif(COMBO_DATA_MAP == 1)
	IndexedParameter parameter = comboDataMap[this->processing->comboName].sortedParameterArray[parameterIndex];
#endif
	string processName = parameter.processName; // need to use process name instead of index because process index
											// in parameterArray does not correspond to process index in processSequence

#if(dbg >= 2)
	cout << "processName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.processName
			<< "\t\tabsProcessIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.absProcessIndex
			<< "\t\tparamName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramName
			<< "\t\tprocessParamIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.processParamIndex
			<< "\t\tabsParamIndex: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.absParamIndex
			<< "\t\tparamName: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramName
			<< "\t\tparamValue: " << /*comboData.sortedParameterArray[parameterIndex]*/parameter.paramValue << endl;
#endif

	int processParameterIndex = parameter.processParamIndex;
#if(dbg >= 2)
	cout << "process: " << processName << "\t\tparameter: " << parameter.paramName << endl;
#endif
	this->processing->updateProcessParameter(processName, processParameterIndex, parameterValue);

#if(dbg >= 1)
	cout << "EXIT: ProcessingControl::updateProcessParameter: " << status << endl;
#endif

	return status;
}

#define dbg 1
int ProcessingControl::updateControlParameter(int parameterIndex, int parameterValue)
{
	int status = 0;

#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
	cout << "parameterIndex: " << parameterIndex << "\tparameterValue: "  << parameterValue << endl;
#endif

#if(dbg >= 2)
	cout << "parameterIndex: " << parameterIndex << endl;
	cout << "parameterValue: " << parameterValue << endl;
	cout << "comboDataArray[this->processing->comboIndex].controlParameterArray[parameterIndex]" << endl;
	cout << "this->processing->comboIndex: " << this->processing->comboIndex << endl;
#endif

#if(COMBO_DATA_VECTOR == 1)
	IndexedControlParameter parameter = comboDataVector[this->processing->comboIndex].controlParameterArray[parameterIndex];
#elif(COMBO_DATA_ARRAY == 1)
	IndexedControlParameter parameter = comboDataArray[this->processing->comboIndex].controlParameterArray[parameterIndex];
#elif(COMBO_DATA_MAP == 1)
	IndexedControlParameter parameter = comboDataMap[this->processing->comboName].controlParameterArray[parameterIndex];
#endif
	string controlName = parameter.controlName; // need to use control name instead of index because control index
											// in parameterArray does not correspond to control index in controlSequence

#if(dbg >= 2)
	//int procParamIndex = comboData.controlParameterArray[parameterIndex].absProcessParamIndex;
	cout << "controlName: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlName
			<< "\tabsControlIndex: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.absControlIndex
			<< "\tcontrolParamName: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamName
			<< "\tcontrolParamIndex: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamIndex
			<< "\tcontrolParamValue: " << /*comboData.controlParameterArray[parameterIndex]*/parameter.controlParamValue << endl;
#endif

	int controlParameterIndex = parameter.controlParamIndex;
	this->processing->updateControlParameter(controlName, controlParameterIndex, parameterValue);

#if(dbg >= 1)
	cout << "control: " << controlName << "\t\tparameter: " << parameter.controlParamName << endl;
	cout << "EXITING: ProcessingControl::updateControlParameter: " << status << endl;
#endif

	return status;
}


int ProcessingControl::enableEffects()
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ProcessingControl::enableEffects" << endl;
#endif


	status = this->processing->enableProcessing();
	//this->start();
#if(dbg >= 1)
	cout << "***** EXITING: ProcessingControl::enableEffects: " << status << endl;
#endif
	return status;
}

int ProcessingControl::disableEffects()
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ProcessingControl::disableEffects" << endl;
#endif


	status = this->processing->disableProcessing();
	//this->stop();
#if(dbg >= 1)
	cout << "***** EXITING: ProcessingControl::disableEffects: " << status << endl;
#endif
	return status;
}

#define dbg 2
double ProcessingControl::getOutputAmplitudes(void)
{
	double amplitude = 0.000;
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::getOutputAmplitudes" << endl;
#endif

	amplitude = this->processing->getOutputAmplitudes();

#if(dbg >= 1)
	cout << "EXITING: ProcessingControl::getOutputAmplitudes: " << amplitude << endl;
#endif
	return amplitude;
}


