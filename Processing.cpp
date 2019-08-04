/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#include "Processing.h"
#include "utilityFunctions.h"
#include "config.h"
#include "ControlSymbols.h"
#include "ComponentSymbols.h"


#include "valueArrays.h"


using namespace std;


#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43

Processing::Processing()
{
	this->bufferSize = 256;

	this->gateCloseThreshold = 0.0;
	this->gateOpenThreshold = 0.0;
	this->gateClosedGain = 0.0;
	this->triggerHighThreshold = 0.0;
	this->triggerLowThreshold = 0.0;
	this->paramControlBufferCount = 0;
	this->processSignalBufferCount = 0;
	this->processCount = 0;
	this->controlCount = 0;
	this->controlVoltageEnabled = false;
	this->gateOpen = true;
	this->gateStatus = 0;
	this->gateGain = 0.0;
	this->envTriggerStatus = 0;
	this->envTrigger = false;
	this->inputsSwitched = false;
	this->inPrevSignalLevel = 0.0;
	this->inSignalDeltaFilterOut = 0.0;
	this->inSignalLevel = 0.0;
	this->processingEnabled = false;
	this->comboTime = 0;
	this->comboLoaded = false;
	this->processSequence = NULL; // array of Process base class pointers
	this->controlSequence = NULL; // array of Control base class pointers

	this->audioOutputEnable = GPIOClass();

	this->portConSwitch[0] = GPIOClass();

	this->portConSwitch[1] = GPIOClass();

	this->portConSwitch[2] = GPIOClass();

}


