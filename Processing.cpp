/*
 * Combo.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#include "Processing.h"
//#include "Filter3bb.h"
using std::cout;
using std::endl;


/**************************************
#if(dbg >= 1)
	cout << "***** ENTERING: Combo2::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: Combo2::" << endl;
#endif

#if(dbg >=2)
#endif
********************************************/
#define BUFFER_SIZE 1024
#define TEST_PROCESS_COUNT 6
#define SIGNAL_DELTA_THRESHOLD 0.03
#define TIMING_DBG 0
#define SIGPROC_DBG 0
#define AVE_ARRAY_SIZE 16
extern struct _processingParams processingParams;
//struct ProcessEvent this->processSequence[10];

//extern ComboDataInt comboData;
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
	this->enableEffects = false;
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
	comboData[loadComboIndex].initProcBuffers(comboData[loadComboIndex].procBufferArray); // reset all data ready flags to 0.

	cout << "Control Count: " << comboData[loadComboIndex].controlCount << endl;

	for(int i = 0; i < comboData[loadComboIndex].controlCount; i++)
	{
		control(0, false, &comboData[loadComboIndex].controlSequence[i], /*&this->processSequence[j]*/NULL);
	}

	for(int i = 0; i < comboData[loadComboIndex].processCount; i++)
	{
		switch(comboData[loadComboIndex].processSequence[i].processType)
		{
			case 0:
				delayb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 1:
				filter3bb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 2:
				filter3bb2(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 3:
				lohifilterb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 4:
				mixerb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 5:
				volumeb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			case 6:
				waveshaperb(0, &comboData[loadComboIndex].processSequence[i], comboData[loadComboIndex].procBufferArray,this->footswitchStatus);
				break;
			default:;
		}
	}



	for(int bufferIndex = 0; bufferIndex < comboData[loadComboIndex].bufferCount; bufferIndex++)
	{
		initBufferAveParameters(&comboData[loadComboIndex].procBufferArray[bufferIndex]);
	}

#if(dbg >= 1)
	cout << "EXITING: Processing::loadCombo" << endl;
#endif

	return status;
}

#define dbg 1
int Processing::enableComboBypass()
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::enableComboBypass" << endl;
#endif

	do
	{
		cout << "waiting for audioCallback." << endl;
	}while(this->audioCallbackRunning == true);
	this->enableEffects = false;

#if(dbg >= 1)
	cout << "EXITING: Processing::enableComboBypass" << endl;
#endif

	return status;
}

#define dbg 1
int Processing::disableComboBypass()
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: Processing::disableComboBypass" << endl;
#endif

	this->enableEffects = true;

#if(dbg >= 1)
	cout << "EXITING: Processing::disableComboBypass" << endl;
#endif

	return status;
}

