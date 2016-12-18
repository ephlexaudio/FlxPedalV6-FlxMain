/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */



#include "ProcessingControl.h"
extern ComboDataInt comboData;

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
	//this->combo = new Combo; /
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



#define dbg 2
int ProcessingControl::load(vector<Process> processesStruct, vector<Json::Value> connectionsJson,
		vector<Control> controlsStruct, vector<ControlConnection> controlConnectionsStruct) // load parameters for processes
{
	int status = 0;
	//cout << "entering ProcessingControl::load.." << endl;
	this->combo = new Combo;// initial ports from constructor created here.
	cout << "loading combo." << endl;
	//volatile int processIndex = 0;
	volatile int processInputCount = 0;
	volatile int processOutputCount = 0;
	//volatile int processSequenceIndex = 0;
	//volatile int procBufferIndex = 0;
	volatile int procBufferCount = 0;

	//volatile int processType = 0;

	string processName;
	string connectionName[20];

	initProcBuffers(this->combo->procBufferArray);

	// setup dummy buffer for unconnected inputs
	setProcBuffer(this->combo->procBufferArray[58],0,1);

	//int parameters[10] = {0,0,0,0,0,0,0,0,0,0};
	//int processInputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//int processOutputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	this->combo->processCount = processesStruct.size();
#if(dbg >= 2)
	cout << "this->combo->processCount: " << this->combo->processCount << endl;
#endif
	this->combo->controlCount = controlsStruct.size();



	//*********** initialize process data in ProcessEvent element **************************************
	for(int processIndex = 0; processIndex < this->combo->processCount; processIndex++)
	{
		setProcData(&this->combo->processSequence[processIndex], processesStruct[processIndex]);

		setProcParameters(&this->combo->processSequence[processIndex], processesStruct[processIndex]);

		this->combo->processSequence[processIndex].inputBufferIndexes = (int *)calloc(this->combo->processSequence[processIndex].processInputCount, sizeof(int));
		this->combo->processSequence[processIndex].outputBufferIndexes = (int *)calloc(this->combo->processSequence[processIndex].processOutputCount, sizeof(int));

		// Initialize input/output buffer indexes with addresses for dummy buffers

		initProcInputBufferIndexes(&this->combo->processSequence[processIndex]);
		initProcOutputBufferIndexes(&this->combo->processSequence[processIndex]);
	}

	//*********** initialize data in ProcessBuffer element **************************************

	this->combo->initProcBufferArray(this->combo->procBufferArray, connectionsJson);
#if(dbg >= 2)
	cout << "OUTPUT BUFFERS: " << endl;
	for(int i = 0; i < this->combo->bufferCount; i++)
	{
		cout << this->combo->procBufferArray[i].processName << ":" << this->combo->procBufferArray[i].portName << endl;
	}
	procBufferCount = connectionsJson.size();
	// set data in procBufferArray elements, using connectionsJson data

	cout << endl;
#endif


#if(dbg >= 2)
	cout << endl;
#endif

	//************* Connect process outputs to process output buffers using procBufferArray*************************
	// loop through connectionsJson
	//for(int bufferIndex = 0; bufferIndex < this->combo->bufferCount; bufferIndex++)
	for(unsigned int connIndex = 0; connIndex < comboData.connectionsJson.size(); connIndex++)
	{
		// 		for each procBufferArray element, get the buffer index and output (source) process:port

		Connector conn, targetConn;

		conn.process = comboData.connectionsJson[connIndex]["srcProcess"].asString();
		conn.port = comboData.connectionsJson[connIndex]["srcPort"].asString();
		targetConn.process = comboData.connectionsJson[connIndex]["destProcess"].asString();
		targetConn.port = comboData.connectionsJson[connIndex]["destPort"].asString();

		cout << "************************************************************" << endl;
		cout << "conn: " << conn.process << ":" << conn.port << endl;
		cout << "targetConn: " << targetConn.process << ":" << targetConn.port << endl;

		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->combo->bufferCount; bufferIndex++)
		{
			if(this->combo->procBufferArray[bufferIndex].processName.compare(conn.process) == 0 &&
					this->combo->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{
				cout << "found buffer index for " << conn.process << ": " << bufferIndex << endl;
				break; // found input process:port index
			}
		}

		//*********** connect relevant system input to output buffer  ********************************

		this->combo->inputProcBufferIndex[0] = 0;
		this->combo->inputProcBufferIndex[1] = 1;
		/*if(conn.process.compare("system") == 0 && conn.port.compare("capture_1") == 0)
		{
			this->combo->inputProcBufferIndex[0] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected to input1: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}
		else if(conn.process.compare("system") == 0 && conn.port.compare("capture_2") == 0)
		{
			this->combo->inputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected to input2: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}

		else*/
		{
			// 		loop through processSequence
			for(int procIndex = 0; procIndex < this->combo->processCount; procIndex++)
			{

				// 		for each processSequence element, loop through the outputs
				for(int outputIndex = 0; outputIndex < this->combo->processSequence[procIndex].processOutputCount; outputIndex++)
				{
					//			for each output, get the process:port
					Connector procSeqOutputConn;
					procSeqOutputConn.process = this->combo->processSequence[procIndex].processName;
					procSeqOutputConn.port = this->combo->processSequence[procIndex].outputBufferNames[outputIndex];
					// 				compare the procesSequence output process:port to the connectionsJson process:port
					// 				if there is a match, set the processSequence output element index to the output process buffer index

					if(procSeqOutputConn.process.compare(conn.process) == 0 &&
							procSeqOutputConn.port.compare(conn.port) == 0)
					{
						this->combo->processSequence[procIndex].outputBufferIndexes[outputIndex] = bufferIndex;
#if(dbg >= 2)
						cout << "Connected output : procBuffer[" << bufferIndex << "]: ";
						cout <<  procSeqOutputConn.process << ":" << procSeqOutputConn.port << ">";
						cout << targetConn.process << ":" << targetConn.port  << endl;
#endif
					}
				}
			}
		}
	}

	//************* Connect process inputs to process output buffers using connectionsJson **************

	// loop through connectionsJson
	for(unsigned int connIndex = 0; connIndex < comboData.connectionsJson.size(); connIndex++)
	{
		// 		for each connectionsJson element, get the buffer index, output (source) process:port and the
		// 		target (dest) process:port
		Connector conn, targetConn;
		conn.process = comboData.connectionsJson[connIndex]["srcProcess"].asString();
		conn.port = comboData.connectionsJson[connIndex]["srcPort"].asString();
		targetConn.process = comboData.connectionsJson[connIndex]["destProcess"].asString();
		targetConn.port = comboData.connectionsJson[connIndex]["destPort"].asString();

		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->combo->bufferCount; bufferIndex++)
		{
			if(this->combo->procBufferArray[bufferIndex].processName.compare(conn.process) == 0 &&
					this->combo->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{

				break; // found output process:port index
			}
		}

		//*********** connect relevant output buffer to system output ********************************
		if(targetConn.process.compare("system") == 0 && targetConn.port.compare("playback_1") == 0)
		{
			this->combo->outputProcBufferIndex[0] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected output1: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}
		else if(targetConn.process.compare("system") == 0 && targetConn.port.compare("playback_2") == 0)
		{
			this->combo->outputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected output2: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}
		else
		{
			// 		loop through processSequence
			for(int procIndex = 0; procIndex < this->combo->processCount; procIndex++)
			{
				// 		for each processSequence element, loop through the inputs
				for(int inputIndex = 0; inputIndex < this->combo->processSequence[procIndex].processInputCount; inputIndex++)
				{
					//			for each input, get the process:port
					Connector procSeqInputConn;
					procSeqInputConn.process = this->combo->processSequence[procIndex].processName;
					procSeqInputConn.port = this->combo->processSequence[procIndex].inputBufferNames[inputIndex];
					// 				compare the procesSequence input process:port to the connectionsJson target process:port
					// 				if there is a match, set the processSequence input element index to the output process buffer index
					if(procSeqInputConn.process.compare(targetConn.process) == 0 &&
							procSeqInputConn.port.compare(targetConn.port) == 0)
					{
						this->combo->processSequence[procIndex].inputBufferIndexes[inputIndex] = bufferIndex;
#if(dbg >= 2)
						cout << "Connected input : procBuffer[" << bufferIndex << "]: ";
						cout << conn.process << ":" << conn.port << ">";
						cout <<  procSeqInputConn.process << ":" << procSeqInputConn.port << endl;
#endif
					}
				}
			}
		}
	}



#if(dbg >= 2)
	cout << "combo->inputProcBufferIndex[0]: " << this->combo->inputProcBufferIndex[0] << endl;
	cout << "combo->inputProcBufferIndex[1]: " << this->combo->inputProcBufferIndex[1] << endl;
	cout << "combo->outputProcBufferIndex[0]: " << this->combo->outputProcBufferIndex[0] << endl;
	cout << "combo->outputProcBufferIndex[1]: " << this->combo->outputProcBufferIndex[1] << endl;
#endif


#if(dbg >= 2)
	for(unsigned int processSequenceIndex = 0; processSequenceIndex < processesStruct.size(); processSequenceIndex++)
	{
		cout << endl;
		cout << "PROCESS " << processSequenceIndex << endl;
		cout << "processName: " << this->combo->processSequence[processSequenceIndex].processName << endl;
		cout << "processType: " << this->combo->processSequence[processSequenceIndex].processType << endl;
		cout << "processInputCount: " << this->combo->processSequence[processSequenceIndex].processInputCount << endl;
		cout << "processOutputCount: " << this->combo->processSequence[processSequenceIndex].processOutputCount << endl;

		cout << "input buffer indexes: ";
		processInputCount = this->combo->processSequence[processSequenceIndex].processInputCount;
		for(int processInputIndex = 0; processInputIndex < processInputCount; processInputIndex++)
		{
			cout << this->combo->processSequence[processSequenceIndex].inputBufferIndexes[processInputIndex] << ',';
		}
		cout << endl;

		cout << "output buffer indexes: ";
		processOutputCount = this->combo->processSequence[processSequenceIndex].processOutputCount;
		for(int processOutputIndex = 0; processOutputIndex < processOutputCount; processOutputIndex++)
		{
			cout << this->combo->processSequence[processSequenceIndex].outputBufferIndexes[processOutputIndex] << ',';
		}
		cout << endl;
	}
#endif

	//********************* Initialize control data into ControlEvent element ************************************

	//loop through controlsStruct vector
	this->combo->controlCount = comboData.controlsStruct.size();

	cout << "CONTROL EVENT:" << endl;
	for(int controlIndex = 0; controlIndex < this->combo->controlCount; controlIndex++)
	{
		this->combo->controlSequence[controlIndex].paramContConnectionCount = 0;
		//for each controlsStruct, enter the control data into the ControlEvent element
		this->combo->controlSequence[controlIndex].name = comboData.controlsStruct[controlIndex].name;
		if(comboData.controlsStruct[controlIndex].type.compare("Norm") == 0)
			this->combo->controlSequence[controlIndex].type = 0;
		else if(comboData.controlsStruct[controlIndex].type.compare("Env") == 0)
			this->combo->controlSequence[controlIndex].type = 1;
		else if(comboData.controlsStruct[controlIndex].type.compare("LFO") == 0)
			this->combo->controlSequence[controlIndex].type = 2;

		cout << "name: " << this->combo->controlSequence[controlIndex].name << endl;
		cout << "type: " << this->combo->controlSequence[controlIndex].type << endl;

		// enter the parameter values
		for(unsigned int paramIndex = 0; paramIndex < comboData.controlsStruct[controlIndex].params.size(); paramIndex++)
		{
			this->combo->controlSequence[controlIndex].parameter[paramIndex] = comboData.controlsStruct[controlIndex].params[paramIndex].value;
			cout << "\tcontrolSequence[" << controlIndex << "].parameter[" << paramIndex << "]: " << this->combo->controlSequence[controlIndex].parameter[paramIndex] << endl;
		}

		// enter the process and parameter index, using controlsStruct absProcessParameterIndexes as indexing for sortedParameterArray

		for(unsigned int paramIndex = 0; paramIndex < comboData.controlsStruct[controlIndex].absProcessParameterIndexes.size(); paramIndex++)
		{
			// get absolute process parameter index
			int absIndex = comboData.controlsStruct[controlIndex].absProcessParameterIndexes[paramIndex];
			cout << "absIndex: " << absIndex << endl;
			// use absolute parameter index to enter absolute process index from sortedParameterArray into controlSequence connected process
			this->combo->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex =
					comboData.sortedParameterArray[absIndex].absProcessIndex;
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processIndex: "
					<< this->combo->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex << endl;
			// use absolute parameter index to enter process parameter index from sortedParameterArray into controlSequence connected process parameter
			this->combo->controlSequence[controlIndex].paramContConnection[paramIndex].processParamIndex =
					comboData.sortedParameterArray[absIndex].processParamIndex;
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processParamIndex: "
					<< this->combo->controlSequence[controlIndex].paramContConnection[paramIndex].processParamIndex << endl;
			this->combo->controlSequence[controlIndex].paramContConnectionCount++;
		}
	}




	//this->combo->inputProcBufferIndex[0] = this->combo->processSequence[0].inputBufferIndexes[0];
	//this->combo->outputProcBufferIndex[0] = this->combo->processSequence[this->combo->processCount-1].outputBufferIndexes[0];
	this->combo->loadEffects();
	this->combo->bypassAll(); // set to straight-thru to avoid XRun
	sleep(1);

	return status;
}