#define dbg 1
#if(DISABLE_AUDIO_THREAD == 0)
Processing::Processing(NoiseGateUtility gateUtil, EnvTriggerUtility triggerUtil, int bufferSize):JackCpp::AudioIO(string("process"), 2,2)
{
#if(dbg >= 1)
	cout << "ENTERING: Processing Constructor" << endl;
#endif
	// TODO Auto-generated constructor stub
	reserveInPorts(8);
	reserveOutPorts(8);

#else
	Processing::Processing(NoiseGateUtility gateUtil, EnvTriggerUtility triggerUtil)
	{
#if(dbg >= 1)
	cout << "ENTERING: Processing Constructor with audio thread disabled" << endl;
#endif

#endif
	this->bufferSize = bufferSize;
	this->gateCloseThreshold = gateUtil.closeThres.value;
	this->gateOpenThreshold = gateUtil.openThres.value;
	this->gateClosedGain = gateUtil.gain.value;
	this->triggerHighThreshold = triggerUtil.highThres.value;
	this->triggerLowThreshold = triggerUtil.lowThres.value;
	this->paramControlBufferCount = 0;
	this->processSignalBufferCount = 0;
	this->processCount = 0;
	this->controlCount = 0;
	this->controlVoltageEnabled = false;
	this->gateOpen = true;
	this->gateStatus = 0;
	this->gateGain = 0.0;
	this->envTriggerStatus = 0;
	this->envTrigger = false;
	this->inputsSwitched = false;
	this->inPrevSignalLevel = 0.0;
	this->inSignalDeltaFilterOut = 0.0;
	this->inSignalLevel = 0.0;
	this->processingEnabled = false;
	this->comboTime = 0;
	this->comboLoaded = false;
	this->processSequence = new Processes*[20]; // array of Process base class pointers
	this->controlSequence = new Controls*[20]; // array of Control base class pointers

	char gpioStr[5];
	strcpy(gpioStr,"out");
	this->audioOutputEnable = GPIOClass(45);
	this->audioOutputEnable.export_gpio();
	this->audioOutputEnable.setdir_gpio(gpioStr);
	this->audioOutputEnable.setval_gpio(1);

	this->portConSwitch[0] = GPIOClass(CONTROL_PIN_0_NUMBER);
	this->portConSwitch[0].export_gpio();
	this->portConSwitch[0].setdir_gpio(gpioStr);
	this->portConSwitch[0].setval_gpio(1);

	this->portConSwitch[1] = GPIOClass(CONTROL_PIN_1_NUMBER);
	this->portConSwitch[1].export_gpio();
	this->portConSwitch[1].setdir_gpio(gpioStr);
	this->portConSwitch[1].setval_gpio(0);

	this->portConSwitch[2] = GPIOClass(CONTROL_PIN_2_NUMBER);
	this->portConSwitch[2].export_gpio();
	this->portConSwitch[2].setdir_gpio(gpioStr);
	this->portConSwitch[2].setval_gpio(0);


#if(dbg >= 1)
	cout << "EXITING: Processing Constructor" << endl;
#endif

};



Processing::~Processing()
{

}


string Processing::getComboName()
{
	return this->comboName;
}

void Processing::setComboName(string comboName)
{
	this->comboName = comboName;
}

void getSignalBufferPeaks()
{

}

#define dbg 0
bool Processing::areInputsSwitched(void) // see if JACK has connected input jacks backwards
{
	int chan1GndCount = 0;
	int chan2GndCount = 0;
	double inPosPeakArray[4][2];



#if(dbg >= 1)
	 cout << "***** ENTERING: Processing::" << endl;
	 cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	 cout << "***** EXITING: Processing::: " << status << endl;
#endif

	usleep(300000);

	this->portConSwitch[0].setval_gpio(0);
	this->portConSwitch[2].setval_gpio(1);
	usleep(100000);
	inPosPeakArray[0][0] = this->inPosPeak[0];
	inPosPeakArray[0][1] = this->inPosPeak[1];
	 cout << "inPosPeak[0]: " <<  this->inPosPeak[0] << "\tinNegPeak[0]: " <<  this->inNegPeak[0] << "\tinPosPeak[1]: " <<  this->inPosPeak[1] << "\tinNegPeak[1]: " <<  this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[1][0] = this->inPosPeak[0];
	inPosPeakArray[1][1] = this->inPosPeak[1];
	 cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[2][0] = this->inPosPeak[0];
	inPosPeakArray[2][1] = this->inPosPeak[1];
	 cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[3][0] = this->inPosPeak[0];
	inPosPeakArray[3][1] = this->inPosPeak[1];
	 cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;

	this->portConSwitch[0].setval_gpio(1);
	this->portConSwitch[2].setval_gpio(0);

	for(int i = 0; i < 4; i++)
	{
		if((0.098 < inPosPeakArray[i][0] && inPosPeakArray[i][0] < 0.1)) chan1GndCount++;
		if((0.098 < inPosPeakArray[i][1] && inPosPeakArray[i][1] < 0.1)) chan2GndCount++;
	}

	 cout << "chan1GndCount: " << chan1GndCount << "\tchan2GndCount: " << chan2GndCount << endl;
	if(chan1GndCount > chan2GndCount)
	{
		this->inputsSwitched = true;
		 cout << "*******************inputs switched"  << endl;
	}
	else
	{
		this->inputsSwitched = false;
		 cout << "*******************inputs not switched"  << endl;
	}

	return this->inputsSwitched;
}



void Processing::printProcessParameterControlBuffer()
{
		cout << "ENTERING: Processing::printProcessParameterControlBuffer" << endl;
		for(auto & paramContBuffer : this->processParamControlBufferArray)
		{
			if(paramContBuffer.destProcessParameter.objectName.empty()) break;

			cout << "paramContBuffer.src: " << paramContBuffer.srcControl.objectName;
			cout << ":" << paramContBuffer.srcControl.portName;
			cout << "\t\tparamContBuffer.parameterValueIndex: " << paramContBuffer.parameterValueIndex;
			cout << "\t\tparamContBuffer.dest: " << paramContBuffer.destProcessParameter.objectName;
			cout << ":" << paramContBuffer.destProcessParameter.portName << endl;
		}
		cout << "EXITING: Processing::printProcessParameterControlBuffer" << endl;
}



#define dbg 1
int Processing::loadCombo( ComboStruct comboStruct)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "ENTERING: Processing::loadCombo" << endl;
#endif

	 try
	 {

		 if(this->comboLoaded == false)
		 {
				 this->comboName = comboStruct.name;
				 cout << "comboName: " << this->comboName << endl;
				this->controlCount = comboStruct.controlCount;
				this->processCount = comboStruct.processCount;
				this->processSignalBufferCount = comboStruct.processSignalBufferCount;
				this->paramControlBufferCount = comboStruct.paramControlBufferCount;
				this->controlVoltageEnabled = comboStruct.controlVoltageEnabled;

				 cout << "Control Count: " << this->controlCount << endl;
				 cout << "Process Count: " << this->processCount << endl;
				 cout << "processSignalBuffer Count: " << this->processSignalBufferCount << endl;
				 cout << "paramControlBuffer Count: " << this->paramControlBufferCount << endl;

				if(this->controlVoltageEnabled == true) // set analog input switches to DC input
				{
					this->portConSwitch[0].setval_gpio(0);
					this->portConSwitch[1].setval_gpio(1);
				}
				else // set analog input switches to AC input
				{
					this->portConSwitch[0].setval_gpio(1);
					this->portConSwitch[1].setval_gpio(0);
				}

				this->processSequence = new Processes*[20];
				this->controlSequence = new Controls*[20];


				this->processSignalBufferArray.fill(ProcessSignalBuffer());
				this->processParamControlBufferArray.fill(ProcessParameterControlBuffer());
				this->processIndexMap.clear();
				this->controlIndexMap.clear();
				/********** Load data structures from comboStruct *********************/
				for(int i = 0; i < this->controlCount; i++)
				{
					switch(comboStruct.controlSequenceData[i].conType)
					{
					case 0: // normal
						this->controlSequence[i] = new Normal(comboStruct.controlSequenceData[i]);
						break;
					case 1:	// envelope generator
						this->controlSequence[i] = new EnvGen(comboStruct.controlSequenceData[i]);
						break;
					case 2: // low frequency oscillator
						this->controlSequence[i] = new LFO(comboStruct.controlSequenceData[i]);
						break;
					default:;
					}
					this->controlSequence[i]->setProcessParameterControlBufferArray(this->processParamControlBufferArray);
#if(dbg >= 2)
					for(auto & param : comboStruct.controlSequenceData[i].parameter)
					{
						cout << "control["<< i << "]: " << param.parameterName << endl;
					}
#endif
				}


				for(int i = 0; i < this->processCount; i++)
				{
					switch(comboStruct.processSequenceData[i].processTypeInt)
					{
						case 0:
							this->processSequence[i] = new Delay(comboStruct.processSequenceData[i]);
							break;
						case 1:
							this->processSequence[i] = new Filter3Band(comboStruct.processSequenceData[i]);
							break;
						case 2:
							this->processSequence[i] = new Filter3Band2(comboStruct.processSequenceData[i]);
							break;
						case 3:
							this->processSequence[i] = new LohiFilter(comboStruct.processSequenceData[i]);
							break;
						case 4:
							this->processSequence[i] = new Mixer(comboStruct.processSequenceData[i]);
							break;
						case 5:
							this->processSequence[i] = new Volume(comboStruct.processSequenceData[i]);
							break;
						case 6:
							this->processSequence[i] = new Waveshaper(comboStruct.processSequenceData[i]);
							break;
						default:;
					}
					this->processSequence[i]->setProcessSignalBufferArray(this->processSignalBufferArray);
					this->processSequence[i]->setProcessParameterControlBufferArray(this->processParamControlBufferArray);
					this->processSequence[i]->setFootswitchStatusArray(this->footswitchStatusArray);

				}

				for(int i = 0; i < this->processSignalBufferCount; i++)
				{
					this->processSignalBufferArray[i] = comboStruct.processSignalBufferArray[i];
				}
				for(int i = 0; i < this->paramControlBufferCount; i++)
				{
					this->processParamControlBufferArray[i] = comboStruct.processParamControlBufferArray[i];
				}
				this->processIndexMap.insert(comboStruct.processIndexMap.begin(),
											 comboStruct.processIndexMap.end());

				this->controlIndexMap.insert(comboStruct.controlIndexMap.begin(),
											 comboStruct.controlIndexMap.end());

				for(int i = 0; i < this->controlCount; i++)
				{

#if(dbg >= 2)
					cout << "loading control: " << this->controlSequence[i]->getName() << endl;
#endif
					 this->controlSequence[i]->load();
				}

				for(int i = 0; i < this->processCount; i++)
				{
#if(dbg >= 2)
					cout << "loading process: " << this->processSequence[i]->getName() << endl;
#endif
					 this->processSequence[i]->load();
				}

				/********** Set the ProcessSignalBufferVector indexes for the system IO ***********/

				this->inputSystemBufferIndex[0] = comboStruct.inputSystemBufferIndex[0];
				this->inputSystemBufferIndex[1] = comboStruct.inputSystemBufferIndex[1];
				this->outputSystemBufferIndex[0] = comboStruct.outputSystemBufferIndex[0];
				this->outputSystemBufferIndex[1] = comboStruct.outputSystemBufferIndex[1];

				this->comboLoaded = true;
		 }

#if(dbg >= 2)
	this->printProcessParameterControlBuffer();
#endif
	 }
	 catch(exception &e)
	 {
		 cout << "exception in Processing::loadCombo: " << e.what() << endl;
	 }

#if(dbg >= 1)
	 cout << "EXITING: Processing::loadCombo: " << status <<  endl;
#endif
	return status;
}


