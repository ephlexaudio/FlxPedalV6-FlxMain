/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */
#include "config.h"
#include "Processing.h"
//#include "Filter3bb.h"
using std::cout;
using std::endl;

#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 0
#define COMBO_STRUCT 1
/**************************************
#if(dbg >= 1)
	cout << "***** ENTERING: Processing::" << endl;
	cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: Processing::: " << status << endl;
#endif

#if(dbg >=2)
#endif
********************************************/

#define TEST_PROCESS_COUNT 6
#define SIGNAL_DELTA_THRESHOLD 0.03
#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define AVE_ARRAY_SIZE 16
extern struct _processingParams processingParams;
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

Processing::Processing():JackCpp::AudioIO("processing", 2,2)
{
	// TODO Auto-generated constructor stub
	reserveInPorts(8);
	reserveOutPorts(8);
	this->gateOffThreshold = processingParams.noiseGate.highThres;
	this->gateOnThreshold = processingParams.noiseGate.lowThres;
	this->triggerHighThreshold = processingParams.trigger.highThres;
	this->triggerLowThreshold = processingParams.trigger.lowThres;
	//this->checkInputs = false;
	this->gateStatus = true;
	//this->inputsSwitched = false;
	this->envTrigger = false;
	this->envTriggerPeriods = 0;
	this->maxAmpFilterIndex = 0;
	this->signalLevelLowPeak = 0.0;
	this->signalLevelHighPeak = 0.0;
	this->gateEnvStatus = 0;
	this->signalDeltaPositiveCount = 0;
	this->signalDeltaNegativeCount = 0;
	this->chan1GndCount = 0;
	this->chan2GndCount = 0;
	this->maxAmpFilterOut = 0;
	this->prevMaxAmpFilterOut = 0.0;
	this->prevSignalLevel = 0.0;
	this->signalDeltaFilterIndex = 0;
	this->signalDeltaFilterOut = 0.0;
	this->signalLevel = 0.0;
	this->processingEnabled = true;
	this->processingUpdated = false;
	this->processingContextAllocationError = false;
	this->updateProcessing = false;
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


bool Processing::areInputsSwitched(void) // see if JACK has connected input jacks backwards
{
	bool intInputsSwitched = false;
#if(dbg >= 1)
	cout << "***** ENTERING: Processing::" << endl;
	cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: Processing::: " << status << endl;
#endif

	usleep(300000);

	system("echo \"0\" > /sys/class/gpio/gpio41/value");
	system("echo \"1\" > /sys/class/gpio/gpio43/value");
	usleep(100000);

	posPeakArray[0][0] = posPeak[0];
	negPeakArray[0][0] = negPeak[0];
	posPeakArray[0][1] = posPeak[1];
	negPeakArray[0][1] = negPeak[1];
	fprintf(stderr,"posPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", posPeak[0], negPeak[0], posPeak[1], negPeak[1]);
	usleep(50000);

	posPeakArray[1][0] = posPeak[0];
	negPeakArray[1][0] = negPeak[0];
	posPeakArray[1][1] = posPeak[1];
	negPeakArray[1][1] = negPeak[1];
	fprintf(stderr,"posPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", posPeak[0], negPeak[0], posPeak[1], negPeak[1]);
	usleep(50000);

	posPeakArray[2][0] = posPeak[0];
	negPeakArray[2][0] = negPeak[0];
	posPeakArray[2][1] = posPeak[1];
	negPeakArray[2][1] = negPeak[1];
	fprintf(stderr,"posPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", posPeak[0], negPeak[0], posPeak[1], negPeak[1]);
	usleep(50000);

	posPeakArray[3][0] = posPeak[0];
	negPeakArray[3][0] = negPeak[0];
	posPeakArray[3][1] = posPeak[1];
	negPeakArray[3][1] = negPeak[1];
	fprintf(stderr,"posPeak[0]:%f\tnegPeak[0]:%f\t\tposPeak[1]:%f\tnegPeak[1]:%f\n", posPeak[0], negPeak[0], posPeak[1], negPeak[1]);

	system("echo \"1\" > /sys/class/gpio/gpio41/value");
	system("echo \"0\" > /sys/class/gpio/gpio43/value");


	chan1GndCount = 0;
	chan2GndCount = 0;
	for(int i = 0; i < 4; i++)
	{
		if((0.098 < posPeakArray[i][0] && posPeakArray[i][0] < 0.1)) chan1GndCount++;
		if((0.098 < posPeakArray[i][1] && posPeakArray[i][1] < 0.1)) chan2GndCount++;
	}

	cout << "chan1GndCount: " << chan1GndCount << "\tchan2GndCount: " << chan2GndCount << endl;
	if(chan1GndCount > chan2GndCount)
	{
		intInputsSwitched = true;//switchedStatus = 1;
		fprintf(stderr,"*******************inputs switched\n");
	}
	else
	{
		intInputsSwitched = false;//switchedStatus = 0;
		fprintf(stderr,"*******************inputs not switched\n");
	}
	return intInputsSwitched;
}


#if(COMBO_DATA_VECTOR == 1)
#define dbg 1
	int Processing::loadCombo(int loadComboIndex)
	{
		int status = 0;
		/*int inputBufferIndex = 0;
		int outputBufferIndex = 1;
		int connectBufferIndex = 0;*/
	#if(dbg >= 1)
		cout << "ENTERING: Processing::loadCombo" << endl;
	#endif

		this->aveArrayIndex = 0;
		comboDataVector[loadComboIndex].initProcBuffers(comboDataVector[loadComboIndex].procBufferArray); // reset all data ready flags to 0.

		cout << "Control Count: " << comboDataVector[loadComboIndex].controlCount << endl;
		cout << "Process Count: " << comboDataVector[loadComboIndex].processCount << endl;
		cout << "Buffer Count: " << comboDataVector[loadComboIndex].bufferCount << endl;

		for(int i = 0; i < comboDataVector[loadComboIndex].controlCount; i++)
		{
			control(0, false, &comboDataVector[loadComboIndex].controlSequence[i], /*&this->processSequence[j]*/NULL);
		}

		for(int i = 0; i < comboDataVector[loadComboIndex].processCount; i++)
		{
			switch(comboDataVector[loadComboIndex].processSequence[i].processType)
			{
				case 0:
					delayb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 1:
					filter3bb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 2:
					filter3bb2(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 3:
					lohifilterb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 4:
					mixerb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 5:
					volumeb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 6:
					waveshaperb(0, &comboDataVector[loadComboIndex].processSequence[i], comboDataVector[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				default:;
			}
		}



		for(int bufferIndex = 0; bufferIndex < comboDataVector[loadComboIndex].bufferCount; bufferIndex++)
		{
			initBufferAveParameters(&comboDataVector[loadComboIndex].procBufferArray[bufferIndex]);
		}

	#if(dbg >= 1)
		cout << "EXITING: Processing::loadCombo" << endl;
	#endif

		return status;
	}

#elif(COMBO_DATA_ARRAY == 1)
	int Processing::loadCombo(int loadComboIndex)
	{
		int status = 0;
		/*int inputBufferIndex = 0;
		int outputBufferIndex = 1;
		int connectBufferIndex = 0;*/
	#if(dbg >= 1)
		cout << "ENTERING: Processing::loadCombo" << endl;
	#endif

		this->aveArrayIndex = 0;
		comboDataArray[loadComboIndex].initProcBuffers(comboDataArray[loadComboIndex].procBufferArray); // reset all data ready flags to 0.

		cout << "Control Count: " << comboDataArray[loadComboIndex].controlCount << endl;
		cout << "Process Count: " << comboDataArray[loadComboIndex].processCount << endl;
		cout << "Buffer Count: " << comboDataArray[loadComboIndex].bufferCount << endl;

		for(int i = 0; i < comboDataArray[loadComboIndex].controlCount; i++)
		{
			control(0, false, &comboDataArray[loadComboIndex].controlSequence[i], /*&this->processSequence[j]*/NULL);
		}

		for(int i = 0; i < comboDataArray[loadComboIndex].processCount; i++)
		{
			switch(comboDataArray[loadComboIndex].processSequence[i].processType)
			{
				case 0:
					delayb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 1:
					filter3bb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 2:
					filter3bb2(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 3:
					lohifilterb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 4:
					mixerb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 5:
					volumeb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 6:
					waveshaperb(0, &comboDataArray[loadComboIndex].processSequence[i], comboDataArray[loadComboIndex].procBufferArray,this->footswitchStatus);
					break;
				default:;
			}
		}



		for(int bufferIndex = 0; bufferIndex < comboDataArray[loadComboIndex].bufferCount; bufferIndex++)
		{
			initBufferAveParameters(&comboDataArray[loadComboIndex].procBufferArray[bufferIndex]);
		}

	#if(dbg >= 1)
		cout << "EXITING: Processing::loadCombo" << endl;
	#endif

		return status;
	}
#elif(COMBO_DATA_MAP == 1)

#endif

#if(COMBO_STRUCT == 1)
#define dbg 2
	int Processing::loadCombo(void)
	{
		int status = 0;
		/*int inputBufferIndex = 0;
		int outputBufferIndex = 1;
		int connectBufferIndex = 0;*/
	#if(dbg >= 1)
		cout << "ENTERING: Processing::loadCombo" << endl;
		cout << "currentComboStructIndex: " << currentComboStructIndex << endl;
	#endif

		this->aveArrayIndex = 0;
		//initProcBuffers(combo.procBufferArray); // reset all data ready flags to 0.

		cout << "Control Count: " << combo.controlCount << endl;
		cout << "Process Count: " << combo.processCount << endl;
		cout << "Buffer Count: " << combo.bufferCount << endl;

		for(int i = 0; i < combo.controlCount; i++)
		{
			control('l', false, &combo.controlSequence[i], /*&this->processSequence[j]*/NULL);
		}

		for(int i = 0; i < combo.processCount; i++)
		{
			switch(combo.processSequence[i].processType)
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
				default:;
			}
		}

		for(unsigned int bufferIndex = 0; bufferIndex < combo.bufferCount; bufferIndex++)
		{
			//initBufferAveParameters(&combo.procBufferArray[bufferIndex]);
#if(dbg >= 2)
			cout << "clearing procBufferArray[" << bufferIndex << "]:" << endl;
#endif
#if(PROC_BUFFER_REF == 1)
			clearProcBuffer(&combo.procBufferArray[bufferIndex]);
#elif(PROC_BUFFER_VAL == 1)
			clearProcBuffer(combo.procBufferArray[bufferIndex]);
#endif
		}
#if(dbg >= 2)
			cout << "clearing procBufferArray[58]:" << endl;
#endif
#if(PROC_BUFFER_REF == 1)
			clearProcBuffer(&combo.procBufferArray[58]);
#elif(PROC_BUFFER_VAL == 1)
			clearProcBuffer(combo.procBufferArray[58]);
#endif

	#if(dbg >= 1)
		cout << "EXITING: Processing::loadCombo: " << status << endl;
	#endif

		return status;
	}
#endif



double testBuffer[10][BUFFER_SIZE];
#define dbg 0

#if(COMBO_STRUCT == 1)
	int Processing::audioCallback(jack_nframes_t nframes,
					// A vector of pointers to each input port.
					audioBufVector inBufs,
					// A vector of pointers to each output port.
					audioBufVector outBufs)
	{
		int status = 0;
		//cout << "ENTERING audioCallback:  " << endl;
		//bool envTrigger = false;

		this->audioCallbackRunning = true;
		bool processDone = false;
		double internalPosPeak[2];
		double internalNegPeak[2];

		internalPosPeak[0] = 0.00;
		internalNegPeak[0] = 0.00;
		internalPosPeak[1] = 0.00;
		internalNegPeak[1] = 0.00;
	#if(dbg >= 1)
		//cout << "ENTERING audioCallback:  " << endl;
	#endif

	//#if(TIMING_DBG == 1)
		startTimer();
	//#endif
		int process = 0;

		//if(audioCallbackComboIndex == this->comboIndex && this->processingEnabled == true && this->updateProcessing == false)
		if(this->processingEnabled == true && this->processingUpdated == true)
		{
			//cout << "PROCESSING ACTIVE." << endl;
			for(unsigned int i = 0; i < bufferSize; i++)
			{
				if(inputsSwitched)
				{
					combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer[i] = inBufs[1][i]*this->inputLevel;
					combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[0][i]*this->inputLevel;
				}
				else
				{
					combo.procBufferArray[combo.inputProcBufferIndex[0]].buffer[i] = inBufs[0][i]*this->inputLevel;
					combo.procBufferArray[combo.inputProcBufferIndex[1]].buffer[i] = inBufs[1][i]*this->inputLevel;
				}

				if(internalPosPeak[0] < inBufs[0][i]) internalPosPeak[0] = inBufs[0][i];
				if(internalNegPeak[0] > inBufs[0][i]) internalNegPeak[0] = inBufs[0][i];
				if(internalPosPeak[1] < inBufs[1][i]) internalPosPeak[1] = inBufs[1][i];
				if(internalNegPeak[1] > inBufs[1][i]) internalNegPeak[1] = inBufs[1][i];

			}
			//cout << "testBuffer[0]: " << testBuffer[0][0] << "\ttestBuffer[1]: " << testBuffer[1][0] << endl;
			this->posPeak[0] = internalPosPeak[0];
			this->negPeak[0] = internalNegPeak[0];
			this->posPeak[1] = internalPosPeak[1];
			this->negPeak[1] = internalNegPeak[1];

			this->maxAmp[0] = this->posPeak[0] - this->negPeak[0];
			this->maxAmp[1] = this->posPeak[1] - this->negPeak[1];

			//********************** Noise gate function *********************
//			if(this->gateStatus == false && ((inputsSwitched == false && this->gateOnThreshold > this->maxAmp[0]) || // noise gate
//					(inputsSwitched == true && this->gateOnThreshold > this->maxAmp[1])))
//			{
//				this->gateStatus = true;
//				this->inputLevel = processingParams.noiseGate.gain;
//			}
//			if(this->gateStatus == true && ((inputsSwitched == false && this->gateOffThreshold < this->maxAmp[0]) || // noise gate
//					(inputsSwitched == true && this->gateOffThreshold < this->maxAmp[1])))
//			{
//				this->gateStatus = false;
//				this->inputLevel = 1.0;
//			}

			if(this->maxAmpFilterIndex < 15)
			{
				this->maxAmpFilter[this->maxAmpFilterIndex++] = this->maxAmp[0];
			}
			else
			{
				this->maxAmpFilter[this->maxAmpFilterIndex] = this->maxAmp[0];
				this->maxAmpFilterIndex = 0;
			}

			this->maxAmpFilterOut = (this->maxAmpFilter[0] + this->maxAmpFilter[1] + this->maxAmpFilter[2] +
					this->maxAmpFilter[3] + this->maxAmpFilter[4] + this->maxAmpFilter[5] +
					this->maxAmpFilter[6] + this->maxAmpFilter[7] + this->maxAmpFilter[8] +
					this->maxAmpFilter[9] + this->maxAmpFilter[10] + this->maxAmpFilter[11] +
					this->maxAmpFilter[12] + this->maxAmpFilter[13] + this->maxAmpFilter[14] +
					this->maxAmpFilter[15])/16;
			this->signalLevel = this->maxAmpFilterOut;


			if(this->signalDeltaFilterIndex < 15)
			{
				this->signalDeltaFilter[this->signalDeltaFilterIndex++] = this->signalLevel - this->prevSignalLevel;
			}
			else
			{
				this->signalDeltaFilter[this->signalDeltaFilterIndex] = this->signalLevel - this->prevSignalLevel;
				this->signalDeltaFilterIndex = 0;
			}
			this->signalDeltaFilterOut = (this->signalDeltaFilter[0] + this->signalDeltaFilter[1] + this->signalDeltaFilter[2] +
					this->signalDeltaFilter[3] + this->signalDeltaFilter[4] + this->signalDeltaFilter[5] +
					this->signalDeltaFilter[6] + this->signalDeltaFilter[7] + this->signalDeltaFilter[8] +
					this->signalDeltaFilter[9] + this->signalDeltaFilter[10] + this->signalDeltaFilter[11] +
					this->signalDeltaFilter[12] + this->signalDeltaFilter[13] + this->signalDeltaFilter[14] +
					this->signalDeltaFilter[15])/16;
			//this->signalLevel = this->maxAmpFilterOut;



		#if(dbg >= 2)
			cout << "signal: " << this->signalLevel << ",signal delta: " << this->signalDeltaFilterOut << endl;
		#endif
			//***************************** Envelope trigger function *********************************

			switch(this->gateEnvStatus)
			{
			case 0:	// noise gate on
				this->gateStatus = true;
				this->inputLevel = processingParams.noiseGate.gain;
				this->envTriggerPeriods = 0;
				this->envTrigger = false;
				if((inputsSwitched == false && this->gateOffThreshold < this->maxAmp[0]) || // noise gate
						(inputsSwitched == true && this->gateOffThreshold < this->maxAmp[1]))
				{
		#if(dbg >= 1)
					cout << "level above noise gate high threshold: going to case 1." << endl;
		#endif
		#if(dbg >= 2)
					cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
		#endif
					this->gateEnvStatus = 1;
				}

				break;
			case 1:
		#if(dbg >= 1)
				cout << "case 1" << endl;
		#endif
				this->gateStatus = false;
				this->inputLevel = 1.0;
				this->envTrigger = false;


				if((inputsSwitched == false && this->gateOnThreshold > this->maxAmp[0]) ||  // going below noise gate threshold
						(inputsSwitched == true && this->gateOnThreshold > this->maxAmp[1]))
				{
		#if(dbg >= 1)
					cout << "level below noise gate low threshold: going to case 0." << endl;
		#endif
					this->gateEnvStatus = 0;
				}
				else if(this->signalDeltaFilterOut > this->triggerHighThreshold)
				{
					// pick applied to string, causing damping
					this->envTriggerPeriods = 0;
					//this->signalLevelLowPeak = this->signalLevel;
					this->gateEnvStatus = 2;
		#if(dbg >= 2)
					cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
		#endif
				}

				break;
			case 2:	// trigger on
		#if(dbg >= 1)
				cout << "case 2" << endl;
		#endif
				this->gateStatus = false;
				//this->inputLevel = 1.0;
				this->envTrigger = true;
				this->gateEnvStatus = 3;
				//cout << "pick detected: going to case 3." << endl;
				break;
			case 3: // trigger off
				this->gateStatus = false;
				//this->inputLevel = 1.0;
				this->envTrigger = false;
		#if(dbg >= 1)
				cout << "case 3" << endl;
		#endif

				if((inputsSwitched == false && this->gateOnThreshold > this->maxAmp[0]) ||  // going below noise gate threshold
						(inputsSwitched == true && this->gateOnThreshold > this->maxAmp[1]))
				{
		#if(dbg >= 1)
					cout << "level below noise gate low threshold: going to case 0." << endl;
		#endif
					this->gateEnvStatus = 0;
				}
				else if(this->signalDeltaFilterOut < this->triggerLowThreshold)
				{
					// pick releasing string, causing new vibration
		#if(dbg >= 1)
					cout << "pick released: going to case 1." << endl;
		#endif
					this->envTriggerPeriods = 0;
					this->signalLevelHighPeak = this->signalLevel;
					this->gateEnvStatus = 1;
		#if(dbg >= 2)
					cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
		#endif
				}

				break;
			default:
				this->gateEnvStatus = 0;

			}
			this->prevMaxAmpFilterOut = this->maxAmpFilterOut;
			this->prevMaxAmp[1] = this->maxAmp[1];
			this->prevSignalLevel = this->signalLevel;




			this->processingContextAllocationError = false;

			//***************************** Run Controls for manipulating process parameters *************


			for(int i = 0; i < combo.controlCount; i++)
			{
				if(control('r', this->envTrigger, &combo.controlSequence[i], combo.processSequence) < 0)
					this->processingContextAllocationError = true;
			}


				//****************************** Run processes *******************************************
				//while(processDone == false)
				//{
				//	processDone = true; // initialize to TRUE.

			for(int i = 0; i < combo.processCount; i++)
			{
		#if(dbg >= 1)
				cout << "process number: " << i << endl;
		#endif
				switch(combo.processSequence[i].processType)
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
					default:;
				}
				if(this->processingContextAllocationError == true)
				{
					this->processingEnabled = false;
					cout << "process or control context allocation failed: disabling processing." <<  endl;
					break;
				}
			}


			if(this->processingEnabled == true) // skip if there is an allocation error detected
			{
				for(unsigned int i = 0; i < bufferSize; i++)
				{
					if(inputsSwitched)
					{
						outBufs[1][i] = combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
						outBufs[0][i] = combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
					}
					else
					{
						outBufs[0][i] = combo.procBufferArray[combo.outputProcBufferIndex[0]].buffer[i];
						outBufs[1][i] = combo.procBufferArray[combo.outputProcBufferIndex[1]].buffer[i];
					}

					if(internalPosPeak[0] < inBufs[0][i]) internalPosPeak[0] = inBufs[0][i];
					if(internalNegPeak[0] > inBufs[0][i]) internalNegPeak[0] = inBufs[0][i];
					if(internalPosPeak[1] < inBufs[1][i]) internalPosPeak[1] = inBufs[1][i];
					if(internalNegPeak[1] > inBufs[1][i]) internalNegPeak[1] = inBufs[1][i];
				}

				this->posPeak[0] = internalPosPeak[0];
				this->negPeak[0] = internalNegPeak[0];
				this->posPeak[1] = internalPosPeak[1];
				this->negPeak[1] = internalNegPeak[1];

			}
			else //allocation error detected, send signal straight thru to avoid Jack crash
			{
				for(unsigned int i = 0; i < bufferSize; i++)
				{
					outBufs[0][i] = inBufs[0][i];
					outBufs[1][i] = inBufs[1][i];
				}
			}





		#if(SIGPROC_DBG)
			int bufferIndex;
			cout << "BUFFER AVERAGES: " << endl;
			for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
			{
				cout << this->procBufferArray[bufferIndex].average << ", ";
			}
			cout << endl;
		#endif
		}
		else
		{
			//cout << "straight thru signal." << endl;
			//cout << "PROCESSING BYPASSED." << endl;
			for(unsigned int i = 0; i < bufferSize; i++)
			{
				outBufs[0][i] = inBufs[0][i];
				outBufs[1][i] = inBufs[1][i];

				if(internalPosPeak[0] < inBufs[0][i]) internalPosPeak[0] = inBufs[0][i];
				if(internalNegPeak[0] > inBufs[0][i]) internalNegPeak[0] = inBufs[0][i];
				if(internalPosPeak[1] < inBufs[1][i]) internalPosPeak[1] = inBufs[1][i];
				if(internalNegPeak[1] > inBufs[1][i]) internalNegPeak[1] = inBufs[1][i];
			}

			//cout << "testBuffer[0]: " << testBuffer[0][0] << "\ttestBuffer[1]: " << testBuffer[1][0] << endl;
			this->posPeak[0] = internalPosPeak[0];
			this->negPeak[0] = internalNegPeak[0];
			this->posPeak[1] = internalPosPeak[1];
			this->negPeak[1] = internalNegPeak[1];

			/*if(this->updateProcessing == true)
			{
				cout << "stopping combo." << endl;
				this->stopCombo();
				cout << "loading combo." << endl;
				this->loadCombo();
				this->processingUpdated = true;
				this->updateProcessing = false;
				this->processingContextAllocationError = false;
				cout << "combo loaded." << endl;
			}*/

		}
		this->audioCallbackRunning = false;
	#if(TIMING_DBG == 1)
		comboTime = stopTimer("audio processing");
	#else
		comboTime = stopTimer(NULL);
	#endif

	#if(dbg >= 1)
		cout << "EXITING audioCallback: " << status << endl;
	#endif

		return status;
	}
#endif

int Processing::getProcessData(int index, double *data)
{
	int status = 0;
/*	static int count;

	for(int i = 0; i < 10; i++)
	{
#if(COMBO_DATA_VECTOR == 1)
		data[i] = comboDataVector[this->comboIndex].processSequence[index].internalData[i];
#elif(COMBO_DATA_ARRAY == 1)
		data[i] = comboDataArray[this->comboIndex].processSequence[index].internalData[i];
#elif(COMBO_DATA_MAP == 1)
		data[i] = comboDataMap[this->comboName].processSequence[index].internalData[i];
#if(COMBO_STRUCT == 1)
		data[i] = combo.processSequence[index].internalData[i];
#endif
	}*/


	return status;
}

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


int loadComponentSymbols(void)
{
	delayb('c', NULL, NULL, NULL);
	filter3bb('c', NULL, NULL, NULL);
	filter3bb2('c', NULL, NULL, NULL);
	lohifilterb('c', NULL, NULL, NULL);
	mixerb('c', NULL, NULL, NULL);
	volumeb('c', NULL, NULL, NULL);
	waveshaperb('c', NULL, NULL, NULL);

	return 0;
}


#if(COMBO_DATA_VECTOR == 1)
	int Processing::stopCombo(int stopComboIndex)
	{
		int status = 0;


		if(stopComboIndex < 15)
		{

			for(int i = 0; i < comboDataVector[stopComboIndex].controlCount; i++)
			{
				control(3, false, &comboDataVector[stopComboIndex].controlSequence[i], NULL);
			}

			for(int i = 0; i < comboDataVector[stopComboIndex].processCount; i++)
			{
				switch(comboDataVector[stopComboIndex].processSequence[i].processType)
				{
					case 0:
						delayb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 1:
						filter3bb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 2:
						filter3bb2(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 3:
						lohifilterb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 4:
						mixerb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 5:
						volumeb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 6:
						waveshaperb(3, &comboDataVector[stopComboIndex].processSequence[i], comboDataVector[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					default:;
				}
			}

		}
		else
			status = -1;

		return status;
	}
#elif(COMBO_DATA_ARRAY == 1)
	int Processing::stopCombo(int stopComboIndex)
	{
		int status = 0;


		if(stopComboIndex < 15)
		{

			for(int i = 0; i < comboDataArray[stopComboIndex].controlCount; i++)
			{
				control(3, false, &comboDataArray[stopComboIndex].controlSequence[i], NULL);
			}

			for(int i = 0; i < comboDataArray[stopComboIndex].processCount; i++)
			{
				switch(comboDataArray[stopComboIndex].processSequence[i].processType)
				{
					case 0:
						delayb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 1:
						filter3bb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 2:
						filter3bb2(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 3:
						lohifilterb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 4:
						mixerb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 5:
						volumeb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					case 6:
						waveshaperb(3, &comboDataArray[stopComboIndex].processSequence[i], comboDataArray[stopComboIndex].procBufferArray,this->footswitchStatus);
						break;
					default:;
				}
			}

		}
		else
			status = -1;

		return status;
	}
#elif(COMBO_DATA_MAP == 1)
#endif

#if(COMBO_STRUCT == 1)
#define dbg 2
	int Processing::stopCombo(void)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::stopCombo" << endl;
	cout << "oldComboStructIndex: " << oldComboStructIndex << endl;
#endif

		int index = 0;

		//if(stopComboIndex < 15)
		{

			for(int i = 0; i < combo.controlCount; i++)
			{
				control('s', false, &combo.controlSequence[i], NULL);
			}

			for(int i = 0; i < combo.processCount; i++)
			{
				switch(combo.processSequence[i].processType)
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
					default:;
				}
			}

		}
		for(unsigned int bufferIndex = 0; bufferIndex < combo.bufferCount; bufferIndex++)
		{
			//initBufferAveParameters(combo.procBufferArray[bufferIndex]);
#if(dbg >= 2)
			cout << "clearing procBufferArray[" << bufferIndex << "]:" << endl;
#endif
#if(PROC_BUFFER_REF == 1)
			clearProcBuffer(&combo.procBufferArray[bufferIndex]);
#elif(PROC_BUFFER_VAL == 1)
			clearProcBuffer(combo.procBufferArray[bufferIndex]);
#endif
		}
#if(dbg >= 2)
			cout << "clearing procBufferArray[58]:" << endl;
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
	cout << "EXITING: Processing::stopCombo: " << status << endl;
#endif

		return status;
	}
#endif


#define dbg 0
int Processing::updateFootswitch(int *footswitchStatus)
{
	int status = 0;

#if(dbg >= 1)
	cout << "ENTERING: Combo::updateFootswitch" << endl;
#endif

#if(dbg >= 1)
	cout << "footswitchStatus: ";
#endif
	for(int i = 0; i < 10; i++)
	{
		this->footswitchStatus[i] = footswitchStatus[i];
#if(dbg >= 1)
		cout << this->footswitchStatus[i] << ",";
#endif
	}
#if(dbg >= 1)
	cout << endl;
#endif

#if(dbg >= 1)
	cout << "EXITING: Combo::updateFootswitch: " << status << endl;
#endif
	return status;
}

#define dbg 0
int Processing::bypassAll()
{
	int status = 0;
#if(dbg >= 2)
	cout << "ENTERING: Combo::bypassAll" << endl;
#endif
	for(int i = 0; i < 10; i++)
	{
		this->footswitchStatus[i] = 0;
	}

#if(dbg >= 2)
	cout << "EXITING: Combo::bypassAll: " << status << endl;
#endif
	return status;
}


#if(COMBO_DATA_VECTOR == 1)
	int Processing::updateProcessParameter(string processName, int parameterIndex, int parameterValue)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::updateProcessParameter" << endl;
	cout << "processName: " << processName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
		std::cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << std::endl;
	#endif
#if(dbg >= 1)
	cout << "EXITING: Processing::updateProcessParameter" << endl;
#endif
		return status;
	}

#elif(COMBO_DATA_ARRAY == 1)
	int Processing::updateProcessParameter(string processName, int parameterIndex, int parameterValue)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::updateProcessParameter" << endl;
	cout << "processName: " << processName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
		std::cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << std::endl;
	#endif
#if(dbg >= 1)
	cout << "EXITING: Processing::updateProcessParameter" << endl;
#endif
		return status;
	}
#elif(COMBO_DATA_MAP == 1)

#endif
#if(COMBO_STRUCT == 1)
#define dbg 0
	int Processing::updateProcessParameter(string processName, int parameterIndex, int parameterValue)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::updateProcessParameter" << endl;
	cout << "processName: " << processName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
			}
		}
	#if(dbg>=2)
		std::cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << std::endl;
	#endif

#if(dbg >= 1)
	cout << "EXITING: Processing::updateProcessParameter: " << status << endl;
#endif
		return status;
	}
