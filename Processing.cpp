/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */
#include "config.h"
#include "Processing.h"

#include "valueArrays.h"

extern bool debugOutput;


#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43



#define dbg 1
Processing::Processing(ProcessingUtility processingUtil):JackCpp::AudioIO(string("process"), 2,2)
{
#if(dbg >= 1)
	cout << "ENTERING: Processing Constructor" << endl;
#endif
	// TODO Auto-generated constructor stub
	reserveInPorts(8);
	reserveOutPorts(8);
	this->bufferSize = processingUtil.procUtil.bufferSize;
	cout << "this->bufferSize: " << this->bufferSize << endl;
	double gateCloseThreshold = processingUtil.noiseGateUtil.closeThres;
	double gateOpenThreshold = processingUtil.noiseGateUtil.openThres;
	double gateClosedGain = processingUtil.noiseGateUtil.gain;
	double triggerHighThreshold = processingUtil.triggerUtil.highThres;
	double triggerLowThreshold = processingUtil.triggerUtil.lowThres;
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


}

Processing::~Processing()
{
	// TODO Auto-generated destructor stub


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
	double inNegPeakArray[4][2];


#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Processing::" << endl;
	if(debugOutput) cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Processing::: " << status << endl;
#endif

	usleep(300000);

	this->portConSwitch[0].setval_gpio(0);
	this->portConSwitch[2].setval_gpio(1);
	usleep(100000);
	inPosPeakArray[0][0] = this->inPosPeak[0];
	inNegPeakArray[0][0] = this->inNegPeak[0];
	inPosPeakArray[0][1] = this->inPosPeak[1];
	inNegPeakArray[0][1] = this->inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " <<  this->inPosPeak[0] << "\tinNegPeak[0]: " <<  this->inNegPeak[0] << "\tinPosPeak[1]: " <<  this->inPosPeak[1] << "\tinNegPeak[1]: " <<  this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[1][0] = this->inPosPeak[0];
	inNegPeakArray[1][0] = this->inNegPeak[0];
	inPosPeakArray[1][1] = this->inPosPeak[1];
	inNegPeakArray[1][1] = this->inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[2][0] = this->inPosPeak[0];
	inNegPeakArray[2][0] = this->inNegPeak[0];
	inPosPeakArray[2][1] = this->inPosPeak[1];
	inNegPeakArray[2][1] = this->inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[3][0] = this->inPosPeak[0];
	inNegPeakArray[3][0] = this->inNegPeak[0];
	inPosPeakArray[3][1] = this->inPosPeak[1];
	inNegPeakArray[3][1] = this->inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << this->inPosPeak[0] << "\tinNegPeak[0]: " << this->inNegPeak[0] << "\tinPosPeak[1]: " << this->inPosPeak[1] << "\tinNegPeak[1]: " << this->inNegPeak[1] << endl;

	this->portConSwitch[0].setval_gpio(1);
	this->portConSwitch[2].setval_gpio(0);

	for(int i = 0; i < 4; i++)
	{
		if((0.098 < inPosPeakArray[i][0] && inPosPeakArray[i][0] < 0.1)) chan1GndCount++;
		if((0.098 < inPosPeakArray[i][1] && inPosPeakArray[i][1] < 0.1)) chan2GndCount++;
	}

	if(debugOutput) cout << "chan1GndCount: " << chan1GndCount << "\tchan2GndCount: " << chan2GndCount << endl;
	if(chan1GndCount > chan2GndCount)
	{
		this->inputsSwitched = true;
		if(debugOutput) cout << "*******************inputs switched"  << endl;
	}
	else
	{
		this->inputsSwitched = false;
		if(debugOutput) cout << "*******************inputs not switched"  << endl;
	}

	return this->inputsSwitched;
}



void Processing::printProcessParameterControlBuffer()
{

	if(debugOutput)
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
}