#define dbg 1
ComboStruct Processing::getComboStruct()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "ENTERING: Processing::getComboStruct" << endl;
	 cout << "comboName: " << this->comboName << endl;
#endif
	ComboStruct combo;
	try
	{
		combo.name = this->comboName;
		//int processIndex = 0;
		//int procParamIndex = 0;
		string processName;
		string procParamName;

		map<string,ProcessIndexing> processMap = this->processIndexMap;
		int bufferIndex = 0;

		for(auto & buffer : this->processParamControlBufferArray)
		{
			if(buffer.destProcessParameter.objectName.empty()) break;

			processName = buffer.destProcessParameter.objectName;

			//processIndex = processMap[processName].processSequenceIndex;
			procParamName = buffer.destProcessParameter.portName;

			//procParamIndex = processMap[processName].paramIndexMap[procParamName].paramIndex;
			int valueIndex = this->getProcessParameter(processName,procParamName);

			combo.processParamControlBufferArray[bufferIndex].destProcessParameter.objectName = processName;
			combo.processParamControlBufferArray[bufferIndex].destProcessParameter.portName = procParamName;
			combo.processParamControlBufferArray[bufferIndex].parameterValueIndex = valueIndex;
			cout << "[" << bufferIndex << "]: "<< processName << ":" << procParamName << "=" << valueIndex << endl;
 			bufferIndex++;
		}


		for(int controlIndex = 0; controlIndex < this->controlCount; controlIndex++)
		{

			combo.controlSequenceData[controlIndex].controlName = this->controlSequence[controlIndex]->getName();
			for(int paramIndex = 0; paramIndex < 10; paramIndex++)
			{
				combo.controlSequenceData[controlIndex].parameter[paramIndex].parameterName = this->controlSequence[controlIndex]->getParameterName(paramIndex);
				combo.controlSequenceData[controlIndex].parameter[paramIndex].value =
								this->getControlParameter(combo.controlSequenceData[controlIndex].controlName,
														  combo.controlSequenceData[controlIndex].parameter[paramIndex].parameterName);
#if(dbg >= 2)
				cout << "[" << controlIndex << ":" << paramIndex << "]: ";
				cout << combo.controlSequenceData[controlIndex].controlName;
				cout << ":" << combo.controlSequenceData[controlIndex].parameter[controlIndex].parameterName;
				cout << "=" << combo.controlSequenceData[controlIndex].parameter[controlIndex].value << endl;
#endif

			}

		}
	}
	catch(exception &e)
	{
		cout << "exception in Processing::getComboStruct: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "EXIT: Processing::getComboStruct: " << status << endl;
#endif
	return combo;
}