int audioCallbackComboIndex = 100;
double testBuffer[10][BUFFER_SIZE];
#define dbg 0
int Processing::audioCallback(jack_nframes_t nframes,
				// A vector of pointers to each input port.
				audioBufVector inBufs,
				// A vector of pointers to each output port.
				audioBufVector outBufs)
{
	int status = 0;
	//bool envTrigger = false;

	this->audioCallbackRunning = true;
	bool processDone = false;
	double internalPosPeak[2];
	double internalNegPeak[2];

	internalPosPeak[0] = 0.00;
	internalNegPeak[0] = 0.00;
	internalPosPeak[1] = 0.00;
	internalNegPeak[1] = 0.00;
#if(dbg == 1)
	//cout << "ENTERING audioCallback:  " << endl;
#endif

//#if(TIMING_DBG == 1)
	startTimer();
//#endif
	int process = 0;

	if(audioCallbackComboIndex == this->comboIndex)
	{

		for(unsigned int i = 0; i < bufferSize; i++)
		{
			if(inputsSwitched)
			{
				comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].inputProcBufferIndex[0]].buffer[i] = inBufs[1][i]*this->inputLevel;
				comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].inputProcBufferIndex[1]].buffer[i] = inBufs[0][i]*this->inputLevel;
			}
			else
			{
				comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].inputProcBufferIndex[0]].buffer[i] = inBufs[0][i]*this->inputLevel;
				comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].inputProcBufferIndex[1]].buffer[i] = inBufs[1][i]*this->inputLevel;
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
		/*if(this->gateStatus == false && ((inputsSwitched == false && this->gateOnThreshold > this->maxAmp[0]) || // noise gate
				(inputsSwitched == true && this->gateOnThreshold > this->maxAmp[1])))
		{
			this->gateStatus = true;
			this->inputLevel = processingParams.noiseGate.gain;
		}
		if(this->gateStatus == true && ((inputsSwitched == false && this->gateOffThreshold < this->maxAmp[0]) || // noise gate
				(inputsSwitched == true && this->gateOffThreshold < this->maxAmp[1])))
		{
			this->gateStatus = false;
			this->inputLevel = 1.0;
		}*/

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
	#if(dbg == 1)
				cout << "level above noise gate high threshold: going to case 1." << endl;
	#endif
	#if(dbg >= 2)
				cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
	#endif
				this->gateEnvStatus = 1;
			}

			break;
		case 1:
	#if(dbg == 1)
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
			/*else if(this->signalLevel > this->signalLevelHighPeak)
			{
				this->signalLevelHighPeak = this->signalLevel;
	#if(dbg >= 2)
				cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
	#endif

			}*/

			break;
		case 2:	// trigger on
	#if(dbg == 1)
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
	#if(dbg == 1)
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
			/*else if(this->signalLevel < this->signalLevelLowPeak)
			{
				this->signalLevelLowPeak = this->signalLevel;
	#if(dbg >= 2)
				cout << "signalLevel: " << this->signalLevel << "\tsignalLevelLowPeak: " << this->signalLevelLowPeak << "\tsignalLevelHighPeak: " << this->signalLevelHighPeak << endl;
	#endif
			}*/

			break;
		default:
			this->gateEnvStatus = 0;

		}
		this->prevMaxAmpFilterOut = this->maxAmpFilterOut;
		this->prevMaxAmp[1] = this->maxAmp[1];
		this->prevSignalLevel = this->signalLevel;





#define dbg 0
		//***************************** Run Controls for manipulating process parameters *************


		for(int i = 0; i < comboData[this->comboIndex].controlCount; i++)
		{
			control(1, this->envTrigger, &comboData[this->comboIndex].controlSequence[i], comboData[this->comboIndex].processSequence);
		}


			//****************************** Run processes *******************************************
			//while(processDone == false)
			//{
			//	processDone = true; // initialize to TRUE.

		for(int i = 0; i < comboData[this->comboIndex].processCount; i++)
		{
	#if(dbg == 1)
			cout << "process number: " << i << endl;
	#endif
			switch(comboData[this->comboIndex].processSequence[i].processType)
			{
				case 0:
					delayb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 1:
					filter3bb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 2:
					filter3bb2(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 3:
					lohifilterb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 4:
					mixerb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 5:
					volumeb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 6:
					waveshaperb(1, &comboData[this->comboIndex].processSequence[i], comboData[this->comboIndex].procBufferArray,this->footswitchStatus);
					break;
				default:;
			}
		}

		for(unsigned int i = 0; i < bufferSize; i++)
		{
			if(inputsSwitched)
			{
				outBufs[1][i] = comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].outputProcBufferIndex[0]].buffer[i];
				outBufs[0][i] = comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].outputProcBufferIndex[1]].buffer[i];
				/*outBufs[1][i] = this->procBufferArray[0].buffer[i];
				outBufs[0][i] = this->procBufferArray[1].buffer[i];*/
			}
			else
			{
				outBufs[0][i] = comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].outputProcBufferIndex[0]].buffer[i];
				outBufs[1][i] = comboData[this->comboIndex].procBufferArray[comboData[this->comboIndex].outputProcBufferIndex[1]].buffer[i];
				/*outBufs[0][i] = this->procBufferArray[0].buffer[i];
				outBufs[1][i] = this->procBufferArray[1].buffer[i];*/
			}
		}


		/*double tempAve;
		for(int bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
		{
			this->procBufferArray[bufferIndex].aveBuffer[this->aveArrayIndex] = this->procBufferArray[bufferIndex].offset;
			tempAve = 0.00000;

			for(int arrayIndex = 0; arrayIndex < AVE_ARRAY_SIZE; arrayIndex++)
			{
				tempAve += this->procBufferArray[bufferIndex].aveBuffer[arrayIndex];
			}

			this->procBufferArray[bufferIndex].offset = tempAve/AVE_ARRAY_SIZE;
		}
		this->aveArrayIndex++;
		if(this->aveArrayIndex == AVE_ARRAY_SIZE) this->aveArrayIndex = 0;*/

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
		cout << "straight thru signal." << endl;
		for(unsigned int i = 0; i < bufferSize; i++)
		{
			outBufs[0][i] = inBufs[0][i];
			outBufs[1][i] = inBufs[1][i];
		}
		cout << "stopping combo." << endl;
		delay(1000);
		this->stopCombo(audioCallbackComboIndex);
		cout << "loading combo." << endl;
		this->loadCombo(this->comboIndex);
		audioCallbackComboIndex = this->comboIndex;
	}
	this->audioCallbackRunning = false;