#define dbg 1
int Processing::loadComboStructData(struct ComboStruct comboStruct)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::loadComboStructData" << endl;
#endif

	this->controlCount = comboStruct.controlCount;
	this->processCount = comboStruct.processCount;
	this->processSignalBufferCount = comboStruct.processSignalBufferCount;
	this->paramControlBufferCount = comboStruct.paramControlBufferCount;
	this->controlVoltageEnabled = comboStruct.controlVoltageEnabled;

	if(debugOutput) cout << "Control Count: " << this->controlCount << endl;
	if(debugOutput) cout << "Process Count: " << this->processCount << endl;
	if(debugOutput) cout << "processSignalBuffer Count: " << this->processSignalBufferCount << endl;
	if(debugOutput) cout << "paramControlBuffer Count: " << this->paramControlBufferCount << endl;

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

	this->controlSequence.fill(this->emptyContEvent);
	this->processSequence.fill(this->emptyProcEvent);
	this->processSignalBufferArray.fill(this->emptyProcSigBuffer);
	this->processParamControlBufferArray.fill(this->emptyProcParamContBuffer);
	this->processIndexMap.clear();
	this->controlIndexMap.clear();
	/********** Load data structures from comboStruct *********************/
	for(int i = 0; i < this->controlCount; i++)
	{
		this->controlSequence[i] = comboStruct.controlSequence[i];
	}
	for(int i = 0; i < this->processCount; i++)
	{
		this->processSequence[i] = comboStruct.processSequence[i];
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

		this->printIndexMappedControlData();

	for(int i = 0; i < this->controlCount; i++)
	{
		switch(this->controlSequence[i].conType)
		{
		case 0: // normal
			normal('l', this->envTrigger, 0, &this->controlSequence[i], this->processParamControlBufferArray);
			break;
		case 1:	// envelope generator
			envGen('l', this->envTrigger, 0, &this->controlSequence[i],this->processParamControlBufferArray);
			break;
		case 2: // low frequency oscillator
			lfo('l', this->envTrigger, 0, &this->controlSequence[i],this->processParamControlBufferArray);
			break;
		default:;
		}

	}

	for(int i = 0; i < this->processCount; i++)
	{
		switch(this->processSequence[i].processTypeInt)
		{
			case 0:
				delayb('l', &this->processSequence[i], this->processSignalBufferArray,
				       this->processParamControlBufferArray, this->footswitchStatus);
				break;
			case 1:
				filter3bb('l', &this->processSequence[i], this->processSignalBufferArray,
					  this->processParamControlBufferArray,this->footswitchStatus);
				break;
			case 2:
				filter3bb2('l', &this->processSequence[i], this->processSignalBufferArray,
					   this->processParamControlBufferArray,this->footswitchStatus);
				break;
			case 3:
				lohifilterb('l', &this->processSequence[i], this->processSignalBufferArray,
					    this->processParamControlBufferArray,this->footswitchStatus);
				break;
			case 4:
				mixerb('l', &this->processSequence[i], this->processSignalBufferArray
				       ,this->processParamControlBufferArray,this->footswitchStatus);
				break;
			case 5:
				volumeb('l', &this->processSequence[i], this->processSignalBufferArray,
					this->processParamControlBufferArray,this->footswitchStatus);
				break;
			case 6:
				waveshaperb('l', &this->processSequence[i], this->processSignalBufferArray,
					    this->processParamControlBufferArray,this->footswitchStatus);
				break;
			default:;
		}

	}

	for(int bufferIndex = 0; bufferIndex < this->processSignalBufferCount; bufferIndex++)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "clearing procBufferArray[" << bufferIndex << "]:" << endl;
#endif
		clearProcBuffer(&this->processSignalBufferArray[bufferIndex]);
	}

	/********** Set the ProcessSignalBufferVector indexes for the system IO ***********/

	this->inputSystemBufferIndex[0] = comboStruct.inputSystemBufferIndex[0];
	this->inputSystemBufferIndex[1] = comboStruct.inputSystemBufferIndex[1];
	this->outputSystemBufferIndex[0] = comboStruct.outputSystemBufferIndex[0];
	this->outputSystemBufferIndex[1] = comboStruct.outputSystemBufferIndex[1];

