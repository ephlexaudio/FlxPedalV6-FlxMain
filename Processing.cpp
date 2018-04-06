/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */
#include "config.h"
#include "Processing.h"
#include "PedalUtilityData.h"
#include "valueArraysTest.h"

//#include "Filter3bb.h"
extern bool debugOutput;
extern PedalUtilityData pedalUtilityData;
//using std::if(debugOutput) cout;
//using std::endl;

#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 0
#define COMBO_STRUCT 1
/**************************************
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Processing::" << endl;
	if(debugOutput) cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Processing::: " << status << endl;
#endif

#if(dbg >=2)
#endif
********************************************/

#define TEST_PROCESS_COUNT 6
#define SIGNAL_DELTA_THRESHOLD 0.03
#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define AVE_ARRAY_SIZE 16
#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43

//extern struct _processingParams processingParams;
//struct ProcessEvent this->processSequence[10];

extern ComboStruct combo;
extern int currentComboStructIndex;
extern int oldComboStructIndex;

#if(COMBO_DATA_VECTOR == 1)
	extern vector<ComboDataInt> comboDataVector;
#elif(COMBO_DATA_ARRAY == 1)
	extern ComboDataInt comboDataArray[15];
#elif(COMBO_DATA_MAP == 1)
	extern map<string, ComboDataInt> comboDataMap;
#endif

extern bool inputsSwitched;
extern unsigned int bufferSize;
//extern int globalComboIndex;
int comboTime;

Processing::Processing():JackCpp::AudioIO(string("processing"), 2,2)
{
	// TODO Auto-generated constructor stub
	reserveInPorts(8);
	reserveOutPorts(8);
	this->gateOpenThreshold = pedalUtilityData.getNoiseGate_OpenThres();//processingParams.noiseGate.highThres;
	this->gateCloseThreshold = pedalUtilityData.getNoiseGate_CloseThres();//processingParams.noiseGate.lowThres;
	this->triggerHighThreshold = pedalUtilityData.getTrigger_HighThres();//processingParams.trigger.highThres;
	this->triggerLowThreshold = pedalUtilityData.getTrigger_LowThres();//processingParams.trigger.lowThres;
	this->gateClosedGain = pedalUtilityData.getNoiseGate_Gain();

	//this->checkInputs = false;
	this->gateOpen = true;
	//this->inputsSwitched = false;
	this->envTrigger = false;
	this->envTriggerPeriods = 0;
	this->inMaxAmpFilterIndex = 0;
	this->inSignalLevelLowPeak = 0.0;
	this->inSignalLevelHighPeak = 0.0;
	this->gateEnvStatus = 0;
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
	this->processingEnabled = true;
	this->processingUpdated = false;
	this->processingContextAllocationError = false;
	this->updateProcessing = false;
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
	//this->portConSwitch[0].setdir_gpio("out");
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

/*int Combo::setCheckInputs()
{
	int status = 0;

	this->checkInputs = true;

	return status;
}


int Combo::clearCheckInputs()
{
	int status = 0;

	this->checkInputs = false;

	return status;
}*/

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

	//system("echo \"0\" > /sys/class/gpio/gpio41/value");
	this->portConSwitch[0].setval_gpio(0);
	//system("echo \"1\" > /sys/class/gpio/gpio43/value");
	this->portConSwitch[2].setval_gpio(1);

	usleep(100000);

	inPosPeakArray[0][0] = inPosPeak[0];
	inNegPeakArray[0][0] = inNegPeak[0];
	inPosPeakArray[0][1] = inPosPeak[1];
	inNegPeakArray[0][1] = inNegPeak[1];
	//fprintf(stderr,"inPosPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", inPosPeak[0], inNegPeak[0], inPosPeak[1], inNegPeak[1]);
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[1][0] = inPosPeak[0];
	inNegPeakArray[1][0] = inNegPeak[0];
	inPosPeakArray[1][1] = inPosPeak[1];
	inNegPeakArray[1][1] = inNegPeak[1];
	//fprintf(stderr,"inPosPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", inPosPeak[0], inNegPeak[0], inPosPeak[1], inNegPeak[1]);
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[2][0] = inPosPeak[0];
	inNegPeakArray[2][0] = inNegPeak[0];
	inPosPeakArray[2][1] = inPosPeak[1];
	inNegPeakArray[2][1] = inNegPeak[1];
	//fprintf(stderr,"inPosPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", inPosPeak[0], inNegPeak[0], inPosPeak[1], inNegPeak[1]);
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;
	usleep(50000);

	inPosPeakArray[3][0] = inPosPeak[0];
	inNegPeakArray[3][0] = inNegPeak[0];
	inPosPeakArray[3][1] = inPosPeak[1];
	inNegPeakArray[3][1] = inNegPeak[1];
	//fprintf(stderr,"inPosPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", inPosPeak[0], inNegPeak[0], inPosPeak[1], inNegPeak[1]);
	if(debugOutput) cout << "inPosPeak[0]: " << inPosPeak[0] << "\tinNegPeak[0]: " << inNegPeak[0] << "\tinPosPeak[1]: " << inPosPeak[1] << "\tinNegPeak[1]: " << inNegPeak[1] << endl;

	//system("echo \"1\" > /sys/class/gpio/gpio41/value");
	this->portConSwitch[0].setval_gpio(1);
	//system("echo \"0\" > /sys/class/gpio/gpio43/value");
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
		intInputsSwitched = true;//switchedStatus = 1;
		if(debugOutput) cout << "*******************inputs switched"  << endl;
		//fprintf(stderr,"*******************inputs switched\n");
	}
	else
	{
		intInputsSwitched = false;//switchedStatus = 0;
		if(debugOutput) cout << "*******************inputs not switched"  << endl;
		//fprintf(stderr,"*******************inputs not switched\n");
	}
	return intInputsSwitched;
}

