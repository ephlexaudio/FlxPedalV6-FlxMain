/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */




#include "ProcessingControl.h"
#include "indexMapStructs.h"


#include "GPIOClass.h"
#include "config.h"
#include "structs.h"



#define FS1_LED	30
#define FS2_LED	31
#define FS1_PIN	33 // 33 and 32 are switch around
#define FS2_PIN	32



#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43
namespace std
{


ProcessingControl::ProcessingControl()
{
	this->processing = NULL;
	// processing object is initialized in start function below
	this->bufferSize = 256;
	this->inputsSwitched = false;

	//right/control port signal switches (audio/control_voltage/ground)
	char gpioStr[5];
	strcpy(gpioStr,"out");
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



#define dbg 2
int ProcessingControl::startJack(void)
{
	int status = 0;
#if(DISABLE_AUDIO_THREAD == 0)

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::startJack" << endl;
#endif


	 cout << "ProcessingControl JACK period: " << this->jackUtil.period.value << "\tJACK buffer: " << this->jackUtil.buffer.value << endl;

	char jackInitString[100];
	sprintf(jackInitString, "jackd -d alsa -p %d -n %d &", this->jackUtil.period.value, this->jackUtil.buffer.value);
#if(dbg >= 2)
	cout << "jackInitString: " << jackInitString << endl;
#endif
	system(jackInitString);
	sleep(2);
	 cout << "setting up I2C port..." << endl;
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
	 cout << "***** EXITING: ProcessingControl::startJack:" << status << endl;
#endif
#endif
	return status;

}


int ProcessingControl::stopJack(void)
{
	int status = 0;

	status = system("killall -9 jackd");

    return status;
}


#define dbg 0
int ProcessingControl::loadComboStruct(ComboStruct comboStruct)
{

	int status = 0;
#if(dbg >= 1)
	 cout << "ENTERING: ProcessingControl::loadComboStruct" << endl;
	 cout << "comboName: " << comboStruct.name << endl;
#endif
	try
	{
		// *************** Stop current combo and delete processes in array

		this->processing->unloadCombo();

		// create new Processes/Controls from comboStruct and put them in their respective arrays.

		this->processing->setComboName(comboStruct.name);
		this->processing->loadCombo(comboStruct);

	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::loadComboStruct: " << e.what() << endl;
	}



#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::loadComboStruct: " << status << endl;
#endif
	return status;
}


#define dbg 0
ComboStruct ProcessingControl::getComboStruct()
{
	int status = 0;

#if(dbg >= 1)
	 cout << "ENTERING: ProcessingControl::getComboStruct" << endl;
	 cout << "comboName: " << this->processing->getComboName() << endl;
#endif
	ComboStruct combo;
	try
	{
		combo = this->processing->getComboStruct();
		cout << "combo name: " << combo.name;
		cout << "ProcessingControl control data: " << endl;
		for(auto & control : combo.controlSequenceData)
		{
			{
				cout << control.controlName << ":" << control.parameter[0].parameterName;
				cout << "=" << control.parameter[0].value << endl;
			}
		}
	}
	catch(exception &e)
	{
		cout << "exceptin in ProcessingControl::getComboStruct: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::getComboStruct: " << status << endl;
#endif
	return combo;
}


#define dbg 1
int ProcessingControl::startComboProcessing() // start clients and connect them
{
	int status = 0;
#if(dbg >= 1)
#if(DISABLE_AUDIO_THREAD == 0)
	 cout << "***** ENTERING: ProcessingControl::startComboProcessing" << endl;
#else
	 cout << "***** ENTERING: ProcessingControl::startComboProcessing with RT audio thread disabled" << endl;
#endif
#endif


#if(dbg >= 1)
	 cout << "creating new Processing object." << endl;
#endif
	this->processing = new Processing(this->getNoiseGateUtility(),this->getEnvTriggerUtility(), this->getBufferSize());// initial ports from constructor created here.
#if(dbg >= 1)
	 cout << "processing Constructor successful." << endl;
#endif
#if(DISABLE_AUDIO_THREAD == 0)
	this->processing->start();	// activate the client
#if(dbg >= 1)
	 cout << "process started." << endl;
#endif
	// reporting some client info
#if(dbg >= 1)
	 cout << "my name: " << this->processing->getName() << endl;
#endif

	// test to see if it is real time
	if(this->processing->isRealTime())
	{
#if(dbg >= 1)
		 cout << "is realtime " << endl;
	}
	else
	{
		 cout << "is not realtime " << endl;
#endif
	}

	//connect our ports to physical ports

	this->processing->connectFromPhysical(0,0);	// connects this client in port 0 to physical source port 0
#if(dbg >= 1)
	 cout << "connecting processes: " << this->processing->getInputPortName(0) << endl;
#endif

	this->processing->connectFromPhysical(1,1);	// connects this client in port 1 to physical source port 1
#if(dbg >= 1)
	 cout << "connecting processes: " << this->processing->getInputPortName(1) << endl;
#endif


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
#endif
#if(dbg >= 1)
#if(DISABLE_AUDIO_THREAD == 0)
	 cout << "***** EXITING: ProcessingControl::startComboProcessing: " << status << endl;
#else
	 cout << "***** EXITING: ProcessingControl::startComboProcessing with RT audio thread disabled" << endl;
#endif
#endif

	return status;

}


#define dbg 1
void ProcessingControl::stopComboProcessing() // stop clients and disconnect them
{
#if(DISABLE_AUDIO_THREAD == 0)
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::stop" << endl;
#endif

	this->processing->disconnectInPort(0);	// Disconnecting ports.
#if(dbg >= 1)
	 cout << "this->disconnectInPort(0)" << endl;
#endif
	this->processing->disconnectOutPort(1);
#if(dbg >= 1)
	 cout << "this->processing->disconnectOutPort(1)" << endl;
#endif
	this->processing->stop();
	this->processing->close();	// stop client.
#if(dbg >= 1)
	 cout << "this->processing->close()" << endl;
#endif
	delete this->processing;	// always clean up after yourself.
	this->stopJack();
#if(dbg >= 1)
	 cout << "delete this->processing" << endl;
#endif
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::stop: " <<  endl;
#endif
#endif
}







#define dbg 0
void ProcessingControl::updateProcessParameter(string parentProcess, string parameter, int parameterValue)
{

#if(dbg >= 1)
	 cout << "ENTERING: ProcessingControl::updateProcessParameter" << endl;
	 cout << "parentProcess: " << parentProcess << "\parameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif


	this->processing->updateProcessParameter(parentProcess, parameter, parameterValue);

#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::updateProcessParameter: " << status << endl;
#endif

}




#define dbg 0
void ProcessingControl::updateControlParameter(string parentControl, string parameter, int parameterValue)
{

#if(dbg >= 1)
	 cout << "ENTERING: ProcessingControl::updateControlParameter" << endl;
	 cout << "parentControl: " << parentControl << "\tparameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif

	this->processing->updateControlParameter(parentControl, parameter, parameterValue);

#if(dbg >= 1)
	 cout << "EXITING: ProcessingControl::updateControlParameter: " << endl;
#endif

}


#define dbg 0
void ProcessingControl::enableEffects()
{

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::enableEffects" << endl;
#endif


	this->processing->enableProcessing();

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::enableEffects: " << status << endl;
#endif

}

void ProcessingControl::disableEffects()
{

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::disableEffects" << endl;
#endif


	this->processing->disableProcessing();

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::disableEffects: " << status << endl;
#endif

}


#define dbg 0
void ProcessingControl::enableAudioOutput()
{

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::enableAudioOutput" << endl;
#endif

	this->processing->enableAudioOutput();

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::enableAudioOutput: " << status << endl;
#endif

}


void ProcessingControl::disableAudioOutput()
{

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::disableAudioOutput" << endl;
#endif

	this->processing->disableAudioOutput();

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::disableAudioOutput: " << status << endl;
#endif

}



#define dbg 0
void ProcessingControl::readFootswitches(void)
{

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::readFootswitches" << endl;
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
				 cout << "footswitch[" << i << "] = 0: " << this->footswitchStatus[i] << endl;
			}
			else if((footSwitchState0 == 1) && (this->footSwitchPressed[i] == false))
			{
				this->footswitchStatus[i] ^= true; // footswitches are active low
				 cout << "footswitch[" << i << "] = 1: " << this->footswitchStatus[i] << endl;
				this->footswitchLed[i].setval_gpio((this->footswitchStatus[i]?0:1));
				this->footSwitchPressed[i] = true;
			}
		}
	}