#if(dbg >= 2)
	this->printProcessParameterControlBuffer();
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::loadComboStructData: " << status << endl;
#endif
	return status;
}


#define dbg 0
ComboStruct Processing::getComboStruct()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::getComboStruct" << endl;
	if(debugOutput) cout << "comboName: " << this->getComboName() << endl;
#endif
	ComboStruct combo;
	try
	{
		string comboName = this->comboName;
		int processIndex = 0;
		int procParamIndex = 0;
		string processName;
		string procParamName;

		map<string,ProcessIndexing> processMap = this->processIndexMap;
		int bufferIndex = 0;

		for(auto & buffer : this->processParamControlBufferArray)
		{
			if(buffer.destProcessParameter.objectName.empty()) break;

			processName = buffer.destProcessParameter.objectName;

			processIndex = processMap[processName].processSequenceIndex;
			procParamName = buffer.destProcessParameter.portName;

			procParamIndex = processMap[processName].paramIndexMap[procParamName].paramIndex;
			int valueIndex = this->getProcessParameter(processName,procParamName);

			combo.processParamControlBufferArray[bufferIndex].destProcessParameter.objectName = processName;
			combo.processParamControlBufferArray[bufferIndex].destProcessParameter.portName = procParamName;
			combo.processParamControlBufferArray[bufferIndex].parameterValueIndex = valueIndex;

			bufferIndex++;
		}

		int controlIndex = 0;
		for(auto & control : this->controlSequence)
		{
			int contParamIndex = 0;
			combo.controlSequence[controlIndex].controlName = control.controlName;
			for(auto & parameter : control.parameter)
			{
				if(parameter.parameterName.empty() == true) break;
				combo.controlSequence[controlIndex].parameter[contParamIndex].parameterName = parameter.parameterName;
				combo.controlSequence[controlIndex].parameter[contParamIndex].value =
				this->getControlParameter(control.controlName, parameter.parameterName);

				cout << combo.controlSequence[controlIndex].controlName;
				cout << ":" << combo.controlSequence[controlIndex].parameter[contParamIndex].parameterName;
				cout << "=" << combo.controlSequence[controlIndex].parameter[contParamIndex].value << endl;
				contParamIndex++;
			}
			controlIndex++;
		}
	}
	catch(exception &e)
	{
		cout << "exception in Processing::getComboStruct: " << e.what() << endl;
		status = -1;
	}



#if(dbg >= 1)
	if(debugOutput) cout << "EXIT: Processing::getComboStruct: " << status << endl;
#endif
	return combo;
}