#define dbg 2

#if(COMBO_STRUCT == 1)

	int Processing::loadCombo(void)
	{
		int status = 0;
		/*int inputBufferIndex = 0;
		int outputBufferIndex = 1;
		int connectBufferIndex = 0;*/
	#if(dbg >= 1)
		if(debugOutput) cout << "ENTERING: Processing::loadCombo" << endl;
		if(debugOutput) cout << "currentComboStructIndex: " << currentComboStructIndex << endl;
	#endif

		this->aveArrayIndex = 0;
		//initProcBuffers(combo.procBufferArray); // reset all data ready flags to 0.

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
			//control('l', false, &combo.controlSequence[i], /*&this->processSequence[j]*/NULL);

			switch(combo.controlSequence[i].conType)
			{
			case 0: // normal
				if(normal('l', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
					this->processingContextAllocationError = true;
				break;
			case 1:	// envelope generator
				if(envGen('l', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
					this->processingContextAllocationError = true;
				break;
			case 2: // low frequency oscillator
				if(lfo('l', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
					this->processingContextAllocationError = true;
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
					reverbb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 8:
					samplerb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				case 9:
					oscillatorb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;
				/*case ?:
					blankb('l', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
					break;*/
				default:;
			}
		}

		for(unsigned int bufferIndex = 0; bufferIndex < combo.bufferCount; bufferIndex++)
		{
			//initBufferAveParameters(&combo.procBufferArray[bufferIndex]);
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
#endif

#define dbg 0
int Processing::triggerInputSignalFiltering()
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
int Processing::noiseGateEnvTrigger()
{
	int status = 0;

	switch(this->gateEnvStatus)
	{
	case 0:	// *************** NOISE GATE CLOSED *****************
#if(dbg >= 1)
//if(debugOutput) cout << "case 0: noise gate closed" << endl;
#endif
		this->gateOpen = false;

		this->inputLevel = 1.0;//processingParams.noiseGate.gain;
		this->envTriggerPeriods = 0;
		this->gatePosition = 0;
		this->envTrigger = false;
		if((inputsSwitched == false && this->gateOpenThreshold < this->inMaxAmp[0]) || // noise gate
				(inputsSwitched == true && this->gateOpenThreshold < this->inMaxAmp[1]))
		{
#if(dbg >= 1)
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:0\tlevel above noise gate high threshold: going to case 1." << endl;
#endif
#if(dbg >= 2)
			if(debugOutput) cout << "gateEnvStatus:0\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
#endif
			this->gateEnvStatus = 1;
			this->inputLevel = 1.0;
		}

		break;
	case 1:// **************** NOISE GATE OPENING ********************
		//if(this->inputLevel >= 0.900)
		{
			this->gateEnvStatus = 2;
			this->enableAudioOutput();
		}

		break;

	case 2: // **************** NOISE GATE OPEN ********************
#if(dbg >= 1)
		//if(debugOutput) cout << "case 1: noise gate open" << endl;
#endif
		this->gateOpen = true;

		this->inputLevel = 1.0;
		this->envTrigger = false;

		if((inputsSwitched == false && this->gateCloseThreshold > this->inMaxAmp[0]) ||  // going below noise gate threshold
				(inputsSwitched == true && this->gateCloseThreshold > this->inMaxAmp[1]))
		{
#if(dbg >= 1)
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:2\tlevel below noise gate low threshold: going to case 5." << this->inputLevel  << endl;
#endif
			this->gateEnvStatus = 5;
			/*if((this->inputLevel - 0.2) < processingParams.noiseGate.gain)
			{
				this->gateEnvStatus = 0;
			}
			else
			{
				this->inputLevel -= 0.2;
			}*/
		}
		else if(this->inSignalDeltaFilterOut > this->triggerHighThreshold)
		{

			this->envTriggerPeriods = 0;
			this->gateEnvStatus = 3;
#if(dbg >= 2)
			if(debugOutput) cout << "gateEnvStatus:2\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
#endif
		}

		break;
	case 3:	// **************** TRIGGER ON ********************
#if(dbg >= 1)
		//if(debugOutput) cout << "PROCESSING: case 2: trigger on" << endl;
#endif
		this->gateOpen = true;//this->gateStatus = false;

		//this->inputLevel = 1.0;
		this->envTrigger = true;
		this->gateEnvStatus = 4;

		break;
	case 4: // **************** TRIGGER OFF ********************
		this->gateOpen = true;//this->gateStatus = false;
		//this->inputLevel = 1.0;
		this->envTrigger = false;
#if(dbg >= 1)
//		if(debugOutput) cout << "case 3: trigger off" << endl;
#endif

		if((inputsSwitched == false && this->gateCloseThreshold > this->inMaxAmp[0]) ||  // going below noise gate threshold
				(inputsSwitched == true && this->gateCloseThreshold > this->inMaxAmp[1]))
		{
#if(dbg >= 1)
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:4\tlevel below noise gate low threshold: going to case 5. " << this->inputLevel << endl;
#endif
#if(dbg >= 2)
			if(debugOutput) cout << "gateEnvStatus:4\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
#endif
			this->gateEnvStatus = 5;
		}
		else if(this->inSignalDeltaFilterOut < this->triggerLowThreshold)
		{

#if(dbg >= 1)
			if(debugOutput) cout << "PROCESSING: gateEnvStatus:4\tpick released: going to case 2." << endl;
#endif
			this->envTriggerPeriods = 0;
			//this->inSignalLevelHighPeak = this->inSignalLevel;
			this->gateEnvStatus = 2;
#if(dbg >= 2)
			if(debugOutput) cout << "gateEnvStatus:4\tinSignalLevel: " << this->inSignalLevel << "\tsignalLevelLowPeak: " << this->inSignalLevelLowPeak << "\tsignalLevelHighPeak: " << this->inSignalLevelHighPeak << endl;
#endif
		}

		break;
	case 5: // **************** NOISE GATE CLOSING ********************
#if(dbg >= 1)
	//if(debugOutput) cout << "PROCESSING: level below noise gate low threshold: " << this->inputLevel << endl;
#endif
		/*if((this->inputLevel - 0.2) < processingParams.noiseGate.gain)
		{
			this->gateEnvStatus = 0;

		}
		else
		{
			this->inputLevel -= 0.2;
		}*/
		/*if(this->inputLevel <= processingParams.noiseGate.gain+0.01)
		{
			this->gateEnvStatus = 0;
		}*/
		this->gateOpen = false;
		this->disableAudioOutput();
		this->gateEnvStatus = 0;
		break;
	default:
		this->gateEnvStatus = 0;

	}
	this->cutSignal = !this->gateOpen;

	return status;
}



//double testBuffer[10][BUFFER_SIZE];
#define dbg 0
int controlVoltageSampleCount = 0;
int secondMarkCounter = 0;
#if(COMBO_STRUCT == 1)
	int Processing::audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs)
	{
		/*if(secondMarkCounter >= 188)
		{
			cout << "*********************** 1 Second ********************************" << endl;
			secondMarkCounter = 0;
		}
		else
		{
			secondMarkCounter++;
		}*/
		int status = 0;
		//if(debugOutput) cout << "ENTERING audioCallback:  " << endl;
		//bool envTrigger = false;

		//this->audioCallbackRunning = true;
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
	#if(dbg >= 1)
		//if(debugOutput) cout << "ENTERING audioCallback:  " << endl;
	#endif

	//#if(TIMING_DBG == 1)
		startTimer();
	//#endif
		int process = 0;

		if(this->processingEnabled == true /*&& this->processingUpdated == true*/)
		{

			for(unsigned int i = 0; i < bufferSize; i++)
			{
				if(inputsSwitched)
				{
					combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer[i] = inBufs[1][i]*this->inputLevel;
					if(combo.controlVoltageEnabled == true)
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[0][i];
					}
					else
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[0][i]*this->inputLevel;
					}
				}
				else
				{
					combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer[i] = inBufs[0][i]*this->inputLevel;
					if(combo.controlVoltageEnabled == true)
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[1][i];
					}
					else
					{
						combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[1][i]*this->inputLevel;
					}
				}


				if(inPosPeak[0] < inBufs[0][i]) inPosPeak[0] = inBufs[0][i];
				if(inNegPeak[0] > inBufs[0][i]) inNegPeak[0] = inBufs[0][i];
				if(inPosPeak[1] < inBufs[1][i]) inPosPeak[1] = inBufs[1][i];
				if(inNegPeak[1] > inBufs[1][i]) inNegPeak[1] = inBufs[1][i];

			}
			//if(debugOutput) cout << "testBuffer[0]: " << testBuffer[0][0] << "\ttestBuffer[1]: " << testBuffer[1][0] << endl;
			this->inPosPeak[0] = inPosPeak[0];
			this->inNegPeak[0] = inNegPeak[0];
			this->inPosPeak[1] = inPosPeak[1];
			this->inNegPeak[1] = inNegPeak[1];

			this->inMaxAmp[0] = this->inPosPeak[0] - this->inNegPeak[0];
			this->inMaxAmp[1] = this->inPosPeak[1] - this->inNegPeak[1];

			this->triggerInputSignalFiltering();


			//this->inSignalLevel = this->inMaxAmpFilterOut;



		#if(dbg >= 2)
			if(debugOutput) cout << "signal: " << this->inSignalLevel << ",signal delta: " << this->inSignalDeltaFilterOut << endl;
		#endif


			this->noiseGateEnvTrigger();


			this->inPrevMaxAmpFilterOut = this->inMaxAmpFilterOut;
			this->inPrevMaxAmp[1] = this->inMaxAmp[1];
			this->inPrevSignalLevel = this->inSignalLevel;

			this->processingContextAllocationError = false;

			//this->getPitch(this->gateOpen, combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer);

			//******************** Get control voltage from pedal ************************
			controlVoltageFloat =  combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[0]+1.000;
			controlVoltageAdjustedFloat =  49.50*controlVoltageFloat;
			controlVoltageIndex = (int)(controlVoltageAdjustedFloat);


			if(controlVoltageIndex < 0) controlVoltageIndex = 0;
			if(controlVoltageIndex > 99) controlVoltageIndex = 99;
			//***************************** Run Controls for manipulating process parameters *************

			for(int i = 0; i < combo.controlCount; i++)
			{
				/*if(control('r', this->envTrigger, &combo.controlSequence[i], combo.processSequence) < 0)
					this->processingContextAllocationError = true;*/
				 //cout << "controller: " << i << " running." << endl;
				switch(combo.controlSequence[i].conType)
				{
				case 0: // normal
					//cout << "running normal" << endl;
					if(normal('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				case 1:	// envelope generator
					//cout << "running envelope generator" << endl;
					if(envGen('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				case 2: // low frequency oscillator
					//cout << "running LFO" << endl;
					if(lfo('r', this->envTrigger, controlVoltageIndex, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				}
			}

			//****************************** Run processes *******************************************

			for(int i = 0; i < combo.processCount; i++)
			{
		#if(dbg >= 1)
				//if(debugOutput) cout << "process number: " << i << endl;
		#endif
				switch(combo.processSequence[i].procType)
				{
					case 0:
						if(delayb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 1:
						if(filter3bb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 2:
						if(filter3bb2('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 3:
						if(lohifilterb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 4:
						if(mixerb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 5:
						if(volumeb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 6:
						if(waveshaperb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 7:
						if(reverbb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 8:
						if(samplerb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					case 9:
						if(oscillatorb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;
					/*case ?:
						if(blankb('r', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus) < 0)
							this->processingContextAllocationError = true;
						break;*/
					default:;
				}
				if(this->processingContextAllocationError == true)
				{
					this->processingEnabled = false;
					if(debugOutput) cout << "process or control context allocation failed: disabling processing." <<  endl;
					break;
				}
			}


			//******************* Sent processed signals out ******************


			//*********************** This was probably used as a software noise-gate**********
			for(unsigned int i = 0; i < bufferSize; i++)
			{

				if(this->cutSignal == true && this->sampleGain > 0.004)
				{
					this->sampleGain = (float)((bufferSize - i )/(1.00 * bufferSize));
				}
				else if(this->cutSignal == false)
				{
					this->sampleGain = 1.0000;
				}


				if(inputsSwitched)
				{
					tempOutBufs[1] = this->sampleGain*combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
					tempOutBufs[0] = this->sampleGain*combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
				}
				else
				{
					tempOutBufs[0] = this->sampleGain*combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
					tempOutBufs[1] = this->sampleGain*combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
				}

				outBufs[0][i] = tempOutBufs[0]*this->outGain;
				outBufs[1][i] = tempOutBufs[1]*this->outGain;
				if(outPosPeak[0] < outBufs[0][i]) outPosPeak[0] = outBufs[0][i];
				if(outNegPeak[0] > outBufs[0][i]) outNegPeak[0] = outBufs[0][i];
				if(outPosPeak[1] < outBufs[1][i]) outPosPeak[1] = outBufs[1][i];
				if(outNegPeak[1] > outBufs[1][i]) outNegPeak[1] = outBufs[1][i];
			}

			//***************** Output automatic level controller (pseudo-compressor) *************
			{

				this->outPosPeak[0] = outPosPeak[0];
				this->outNegPeak[0] = outNegPeak[0];
				this->outPosPeak[1] = outPosPeak[1];
				this->outNegPeak[1] = outNegPeak[1];

				this->outMaxAmp[0][0] = this->outPosPeak[0] - this->outNegPeak[0];
				this->outMaxAmp[1][0] = this->outPosPeak[1] - this->outNegPeak[1];
				this->outMaxAmp[0][1] = this->outMaxAmp[0][0];
				this->outMaxAmp[1][1] = this->outMaxAmp[1][0];
#if(dbg >= 1)
				//if(debugOutput) cout << "out gain: " << this->outGain << "\toutput levels: " << this->outMaxAmp[0][0] << '\t' << this->outMaxAmp[1][0] << endl;
#endif

				if(this->outMaxAmp[0][1] > this->outMaxAmp[0][0]) this->outMaxAmp[0][0] = this->outMaxAmp[0][1];

				if(this->outMaxAmp[0][0] > 1.500)
				{
					if(this->outGain > 0.1)
					{
						//this->outGain = 1.000/this->outMaxAmp[0][0];
						this->outGain -= 0.02;
					}
				}
				else if(this->outGain < 1.000)
				{
					this->outGain += 0.02;
				}
				else
				{

					this->outGain = 1.000;
				}
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
			//if(debugOutput) cout << "straight thru signal." << endl;
			//if(debugOutput) cout << "PROCESSING BYPASSED." << endl;
			for(unsigned int i = 0; i < bufferSize; i++)
			{
				outBufs[0][i] = inBufs[0][i];
				outBufs[1][i] = inBufs[1][i];

				if(inPosPeak[0] < inBufs[0][i]) inPosPeak[0] = inBufs[0][i];
				if(inNegPeak[0] > inBufs[0][i]) inNegPeak[0] = inBufs[0][i];
				if(inPosPeak[1] < inBufs[1][i]) inPosPeak[1] = inBufs[1][i];
				if(inNegPeak[1] > inBufs[1][i]) inNegPeak[1] = inBufs[1][i];
			}

			//if(debugOutput) cout << "testBuffer[0]: " << testBuffer[0][0] << "\ttestBuffer[1]: " << testBuffer[1][0] << endl;
			this->inPosPeak[0] = inPosPeak[0];
			this->inNegPeak[0] = inNegPeak[0];
			this->inPosPeak[1] = inPosPeak[1];
			this->inNegPeak[1] = inNegPeak[1];


		}
		//this->audioCallbackRunning = false;
	#if(TIMING_DBG == 1)
		comboTime = stopTimer("audio processing");
	#else
		comboTime = stopTimer(NULL);
	#endif

	#if(dbg >= 1)
		//if(debugOutput) cout << "EXITING audioCallback: " << status << endl;
	#endif

		return status;
	}
#endif




//double pitchDetectionBuffer[PITCH_DETECTION_BUFFER_SIZE]
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
/*preFilterHigh.amplitude = 0.0000;
preFilterLow.amplitude = 0.0000;
postFilterHigh.amplitude = 0.0000;
postFilterLow.amplitude = 0.0000;*/


/*int Processing::clearProcessData(int index, double *data)
{
	int status = 0;
	static int count;

	for(int i = 0; i < 256; i++)
	{
		data[i] = 0.0000;
	}


	return status;
}*/

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
	//reverbb('c', NULL, NULL, NULL);
	waveshaperb('c', NULL, NULL, NULL);
	//samplerb('c', NULL, NULL, NULL);
	//oscillatorb('c', NULL, NULL, NULL);
	//blankb('c', NULL, NULL, NULL);
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
	//loadControlType.conType = 3; // load test control type symbol data;
	//control('c', NULL, &loadControlType, NULL);
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING ProcessingControl::loadControlTypeSymbols" << endl;
#endif

	return 0;
}


#if(COMBO_STRUCT == 1)

	int Processing::stopCombo(void)
	{
		int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::stopCombo" << endl;
	if(debugOutput) cout << "oldComboStructIndex: " << oldComboStructIndex << endl;
#endif

		int index = 0;

		//if(stopComboIndex < 15)
		{

			for(int i = 0; i < combo.controlCount; i++)
			{
				//control('s', false, &combo.controlSequence[i], NULL);
				switch(combo.controlSequence[i].conType)
				{
				case 0: // normal
					if(normal('s', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				case 1:	// envelope generator
					if(envGen('s', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				case 2: // low frequency oscillator
					if(lfo('s', this->envTrigger, NULL, &combo.controlSequence[i], combo.processSequence) < 0)
						this->processingContextAllocationError = true;
					break;
				}
			}

			for(int i = 0; i < combo.processCount; i++)
			{
				switch(combo.processSequence[i].procType)
				{
					case 0:
						delayb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 1:
						filter3bb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 2:
						filter3bb2('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 3:
						lohifilterb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 4:
						mixerb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 5:
						volumeb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 6:
						waveshaperb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 7:
						reverbb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 8:
						samplerb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					case 9:
						oscillatorb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;
					/*case ?:
						blankb('s', &combo.processSequence[i], combo.procBufferArray,this->footswitchStatus);
						break;*/
					default:;
				}
			}

		}
		for(unsigned int bufferIndex = 0; bufferIndex < combo.bufferCount; bufferIndex++)
		{
			//initBufferAveParameters(combo.procBufferArray[bufferIndex]);
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

		//oldComboStructIndex = currentComboStructIndex;
		//currentComboStructIndex ^= 1;
		/*else
			status = -1;*/
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::stopCombo: " << status << endl;
#endif

		return status;
	}
#endif


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
int Processing::bypassAll()
{
	int status = 0;
#if(dbg >= 2)
	if(debugOutput) cout << "ENTERING: Combo::bypassAll" << endl;
#endif
	for(int i = 0; i < 10; i++)
	{
		this->footswitchStatus[i] = 0;
	}

#if(dbg >= 2)
	if(debugOutput) cout << "EXITING: Combo::bypassAll: " << status << endl;
#endif
	return status;
}

#define dbg 0
#if(COMBO_DATA_VECTOR == 1)
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

		for(int processIndex = 0; processIndex < comboDataVector[this->comboIndex].processCount; processIndex++)
		{
			if(comboDataVector[this->comboIndex].processSequence[processIndex].processName.compare(processName) == 0)
			{
				comboDataVector[this->comboIndex].processSequence[processIndex].parameters[parameterIndex] = parameterValue;
				procSequenceIndex = processIndex;
			}
		}
	#if(dbg==1)
		if(debugOutput) cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << endl;
	#endif
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::updateProcessParameter" << endl;
#endif
		return status;
	}

#elif(COMBO_DATA_ARRAY == 1)
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

		for(int processIndex = 0; processIndex < comboDataArray[this->comboIndex].processCount; processIndex++)
		{
			if(comboDataArray[this->comboIndex].processSequence[processIndex].processName.compare(processName) == 0)
			{
				comboDataArray[this->comboIndex].processSequence[processIndex].parameters[parameterIndex] = parameterValue;
				procSequenceIndex = processIndex;
			}
		}
	#if(dbg==1)
		if(debugOutput) cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << endl;
	#endif
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::updateProcessParameter" << endl;
#endif
		return status;
	}
#elif(COMBO_DATA_MAP == 1)

#endif
#if(COMBO_STRUCT == 1)

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
#endif


#define dbg 0
#if(COMBO_DATA_VECTOR == 1)
	#define dbg 0
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg>=1)
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
			if(comboDataVector[this->comboIndex].controlSequence[controlIndex].name.compare(controlName) == 0)
			{
				comboDataVector[this->comboIndex].controlSequence[controlIndex].parameter[parameterIndex].value = parameterValue;
				if(debugOutput) cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
				controlSequenceIndex = controlIndex;
				break;
			}
		}
	#if(dbg>=2)
		if(debugOutput) cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << this->controlSequence[controlIndex].parameter[parameterIndex].value << endl;
		if(debugOutput) cout << "\t\ttargetProcessIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processIndex;
		if(debugOutput) cout << "\t\ttargetProcessName: " << this->processSequence[this->controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		if(debugOutput) cout << "\t\ttargetProcessParameterIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg>=1)
		if(debugOutput) cout << "EXITING: Processing::updateControlParameter" << endl;
	#endif
		return status;
	}
#elif(COMBO_DATA_ARRAY == 1)
	#define dbg 0
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg>=1)
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
			if(comboDataArray[this->comboIndex].controlSequence[controlIndex].name.compare(controlName) == 0)
			{
				comboDataArray[this->comboIndex].controlSequence[controlIndex].parameter[parameterIndex].value = parameterValue;
				if(debugOutput) cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
				controlSequenceIndex = controlIndex;
				break;
			}
		}
	#if(dbg>=2)
		if(debugOutput) cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << this->controlSequence[controlIndex].parameter[parameterIndex].value << endl;
		if(debugOutput) cout << "\t\ttargetProcessIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processIndex;
		if(debugOutput) cout << "\t\ttargetProcessName: " << this->processSequence[this->controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		if(debugOutput) cout << "\t\ttargetProcessParameterIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg>=1)
		if(debugOutput) cout << "EXITING: Processing::updateControlParameter" << endl;
	#endif
		return status;
	}
#elif(COMBO_DATA_MAP == 1)

#endif
#if(COMBO_STRUCT == 1)
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
#endif


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


int Processing::enableAudioOutput()
{
	int status = 0;

	//this->audioOutputEnable.setval_gpio(1);
	this->cutSignal = false;
	return status;
}


int Processing::disableAudioOutput()
{
	int status = 0;

	//this->audioOutputEnable.setval_gpio(0);
	this->cutSignal = true;
	return status;
}


#define dbg 0
double Processing::getOutputAmplitudes(void)
{
	double amplitude = 0.0000;

#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: Processing::getOutputAmplitudes" << endl;
#endif
	amplitude = this->inPosPeak[0] - this->inNegPeak[0];

#if(dbg >= 2)
	if(debugOutput) cout << "amplitude[0]: " <<  this->inPosPeak[0] - this->inNegPeak[0];
	if(debugOutput) cout << "\tamplitude[1]: " <<  this->inPosPeak[1] - this->inNegPeak[1] << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: Processing::getOutputAmplitudes: " << amplitude << endl;
#endif

	return amplitude;
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
	static double lp_y[4], lp_x[4]; // needs to be static to retain data from previous processing
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
	//lp_a[i][4] = lp[tempIndex][4];
	lp_a[1] = lp3[getPitchFilterIndex][5];
	lp_a[2] = lp3[getPitchFilterIndex][6];
	lp_a[3] = lp3[getPitchFilterIndex][7];
	//lp_b[i][4] = lp[tempIndex][9];

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
			//if(debugOutput) cout << "posSlope: " << filterOutSample[0] << ":" << filterOutSample[1] << endl;
		}

		if(filterOutSample[1] > 0.0000  &&  filterOutSample[0] < 0.0000) // negative-slope zero-crossing
		{
			tempPostFilterLow.amplitude = 0.0000; // reset to get new negative peak
			tempPreFilterLow.amplitude = 0.0000; // reset to get new negative peak
			postFilterHigh.amplitude = tempPostFilterHigh.amplitude; //reset to get new positive peak
			preFilterHigh.amplitude = tempPreFilterHigh.amplitude; //reset to get new positive peak
			//if(debugOutput) cout << "negSlope: " << filterOutSample[0] << ":" << filterOutSample[1] << endl;
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
				//int tempPitch = zeroCrossingCountArray[0] - zeroCrossingCountArray[1];
				//if(0 < tempPitch && tempPitch < 1000)
					pitch = tempPitch;
			}
			//if(debugOutput) cout << "preFilterAmp: " << preFilterPeakToPeakAmplitude << "\tAmpRatio: " << postPreAmplitudeRatio  << "\tzCrossings: " << zeroCrossingCountArray[0] << ":" << zeroCrossingCountArray[1] << "\tpitch: " << pitch << "\tgetPitchFilterIndex: " << getPitchFilterIndex << endl;
		}
		else
		{
			/*if(getPitchFilterIndexAddendIndex == 0)
			{
				getPitchFilterIndexAddendIndex = 5;
				getPitchFilterIndex = getPitchFilterIndexAddends[getPitchFilterIndexAddendIndex];

			}
			else
			{
				if(postPreAmplitudeRatio >= 0.500) // filtered signal too big: lower cut-off frequency
				{
					getPitchFilterIndex -= getPitchFilterIndexAddends[getPitchFilterIndexAddendIndex+1];
					getPitchFilterIndex += getPitchFilterIndexAddends[getPitchFilterIndexAddendIndex];
				}
				else if(postPreAmplitudeRatio <= 0.100) // filtered signal too small: raise cut-off frequency
				{
					getPitchFilterIndex += getPitchFilterIndexAddends[getPitchFilterIndexAddendIndex];
				}
			}
			getPitchFilterIndexAddendIndex--;*/
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