#define dbg 0
void Processing::triggerInputSignalFiltering()
{
	static double inMaxAmpFilter[16];
	static double inMaxAmpFilterOut;
	static int  inMaxAmpFilterIndex;
	static double inSignalDeltaFilter[16];
	static int  inSignalDeltaFilterIndex;




	if(inMaxAmpFilterIndex < 15)
	{
		inMaxAmpFilter[inMaxAmpFilterIndex++] = this->inMaxAmp[0];
	}
	else
	{
		inMaxAmpFilter[inMaxAmpFilterIndex] = this->inMaxAmp[0];
		inMaxAmpFilterIndex = 0;
	}

	inMaxAmpFilterOut = (inMaxAmpFilter[0] + inMaxAmpFilter[1] + inMaxAmpFilter[2] +
					inMaxAmpFilter[3] + inMaxAmpFilter[4] + inMaxAmpFilter[5] +
					inMaxAmpFilter[6] + inMaxAmpFilter[7] + inMaxAmpFilter[8] +
					inMaxAmpFilter[9] + inMaxAmpFilter[10] + inMaxAmpFilter[11] +
					inMaxAmpFilter[12] + inMaxAmpFilter[13] + inMaxAmpFilter[14] +
					inMaxAmpFilter[15])/16;
	this->inSignalLevel = inMaxAmpFilterOut;

#if(dbg >= 1)
	cout << "this->inSignalLevel: " <<  (this->inSignalLevel) << endl;
#endif

	if(inSignalDeltaFilterIndex < 15)
	{
		inSignalDeltaFilter[inSignalDeltaFilterIndex++] = this->inSignalLevel - this->inPrevSignalLevel;
	}
	else
	{
		inSignalDeltaFilter[inSignalDeltaFilterIndex] = this->inSignalLevel - this->inPrevSignalLevel;
		inSignalDeltaFilterIndex = 0;
	}
	this->inSignalDeltaFilterOut = (inSignalDeltaFilter[0] + inSignalDeltaFilter[1] + inSignalDeltaFilter[2] +
					inSignalDeltaFilter[3] + inSignalDeltaFilter[4] + inSignalDeltaFilter[5] +
					inSignalDeltaFilter[6] + inSignalDeltaFilter[7] + inSignalDeltaFilter[8] +
					inSignalDeltaFilter[9] + inSignalDeltaFilter[10] + inSignalDeltaFilter[11] +
					inSignalDeltaFilter[12] + inSignalDeltaFilter[13] + inSignalDeltaFilter[14] +
					inSignalDeltaFilter[15])/16;
}