static void printSignalBufferData(ProcessSignalBuffer procSigBuffer)
{
	cout << "process signal data: " << procSigBuffer.srcProcess.objectName << ":  ";
	for(int i = 0; i < 10; i++)
	{
		cout << procSigBuffer.buffer[i] << ",";
	}
	cout << endl;
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
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:0\tlevel above noise gate high threshold: going to case 1." << endl;
#endif
#if(dbg >= 2)
			if(debugOutput) cout << "gateEnvStatus:0\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
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
		if(debugOutput) cout << "case 1: noise gate open" << endl;
#endif
		this->gateOpen = true;


		if((inputsSwitched == false && this->gateCloseThreshold > this->inSignalLevel) ||  // going below noise gate close threshold
				(inputsSwitched == true && this->gateCloseThreshold > this->inSignalLevel))
		{
#if(dbg >= 2)
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:2\tlevel below noise gate low threshold: going to case 5." << endl;
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
	if(debugOutput) cout << "PROCESSING: level below noise gate low threshold: "  << endl;
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

#define dbg 0
void Processing::envelopeTrigger()
{
#if(dbg >= 1)
	cout << "ENTERING:  Processing::envelopeTrigger()" <<   endl;
#endif

	switch(this->envTriggerStatus)
	{
	case 0: // **************** WAITING FOR TRIGGER CONDITION TO SEND SIGNAL ********************

		this->envTrigger = false;

		if(this->inSignalLevel > this->triggerHighThreshold)
		{
			this->envTriggerStatus = 1;
#if(dbg >= 2)
		if(debugOutput) cout << "PROCESSING: case 1: sending trigger signal" << endl;
#endif
		}
		break;
	case 1:	// **************** SEND TRIGGER SIGNAL ********************

		this->envTrigger = true;
		this->envTriggerStatus = 2;

		break;
	case 2:	// **************** WAIT FOR REST CONDITION ********************

		this->envTrigger = false;
		if(this->inSignalLevel < this->triggerLowThreshold) // reset condition
		{
			this->envTriggerStatus = 0;
#if(dbg >= 2)
		if(debugOutput) cout << "PROCESSING: case 0: reseting for next trigger condition" << endl;
#endif
		}

		break;
	default:;
	}
#if(dbg >= 1)
	cout << "EXITING:  Processing::envelopeTrigger()" <<   endl;
#endif

}


#define dbg 0

int callbackPasses = 0;
int controlVoltageSampleCount = 0;
int secondMarkCounter = 0;

	int Processing::audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs)
	{
		int status = 0;

		double inPosPeak[2];
		double inNegPeak[2];
		double outPosPeak[2];
		double outNegPeak[2];
		double tempOutBufs[2];
		double controlVoltageFloat = 0.00;
		double controlVoltageAdjustedFloat = 0.00;
		int  controlVoltageIndex = 0;
		inPosPeak[0] = 0.00;
		inNegPeak[0] = 0.00;
		inPosPeak[1] = 0.00;
		inNegPeak[1] = 0.00;
		outPosPeak[0] = 0.00;
		outNegPeak[0] = 0.00;
		outPosPeak[1] = 0.00;
		outNegPeak[1] = 0.00;

		startTimer();
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
			if(debugOutput) cout << "signal: " << this->inSignalLevel << ",signal delta: " << this->inSignalDeltaFilterOut << endl;
		#endif

			this->noiseGate(this->noiseGateBuffer, this->processSignalBufferArray[this->inputSystemBufferIndex[0]].buffer);
			this->envelopeTrigger();

			this->inPrevSignalLevel = this->inSignalLevel;

			//******************** Get control voltage from pedal ************************
			controlVoltageFloat =  this->processSignalBufferArray[this->inputSystemBufferIndex[1]].buffer[0]+1.000;
			controlVoltageAdjustedFloat =  49.50*controlVoltageFloat;
			controlVoltageIndex = (int)(controlVoltageAdjustedFloat);


			if(controlVoltageIndex < 0) controlVoltageIndex = 0;
			if(controlVoltageIndex > 99) controlVoltageIndex = 99;
			//***************************** Run Controls for manipulating process parameters *************

			for(int i = 0; i < this->controlCount; i++)
			{
				switch(this->controlSequence[i].conType)
				{
				case 0: // normal
					normal('r', this->envTrigger, controlVoltageIndex, &this->controlSequence[i],this->processParamControlBufferArray);
					break;
				case 1:	// envelope generator
					envGen('r', this->envTrigger, controlVoltageIndex, &this->controlSequence[i],this->processParamControlBufferArray);
					break;
				case 2: // low frequency oscillator
					lfo('r', this->envTrigger, controlVoltageIndex, &this->controlSequence[i],this->processParamControlBufferArray);
					break;
				}
			}

			//****************************** Run processes *******************************************

	for(int i = 0; i < this->processCount; i++)
	{
				switch(this->processSequence[i].processTypeInt)
				{
					case 0:
						delayb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 1:
						filter3bb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 2:
						filter3bb2('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 3:
						lohifilterb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 4:
						mixerb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 5:
						volumeb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					case 6:
						waveshaperb('r', &this->processSequence[i], this->processSignalBufferArray,this->processParamControlBufferArray,this->footswitchStatus);
						break;
					default:;
				}
			}


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

				outBufs[0][i] = tempOutBufs[0];
				outBufs[1][i] = tempOutBufs[1];
			}



		#if(SIGPROC_DBG)
			int  bufferIndex;
			if(debugOutput) cout << "BUFFER AVERAGES: " << endl;
			for(bufferIndex = 0; bufferIndex < this->processSignalBufferCount; bufferIndex++)
			{
				if(debugOutput) cout << this->processSignalBufferArray[bufferIndex].average << ", ";
			}
			if(debugOutput) cout << endl;
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

	#if(TIMING_DBG == 1)
		comboTime = stopTimer("audio processing");
	#else
		comboTime = stopTimer(NULL);
	#endif

		return status;
	}




struct peak{
	double amplitude;
	int bufferPosition;
};

int zeroCrossingCountArray[2];
int zeroCrossingCount;
struct peak tempPreFilterHigh;
struct peak tempPreFilterLow;
struct peak tempPostFilterHigh;
struct peak tempPostFilterLow;
struct peak preFilterHigh;
struct peak preFilterLow;
struct peak postFilterHigh;
struct peak postFilterLow;
int getPitchFilterIndexAddendIndex = 0;
int tempPitch;

#define dbg 0
int Processing::updateFootswitch(bool *footswitchStatus)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Combo::updateFootswitch" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "footswitchStatus: ";
#endif
	for(int i = 0; i < FOOTSWITCH_COUNT; i++)
	{
		this->footswitchStatus[i] = footswitchStatus[i];
#if(dbg >= 1)
		if(debugOutput) cout << this->footswitchStatus[i] << ",";
#endif
	}
#if(dbg >= 1)
	if(debugOutput) cout << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Combo::updateFootswitch: " << status << endl;
#endif
	return status;
}



