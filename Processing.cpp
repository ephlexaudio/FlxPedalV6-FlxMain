/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */
#include "config.h"
#include "Processing.h"
#include "PedalUtilityData.h"
#include "valueArrays.h"

extern bool debugOutput;
extern PedalUtilityData pedalUtilityData;


#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43


extern ComboStruct combo;
extern int currentComboStructIndex;
extern int oldComboStructIndex;

	extern map<string, ComboDataInt> comboDataMap;

extern bool inputsSwitched;

int comboTime;

Processing::Processing():JackCpp::AudioIO(string("process"), 2,2)
{
	// TODO Auto-generated constructor stub
	reserveInPorts(8);
	reserveOutPorts(8);
	this->gateOpenThreshold = pedalUtilityData.getNoiseGate_OpenThres();
	cout << "this->gateOpenThreshold :" <<  this->gateOpenThreshold << endl;
	this->gateCloseThreshold = pedalUtilityData.getNoiseGate_CloseThres();
	cout << "this->gateCloseThreshold :" <<  this->gateCloseThreshold << endl;
	this->triggerHighThreshold = pedalUtilityData.getTrigger_HighThres();
	cout << "this->triggerHighThreshold :" << this->triggerHighThreshold  << endl;
	this->triggerLowThreshold = pedalUtilityData.getTrigger_LowThres();
	cout << "this->triggerLowThreshold :" << this->triggerLowThreshold  << endl;
	this->gateClosedGain = pedalUtilityData.getNoiseGate_Gain();
	cout << "this->gateClosedGain :" <<  this->gateClosedGain << endl;
	this->bufferSize = pedalUtilityData.getBufferSize();
	this->gateOpen = true;
	this->inMaxAmpFilterIndex = 0;
	this->inSignalLevelLowPeak = 0.0;
	this->inSignalLevelHighPeak = 0.0;
	this->gateStatus = 0;
	this->envTriggerStatus = 0;
	this->inSignalDeltaPositiveCount = 0;
	this->inSignalDeltaNegativeCount = 0;
	this->chan1GndCount = 0;
	this->chan2GndCount = 0;
	this->inMaxAmpFilterOut = 0;
	this->inPrevMaxAmpFilterOut = 0.0;
	this->inPrevSignalLevel = 0.0;
	this->inSignalDeltaFilterIndex = 0;
	this->inSignalDeltaFilterOut = 0.0;
	this->inSignalLevel = 0.0;
	this->processingEnabled = false;
	this->outGain = 1.000;
	this->gatePosition = 0;
	this->sampleGain = 1.000;
	this->cutSignal = false;
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


#define dbg 0
bool Processing::areInputsSwitched(void) // see if JACK has connected input jacks backwards
{
	bool intInputsSwitched = false;
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
	inPosPeakArray[0][0] = inPosPeak[0];
	inNegPeakArray[0][0] = inNegPeak[0];
	inPosPeakArray[0][1] = inPosPeak[1];
	inNegPeakArray[0][1] = inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[1][0] = inPosPeak[0];
	inNegPeakArray[1][0] = inNegPeak[0];
	inPosPeakArray[1][1] = inPosPeak[1];
	inNegPeakArray[1][1] = inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[2][0] = inPosPeak[0];
	inNegPeakArray[2][0] = inNegPeak[0];
	inPosPeakArray[2][1] = inPosPeak[1];
	inNegPeakArray[2][1] = inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[3][0] = inPosPeak[0];
	inNegPeakArray[3][0] = inNegPeak[0];
	inPosPeakArray[3][1] = inPosPeak[1];
	inNegPeakArray[3][1] = inNegPeak[1];
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;

	this->portConSwitch[0].setval_gpio(1);
	this->portConSwitch[2].setval_gpio(0);

	chan1GndCount = 0;
	chan2GndCount = 0;
	for(int i = 0; i < 4; i++)
	{
		if((0.098 < inPosPeakArray[i][0] && inPosPeakArray[i][0] < 0.1)) chan1GndCount++;
		if((0.098 < inPosPeakArray[i][1] && inPosPeakArray[i][1] < 0.1)) chan2GndCount++;
	}

	if(debugOutput) cout << "chan1GndCount: " << chan1GndCount << "\tchan2GndCount: " << chan2GndCount << endl;
	if(chan1GndCount > chan2GndCount)
	{
		intInputsSwitched = true;
		if(debugOutput) cout << "*******************inputs switched"  << endl;
	}
	else
	{
		intInputsSwitched = false;
		if(debugOutput) cout << "*******************inputs not switched"  << endl;
	}
	return intInputsSwitched;
}

#define dbg 0


	int Processing::loadCombo(void)
	{
		int status = 0;
	#if(dbg >= 1)
		if(debugOutput) cout << "ENTERING: Processing::loadCombo" << endl;
		if(debugOutput) cout << "currentComboStructIndex: " << currentComboStructIndex << endl;
	#endif

		this->aveArrayIndex = 0;

		if(debugOutput) cout << "Control Count: " << combo.controlCount << endl;
		if(debugOutput) cout << "Process Count: " << combo.processCount << endl;
		if(debugOutput) cout << "Buffer Count: " << combo.bufferCount << endl;

		if(combo.controlVoltageEnabled == true) // set analog input switches to DC input
		{
			this->portConSwitch[0].setval_gpio(0);
			this->portConSwitch[1].setval_gpio(1);
		}
		else // set analog input switches to AC input
		{
			this->portConSwitch[0].setval_gpio(1);
			this->portConSwitch[1].setval_gpio(0);
		}


		for(int i = 0; i < combo.controlCount; i++)
		{
			switch(combo.controlSequence[i].conType)
			{
			case 0: // normal
				normal('l', this->envTriggerStatus, 0, &combo.controlSequence[i], combo.processSequence);
				break;
			case 1:	// envelope generator
				envGen('l', this->envTriggerStatus, 0, &combo.controlSequence[i], combo.processSequence);
				break;
			case 2: // low frequency oscillator
				lfo('l', this->envTriggerStatus, 0, &combo.controlSequence[i], combo.processSequence);
				break;
			}
		}

		for(int i = 0; i < combo.processCount; i++)
		{
			switch(combo.processSequence[i].procType)
			{
				case 0:
					delayb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 1:
					filter3bb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 2:
					filter3bb2('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 3:
					lohifilterb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 4:
					mixerb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 5:
					volumeb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 6:
					waveshaperb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 7:
					break;
				case 8:
					samplerb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 9:
					oscillatorb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				default:;
			}
		}

		for(unsigned int bufferIndex = 0; bufferIndex < combo.bufferCount; bufferIndex++)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "clearing procBufferArray[" << bufferIndex << "]:" << endl;
#endif
#if(PROC_BUFFER_REF == 1)
			clearProcBuffer(&combo.procBufferArray[bufferIndex]);
#elif(PROC_BUFFER_VAL == 1)
			clearProcBuffer(combo.procBufferArray[bufferIndex]);
#endif
		}
#if(dbg >= 2)
			if(debugOutput) cout << "clearing procBufferArray[58]:" << endl;
#endif
#if(PROC_BUFFER_REF == 1)
			clearProcBuffer(&combo.procBufferArray[58]);
#elif(PROC_BUFFER_VAL == 1)
			clearProcBuffer(combo.procBufferArray[58]);
#endif

	#if(dbg >= 1)
		if(debugOutput) cout << "EXITING: Processing::loadCombo: " << status << endl;
	#endif

		return status;
	}

#define dbg 0
void Processing::triggerInputSignalFiltering()
{
	if(this->inMaxAmpFilterIndex < 15)
	{
		this->inMaxAmpFilter[this->inMaxAmpFilterIndex++] = this->inMaxAmp[0];
	}
	else
	{
		this->inMaxAmpFilter[this->inMaxAmpFilterIndex] = this->inMaxAmp[0];
		this->inMaxAmpFilterIndex = 0;
	}

	this->inMaxAmpFilterOut = (this->inMaxAmpFilter[0] + this->inMaxAmpFilter[1] + this->inMaxAmpFilter[2] +
			this->inMaxAmpFilter[3] + this->inMaxAmpFilter[4] + this->inMaxAmpFilter[5] +
			this->inMaxAmpFilter[6] + this->inMaxAmpFilter[7] + this->inMaxAmpFilter[8] +
			this->inMaxAmpFilter[9] + this->inMaxAmpFilter[10] + this->inMaxAmpFilter[11] +
			this->inMaxAmpFilter[12] + this->inMaxAmpFilter[13] + this->inMaxAmpFilter[14] +
			this->inMaxAmpFilter[15])/16;
	this->inSignalLevel = this->inMaxAmpFilterOut;

#if(dbg >= 1)
	cout << "this->inSignalLevel: " <<  (this->inSignalLevel) << endl;
#endif

	if(this->inSignalDeltaFilterIndex < 15)
	{
		this->inSignalDeltaFilter[this->inSignalDeltaFilterIndex++] = this->inSignalLevel - this->inPrevSignalLevel;
	}
	else
	{
		this->inSignalDeltaFilter[this->inSignalDeltaFilterIndex] = this->inSignalLevel - this->inPrevSignalLevel;
		this->inSignalDeltaFilterIndex = 0;
	}
	this->inSignalDeltaFilterOut = (this->inSignalDeltaFilter[0] + this->inSignalDeltaFilter[1] + this->inSignalDeltaFilter[2] +
			this->inSignalDeltaFilter[3] + this->inSignalDeltaFilter[4] + this->inSignalDeltaFilter[5] +
			this->inSignalDeltaFilter[6] + this->inSignalDeltaFilter[7] + this->inSignalDeltaFilter[8] +
			this->inSignalDeltaFilter[9] + this->inSignalDeltaFilter[10] + this->inSignalDeltaFilter[11] +
			this->inSignalDeltaFilter[12] + this->inSignalDeltaFilter[13] + this->inSignalDeltaFilter[14] +
			this->inSignalDeltaFilter[15])/16;
}

#define dbg 0
void Processing::noiseGate(double *bufferIn, double *bufferOut)
{
	int status = 0;
	static int closeIndex;
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
			closeIndex = 0;
			this->gateGain = 1.0;
		}
		else
		{
			for(int i = 0; i < BUFFER_SIZE; i++)
			{
				bufferOut[i] = this->gateGain*bufferIn[i];
			}
		}

		break;
	case 1:// **************** NOISE GATE OPENING ********************
		{
			this->gateGain = 1.0;
			this->gateStatus = 2;
			for(int i = 0; i < BUFFER_SIZE; i++)
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
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:2\tlevel below noise gate low threshold: going to case 5." << this->inputGain  << endl;
#endif

			for(int i = 0; i < BUFFER_SIZE; i++)
			{
				if(this->gateGain < this->gateClosedGain )
				{
					this->gateStatus = 3;
					closeIndex = 0;
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
			for(int i = 0; i < BUFFER_SIZE; i++)
			{
				bufferOut[i] = this->gateGain*bufferIn[i];
			}
		}

		break;

	case 3: // **************** NOISE GATE CLOSING ********************
#if(dbg >= 1)
	if(debugOutput) cout << "PROCESSING: level below noise gate low threshold: " << this->inputGain << endl;
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
	int status = 0;
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
int controlVoltageSampleCount = 0;
int secondMarkCounter = 0;
	int Processing::audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs)
	{
		int status = 0;
		bool processDone = false;

		double inPosPeak[2];
		double inNegPeak[2];
		double outPosPeak[2];
		double outNegPeak[2];
		double tempOutBufs[2];
		float controlVoltageFloat = 0.00;
		float controlVoltageAdjustedFloat = 0.00;
		int controlVoltageIndex = 0;
		float gateGainIncrementSize = 0.0000;
		int gatePositionCount = 5;
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

			for(unsigned int i = 0; i < bufferSize; i++)
			{
				if(inputsSwitched)
				{
					this->noiseGateBuffer[0][i] = inBufs[1][i]*this->inputGain;
					if(combo.controlVoltageEnabled == true)
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[0][i];
					}
					else
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[0][i]*this->inputGain;
					}
				}
				else
				{
					this->noiseGateBuffer[0][i] = inBufs[0][i]*this->inputGain;
					if(combo.controlVoltageEnabled == true)
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[1][i];
					}
					else
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[1][i]*this->inputGain;
					}
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

			this->noiseGate(this->noiseGateBuffer[0], combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer);
			this->envelopeTrigger();

			this->inPrevMaxAmpFilterOut = this->inMaxAmpFilterOut;
			this->inPrevMaxAmp[1] = this->inMaxAmp[1];
			this->inPrevSignalLevel = this->inSignalLevel;


			//******************** Get control voltage from pedal ************************
			controlVoltageFloat =  combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[0]+1.000;
			controlVoltageAdjustedFloat =  49.50*controlVoltageFloat;
			controlVoltageIndex = (int)(controlVoltageAdjustedFloat);


			if(controlVoltageIndex < 0) controlVoltageIndex = 0;
			if(controlVoltageIndex > 99) controlVoltageIndex = 99;
			//***************************** Run Controls for manipulating process parameters *************

			for(int i = 0; i < combo.controlCount; i++)
			{
				switch(combo.controlSequence[i].conType)
				{
				case 0: // normal
					normal('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence);
					break;
				case 1:	// envelope generator
					envGen('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence);
					break;
				case 2: // low frequency oscillator
					lfo('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence);
					break;
				}
			}

			//****************************** Run processes *******************************************

			for(int i = 0; i < combo.processCount; i++)
			{
		#if(dbg >= 1)
				if(debugOutput) cout << "process number: " << i << endl;
		#endif
				switch(combo.processSequence[i].procType)
				{
					case 0:
						delayb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 1:
						filter3bb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 2:
						filter3bb2('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 3:
						lohifilterb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 4:
						mixerb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 5:
						volumeb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 6:
						waveshaperb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 7:
						break;
					case 8:
						samplerb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 9:
						oscillatorb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					default:;
				}
			}


			for(unsigned int i = 0; i < bufferSize; i++)
			{

				if(inputsSwitched)
				{
					tempOutBufs[1] = combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
					tempOutBufs[0] = combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
				}
				else
				{
					tempOutBufs[0] = combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
					tempOutBufs[1] =combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
				}

				outBufs[0][i] = tempOutBufs[0];
				outBufs[1][i] = tempOutBufs[1];
				if(outPosPeak[0] < outBufs[0][i]) outPosPeak[0] = outBufs[0][i];
				if(outNegPeak[0] > outBufs[0][i]) outNegPeak[0] = outBufs[0][i];
				if(outPosPeak[1] < outBufs[1][i]) outPosPeak[1] = outBufs[1][i];
				if(outNegPeak[1] > outBufs[1][i]) outNegPeak[1] = outBufs[1][i];
			}

			//***************** Output automatic level controller (pseudo-compressor) ????? *************
			{
				this->outPosPeak[0] = outPosPeak[0];
				this->outNegPeak[0] = outNegPeak[0];
				this->outPosPeak[1] = outPosPeak[1];
				this->outNegPeak[1] = outNegPeak[1];

				this->outMaxAmp[0][0] = this->outPosPeak[0] - this->outNegPeak[0];
				this->outMaxAmp[1][0] = this->outPosPeak[1] - this->outNegPeak[1];
				this->outMaxAmp[0][1] = this->outMaxAmp[0][0];
				this->outMaxAmp[1][1] = this->outMaxAmp[1][0];

				if(this->outMaxAmp[0][1] > this->outMaxAmp[0][0]) this->outMaxAmp[0][0] = this->outMaxAmp[0][1];

			}


		#if(SIGPROC_DBG)
			int bufferIndex;
			if(debugOutput) cout << "BUFFER AVERAGES: " << endl;
			for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
			{
				if(debugOutput) cout << this->procBufferArray[bufferIndex].average << ", ";
			}
			if(debugOutput) cout << endl;
		#endif
		}
		else
		{
			for(unsigned int i = 0; i < bufferSize; i++)
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
int loadComponentSymbols(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::loadComponentSymbols" << endl;
#endif

	delayb('c', NULL, NULL, NULL);
	filter3bb('c', NULL, NULL, NULL);
	filter3bb2('c', NULL, NULL, NULL);
	lohifilterb('c', NULL, NULL, NULL);
	mixerb('c', NULL, NULL, NULL);
	volumeb('c', NULL, NULL, NULL);
	waveshaperb('c', NULL, NULL, NULL);
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::loadComponentSymbols" << endl;
#endif

	return 0;
}

#define dbg 0
int loadControlTypeSymbols(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ProcessingControl::loadControlTypeSymbols" << endl;
#endif

	struct ControlEvent loadControlType;
	loadControlType.conType = 0; // load Normal control type symbol data
	normal('c', NULL, NULL, &loadControlType, NULL);
	loadControlType.conType = 1; // load Envelope Generator control type symbol data;
	envGen('c', NULL, NULL, &loadControlType, NULL);
	loadControlType.conType = 2; // load LFO control type symbol data;
	lfo('c', NULL, NULL, &loadControlType, NULL);
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::loadControlTypeSymbols" << endl;
#endif

	return 0;
}




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


#define dbg 1

	int Processing::updateProcessParameter(string processName, int parameterIndex, int parameterValue)
	{
		int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::updateProcessParameter" << endl;
	if(debugOutput) cout << "processName: " << processName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
#endif
		int procSequenceIndex = 0;
		// processIndex for processSequence doesn't correspond to process indexing in
		// parameterArray, so needs to be calculated.
		// indexes can't be redone in getCombo or getParameterArray because
		// they are used in updating combo files.

		for(int processIndex = 0; processIndex < combo.processCount; processIndex++)
		{
			if(combo.processSequence[processIndex].processName.compare(processName) == 0)
			{
				combo.processSequence[processIndex].parameters[parameterIndex] = parameterValue;
				procSequenceIndex = processIndex;
				break;
			}
		}
	#if(dbg>=2)
		if(debugOutput) cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << endl;
	#endif

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::updateProcessParameter: " << status << endl;
#endif
		return status;
	}


#define dbg 1
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg >= 1)
		if(debugOutput) cout << "ENTERING: Processing::updateControlParameter" << endl;
		if(debugOutput) cout << "controlName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
	#endif
		int status = 0;
		int controlSequenceIndex = 0;
		int controlIndex;
		// processIndex for processSequence doesn't correspond to process indexing in
		// parameterArray, so needs to be calculated.
		// indexes can't be redone in getCombo or getParameterArray because
		// they are used in updating combo files.

		for(controlIndex = 0; controlIndex < 20; controlIndex++)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "comparing: " << combo.controlSequence[controlIndex].name << " and " << controlName << endl;
#endif
			if(combo.controlSequence[controlIndex].name.compare(controlName) == 0)
			{
				combo.controlSequence[controlIndex].parameter[parameterIndex].value = parameterValue;
#if(dbg >= 1)
				if(debugOutput) cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
#endif
				controlSequenceIndex = controlIndex;
				break;
			}
		}

	#if(dbg >= 2)
		if(debugOutput) cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << combo.controlSequence[controlIndex].parameter[parameterIndex].value << endl;
		if(debugOutput) cout << "\t\ttargetProcessIndex: " << combo.controlSequence[controlIndex].paramContConnection[0].processIndex;
		if(debugOutput) cout << "\t\ttargetProcessName: " << combo.processSequence[combo.controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		if(debugOutput) cout << "\t\ttargetProcessParameterIndex: " << combo.controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg >= 1)
		if(debugOutput) cout << "EXITING: Processing::updateControlParameter: " << status << endl;
	#endif
		return status;
	}


#define dbg 0
int Processing::enableProcessing()
{

#if(dbg==1)
	if(debugOutput) cout << "ENTERING: Processing::enableProcessing" << endl;
#endif

	this->processingEnabled = true;
#if(dbg>=1)
	if(debugOutput) cout << "EXITING: Processing::enableProcessing" << endl;
#endif

	return 0;
}

#define dbg 0
int Processing::disableProcessing()
{
#if(dbg==1)
	if(debugOutput) cout << "ENTERING: Processing::disableProcessing" << endl;
#endif

	this->processingEnabled = false;
#if(dbg>=1)
	if(debugOutput) cout << "EXITING: Processing::disableProcessing" << endl;
#endif

	return 0;
}

int Processing::enableAudioInput()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::enableAudioInput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x00 0x57");
	system("i2cset -f -y 1 0x1a 0x02 0x57");
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::enableAudioInput" << endl;
#endif
	return status;
}


int Processing::disableAudioInput()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioInput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x00 0x50");
	system("i2cset -f -y 1 0x1a 0x02 0x50");

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioInput" << endl;
#endif
	return status;
}


