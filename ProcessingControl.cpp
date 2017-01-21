/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */



#include "ProcessingControl.h"
extern ComboDataInt comboData[15];
extern int globalComboIndex;
extern bool justPoweredUp;
extern bool inputsSwitched;
using std::cout;
using std::endl;

#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43



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


#define dbg 1
int ProcessingControl::load(int comboIndex)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::load" << endl;
#endif
	/*this->stop();
	usleep(100000);
	this->start();
	cout << "restarting JACK." << endl;*/
	this->processing->comboIndex = comboIndex;
	/*this->processing->enableComboBypass();
	this->processing->stopCombo();
	this->processing->loadCombo(comboIndex);
	this->processing->disableComboBypass();*/

#if(dbg >= 1)
	cout << "EXIT: ProcessingControl::load" << endl;
#endif
	return status;
}


#define dbg 0
int ProcessingControl::start() // start clients and connect them
{
	int status = 0;

#if(dbg == 1)
	cout << "starting combo." << endl;
#endif
	this->processing = new Processing;// initial ports from constructor created here.
	//this->processing->load();
	/*
#if(dbg == 1)
	cout << "process created." << endl;
#endif
	this->processing->addOutPort("proc_out0");		// add new out port (2) named "blahout0"
#if(dbg == 1)
	cout << "port added: " << this->processing->getOutputPortName(0) << endl;
#endif
	this->processing->addOutPort("proc_out1");		// add new out port (3) named "blahout1"
#if(dbg == 1)
	cout << "port added: " << this->processing->getOutputPortName(1) << endl;
#endif
	this->processing->addInPort("proc_in0");		// add new in port (2) named "blahin0"
#if(dbg == 1)
	cout << "port added: " << this->processing->getInputPortName(0) << endl;
#endif
	this->processing->addInPort("proc_in1");		// add new in port (3) named "blahin1"
#if(dbg == 1)
	cout << "port added: " << this->processing->getInputPortName(1) << endl;
#endif
	//this->bypassAll(); // set to straight-thru to avoid XRun
#if(dbg == 1)
	cout << "process bypassed." << endl;
#endif
	*/
	this->processing->start();	// activate the client
#if(dbg == 1)
	cout << "process started." << endl;
#endif
	// reporting some client info
#if(dbg == 1)
	cout << endl << "my name: " << this->processing->getName() << endl;
#endif

	// test to see if it is real time
	if (this->processing->isRealTime())
#if(dbg == 1)
#endif
		cout << "is realtime " << endl;
	else
#if(dbg == 1)
#endif
		cout << "is not realtime " << endl;

	//connect our ports to physical ports
	this->processing->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
	this->processing->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1

	//this->connectToPhysical(0,0);	// connects this client in port 0 to physical source port 0
	//this->connectToPhysical(1,1);	// connects this client in port 1 to physical source port 1
	//char connToString[25];
#if(dbg == 1)
	cout << "connecting processes." << endl;
#endif

	this->processing->connectToPhysical(0,0);		// connects this client out port 0 to physical destination port 0
#if(dbg == 1)
	cout << "connecting processes: " << this->processing->getOutputPortName(0) << endl;
#endif
	this->processing->connectToPhysical(1,1);		// connects this client out port 1 to physical destination port 1
#if(dbg == 1)
	cout << "connecting processes: " << this->processing->getOutputPortName(1) << endl;
#endif

/*
	// exception test example
	cout << endl << "testing exceptions, trying to connect to a fake port" << endl;
	try {
		this->processing->connectFrom(1,"fakeportname");
	} catch (std::runtime_error e){
		cout << "YES: " << e.what() << endl;
	}

	cout << "try to query a port we don't have" << endl;
	try {
		this->processing->numConnectionsInPort(100);
	} catch (std::range_error e){
		cout << "YES: " << e.what() << endl;
	}

	// port connection status
#if(dbg == 1)
	cout << endl;
	cout << "proc 0 output 0 is connected to " << this->processing->numConnectionsOutPort(0) << " ports" << endl;
	cout << "proc 0 output 1 is connected to " << this->processing->numConnectionsOutPort(1) << " ports" << endl;
	cout << "proc 0 input 0 is connected to " << this->processing->numConnectionsInPort(0) << " ports" << endl;
	cout << "proc 0 input 1 is connected to " << this->processing->numConnectionsInPort(1) << " ports" << endl;
#endif

	//print names
#if(dbg == 1)
	cout << endl;
	cout << "inport names:" << endl;
	for(unsigned int i = 0; i < this->processing->inPorts(); i++)
		cout << "\t" << this->processing->getInputPortName(i) << endl;

	cout << "outport names:" << endl;
	for(unsigned int i = 0; i < this->processing->outPorts(); i++)
		cout << "\t" << this->processing->getOutputPortName(i) << endl;
#endif
	*/
	if(justPoweredUp)
	{
		inputsSwitched = this->processing->areInputsSwitched();
		justPoweredUp = false;
	}

	/*sleep(3);
	cout << "adding a new output port!" << endl;
	this->addOutPort("newOut1");
	sleep(3);*/

	return status;

}