#endif



#if(COMBO_DATA_VECTOR == 1)
	#define dbg 0
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg>=1)
		cout << "ENTERING: Processing::updateControlParameter" << endl;
		cout << "controlName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
				comboDataVector[this->comboIndex].controlSequence[controlIndex].parameter[parameterIndex] = parameterValue;
				cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
				controlSequenceIndex = controlIndex;
				break;
			}
		}
	#if(dbg>=2)
		std::cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << this->controlSequence[controlIndex].parameter[parameterIndex] << std::endl;
		std::cout << "\t\ttargetProcessIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processIndex;
		std::cout << "\t\ttargetProcessName: " << this->processSequence[this->controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		std::cout << "\t\ttargetProcessParameterIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg>=1)
		cout << "EXITING: Processing::updateControlParameter" << endl;
	#endif
		return status;
	}
#elif(COMBO_DATA_ARRAY == 1)
	#define dbg 0
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg>=1)
		cout << "ENTERING: Processing::updateControlParameter" << endl;
		cout << "controlName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
				comboDataArray[this->comboIndex].controlSequence[controlIndex].parameter[parameterIndex] = parameterValue;
				cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
				controlSequenceIndex = controlIndex;
				break;
			}
		}
	#if(dbg>=2)
		std::cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << this->controlSequence[controlIndex].parameter[parameterIndex] << std::endl;
		std::cout << "\t\ttargetProcessIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processIndex;
		std::cout << "\t\ttargetProcessName: " << this->processSequence[this->controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		std::cout << "\t\ttargetProcessParameterIndex: " << this->controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg>=1)
		cout << "EXITING: Processing::updateControlParameter" << endl;
	#endif
		return status;
	}