#define dbg 0
int Processing::enableAudioOutput()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x07");
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif
	return status;
}


int Processing::disableAudioOutput()
{

	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x0F");
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::disableAudioOutput" << endl;
#endif

	return status;
}




int Processing::setNoiseGateCloseThreshold(float closeThres)
{
	int status = 0;

	this->gateCloseThreshold = closeThres;

	return status;
}

int Processing::setNoiseGateOpenThreshold(float openThres)
{
	int status = 0;

	this->gateOpenThreshold = openThres;

	return status;
}

int Processing::setNoiseGateGain(float gain)
{
	int status = 0;

	this->gateClosedGain = gain;

	return status;
}

int Processing::setTriggerLowThreshold(float lowThres)
{
	int status = 0;

	this->triggerLowThreshold = lowThres;

	return status;
}

int Processing::setTriggerHighThreshold(float highThres)
{
	int status = 0;

	this->triggerHighThreshold = highThres;

	return status;
}

int getPitchFilterIndex = 30;
#define dbg 0
int Processing::getPitch(bool activate, double *signal)
{
	static int pitch;
	int getPitchFilterIndexAddends[6] = {1,3,6,11,22,45};
	double lp_a[4], lp_b[4];
	static double lp_y[4], lp_x[4];
	double filterOutSample[2];
	int zeroCrossingPosition[2];
	filterOutSample[0] = 0.00000;
	filterOutSample[1] = 0.00000;
	zeroCrossingPosition[0] = 0;
	zeroCrossingPosition[1] = 0;
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


	for(unsigned int i = 0; i < bufferSize; i++)
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