#define dbg 0
int ProcessingControl::stop() // stop clients and disconnect them
{
	int status = 0;
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

#define dbg 0

int ProcessingControl::updateProcessParameter(int parameterIndex, int parameterValue)
{
	int status = 0;

	IndexedParameter parameter = comboData[this->processing->comboIndex].sortedParameterArray[parameterIndex];
	string processName = parameter.processName; // need to use process name instead of index because process index
											// in parameterArray does not correspond to process index in processSequence

#if(dbg >= 2)
	cout << "processName: " << comboData.sortedParameterArray[parameterIndex].processName
			<< "\t\tabsProcessIndex: " << comboData.sortedParameterArray[parameterIndex].absProcessIndex
			<< "\t\tparamName: " << comboData.sortedParameterArray[parameterIndex].paramName
			<< "\t\tprocessParamIndex: " << comboData.sortedParameterArray[parameterIndex].processParamIndex
			<< "\t\tabsParamIndex: " << comboData.sortedParameterArray[parameterIndex].absParamIndex
			<< "\t\tparamName: " << comboData.sortedParameterArray[parameterIndex].paramName
			<< "\t\tparamValue: " << comboData.sortedParameterArray[parameterIndex].paramValue << endl;
#endif

	int processParameterIndex = parameter.processParamIndex;
#if(dbg >= 2)
	cout << "process: " << processName << "\t\tparameter: " << parameter.paramName << endl;
#endif
	this->processing->updateProcessParameter(processName, processParameterIndex, parameterValue);

	return status;
}

#define dbg 0
int ProcessingControl::updateControlParameter(int parameterIndex, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
#endif
	IndexedControlParameter parameter = comboData[this->processing->comboIndex].controlParameterArray[parameterIndex];
	string controlName = parameter.controlName; // need to use control name instead of index because control index
											// in parameterArray does not correspond to control index in controlSequence

#if(dbg >= 2)
	//int procParamIndex = comboData.controlParameterArray[parameterIndex].absProcessParamIndex;
	cout << "controlName: " << comboData.controlParameterArray[parameterIndex].controlName
			<< "\tabsControlIndex: " << comboData.controlParameterArray[parameterIndex].absControlIndex
			<< "\tcontrolParamName: " << comboData.controlParameterArray[parameterIndex].controlParamName
			<< "\tcontrolParamIndex: " << comboData.controlParameterArray[parameterIndex].controlParamIndex
			<< "\tcontrolParamValue: " << comboData.controlParameterArray[parameterIndex].controlParamValue
			/*<< "\tabsProcessParamIndex: " << comboData.controlParameterArray[parameterIndex].absProcessParamIndex
			<< "\tprocessParamName: " << comboData.unsortedParameterArray[procParamIndex].paramName
			<< "\tprocessName: " << comboData.unsortedParameterArray[procParamIndex].processName*/ << endl;
#endif

	int controlParameterIndex = parameter.controlParamIndex;
	this->processing->updateControlParameter(controlName, controlParameterIndex, parameterValue);
#if(dbg >= 1)
	cout << "control: " << controlName << "\t\tparameter: " << parameter.controlParamName << endl;
	cout << "EXITING: ProcessingControl::updateControlParameter" << endl;
#endif

	return status;
}


int ProcessingControl::enableEffects()
{

	this->processing->enableProcessing();
	return 0;
}

int ProcessingControl::disableEffects()
{

	this->processing->disableProcessing();
	return 0;
}




