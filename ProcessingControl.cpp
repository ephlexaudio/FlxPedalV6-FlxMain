/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */


#include "config.h"
#include "ProcessingControl.h"
#include "indexMapStructs.h"

using namespace std;

extern bool debugOutput;


#define FS1_LED	30
#define FS2_LED	31
#define FS1_PIN	33 // 33 and 32 are switch around
#define FS2_PIN	32



#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43


ProcessingControl::ProcessingControl()
{

	//right/control port signal switches (audio/control_voltage/ground)
	char gpioStr[5];
	strcpy(gpioStr,"out");

	// processing object is initialized in start function below
	this->footswitchLed[0] = GPIOClass(FS1_LED);
	this->footswitchLed[0].export_gpio();
	this->footswitchLed[0].setdir_gpio(gpioStr);
	this->footswitchLed[0].setval_gpio(1);

	this->footswitchLed[1] = GPIOClass(FS2_LED);
	this->footswitchLed[1].export_gpio();
	this->footswitchLed[1].setdir_gpio(gpioStr);
	this->footswitchLed[1].setval_gpio(1);

	strcpy(gpioStr,"in");
	this->footswitchPin[0] = GPIOClass(FS1_PIN);
	this->footswitchPin[0].export_gpio();
	this->footswitchPin[0].setdir_gpio(gpioStr);

	this->footswitchPin[1] = GPIOClass(FS2_PIN);
	this->footswitchPin[1].export_gpio();
	this->footswitchPin[1].setdir_gpio(gpioStr);

	this->justPoweredUp = true;

}


ProcessingControl::~ProcessingControl()
{

}

int audioCallbackComboIndex = 100;
#define dbg 1




#define dbg 2
int ProcessingControl::startJack(void)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::startJack" << endl;
#endif


	if(debugOutput) cout << "ProcessingControl JACK period: " << this->jackUtil.period << "\tJACK buffer: " << this->jackUtil.buffer << endl;

	char jackInitString[100];
	sprintf(jackInitString, "jackd -d alsa -p %d -n %d &", this->jackUtil.period, this->jackUtil.buffer);
#if(dbg >= 2)
	cout << "jackInitString: " << jackInitString << endl;
#endif
	system(jackInitString);
	sleep(2);
	if(debugOutput) cout << "setting up I2C port..." << endl;
	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4A},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x07},	// disable DAC soft mute, disable HPF, enable de-emphasis
			{0x0C ,0x5F},	// disabling POWEROFF
			{0x0C ,0x5E},	// powering line inputs
			{0x0C ,0x5A},	// powering ADC
			{0x0C ,0x52},	// powering DAC
			{0x0C ,0x42},	// powering outputs
			{0x12 ,0x01}	// setting to active
	};
	uint8_t i2cInitLength = 13;

	int i2cDevFile;
	int i2cAdapter=1;
	char i2cDevFileName[20];
	ssize_t i2cStatus = 0;


	//*************  Finish setting codec registers ************
	  snprintf(i2cDevFileName, 19, "/dev/i2c-%d", i2cAdapter);
	  i2cDevFile = open(i2cDevFileName, O_WRONLY);
	  if (i2cDevFile < 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg>=2)
		cout << "error creating I2C device file" << endl;
#endif
		  status = -1;
	  }
	  int addr = 0x1a; // The I2C address

	  if((ioctl(i2cDevFile, I2C_SLAVE_FORCE, addr) < 0) && status == 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg>=2)
		cout << "error opening I2C device file: " << errno << endl;
#endif
		  status = -1;
	  }

	  if(status == 0)
	  {
		  do{
			  errno = 0;
			  i2cStatus = write(i2cDevFile,i2cInitSequence[0],2);
#if(dbg>=2)
			cout << "i2c deactivate result: " << errno << endl;
#endif
			  nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);

		  }while ((errno != 0));


		  for(uint8_t i = 1; i < i2cInitLength; i++)
		  {
			  i2cStatus = write(i2cDevFile,i2cInitSequence[i],2);
#if(dbg>=2)
			  cout << "i2c init result: " << errno << endl;
#endif

		  }
	  }
	  close(i2cDevFile);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::startJack:" << status << endl;
#endif
	return status;

}


int ProcessingControl::stopJack(void)
{
	int status = 0;

	status = system("killall -9 jackd");

    return status;
}