#define dbg 0
int Processing::updateProcessParameter(string processName, string parameter, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::updateProcessParameter" << endl;
	if(debugOutput) cout << "processName: " << processName << "\parameter: " << parameter << "\tparameterValue: " << parameterValue << endl;
#endif


	int connectedBufferIndex = this->processIndexMap[processName.c_str()].
					paramIndexMap[parameter.c_str()].connectedBufferIndex;

	cout << "connectedBufferIndex: " << connectedBufferIndex << endl;
	this->processParamControlBufferArray[connectedBufferIndex].parameterValueIndex = parameterValue;

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::updateProcessParameter: " << status << endl;
#endif



	return status;
}
#define dbg 2
int Processing::getProcessParameter(string processName, string parameter)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::getProcessParameter" << endl;
	if(debugOutput) cout << "comboName: " << this->comboName << "processName: " << processName << "\parameter: " << parameter << endl;
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
	if(debugOutput) cout << "EXITING: Processing::getProcessParameter: " << status << endl;
#endif
	return valueIndex;
}




#define dbg 1
int Processing::updateControlParameter(string controlName, string parameter, int parameterValue)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::updateControlParameter" << endl;
	if(debugOutput) cout << "controlName: " << controlName << "\tparameter: " << parameter << "\tparameterValue: " << parameterValue << endl;
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
			this->controlSequence[contIndex].parameter[paramIndex].value = parameterValue;
	}
	catch(exception &e)
	{
		cout << "exception in Processing::updateControlParameter: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::updateControlParameter: " << status << endl;
#endif

	return status;
}