#elif(COMBO_DATA_MAP == 1)

#endif
#if(COMBO_STRUCT == 1)
	#define dbg 1
	int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
	{
	#if(dbg >= 1)
		cout << "ENTERING: Processing::updateControlParameter" << endl;
		cout << "controlName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tparameterValue: " << parameterValue << endl;
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
			cout << "comparing: " << combo.controlSequence[controlIndex].name << " and " << controlName << endl;
#endif
			if(combo.controlSequence[controlIndex].name.compare(controlName) == 0)
			{
				combo.controlSequence[controlIndex].parameter[parameterIndex] = parameterValue;
				cout << "UPDATING: control index: " << controlIndex << "\t parameter index: " << parameterIndex << "\t parameter value: " << parameterValue << endl;
				controlSequenceIndex = controlIndex;
				break;
			}
		}

	#if(dbg >= 2)
		std::cout << "\t\tcontrolName: " << controlName  << "parameterIndex: " << parameterIndex  << "\tparameterValue: " << combo.controlSequence[controlIndex].parameter[parameterIndex] << std::endl;
		std::cout << "\t\ttargetProcessIndex: " << combo.controlSequence[controlIndex].paramContConnection[0].processIndex;
		std::cout << "\t\ttargetProcessName: " << combo.processSequence[combo.controlSequence[controlIndex].paramContConnection[0].processIndex].processName;
		std::cout << "\t\ttargetProcessParameterIndex: " << combo.controlSequence[controlIndex].paramContConnection[0].processParamIndex << endl;
	#endif
	#if(dbg >= 1)
		cout << "EXITING: Processing::updateControlParameter: " << status << endl;
	#endif
		return status;
	}