#define dbg 0
int ProcessingControl::start() // start clients and connect them
{
	int status = 0;

#if(dbg == 1)
	cout << "starting combo." << endl;
#endif
	this->combo->start();
	//this->combo = new Combo;// initial ports from constructor created here.
	//this->combo->load();
	/*
#if(dbg == 1)
	cout << "process created." << endl;
#endif
	this->combo->addOutPort("proc_out0");		// add new out port (2) named "blahout0"
#if(dbg == 1)
	cout << "port added: " << this->combo->getOutputPortName(0) << endl;
#endif
	this->combo->addOutPort("proc_out1");		// add new out port (3) named "blahout1"
#if(dbg == 1)
	cout << "port added: " << this->combo->getOutputPortName(1) << endl;
#endif
	this->combo->addInPort("proc_in0");		// add new in port (2) named "blahin0"
#if(dbg == 1)
	cout << "port added: " << this->combo->getInputPortName(0) << endl;
#endif
	this->combo->addInPort("proc_in1");		// add new in port (3) named "blahin1"
#if(dbg == 1)
	cout << "port added: " << this->combo->getInputPortName(1) << endl;
#endif
	//this->bypassAll(); // set to straight-thru to avoid XRun
#if(dbg == 1)
	cout << "process bypassed." << endl;
#endif
	*/
	this->combo->start();	// activate the client
#if(dbg == 1)
	cout << "process started." << endl;
#endif
	// reporting some client info
#if(dbg == 1)
	cout << endl << "my name: " << this->combo->getName() << endl;
#endif

	// test to see if it is real time
	if (this->combo->isRealTime())
#if(dbg == 1)
#endif
		cout << "is realtime " << endl;
	else
#if(dbg == 1)
#endif
		cout << "is not realtime " << endl;





	//connect our ports to physical ports
	this->combo->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
	this->combo->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1

	//this->connectToPhysical(0,0);	// connects this client in port 0 to physical source port 0
	//this->connectToPhysical(1,1);	// connects this client in port 1 to physical source port 1
	//char connToString[25];
#if(dbg == 1)
	cout << "connecting processes." << endl;
#endif

	this->combo->connectToPhysical(0,0);		// connects this client out port 0 to physical destination port 0
#if(dbg == 1)
	cout << "connecting processes: " << this->combo->getOutputPortName(0) << endl;
#endif
	this->combo->connectToPhysical(1,1);		// connects this client out port 1 to physical destination port 1
#if(dbg == 1)
	cout << "connecting processes: " << this->combo->getOutputPortName(1) << endl;
#endif

/*
	// exception test example
	cout << endl << "testing exceptions, trying to connect to a fake port" << endl;
	try {
		this->combo->connectFrom(1,"fakeportname");
	} catch (std::runtime_error e){
		cout << "YES: " << e.what() << endl;
	}

	cout << "try to query a port we don't have" << endl;
	try {
		this->combo->numConnectionsInPort(100);
	} catch (std::range_error e){
		cout << "YES: " << e.what() << endl;
	}

	// port connection status
#if(dbg == 1)
	cout << endl;
	cout << "proc 0 output 0 is connected to " << this->combo->numConnectionsOutPort(0) << " ports" << endl;
	cout << "proc 0 output 1 is connected to " << this->combo->numConnectionsOutPort(1) << " ports" << endl;
	cout << "proc 0 input 0 is connected to " << this->combo->numConnectionsInPort(0) << " ports" << endl;
	cout << "proc 0 input 1 is connected to " << this->combo->numConnectionsInPort(1) << " ports" << endl;
#endif



	//print names
#if(dbg == 1)
	cout << endl;
	cout << "inport names:" << endl;
	for(unsigned int i = 0; i < this->combo->inPorts(); i++)
		cout << "\t" << this->combo->getInputPortName(i) << endl;

	cout << "outport names:" << endl;
	for(unsigned int i = 0; i < this->combo->outPorts(); i++)
		cout << "\t" << this->combo->getOutputPortName(i) << endl;
#endif
	*/
	if(justPoweredUp)
	{
		inputsSwitched = this->combo->areInputsSwitched();
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
	this->combo->bypassAll(); // set to straight-thru to avoid XRun
	//this->combo->stopEffects();
	//sleep(2);
	this->combo->disconnectInPort(0);	// Disconnecting ports.
#if(dbg == 1)
	cout << "this->combo->disconnectInPort(0)" << endl;
#endif
	this->combo->disconnectOutPort(1);
#if(dbg == 1)
	cout << "this->combo->disconnectOutPort(1)" << endl;
#endif
	this->combo->stop();
	this->combo->close();	// stop client.
#if(dbg == 1)
	cout << "this->combo->close()" << endl;
#endif
	delete this->combo;	// always clean up after yourself.
#if(dbg == 1)
	cout << "delete this->combo" << endl;
#endif
	//exit(0);

	return status;
}


int ProcessingControl::getProcessData(int index, float *data)
{
	int status = 0;

	this->combo->getProcessData(index, data);

	return status;
}

int ProcessingControl::clearProcessData(int index, float *data)
{
	int status = 0;

	this->combo->clearProcessData(index, data);

	return status;
}

#define dbg 0
int ProcessingControl::updateFootswitch(int *footswitchStatus)
{
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateFootswitch" << endl;
#endif
	int status = 0;

	this->combo->updateFootswitch(footswitchStatus);

#if(dbg >= 1)
	cout << "EXIT: ProcessingControl::updateFootswitch" << endl;
#endif
	return status;
}

#define dbg 0

int ProcessingControl::updateProcessParameter(int parameterIndex, int parameterValue)
{
	int status = 0;

	IndexedParameter parameter = comboData.sortedParameterArray[parameterIndex];
	string processName = parameter.processName; // need to use process name instead of index because process index
											// in parameterArray does not correspond to process index in processSequence

#if(dbg == 0)
	cout << "processName: " << comboData.sortedParameterArray[parameterIndex].processName
			<< "\t\tabsProcessIndex: " << comboData.sortedParameterArray[parameterIndex].absProcessIndex
			<< "\t\tparamName: " << comboData.sortedParameterArray[parameterIndex].paramName
			<< "\t\tprocessParamIndex: " << comboData.sortedParameterArray[parameterIndex].processParamIndex
			<< "\t\tabsParamIndex: " << comboData.sortedParameterArray[parameterIndex].absParamIndex
			<< "\t\tparamName: " << comboData.sortedParameterArray[parameterIndex].paramName
			<< "\t\tparamValue: " << comboData.sortedParameterArray[parameterIndex].paramValue << endl;
#endif


	int processParameterIndex = parameter.processParamIndex;
#if(dbg == 1)
	cout << "process: " << processName << "\t\tparameter: " << parameter.paramName << endl;
#endif
	this->combo->updateProcessParameter(processName, processParameterIndex, parameterValue);

	return status;
}

#define dbg 0
int ProcessingControl::updateControlParameter(int parameterIndex, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
#endif
	IndexedControlParameter parameter = comboData.controlParameterArray[parameterIndex];
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

	this->combo->updateControlParameter(controlName, controlParameterIndex, parameterValue);
#if(dbg >= 1)
	cout << "control: " << controlName << "\t\tparameter: " << parameter.controlParamName << endl;
	cout << "EXITING: ProcessingControl::updateControlParameter" << endl;
#endif

	return status;
}



int ProcessingControl::bypassAllEffects()
{
	int status = 0;

	this->combo->bypassAll();

	return status;
}