#define dbg 0
int Processing::getControlParameter(string controlName, string parameter)
{
	int status = 0;
	int valueIndex = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::getControlParameter" << endl;
	if(debugOutput) cout << "controlName: " << controlName << "\tparameter: " << parameter << endl;
#endif
	int contIndex = this->controlIndexMap[controlName.c_str()].controlIndex;
	int paramIndex = this->controlIndexMap[controlName.c_str()].
					paramIndexMap[parameter.c_str()].contParamIndex;

	valueIndex = this->controlSequence[contIndex].parameter[paramIndex].value;
	if(0 <= valueIndex && valueIndex <= 99) status = valueIndex;
	else status = -1;


#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::getControlParameter: " << status << endl;
#endif


	return status;
}




#define dbg 0
void Processing::enableProcessing()
{

#if(dbg==1)
	if(debugOutput) cout << "ENTERING: Processing::enableProcessing" << endl;
#endif

	this->processingEnabled = true;
#if(dbg>=1)
	if(debugOutput) cout << "EXITING: Processing::enableProcessing" << endl;
#endif


}

#define dbg 0
void Processing::disableProcessing()
{
#if(dbg==1)
	if(debugOutput) cout << "ENTERING: Processing::disableProcessing" << endl;
#endif

	this->processingEnabled = false;
#if(dbg>=1)
	if(debugOutput) cout << "EXITING: Processing::disableProcessing" << endl;
#endif


}

#define dbg 0
void Processing::enableAudioOutput()
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x07");
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif

}


void Processing::disableAudioOutput()
{


#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x0F");
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif


}



#define dbg 1
void Processing::setNoiseGateCloseThreshold(double closeThres)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::setNoiseGateCloseThreshold: " << closeThres << endl;
#endif
	this->gateCloseThreshold = closeThres;
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::setNoiseGateCloseThreshold" << endl;
#endif


}

void Processing::setNoiseGateOpenThreshold(double openThres)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::setNoiseGateOpenThreshold: " << openThres << endl;
#endif
	this->gateOpenThreshold = openThres;
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::setNoiseGateOpenThreshold" << endl;
#endif


}

void Processing::setNoiseGateGain(double gain)
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::setNoiseGateGain: " << gain << endl;
#endif
	this->gateClosedGain = gain;
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::setNoiseGateGain" << endl;
#endif



}

void Processing::setTriggerLowThreshold(double lowThres)
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::setTriggerLowThreshold: " << lowThres << endl;
#endif
	this->triggerLowThreshold = lowThres;
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::setTriggerLowThreshold" << endl;
#endif



}

void Processing::setTriggerHighThreshold(double highThres)
{

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::setTriggerHighThreshold: " << highThres << endl;
#endif
	this->triggerHighThreshold = highThres;
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::setTriggerHighThreshold" << endl;
#endif



}