	this->processing->updateFootswitch(this->footswitchStatus);

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::readFootswitches:" << this->footswitchStatus[0] << "," << this->footswitchStdl;
#endif

}



void ProcessingControl::setBufferSize(int bufferSize)
{
	this->bufferSize = bufferSize;
}

#define dbg 1
int ProcessingControl::getBufferSize()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::getBufferSize" << endl;
#endif
	return 256;
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::getBufferSize: " <<  endl;
#endif
}

void ProcessingControl::setNoiseGateUtility(NoiseGateUtility gateUtil)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::setGateTriggerParameters" << endl;
#endif

	try
	{
		this->gateUtil.closeThres.value = gateUtil.closeThres.value;
		this->gateUtil.openThres.value = gateUtil.openThres.value;
		this->gateUtil.gain.value = gateUtil.gain.value;

	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::setGateTriggerParameters: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::setGateTriggerParameters" << endl;
#endif
}


void ProcessingControl::updateNoiseGateUtility(NoiseGateUtility gateUtil)
{
	this->processing->setNoiseGateUtility(gateUtil);
}

#define dbg 1
NoiseGateUtility ProcessingControl::getNoiseGateUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::getNoiseGateUtility" << endl;
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
		cout << "exception in ProcessingControl::getNoiseGateUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::getNoiseGateUtility" << endl;