#define dbg 1
int ProcessingControl::loadComboStruct(ComboStruct comboStruct)
{

	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::loadComboStruct" << endl;
	if(debugOutput) cout << "comboName: " << comboStruct.name << endl;
#endif

	this->processing->setComboName(comboStruct.name);
	this->processing->loadComboStructData(comboStruct);


#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: ProcessingControl::loadComboStruct: " << status << endl;
#endif
	return status;
}


#define dbg 1
ComboStruct ProcessingControl::getComboStruct()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::getComboStruct" << endl;
	if(debugOutput) cout << "comboName: " << this->processing->getComboName() << endl;
#endif
	ComboStruct combo;
	try
	{
		combo = this->processing->getComboStruct();
	}
	catch(exception &e)
	{
		cout << "exceptin in ProcessingControl::getComboStruct: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: ProcessingControl::getComboStruct: " << status << endl;
#endif
	return combo;
}


#define dbg 1
int ProcessingControl::startComboProcessing() // start clients and connect them
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::start" << endl;
#endif


#if(dbg >= 1)
	if(debugOutput) cout << "starting combo." << endl;
#endif
	this->processing = new Processing(this->getProcessingUtility());// initial ports from constructor created here.
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
	if(this->processing->isRealTime())
	{
#if(dbg >= 1)
		if(debugOutput) cout << "is realtime " << endl;
	}
	else
	{
		if(debugOutput) cout << "is not realtime " << endl;
#endif
	}

	//connect our ports to physical ports

	this->processing->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
#if(dbg >= 1)
	if(debugOutput) cout << "connecting processes: " << this->processing->getInputPortName(0) << endl;
#endif

	this->processing->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1
#if(dbg >= 1)
	if(debugOutput) cout << "connecting processes: " << this->processing->getInputPortName(1) << endl;
#endif


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
int ProcessingControl::stopComboProcessing() // stop clients and disconnect them
{

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
	this->stopJack();
#if(dbg >= 1)
	if(debugOutput) cout << "delete this->processing" << endl;
#endif
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::stop: " << status << endl;
#endif

}



#define dbg 0



#define dbg 0
void  ProcessingControl::updateProcessParameter(string parentProcess, string parameter, int parameterValue)
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::updateProcessParameter" << endl;
	if(debugOutput) cout << "parentProcess: " << parentProcess << "\parameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif


	this->processing->updateProcessParameter(parentProcess, parameter, parameterValue);

#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: ProcessingControl::updateProcessParameter: " << status << endl;
#endif

}




#define dbg 1
void ProcessingControl::updateControlParameter(string parentControl, string parameter, int parameterValue)
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
	if(debugOutput) cout << "parentControl: " << parentControl << "\tparameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif

	this->processing->updateControlParameter(parentControl, parameter, parameterValue);

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: ProcessingControl::updateControlParameter: " << endl;
#endif

}


#define dbg 0
void ProcessingControl::enableEffects()
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::enableEffects" << endl;
#endif


	this->processing->enableProcessing();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::enableEffects: " << status << endl;
#endif

}

void ProcessingControl::disableEffects()
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::disableEffects" << endl;
#endif


	this->processing->disableProcessing();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::disableEffects: " << status << endl;
#endif

}


#define dbg 0
void ProcessingControl::enableAudioOutput()
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::enableAudioOutput" << endl;
#endif

	this->processing->enableAudioOutput();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::enableAudioOutput: " << status << endl;
#endif

}


void ProcessingControl::disableAudioOutput()
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::disableAudioOutput" << endl;
#endif

	this->processing->disableAudioOutput();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::disableAudioOutput: " << status << endl;
#endif

}



#define dbg 0
void ProcessingControl::readFootswitches(void)
{

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
				this->footswitchLed[i].setval_gpio((this->footswitchStatus[i]?0:1));
				this->footSwitchPressed[i] = true;
			}
		}
	}


	this->processing->updateFootswitch(this->footswitchStatus);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::readFootswitches:" << this->footswitchStatus[0] << "," << this->footswitchStdl;
#endif

}



void ProcessingControl::setNoiseGateCloseThreshold(double closeThres)
{

	this->processing->setNoiseGateCloseThreshold(closeThres);

}

void ProcessingControl::setNoiseGateOpenThreshold(double openThres)
{


	this->processing->setNoiseGateOpenThreshold(openThres);

}