int  getPitchFilterIndex = 30;
#define dbg 0
int Processing::getPitch(bool activate, double *signal)
{
	static int pitch;
	double lp_a[4], lp_b[4];
	static double lp_y[4], lp_x[4];
	double filterOutSample[2];
	filterOutSample[0] = 0.00000;
	filterOutSample[1] = 0.00000;
	double preFilterPeakToPeakAmplitude = 0.0000;
	double postFilterPeakToPeakAmplitude = 0.0000;
	double postPreAmplitudeRatio = 0.000;


	lp_b[0] = lp3[getPitchFilterIndex][0];
	lp_b[1] = lp3[getPitchFilterIndex][1];
	lp_b[2] = lp3[getPitchFilterIndex][2];
	lp_b[3] = lp3[getPitchFilterIndex][3];
	lp_a[1] = lp3[getPitchFilterIndex][5];
	lp_a[2] = lp3[getPitchFilterIndex][6];
	lp_a[3] = lp3[getPitchFilterIndex][7];

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::getPitch" << endl;
#endif

	//filter signal and get peak amplitudes. Get buffer positions using zero-crossing detection


	for(int i = 0; i < this->bufferSize; i++)
	{
		lp_x[0] = signal[i];

		if(lp_x[0] > tempPreFilterHigh.amplitude)
		{
			tempPreFilterHigh.amplitude = lp_x[0];
		}
		else if(lp_x[0] < tempPreFilterLow.amplitude)
		{
			tempPreFilterLow.amplitude = lp_x[0];
		}

		lp_y[0] = lp_b[0]*lp_x[0] + lp_b[1]*lp_x[1] + lp_b[2]*lp_x[2] + lp_b[3]*lp_x[3] - lp_a[1]*lp_y[1] - lp_a[2]*lp_y[2] - lp_a[3]*lp_y[3];

		lp_x[3] = lp_x[2];
		lp_x[2] = lp_x[1];
		lp_x[1] = lp_x[0];

		lp_y[3] = lp_y[2];
		lp_y[2] = lp_y[1];
		lp_y[1] = lp_y[0];


		if(lp_y[0] > tempPostFilterHigh.amplitude)
		{
			tempPostFilterHigh.amplitude = lp_y[0];
		}
		else if(lp_y[0] < tempPostFilterLow.amplitude)
		{
			tempPostFilterLow.amplitude = lp_y[0];
		}

		filterOutSample[1] = filterOutSample[0];
		filterOutSample[0] = lp_y[0];

		if(filterOutSample[1] < 0.0000  &&  filterOutSample[0] > 0.0000) // positive-slope zero-crossing
		{
			tempPostFilterHigh.amplitude = 0.0000; //reset to get new positive peak
			tempPreFilterHigh.amplitude = 0.0000; //reset to get new positive peak
			postFilterLow.amplitude = tempPostFilterLow.amplitude; // reset to get new negative peak
			preFilterLow.amplitude = tempPreFilterLow.amplitude; // reset to get new negative peak
			{
				zeroCrossingCountArray[1] = zeroCrossingCountArray[0];
				zeroCrossingCountArray[0] = zeroCrossingCount;
				tempPitch = zeroCrossingCountArray[0] - zeroCrossingCountArray[1];
			}
		}

		if(filterOutSample[1] > 0.0000  &&  filterOutSample[0] < 0.0000) // negative-slope zero-crossing
		{
			tempPostFilterLow.amplitude = 0.0000; // reset to get new negative peak
			tempPreFilterLow.amplitude = 0.0000; // reset to get new negative peak
			postFilterHigh.amplitude = tempPostFilterHigh.amplitude; //reset to get new positive peak
			preFilterHigh.amplitude = tempPreFilterHigh.amplitude; //reset to get new positive peak
		}

		if(zeroCrossingCount > 10000)
		{
			zeroCrossingCountArray[0] = 0;
			zeroCrossingCountArray[1] = 0;
			zeroCrossingCount = 0;
		}
		else zeroCrossingCount++;
	}

	preFilterPeakToPeakAmplitude = preFilterHigh.amplitude - preFilterLow.amplitude;

	postFilterPeakToPeakAmplitude = postFilterHigh.amplitude - postFilterLow.amplitude;

	postPreAmplitudeRatio = postFilterPeakToPeakAmplitude/preFilterPeakToPeakAmplitude;

	if(preFilterPeakToPeakAmplitude > 0.1) // signal is present
	{
		if( 0.100 < postPreAmplitudeRatio && postPreAmplitudeRatio < 0.500)
		{
			if(zeroCrossingCountArray[0] != 0 && zeroCrossingCountArray[1] != 0)
			{
					pitch = tempPitch;
			}
		}
		else
		{
			if(postPreAmplitudeRatio >= 0.100  && getPitchFilterIndex > 0) // filtered signal too big: lower cut-off frequency
			{
				getPitchFilterIndex--;
			}
			else if(getPitchFilterIndex < 99)// filtered signal too small: raise cut-off frequency
			{
				getPitchFilterIndex++;
			}

			if(debugOutput) cout << "getPitchFilterIndex: " << getPitchFilterIndex << "\tgetPitchFilterIndexAddendIndex: " << getPitchFilterIndexAddendIndex << endl;
		}
		if(debugOutput) cout << "preFilterAmp: " << preFilterPeakToPeakAmplitude << "\tAmpRatio: " << postPreAmplitudeRatio  << "\tzCrossings: " << zeroCrossingCountArray[0] << ":" << zeroCrossingCountArray[1] << "\tpitch: " << pitch << "\tgetPitchFilterIndex: " << getPitchFilterIndex << endl;

	}
	else
	{
		zeroCrossingCountArray[0] = 0;
		zeroCrossingCountArray[1] = 0;
		zeroCrossingCount = 0;
		pitch = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::getPitch: ";
#if(dbg >= 2)
	if(debugOutput) cout << "preFilterAmp: " << preFilterPeakToPeakAmplitude << "\tAmpRatio: " << postPreAmplitudeRatio  << "\tzCrossings: " << zeroCrossingCountArray[0] << ":" << zeroCrossingCountArray[1] << "\\tpitch: " << pitch << "\tgetPitchFilterIndex: " << getPitchFilterIndex << endl;
#else
	if(debugOutput) cout << endl;
	#endif
#endif

	return pitch;
}



#define dbg 1
void Processing::printIndexMappedProcessData()
{
#if(dbg >= 1)
	if(debugOutput)
	{
		cout << "*****ENTERING ComboDataInt::printIndexMappedProcessData" << endl;
		cout << "PROCESS INDEX MAPPING" << endl;
		cout << "size: " << this->processIndexMap.size() << endl;
		for (auto &  processIndexing : this->processIndexMap)
		{

			cout << "PROCESS:" << processIndexing.second.processName << endl;
			cout << "name: " << processIndexing.second.processName << endl;
			cout << "processSequenceIndex: " << processIndexing.second.processSequenceIndex << endl;
			cout << "parentEffect: " << processIndexing.second.parentEffect << endl;


			map<string,ProcessParameterIndexing>::iterator processParamIndexing = processIndexing.second.paramIndexMap.begin();
			cout << "paramIndexMap size: " << processIndexing.second.paramIndexMap.size() << endl;
			for(auto &  processParamIndexing : processIndexing.second.paramIndexMap)
			{
				cout << "\t\t parameter name: " << processParamIndexing.second.paramName << endl;
				cout << "\t\t parameter index: " << processParamIndexing.second.paramIndex << endl;
				cout << "\t\t parentProcess: " << processParamIndexing.second.parentProcess << endl;
				cout << "\t\t parameter paramControlBufferIndex: " << processParamIndexing.second.connectedBufferIndex << endl;
			}
			cout << "**********************************************" << endl;
		}
		cout << "***** EXITING ComboDataInt::printIndexMappedProcessData: "  << endl;
	}

#endif

}

void Processing::printIndexMappedControlData()
{
#if(dbg >= 1)
	if(debugOutput)
	{
		cout << "*****ENTERING Processing::printIndexMappedControlData" << endl;
		cout << "CONTROL INDEX MAPPING" << endl;

		for (auto &  controlIndexing : this->controlIndexMap)
		{
			cout << "CONTROL:" << controlIndexing.second.controlName << endl;
			cout << "name: " << controlIndexing.second.controlName << endl;;
			cout << "index: " << controlIndexing.second.controlIndex << endl;
			cout << "parentEffect: " << controlIndexing.second.parentEffect << endl;
			cout << "conType: " << controlIndexing.second.controlTypeInt << endl;


			cout << "paramIndexMap size: " << controlIndexing.second.paramIndexMap.size() << endl;

			for(auto &  contParamIndexing : controlIndexing.second.paramIndexMap)
			{
				cout << "\t\t contParam.name: " << contParamIndexing.second.contParamName << endl;
				cout << "\t\t contParam.index: " << contParamIndexing.second.contParamIndex << endl;
				cout << "\t\t contParam.parentControl: " << contParamIndexing.second.parentControl << endl;
			}
		}

		cout << "***** EXITING Processing::printIndexMappedControlData: "  << endl;
	}


#endif
}