#define dbg 0
void Processing::noiseGate(double *bufferIn, double *bufferOut)
{

#if(dbg >= 1)
	cout << "ENTERING:  Processing::noiseGate()" <<   endl;
#endif

	switch(this->gateStatus)
	{
		case 0:	// *************** NOISE GATE CLOSED *****************
			this->gateOpen = false;

			if((inputsSwitched == false && this->gateOpenThreshold < this->inSignalLevel) ||  // going above noise gate open threshold
							(inputsSwitched == true && this->gateOpenThreshold < this->inSignalLevel))
			{

#if(dbg >= 2)
				 cout << "PROCESSING: gateEnvStatus:0\tlevel above noise gate high threshold: going to case 1." << endl;
#endif
#if(dbg >= 2)
				 cout << "gateEnvStatus:0\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
#endif
				this->gateStatus = 1;
				this->gateGain = 1.0;
			}
			else
			{
				for(int i = 0; i < this->bufferSize; i++)
				{
					bufferOut[i] = this->gateGain*bufferIn[i];
				}
			}

			break;
		case 1:// **************** NOISE GATE OPENING ********************
		{
			this->gateGain = 1.0;
			this->gateStatus = 2;
			for(int i = 0; i < this->bufferSize; i++)
			{
				bufferOut[i] = this->gateGain*bufferIn[i];
			}
		}
		break;

		case 2: // **************** NOISE GATE OPEN ********************
#if(dbg >= 1)
			 cout << "case 1: noise gate open" << endl;
#endif
			this->gateOpen = true;


			if((inputsSwitched == false && this->gateCloseThreshold > this->inSignalLevel) ||  // going below noise gate close threshold
							(inputsSwitched == true && this->gateCloseThreshold > this->inSignalLevel))
			{
#if(dbg >= 2)
				 cout << "PROCESSING: gateEnvStatus:2\tlevel below noise gate low threshold: going to case 5." << endl;
#endif

				for(int i = 0; i < this->bufferSize; i++)
				{
					if(this->gateGain < this->gateClosedGain )
					{
						this->gateStatus = 3;
					}
					else
					{
						this->gateGain -= 0.00075;
					}
					bufferOut[i] = this->gateGain*bufferIn[i];
				}
			}
			else
			{
				for(int i = 0; i < this->bufferSize; i++)
				{
					bufferOut[i] = this->gateGain*bufferIn[i];
				}
			}

			break;

		case 3: // **************** NOISE GATE CLOSING ********************
#if(dbg >= 1)
			 cout << "PROCESSING: level below noise gate low threshold: "  << endl;
#endif
			this->gateOpen = false;

			this->gateStatus = 0;
			break;
		default:;

	}
#if(dbg >= 1)
	cout << "EXITING:  Processing::noiseGate()" <<   endl;
#endif

}

#define dbg 1
void Processing::envelopeTrigger()
{
#if(dbg >= 1)
	//cout << "ENTERING:  Processing::envelopeTrigger()" <<   endl;
#endif

	switch(this->envTriggerStatus)
	{
		case 0: // **************** WAITING FOR TRIGGER CONDITION TO SEND SIGNAL ********************

			this->envTrigger = false;

			if(this->inSignalLevel > this->triggerHighThreshold)
			{
				this->envTriggerStatus = 1;
#if(dbg >= 2)
				 cout << "PROCESSING: case 1: sending trigger signal" << endl;
#endif
			}
			break;
		case 1:	// **************** SEND TRIGGER SIGNAL ********************

			this->envTrigger = true;
			this->envTriggerStatus = 2;

			break;
		case 2:	// **************** WAIT FOR RESET CONDITION ********************

			this->envTrigger = false;
			if(this->inSignalLevel < this->triggerLowThreshold) // reset condition
			{
				this->envTriggerStatus = 0;
#if(dbg >= 2)
				 cout << "PROCESSING: case 0: reseting for next trigger condition" << endl;
#endif
			}

			break;
		default:;
	}
#if(dbg >= 1)
	//cout << "EXITING:  Processing::envelopeTrigger()" <<   endl;
#endif

}


#define dbg 0

int callbackPasses = 0;
int controlVoltageSampleCount = 0;
int secondMarkCounter = 0;
#if(DISABLE_AUDIO_THREAD == 0)