#if(TIMING_DBG == 1)
	comboTime = stopTimer("audio processing");
#else
	comboTime = stopTimer(NULL);
#endif

#if(dbg == 1)
	//cout << "EXITING audioCallback:  " << endl;
#endif

	return status;
}

int Processing::getProcessData(int index, double *data)
{
	int status = 0;
	static int count;

	for(int i = 0; i < 10; i++)
	{
		data[i] = comboData[this->comboIndex].processSequence[index].internalData[i];
	}

	comboData[this->comboIndex].processSequence[index].dataReadDone = true;
	comboData[this->comboIndex].processSequence[index].dataReadReady = false;

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

	this->processSequence[index].dataReadDone = true;
	this->processSequence[index].dataReadReady = false;

	return status;
}*/

int Processing::stopCombo(int stopComboIndex)
{
	int status = 0;


	if(stopComboIndex < 10)
	{

		for(int i = 0; i < comboData[stopComboIndex].controlCount; i++)
		{
			control(3, false, &comboData[stopComboIndex].controlSequence[i], NULL);
		}

		for(int i = 0; i < comboData[stopComboIndex].processCount; i++)
		{
			switch(comboData[stopComboIndex].processSequence[i].processType)
			{
				case 0:
					delayb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 1:
					filter3bb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 2:
					filter3bb2(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 3:
					lohifilterb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 4:
					mixerb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 5:
					volumeb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				case 6:
					waveshaperb(3, &comboData[stopComboIndex].processSequence[i], comboData[stopComboIndex].procBufferArray,this->footswitchStatus);
					break;
				default:;
			}
		}

	}
	else
		status = -1;

	return status;
}

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
	cout << "EXITING: Combo::updateFootswitch" << endl;
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
	cout << "EXITING: Combo::bypassAll" << endl;
#endif
	return status;
}

int Processing::updateProcessParameter(string processName, int parameterIndex, int parameterValue)
{
	int status = 0;
	int procSequenceIndex = 0;
	// processIndex for processSequence doesn't correspond to process indexing in
	// parameterArray, so needs to be calculated.
	// indexes can't be redone in getCombo or getParameterArray because
	// they are used in updating combo files.

	for(int processIndex = 0; processIndex < 10; processIndex++)
	{
		if(comboData[this->comboIndex].processSequence[processIndex].processName.compare(processName) == 0)
		{
			comboData[this->comboIndex].processSequence[processIndex].parameters[parameterIndex] = parameterValue;
			procSequenceIndex = processIndex;
		}
	}
#if(dbg==1)
	std::cout << "\t\tprocessName: " << processName << "\t\procSequenceIndex: " << procSequenceIndex << "parameterIndex: " << parameterIndex  << "\t\parameterValue: " << parameterValue << std::endl;
#endif
	return status;
}

#define dbg 0
int Processing::updateControlParameter(string controlName, int parameterIndex, int parameterValue)
{
#if(dbg==1)
	cout << "ENTERING: Combo::updateControlParameter" << endl;
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
		if(comboData[this->comboIndex].controlSequence[controlIndex].name.compare(controlName) == 0)
		{
			comboData[this->comboIndex].controlSequence[controlIndex].parameter[parameterIndex] = parameterValue;
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
	cout << "EXITING: Combo::updateControlParameter" << endl;
#endif
	return status;
}