void ProcessingControl::setNoiseGateGain(double gain)
{
	this->processing->setNoiseGateGain(gain);
}

void ProcessingControl::setTriggerLowThreshold(double lowThres)
{
	this->processing->setTriggerLowThreshold(lowThres);
}

void ProcessingControl::setTriggerHighThreshold(double highThres)
{
	this->processing->setTriggerHighThreshold(highThres);
}



#define dbg 1
void ProcessingControl::setProcessingUtility(ProcessingUtility processingUtil)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ProcessingControl::setGateTriggerParameters" << endl;
#endif

	try
	{
		this->processingUtil.noiseGateUtil.closeThres = processingUtil.noiseGateUtil.closeThres;
		this->processingUtil.noiseGateUtil.openThres = processingUtil.noiseGateUtil.openThres;
		this->processingUtil.noiseGateUtil.gain = processingUtil.noiseGateUtil.gain;
		this->processingUtil.triggerUtil.highThres = processingUtil.triggerUtil.highThres;
		this->processingUtil.triggerUtil.lowThres = processingUtil.triggerUtil.lowThres;
		this->processingUtil.procUtil.bufferSize=processingUtil.procUtil.bufferSize;
		this->processingUtil.procUtil.inputCouplingMode=processingUtil.procUtil.inputCouplingMode;
		this->processingUtil.procUtil.antiAliasingNumber=processingUtil.procUtil.antiAliasingNumber;
		this->processingUtil.procUtil.waveshaperMode=processingUtil.procUtil.waveshaperMode;

	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::setGateTriggerParameters: " << e.what() << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ProcessingControl::setGateTriggerParameters" << endl;
#endif

}


void ProcessingControl::setJackUtility(JackUtility jackUtil)
{
	this->jackUtil.buffer = jackUtil.buffer;
	this->jackUtil.period = jackUtil.period;
}






ProcessingUtility ProcessingControl::getProcessingUtility()
{
	cout << "this->processingUtil.noiseGateUtil.openThres: " << this->processingUtil.noiseGateUtil.openThres << endl;
	cout << "this->processingUtil.noiseGateUtil.closeThres: " << this->processingUtil.noiseGateUtil.closeThres << endl;
	cout << "this->processingUtil.noiseGateUtil.gain: " << this->processingUtil.noiseGateUtil.gain << endl;
	cout << "this->processingUtil.triggerUtil.highThres: " << this->processingUtil.triggerUtil.highThres << endl;
	cout << "this->processingUtil.triggerUtil.lowThres: " << this->processingUtil.triggerUtil.lowThres << endl;
	cout << "this->processingUtil.procUtil.bufferSize: " << this->processingUtil.procUtil.bufferSize << endl;
	cout << "this->processingUtil.procUtil.inputCouplingMode: " << this->processingUtil.procUtil.inputCouplingMode << endl;
	cout << "this->processingUtil.procUtil.antiAliasingNumber: " << this->processingUtil.procUtil.antiAliasingNumber << endl;
	cout << "this->processingUtil.procUtil.waveshaperMode: " << this->processingUtil.procUtil.waveshaperMode << endl;

	return this->processingUtil;

}





#define dbg 0
int loadComponentSymbols(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::loadComponentSymbols" << endl;
#endif

	array<ProcessSignalBuffer,60> nullProcSigBufVec;
	array<ProcessParameterControlBuffer,60> nullParamContBufVec;

	delayb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	filter3bb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	filter3bb2('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	lohifilterb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	mixerb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	volumeb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
	waveshaperb('c', NULL, nullProcSigBufVec, nullParamContBufVec, NULL);
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::loadComponentSymbols" << endl;
#endif

	return 0;
}

#define dbg 0
int loadControlSymbols(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::loadControlSymbols" << endl;
#endif

	array<ProcessParameterControlBuffer,60> nullParamContBufVec;

	struct ControlEvent loadControlType;
	struct ProcessEvent nullProcessEvent;
	loadControlType.conType = 0; // load Normal control symbol data
	normal('c', false, 0, &loadControlType, nullParamContBufVec);
	loadControlType.conType = 1; // load Envelope Generator control symbol data;
	envGen('c', false, 0, &loadControlType, nullParamContBufVec);
	loadControlType.conType = 2; // load LFO control symbol data;
	lfo('c', false, 0, &loadControlType, nullParamContBufVec);
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::loadControlSymbols" << endl;
#endif

	return 0;
}