int Processing::audioCallback(jack_nframes_t nframes,
							  // A vector of pointers to each input port.
							  audioBufVector inBufs,
							  // A vector of pointers to each output port.
							  audioBufVector outBufs)
{
	int status = 0;
	double inPosPeak[2];
	double inNegPeak[2];
	double tempOutBufs[2];
	double controlVoltageFloat = 0.00;
	double controlVoltageAdjustedFloat = 0.00;
	int  controlVoltageIndex = 0;
	inPosPeak[0] = 0.00;
	inNegPeak[0] = 0.00;
	inPosPeak[1] = 0.00;
	inNegPeak[1] = 0.00;


	if(this->processingEnabled == true )
	{

		for(int i = 0; i < this->bufferSize; i++)
		{
			if(inputsSwitched)
			{
				this->noiseGateBuffer[i] = inBufs[1][i];
				this->processSignalBufferArray[this->inputSystemBufferIndex[1]].buffer[i] = inBufs[0][i];

			}
			else
			{
				this->noiseGateBuffer[i] = inBufs[0][i];
				this->processSignalBufferArray[this->inputSystemBufferIndex[1]].buffer[i] = inBufs[1][i];

			}

			if(inPosPeak[0] < inBufs[0][i]) inPosPeak[0] = inBufs[0][i];
			if(inNegPeak[0] > inBufs[0][i]) inNegPeak[0] = inBufs[0][i];
			if(inPosPeak[1] < inBufs[1][i]) inPosPeak[1] = inBufs[1][i];
			if(inNegPeak[1] > inBufs[1][i]) inNegPeak[1] = inBufs[1][i];

		}
		this->inPosPeak[0] = inPosPeak[0];
		this->inNegPeak[0] = inNegPeak[0];
		this->inPosPeak[1] = inPosPeak[1];
		this->inNegPeak[1] = inNegPeak[1];

		this->inMaxAmp[0] = this->inPosPeak[0] - this->inNegPeak[0];
		this->inMaxAmp[1] = this->inPosPeak[1] - this->inNegPeak[1];

		this->triggerInputSignalFiltering();

#if(dbg >= 2)
	 cout << "signal: " << this->inSignalLevel << ",signal delta: " << this->inSignalDeltaFilterOut << endl;
#endif

	this->noiseGate(this->noiseGateBuffer, this->processSignalBufferArray[this->inputSystemBufferIndex[0]].buffer);
	this->envelopeTrigger();
#if(AUDIO_THREAD_EFFECTS_BYPASS == 1)
	this->inputSystemBufferIndex[0] = 0;
	this->inputSystemBufferIndex[1] = 1;
	this->outputSystemBufferIndex[0] = 0;
	this->outputSystemBufferIndex[1] = 1;


	this->inPrevSignalLevel = this->inSignalLevel;
	for(int i = 0; i < this->bufferSize; i++)
	{
		this->processSignalBufferArray[this->outputSystemBufferIndex[0]].buffer[i] =
						this->processSignalBufferArray[this->inputSystemBufferIndex[0]].buffer[i];
		this->processSignalBufferArray[this->outputSystemBufferIndex[1]].buffer[i] =
						this->processSignalBufferArray[this->inputSystemBufferIndex[1]].buffer[i];
	}
#else

	//******************** Get control voltage from pedal ************************
	controlVoltageFloat =  this->processSignalBufferArray[this->inputSystemBufferIndex[1]].buffer[0]+1.000;
	controlVoltageAdjustedFloat =  49.50*controlVoltageFloat;
	controlVoltageIndex = static_cast<int>(controlVoltageAdjustedFloat);


	if(controlVoltageIndex < 0) controlVoltageIndex = 0;
	if(controlVoltageIndex > 99) controlVoltageIndex = 99;


	//***************************** Run Controls for manipulating process parameters *************

	for(int i = 0; i < this->controlCount; i++)
	{
		this->controlSequence[i]->run(controlVoltageIndex,this->envTrigger);
	}

	//****************************** Run processes *******************************************
	for(int i = 0; i < this->processCount; i++)
	{
			this->processSequence[i]->run();
	}

#endif
	for(int i = 0; i < this->bufferSize; i++)
	{

		if(inputsSwitched)
		{
			tempOutBufs[1] = this->processSignalBufferArray[this->outputSystemBufferIndex[0]].buffer[i];
			tempOutBufs[0] = this->processSignalBufferArray[this->outputSystemBufferIndex[1]].buffer[i];
		}
		else
		{
			tempOutBufs[0] = this->processSignalBufferArray[this->outputSystemBufferIndex[0]].buffer[i];
			tempOutBufs[1] =this->processSignalBufferArray[this->outputSystemBufferIndex[1]].buffer[i];
		}

		outBufs[0][i] = (float)tempOutBufs[0];
		outBufs[1][i] = (float)tempOutBufs[1];
	}



#if(SIGPROC_DBG)
	int  bufferIndex;
	 cout << "BUFFER AVERAGES: " << endl;
	for(bufferIndex = 0; bufferIndex < this->processSignalBufferCount; bufferIndex++)
	{
		 cout << this->processSignalBufferArray[bufferIndex].average << ", ";
	}
	 cout << endl;
#endif
	}
	else
	{
		for(int i = 0; i < this->bufferSize; i++)
		{
			outBufs[0][i] = inBufs[0][i];
			outBufs[1][i] = inBufs[1][i];

			if(inPosPeak[0] < inBufs[0][i]) inPosPeak[0] = inBufs[0][i];
			if(inNegPeak[0] > inBufs[0][i]) inNegPeak[0] = inBufs[0][i];
			if(inPosPeak[1] < inBufs[1][i]) inPosPeak[1] = inBufs[1][i];
			if(inNegPeak[1] > inBufs[1][i]) inNegPeak[1] = inBufs[1][i];
		}

		this->inPosPeak[0] = inPosPeak[0];
		this->inNegPeak[0] = inNegPeak[0];
		this->inPosPeak[1] = inPosPeak[1];
		this->inNegPeak[1] = inNegPeak[1];


	}


	return status;
}

#endif