#endif
	 return gateUtil;
}


void ProcessingControl::setEnvTriggerUtility(EnvTriggerUtility triggerUtil)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::setEnvTriggerUtility" << endl;
#endif

	try
	{
		this->triggerUtil.highThres = triggerUtil.highThres;
		this->triggerUtil.lowThres = triggerUtil.lowThres;
	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::setEnvTriggerUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::setEnvTriggerUtility" << endl;
#endif
}


void ProcessingControl::updateEnvTriggerUtility(EnvTriggerUtility triggerUtil)
{
	this->setEnvTriggerUtility(triggerUtil);
}


EnvTriggerUtility ProcessingControl::getEnvTriggerUtility()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::getEnvTriggerUtility" << endl;
#endif
	 EnvTriggerUtility triggerUtil;
	try
	{
		triggerUtil.highThres = this->triggerUtil.highThres;
		triggerUtil.lowThres = this->triggerUtil.lowThres;
	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::getEnvTriggerUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::getEnvTriggerUtility" << endl;
#endif
	 return triggerUtil;
}



void ProcessingControl::setJackUtility(JackUtility jackUtil)
{
	this->jackUtil.buffer = jackUtil.buffer;
	this->jackUtil.period = jackUtil.period;
}






void ProcessingControl::loadSymbols()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::loadSymbols" << endl;
#endif

	this->processing->loadSymbols();
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::loadSymbols" << endl;
#endif

}

vector<string> ProcessingControl::getComponentSymbols()
{
	return this->processing->getComponentSymbols();
}
vector<string> ProcessingControl::getControlSymbols()
{
	return this->processing->getControlSymbols();
}

/*int ProcessingControl::processUtilityChange(PedalUtilityChange utilChange)
{
	int status = 0;

		try
		{
			if(utilChange.utility.compare("noiseGate") == 0)
			{
				if(utilChange.utilityParameter.compare("closeThres") == 0)
				{
					this->gateUtil.closeThres.value = utilChange.doubleValue;
		#if(dbg >=2)
			 cout << "pedalUtilData.noiseGateUtil.closeThres: " << utilChange.doubleValue << endl;
		#endif
				}
				else if(utilChange.utilityParameter.compare("openThres") == 0)
				{
					this->gateUtil.openThres.value = utilChange.doubleValue;

		#if(dbg >=2)
			 cout << "pedalUtilData.noiseGateUtil.openThres: " << utilChange.doubleValue << endl;
		#endif
				}
				else if(utilChange.utilityParameter.compare("gain") == 0)
				{
					this->gateUtil.gain.value = utilChange.doubleValue;

		#if(dbg >=2)
			 cout << "pedalUtilData.noiseGateUtil.gain: " << utilChange.doubleValue << endl;
		#endif
				}

			}
			else if(utilChange.utility.compare("trigger") == 0)
			{
				if(utilChange.utilityParameter.compare("highThres") == 0)
				{
					this->triggerUtil.highThres.value = utilChange.doubleValue;

		#if(dbg >=2)
			 cout << "pedalUtilData.triggerUtil.highThres: " << utilChange.doubleValue << endl;
		#endif
				}
				else if(utilChange.utilityParameter.compare("lowThres") == 0)
				{
					this->triggerUtil.lowThres.value = utilChange.doubleValue;
		#if(dbg >=2)
			 cout << "pedalUtilData.triggerUtil.lowThres: " << utilChange.doubleValue << endl;
		#endif
				}
			}

		}
		catch(exception &e)
		{
			cout << "exception in ProcessingControl::processUtilityChange: " << e.what() << endl;
			status = -1;
		}

	return status;
}*/


}