#endif


#define dbg 1
int Processing::enableProcessing()
{

#if(dbg==1)
	cout << "ENTERING: Processing::enableProcessing" << endl;
#endif

	this->processingEnabled = true;
#if(dbg>=1)
	cout << "EXITING: Processing::enableProcessing" << endl;
#endif

	return 0;
}

#define dbg 1
int Processing::disableProcessing()
{
#if(dbg==1)
	cout << "ENTERING: Processing::disableProcessing" << endl;
#endif

	this->processingEnabled = false;
#if(dbg>=1)
	cout << "EXITING: Processing::disableProcessing" << endl;
#endif

	return 0;
}

#define dbg 2
double Processing::getOutputAmplitudes(void)
{
	double amplitude = 0.0000;

#if(dbg >= 1)
	cout << "ENTERING: Processing::getOutputAmplitudes" << endl;
#endif
	amplitude = this->posPeak[0] - this->negPeak[0];

#if(dbg >= 2)
	cout << "amplitude[0]: " <<  this->posPeak[0] - this->negPeak[0];
	cout << "\tamplitude[1]: " <<  this->posPeak[1] - this->negPeak[1] << endl;
#endif

#if(dbg >= 1)
	cout << "EXITING: Processing::getOutputAmplitudes: " << amplitude << endl;
#endif

	return amplitude;
}