#define dbg 1
void Processing::unloadCombo()
{

#if(dbg >= 1)
	 cout << "ENTERING: Processing::unloadCombo" << endl;
#endif

	try
	{
		if(this->comboLoaded == true)
		{
			for(int i = this->processCount-1; i >= 0; i--)
			{
#if(dbg >= 2)
				cout << "unloading process: " << this->processSequence[i]->getName() << endl;
#endif
				this->processSequence[i]->stop();
				delete this->processSequence[i];
				this->processSequence[i] = NULL;
			}

#if(dbg >= 2)
			cout << "deleting process arrays" << endl;
#endif
			delete [] this->processSequence;

			for(int i = this->controlCount-1; i >= 0; i--)
			{
#if(dbg >= 2)
				cout << "unloading control: " << this->controlSequence[i]->getName() << endl;
#endif
				this->controlSequence[i]->stop();
				delete this->controlSequence[i];
				this->controlSequence[i] = NULL;
			}

#if(dbg >= 2)
			cout << "deleting control arrays" << endl;
#endif
			delete [] this->controlSequence;

			this->comboLoaded = false;
		}
	}

	catch(exception &e)
	{
		cout << "exception in Processing::unloadCombo: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "EXITING: Processing::unloadCombo: "  << endl;
#endif

}



#define dbg 0
int Processing::updateFootswitch(bool *footswitchStatus)
{
	int status = 0;

#if(dbg >= 1)
	 cout << "ENTERING: Processing::updateFootswitch" << endl;
#endif

#if(dbg >= 1)
	 cout << "footswitchStatus: ";
#endif
	for(int i = 0; i < FOOTSWITCH_COUNT; i++)
	{
		this->footswitchStatusArray[i] = footswitchStatus[i];
#if(dbg >= 1)
		 cout << this->footswitchStatusArray[i] << ",";
#endif
	}
#if(dbg >= 1)
	 cout << endl;
#endif

#if(dbg >= 1)
	 cout << "EXITING: Processing::updateFootswitch: " << status << endl;
#endif
	return status;
}



#define dbg 0
int Processing::updateProcessParameter(string processName, string parameter, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "ENTERING: Processing::updateProcessParameter" << endl;
	 cout << "processName: " << processName << "\parameter: " << parameter << "\tparameterValue: " << parameterValue << endl;
#endif


	int connectedBufferIndex = this->processIndexMap[processName.c_str()].
					paramIndexMap[parameter.c_str()].connectedBufferIndex;
#if(dbg >= 2)
	cout << "connectedBufferIndex: " << connectedBufferIndex << endl;
#endif
	this->processParamControlBufferArray[connectedBufferIndex].parameterValueIndex = parameterValue;

#if(dbg >= 1)
	 cout << "EXITING: Processing::updateProcessParameter: " << status << endl;
#endif



	return status;
}
#define dbg 0
int Processing::getProcessParameter(string processName, string parameter)
{
	int status = 0;

#if(dbg >= 1)
	 cout << "ENTERING: Processing::getProcessParameter" << endl;
#endif
	int valueIndex = 0;
	try
	{
		int connectedBufferIndex = this->processIndexMap[processName.c_str()].
						paramIndexMap[parameter.c_str()].connectedBufferIndex;

#if(dbg >= 2)
		cout << "connectedBufferIndex: " << connectedBufferIndex << endl;
#endif
		if(0 > connectedBufferIndex || 59 < connectedBufferIndex)
		{
			cout << "connectedBufferIndex out of bounds: " << connectedBufferIndex << endl;
			status = -1;
			connectedBufferIndex = -1;
		}

		valueIndex = this->processParamControlBufferArray[connectedBufferIndex].parameterValueIndex;
		if(0 > valueIndex || 99 < valueIndex)
		{
			cout << "valueIndex out of bounds: " << valueIndex << endl;
			status = -1;
			valueIndex = -1;
		}

	}
	catch(exception &e)
	{
		cout << "exception in Processing::getProcessParameter: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "EXITING: Processing::getProcessParameter: " << status << endl;
#endif
	return valueIndex;
}




#define dbg 0
int Processing::updateControlParameter(string controlName, string parameter, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "ENTERING: Processing::updateControlParameter" << endl;
	 cout << "controlName: " << controlName << "\tparameter: " << parameter << "\tparameterValue: " << parameterValue << endl;
#endif
	try
	{
		int contIndex = this->controlIndexMap[controlName.c_str()].controlIndex;
		if(0 > contIndex || 99 < contIndex)
		{
			cout << "contIndex out of bounds: " << contIndex << endl;
			status = -1;
		}

		int paramIndex = this->controlIndexMap[controlName.c_str()].
						paramIndexMap[parameter.c_str()].contParamIndex;
		if(0 > paramIndex || 99 < paramIndex)
		{
			cout << "paramIndex out of bounds: " << paramIndex << endl;
			status = -1;
		}

#if(dbg >= 2)
		cout << "contIndex: " << contIndex << "paramIndex: " << paramIndex << endl;
#endif
		if(status == 0)
			this->controlSequence[contIndex]->setParameter(paramIndex,parameterValue);
	}
	catch(exception &e)
	{
		cout << "exception in Processing::updateControlParameter: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "EXITING: Processing::updateControlParameter: " << status << endl;
#endif

	return status;
}

#define dbg 0
int Processing::getControlParameter(string controlName, string parameter)
{
	int status = 0;
	int valueIndex = 0;

#if(dbg >= 1)
	 cout << "ENTERING: Processing::getControlParameter" << endl;
	 cout << "controlName: " << controlName << "\tparameter: " << parameter << endl;
#endif
	int contIndex = this->controlIndexMap[controlName.c_str()].controlIndex;
	int paramIndex = this->controlIndexMap[controlName.c_str()].
					paramIndexMap[parameter.c_str()].contParamIndex;

	valueIndex = this->controlSequence[contIndex]->getParameter(paramIndex);
	if(0 <= valueIndex && valueIndex <= 99) status = valueIndex;
	else status = -1;


#if(dbg >= 1)
	 cout << "EXITING: Processing::getControlParameter: " << status << endl;
#endif


	return status;
}




#define dbg 0
void Processing::enableProcessing()
{

#if(dbg==1)
	 cout << "ENTERING: Processing::enableProcessing" << endl;
#endif

	this->processingEnabled = true;
#if(dbg>=1)
	 cout << "EXITING: Processing::enableProcessing" << endl;
#endif


}

#define dbg 0
void Processing::disableProcessing()
{
#if(dbg==1)
	 cout << "ENTERING: Processing::disableProcessing" << endl;
#endif

	this->processingEnabled = false;
#if(dbg>=1)
	 cout << "EXITING: Processing::disableProcessing" << endl;
#endif


}

#define dbg 0
void Processing::enableAudioOutput()
{

#if(dbg >= 1)
	 cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x07");
#if(dbg >= 1)
	 cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif

}


void Processing::disableAudioOutput()
{

#if(dbg >= 1)
	 cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x0F");
#if(dbg >= 1)
	 cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif

}


#define dbg 1

void Processing::setNoiseGateUtility(NoiseGateUtility gateUtil)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: Processing::setNoiseGateUtility" << endl;
#endif

	try
	{
		this->gateCloseThreshold = gateUtil.closeThres.value;
		this->gateOpenThreshold = gateUtil.openThres.value;
		this->gateClosedGain = gateUtil.gain.value;
	}
	catch(exception &e)
	{
		cout << "exception in Processing::setNoiseGateUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: Processing::setNoiseGateUtility" << endl;
#endif
}

void Processing::setEnvTriggerUtility(EnvTriggerUtility triggerUtil)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: Processing::setEnvTriggerUtility" << endl;
#endif

	try
	{
		this->triggerHighThreshold = triggerUtil.highThres.value;
		this->triggerLowThreshold = triggerUtil.lowThres.value;
	}
	catch(exception &e)
	{
		cout << "exception in Processing::setEnvTriggerUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: Processing::setEnvTriggerUtility" << endl;
#endif

}

#define dbg 1
void Processing::loadComponentSymbolVector()
{
#if(dbg >= 1)
	 cout << "ENTERING: Processing::loadComponentSymbolVector" << endl;
#endif

	try
	{
		componentSymbolVector.push_back(delaybSymbol);
		componentSymbolVector.push_back(filter3bbSymbol);
		componentSymbolVector.push_back((filter3bb2Symbol));
		componentSymbolVector.push_back((lohifilterbSymbol));
		componentSymbolVector.push_back((mixerbSymbol));
		componentSymbolVector.push_back((volumebSymbol));
		componentSymbolVector.push_back((waveshaperbSymbol));

	}
	catch(exception &e)
	{
		cout << "exception in Processing::loadComponentSymbolVector: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "EXITING: Processing::loadComponentSymbolVector" << endl;
#endif

}


void Processing::loadControlSymbolVector()
{
#if(dbg >= 1)
	 cout << "ENTERING: Processing::loadControlSymbolVector" << endl;
#endif

	try
	{
		controlSymbolVector.push_back(normSymbol);
		controlSymbolVector.push_back(envGenSymbol);
		controlSymbolVector.push_back(lfoSymbol);
	}
	catch(exception &e)
	{
		cout << "exception in Processing::loadControlSymbolVector: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "EXITING: Processing::loadControlSymbolVector" << endl;
#endif

}
#define dbg 1
void Processing::loadSymbols()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: Processing::loadSymbols" << endl;
#endif
	try
	{

		this->loadComponentSymbolVector();
		this->loadControlSymbolVector();
	}
	catch(exception &e)
	{
		cout << "exception in Processing::loadSymbols: " << e.what() << endl;
	}
#if(dbg >= 1)
	 cout << "***** EXITING: Processing::loadSymbols" << endl;
#endif

}

vector<string> Processing::getComponentSymbols()
{
	return this->componentSymbolVector;
}
vector<string> Processing::getControlSymbols()
{
	return this->controlSymbolVector;
}



