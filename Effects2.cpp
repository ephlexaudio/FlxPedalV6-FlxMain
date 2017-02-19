/*
 * Effects.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */
#include "config.h"
#include "Effects2.h"
#include "ComponentSymbols.h"



#define NUMBER_OF_BANDS 2
#define CONTROL_VALUE_INDEX_MAX 100
#define AVE_BUFFER_SIZE 16
#define DIST_AVE_BUFFER_SIZE 3
#define FOOTSWITCH_ALWAYS_ON 0
#define AVERAGE_OFFSET_ON 1
#define COUPLING_FILTER_ON 0
#define NOISE_FILTER_ON 1



/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1*/

/* actions:
 * 		0: load the ProcessEvent data
 * 		1: run the process
 * 		2: read data from process??
 * 		3: stop the process
 *
 */

// set


extern vector<string> componentVector;

extern unsigned int bufferSize;
extern int inputCouplingMode;
extern int waveshaperMode;
#define dbg 1

int envGenCount = 0;
int lfoCount = 0;

int delaybCount = 0;
int filter3bbCount = 0;
int filter3bb2Count = 0;
int lohifilterbCount = 0;
int mixerbCount = 0;
int volumebCount = 0;
int waveshaperbCount = 0;

/*************************************
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::" << endl;
	cout << ": " <<  << endl;
#endif
#if(dbg >= 1)
	cout << "***** EXITING: Effects2::: " << status << endl;
#endif
******************************************/

/*int resetProcBuffer(struct ProcessBuffer procBufferArray)
{
	int status = 0;

	procBufferArray.processed = 0;
	procBufferArray.ready = 0;

	return status;
}*/


/*int setProcBuffer(struct ProcessBuffer procBufferArray, int processed, int ready)
{
	int status = 0;

	procBufferArray.processed = processed;
	procBufferArray.ready = ready;

	return status;
}*/

/*int initProcBuffers(struct ProcessBuffer *procBufferArray)
{
	int status = 0;

	for(int i = 0; i < 60; i++)
	{
		resetProcBuffer(procBufferArray[i]);
	}

	return status;
}*/

#define dbg 1
int clearProcBuffer(struct ProcessBuffer *procBuffer)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: clearProcBuffer" << endl;
	cout << "procBuffer name: " << procBuffer->processName << endl;
#endif

#if(dbg >= 2)
#endif

#if(dbg >= 2)
	cout << "clearing: " << procBuffer->processName << ":" << procBuffer->portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		cout << procBuffer->buffer[i] << ",";
	}
	cout << endl;
#endif

	for(unsigned int i = 0; i < BUFFER_SIZE; i++)
	{
		procBuffer->buffer[i] = 0.000;
	}

#if(dbg >= 2)
	cout << "cleared: " << procBuffer->processName << ":" << procBuffer->portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		cout << procBuffer->buffer[i] << ",";
	}
	cout << endl;
#endif

#if(dbg >= 1)
	cout << "EXITING: clearProcBuffer" << endl;
#endif

	return status;

}


#define dbg 1
int clearProcBuffer(struct ProcessBuffer procBuffer)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: clearProcBuffer" << endl;
#endif

#if(dbg >= 2)
	cout << "clearing: " << procBuffer.processName << ":" << procBuffer.portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		cout << procBuffer.buffer[i] << ",";
	}
	cout << endl;
#endif

	for(unsigned int i = 0; i < BUFFER_SIZE; i++)
	{
		procBuffer.buffer[i] = 0.000;
	}

#if(dbg >= 2)
	cout << "cleared: " << procBuffer.processName << ":" << procBuffer.portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		cout << procBuffer.buffer[i] << ",";
	}
	cout << endl;
#endif

#if(dbg >= 1)
	cout << "EXITING: clearProcBuffer" << endl;
#endif

	return status;

}


#define dbg 0

int control(/*int*/ char action, bool gate, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent)
{
	int targetParameterValueIndex = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: control" << endl;
	cout << "action: " << action << endl;
#endif

	static EnvGenContext envGenContext[20];
	static LfoContext lfoContext[20];

	if(action == 'l')
	{
		if(controlEvent->type == 0)
		{

		}
		else if(controlEvent->type == 1)
		{
			//int attack = controlEvent->parameter[0];
			//int peakValueIndex = 50;//controlEvent->parameter[4];

			/*controlEvent->controlContext = (EnvGenContext *)calloc(1, sizeof(EnvGenContext));
			if(controlEvent->controlContext == NULL)
			{
				std::cout << "EnvGenContext control calloc failed." << std::endl;
			}
			else
			{
				std::cout << "EnvGenContext control calloc succeeded." << std::endl;
				((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
			}*/
			controlEvent->controlTypeIndex = envGenCount;
			envGenCount++;
			envGenContext[controlEvent->controlTypeIndex].envStage = 0;
			envGenContext[controlEvent->controlTypeIndex].stageTimeValue = 0;
		}
		else if(controlEvent->type == 2)
		{
			/*controlEvent->controlContext = (LfoContext *)calloc(1, sizeof(LfoContext));
			if(controlEvent->controlContext == NULL)
			{
				std::cout << "LfoContext control calloc failed." << std::endl;
			}
			else
			{
				std::cout << "LfoContext control calloc succeeded." << std::endl;
				((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = 0;
			}*/
			controlEvent->controlTypeIndex = lfoCount;
			lfoCount++;
			lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = 0;
		}
	}
	else if(action == 'r')
	{

		if(controlEvent->type == 0)
		{
			controlEvent->int_output = controlEvent->parameter[0];  // direct transfer of parameter value
#if(dbg >= 2)
			//cout << "PARAM CONTROL: Normal" << endl;
#endif
		}
		else if(controlEvent->type == 1) // envelope generator
		{
			/*if(controlEvent->controlContext == NULL)
			{
				cout << "audioCallback control context not allocated" << endl;
				status = -1;
			}*/
			if(gate == true && controlEvent->envTriggerStatus == false) // pick detected
			{
				//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				//((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
				envGenContext[controlEvent->controlTypeIndex].envStage = 0;
				envGenContext[controlEvent->controlTypeIndex].stageTimeValue = 0;

#if(dbg >= 2)
				cout << "gate: true." << endl;

#endif
				controlEvent->envTriggerStatus = true;
			}
			else if(gate == false && controlEvent->envTriggerStatus == true) // string muted
			{
#if(dbg >= 2)
				cout << "gate: false." << endl;
#endif
				controlEvent->envTriggerStatus = false;
			}


			//int stageTimeValue = ((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue;
			//double slewRate = ((EnvGenContext *)(controlEvent->controlContext))->slewRate;
			int stageTimeValue = envGenContext[controlEvent->controlTypeIndex].stageTimeValue;
			double slewRate = envGenContext[controlEvent->controlTypeIndex].slewRate;
			int attack = controlEvent->parameter[0];
			int decay = controlEvent->parameter[1];
			//int sustain = controlEvent->parameter[2];
			//int release = controlEvent->parameter[3];
			int attackPeakValueIndex = 100;//controlEvent->parameter[4];
			int decayBottomValueIndex = 10;//controlEvent->parameter[5];
			//int internalOutput = 50*controlEvent->output;

			//switch(((EnvGenContext *)(controlEvent->controlContext))->envStage)
			switch(envGenContext[controlEvent->controlTypeIndex].envStage)
			{
			case 0: // idle
				controlEvent->output = 0.0;
				// pick detected, go to attack
				if(controlEvent->envTriggerStatus == true)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage++;
					envGenContext[controlEvent->controlTypeIndex].envStage++;
#if(dbg >= 3)
				cout << "case 0: output: " << controlEvent->output << endl;
#endif
					slewRate = 10.1 - envTime[attack];
#if(dbg >= 2)
					cout << "ATTACK" << endl;
#endif
				}
				break;
			case 1:	// attack
				if(controlEvent->output < double(attackPeakValueIndex))
				{
					controlEvent->output += slewRate;
#if(dbg >= 3)
				cout << "case 1: output: " << controlEvent->output << endl;
#endif
				}
				// peak reached, go to decay
				else if(/*controlEvent->output >= double(attackPeakValueIndex) && */controlEvent->envTriggerStatus == false)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 2;
					envGenContext[controlEvent->controlTypeIndex].envStage = 2;
					slewRate = 10.1 - envTime[decay];
#if(dbg >= 2)
					cout << "DECAY" << endl;
#endif
				}
				// string muted, go to idle
				/*else if(controlEvent->envTriggerStatus == false)
				{
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
					slewRate = 10.1 - envTime[sustain];
#if(dbg >= 2)
					cout << "RELEASE" << endl;
#endif
				}*/

				break;
			case 2:	// decay
#if(dbg >= 3)
				cout << "case 2: output: " << controlEvent->output << endl;
#endif
				// pick detected, go back to attack
				if(controlEvent->envTriggerStatus == true)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 1;
					envGenContext[controlEvent->controlTypeIndex].envStage = 1;
					slewRate = 10.1 - envTime[attack];
#if(dbg >= 2)
					cout << "ATTACK" << endl;
#endif
				}
				else if(controlEvent->output > double(decayBottomValueIndex))
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;//envStage++;
					controlEvent->output -= slewRate;
				}
				else // output is below decay bottom value
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;//envStage++;
					envGenContext[controlEvent->controlTypeIndex].envStage = 0;//envStage++;
#if(dbg >= 2)
					cout << "RELEASE" << endl;
#endif
				}
				// string muted, go to idle
				/*else if(controlEvent->envTriggerStatus == false)
				{
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
					//slewRate = 10.1 - envTime[sustain];
#if(dbg >= 2)
					cout << "RELEASE" << endl;
#endif
				}*/
				break;

			default:;
			}
			controlEvent->int_output  = (int)(controlEvent->output);

			if(controlEvent->int_output < 0) controlEvent->int_output = 0;
			if(controlEvent->int_output > 99) controlEvent->int_output = 99;



			//((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = stageTimeValue;
			//((EnvGenContext *)(controlEvent->controlContext))->slewRate = slewRate;
			envGenContext[controlEvent->controlTypeIndex].stageTimeValue = stageTimeValue;
			envGenContext[controlEvent->controlTypeIndex].slewRate = slewRate;
		}
		else if(controlEvent->type == 2) // LFO
		{
			/*if(controlEvent->controlContext == NULL)
			{
				cout << "audioCallback control context not allocated" << endl;
				status = -1;
			}*/

			//unsigned int cycleTimeValueIndex = ((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex;
			//double cyclePositionValue = ((LfoContext *)(controlEvent->controlContext))->cyclePositionValue;
			unsigned int cycleTimeValueIndex = lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex;
			double cyclePositionValue = lfoContext[controlEvent->controlTypeIndex].cyclePositionValue;
			int int_cyclePositionValue;
			unsigned int frequencyIndex = controlEvent->parameter[0];
			unsigned int amplitudeIndex = controlEvent->parameter[1];
			unsigned int offsetIndex = controlEvent->parameter[2];
			//double waveValue;
			//cout << "output: " << controlEvent->output << endl;

			if(cyclePositionValue >= double(99))
			{
				cyclePositionValue = 0.0;
			}
			else
			{
				cyclePositionValue += lfoFreq[frequencyIndex];
				int_cyclePositionValue = (unsigned int)cyclePositionValue;
				if(int_cyclePositionValue < 0) int_cyclePositionValue = 0;
				if(int_cyclePositionValue > 99) int_cyclePositionValue = 99;
				controlEvent->output = lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
			}


			controlEvent->int_output  = (unsigned int)(controlEvent->output);
			if(controlEvent->int_output > 99) controlEvent->int_output = 99;
			{
#if(dbg >= 2)
				cout << "PARAM CONTROL[" << controlEvent->name << "]: LFO: value index " << controlEvent->int_output << endl;
#endif
			}
			//((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = cycleTimeValueIndex;
			//((LfoContext *)(controlEvent->controlContext))->cyclePositionValue = cyclePositionValue;
			lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = cycleTimeValueIndex;
			lfoContext[controlEvent->controlTypeIndex].cyclePositionValue = cyclePositionValue;
		}

		for(int paramControlConnectionIndex = 0; paramControlConnectionIndex < controlEvent->paramContConnectionCount; paramControlConnectionIndex++)
		{
			int paramContProcessIndex = controlEvent->paramContConnection[paramControlConnectionIndex].processIndex;
			int paramContParameterIndex = controlEvent->paramContConnection[paramControlConnectionIndex].processParamIndex;

			procEvent[paramContProcessIndex].parameters[paramContParameterIndex] = controlEvent->int_output;
			/*cout << "procEvent[" << paramContProcessIndex << "].parameters[" << paramContParameterIndex << "]: " <<
					procEvent[paramContProcessIndex].parameters[paramContParameterIndex] << endl;*/
		}
	}
	else if(action == 'd')
	{

	}
	else if(action == 's')
	{

		if(controlEvent->type == 1 || controlEvent->type == 2)
		{
			/*if(controlEvent->controlContext == NULL || controlEvent == NULL)
			{
				std::cout << "controlContext missing." << std::endl;
			}
			else
			{
				std::cout << "freeing allocated controlContext." << std::endl;
				free(controlEvent->controlContext);
			}*/
		}
		if(controlEvent->type == 1) envGenCount--;
		if(controlEvent->type == 2) lfoCount--;
		targetParameterValueIndex = 0;

	}
	else
	{

	}

#if(dbg >= 1)
	cout << "***** EXITING: control: " << targetParameterValueIndex << endl;
#endif

	return targetParameterValueIndex;
}


void initBufferAveParameters(struct ProcessBuffer *bufferArray)
{
	bufferArray->offset = 0.00000;
	bufferArray->bufferSum = 0.00000;
	for(int i = 0; i < AVE_ARRAY_SIZE; i++)
	{
		bufferArray->aveArray[i] = 0.00000;
	}

	bufferArray->aveArrayIndex = 0;
}


void resetBufferAve(struct ProcessBuffer *bufferArray)
{
	bufferArray->bufferSum = 0.00000;
}


int processBufferAveSample(double sample, struct ProcessBuffer *bufferArray)
{
	int status = 0;

    if(bufferArray->aveArrayIndex == AVE_ARRAY_SIZE-1)
    {
    	bufferArray->aveArrayIndex = 0;
    }
    else
    {
    	bufferArray->aveArrayIndex += 1;
    }

    bufferArray->offset = bufferArray->aveArray[bufferArray->aveArrayIndex]/AVE_BUFFER_SIZE;

	return status;
}


void updateBufferOffset(struct ProcessBuffer *bufferArray)
{
	bufferArray->offset = bufferArray->aveArray[bufferArray->aveArrayIndex]/AVE_BUFFER_SIZE;
}


#define dbg 0

int delayb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	static DelayContext context[20];
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::delayb" << endl;
	cout << "action: " << action << endl;
#endif
	int status = 0;

	if(action == 'c')
	{
		componentVector.push_back(string(delaybSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processTypeIndex = delaybCount;//processContext = (DelayContext *)calloc(1, sizeof(DelayContext));
		delaybCount++;
		/*if(procEvent->processContext == NULL)
		{
			std::cout << "delayb calloc failed." << std::endl;
		}
		else
		{
			std::cout << "delayb calloc succeeded." << std::endl;
			((DelayContext *)(procEvent->processContext))->inputPtr = 0;
			((DelayContext *)(procEvent->processContext))->outputPtr = 0;
			((DelayContext *)(procEvent->processContext))->delayBuffer[0]=0.0;
		}*/
		context[procEvent->processTypeIndex].inputPtr = 0;
		context[procEvent->processTypeIndex].outputPtr = 0;
		for(unsigned int delayBufferIndex = 0; delayBufferIndex < DELAY_BUFFER_LENGTH; delayBufferIndex++)
		{
			context[procEvent->processTypeIndex].delayBuffer[delayBufferIndex]=0.0;
		}


		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		status = 0;
	}
	else if(action == 'r')
	{
		//double internalPosPeak = 0.0;
		//double internalNegPeak = 0.0;
		double outputSum = 0.00000;
		volatile double tempInput;
		volatile unsigned int delayCoarse = 0 ;
		volatile unsigned int delayFine = 0 ;
		volatile unsigned int delay = 0;

		//double gain, breakpoint, edge;

		/*if(procEvent->processContext == NULL)
		{
			cout << "audioCallback delayb context not allocated" << endl;
			return -1;
		}*/

		volatile unsigned int i;
		volatile unsigned int inputPtr = context[procEvent->processTypeIndex].inputPtr;
		volatile unsigned int outputPtr = context[procEvent->processTypeIndex].outputPtr;


		delayCoarse = delayTime[procEvent->parameters[0]];
		delayFine = delayTime[procEvent->parameters[1]]/100;
		delay = delayCoarse + delayFine;

		if(inputPtr >= delay) outputPtr = inputPtr - delay;
		else outputPtr = DELAY_BUFFER_LENGTH - (delay - inputPtr);

		if(inputPtr >= DELAY_BUFFER_LENGTH) inputPtr = 0;
		if(outputPtr >= DELAY_BUFFER_LENGTH) outputPtr = 0;

#if(dbg >= 2)
		cout << "procEvent->parameters[0]: " << procEvent->parameters[0];
		cout << "procEvent->parameters[1]: " << procEvent->parameters[1];
		cout << "delayCoarse: " << delayCoarse;
		cout << "delayFine: " << delayFine;
		cout << "delay: " << delay;
		cout << "inputPtr: " << inputPtr;
		cout << "outputPtr: " << outputPtr << endl;
#endif

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if(footswitchStatus[procEvent->footswitchNumber] == 0)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				if(inputPtr >= DELAY_BUFFER_LENGTH) inputPtr = 0;
				context[procEvent->processTypeIndex].delayBuffer[inputPtr++] = 0.0;
			}
			else
#endif
			{
				tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];

				context[procEvent->processTypeIndex].delayBuffer[inputPtr] = tempInput;

				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = context[procEvent->processTypeIndex].delayBuffer[outputPtr];

				if(inputPtr >= unsigned(DELAY_BUFFER_LENGTH)) inputPtr = 0;
				else inputPtr++;

				if(outputPtr >= unsigned(DELAY_BUFFER_LENGTH)) outputPtr = 0;
				else outputPtr++;
				//outputSum += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
			}
			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		context[procEvent->processTypeIndex].inputPtr = inputPtr;
		context[procEvent->processTypeIndex].outputPtr = outputPtr;

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		status = 0;
	}

	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{
		delaybCount--;
		/*if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "delayb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated delayb." << std::endl;
			//delete[] procEvent->processContext; //free(procEvent->processContext);
		}*/

		status = 0;
	}
	else
	{
		std::cout << "delayb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "***** EXITING: Effects2::delayb: " << status << endl;
#endif
	return status;
}

#define dbg 0
int filter3bb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	static Filter3bbContext context[20];
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::filter3bb" << endl;
	cout << "action: " << action << endl;
#endif


	if(action == 'c')
	{
		componentVector.push_back(string(filter3bbSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);

		/*procEvent->processContext = (double *)calloc(50, sizeof(double));
		if(procEvent->processContext == NULL)
		{
			std::cout << "filter3bb calloc failed." << std::endl;
		}
		else
		{
			std::cout << "filter3bb calloc succeeded." << std::endl;
			for(unsigned int i = 0; i < 50; i++)
			{
				((double *)procEvent->processContext)[i] = 0.00;
			}
		}*/

		procEvent->processTypeIndex = filter3bbCount;
		filter3bbCount++;

		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for(unsigned int i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for(unsigned int j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].lp_y[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_y[i][j] = 0.00000;
			}
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].noiseFilter_y[j] = 0.00000;
		}


		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		status = 0;

	}
	else if(action == 'r')
	{
		unsigned int i,j;
		unsigned int contextIndex = 0;
		//double internalPosPeak = 0.0;
		//double internalNegPeak = 0.0;
		double tempInput;
		double outputSum[3];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;
		outputSum[2] = 0.00000;

		double lp_a[NUMBER_OF_BANDS][4], lp_b[NUMBER_OF_BANDS][4];
		double hp_a[NUMBER_OF_BANDS][4], hp_b[NUMBER_OF_BANDS][4];
		ProcessBuffer procOutputBuffer[NUMBER_OF_BANDS]; // transfers data from HP output of lower band to LP input higher band

		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
		double couplingFilter_y[4], couplingFilter_x[4];
		double noiseFilter_y[4], noiseFilter_x[4];

		/*if(procEvent->processContext == NULL)
		{
			cout << "audioCallback filter3bb context not allocated" << endl;
			status = -1;
		}*/


//		for(j = 0; j < 3; j++)
//		{
//			couplingFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
//			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
//		}
//		for(i = 0; i < NUMBER_OF_BANDS; i++)
//		{
//			for(j = 0; j < 4; j++)
//			{
//				lp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
//				lp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
//				hp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++];
//				hp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++];
//			}
//		}
//		for(j = 0; j < 3; j++)
//		{
//			noiseFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
//			noiseFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
//		}

		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		for(i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for(j = 0; j < 4; j++)
			{
				lp_x[i][j] = context[procEvent->processTypeIndex].lp_x[i][j];
				lp_y[i][j] = context[procEvent->processTypeIndex].lp_y[i][j];
				hp_x[i][j] = context[procEvent->processTypeIndex].hp_x[i][j];
				hp_y[i][j] = context[procEvent->processTypeIndex].hp_y[i][j];
			}
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = context[procEvent->processTypeIndex].noiseFilter_x[j];
			noiseFilter_y[j] = context[procEvent->processTypeIndex].noiseFilter_y[j];
		}


#if(dbg == 1)
		cout << "filter3bb procEvent->parameters: ";
#endif

		//int tempIndex = valueIndex;
		for(i = 0; i < NUMBER_OF_BANDS; i++)
		{
			//if(i == 0)
			{
#if(dbg == 1)
				cout << procEvent->parameters[i] << ", ";
#endif
				lp_a[i][0] = lp[procEvent->parameters[i]][0];
				lp_a[i][1] = lp[procEvent->parameters[i]][1];
				lp_a[i][2] = lp[procEvent->parameters[i]][2];
				//lp_a[i][3] = lp2[procEvent->parameters[i]][3];
				//lp_a[i][4] = lp[tempIndex][4];
				lp_b[i][1] = lp[procEvent->parameters[i]][4];
				lp_b[i][2] = lp[procEvent->parameters[i]][5];
				//lp_b[i][3] = lp2[procEvent->parameters[i]][7];
				//lp_b[i][4] = lp[tempIndex][9];

				hp_a[i][0] = hp[procEvent->parameters[i]][0];
				hp_a[i][1] = hp[procEvent->parameters[i]][1];
				hp_a[i][2] = hp[procEvent->parameters[i]][2];
				//hp_a[i][3] = hp2[procEvent->parameters[i]][3];
				//hp_a[i][4] = hp[tempIndex][4];
				hp_b[i][1] = hp[procEvent->parameters[i]][4];
				hp_b[i][2] = hp[procEvent->parameters[i]][5];
				//hp_b[i][3] = hp2[procEvent->parameters[i]][7];
				//hp_b[i][4] = hp[tempIndex][9];
			}
		}
#if(dbg == 1)
		cout << endl;
#endif
		//footswitchStatus = 1;

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[2]]);

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if(footswitchStatus[procEvent->footswitchNumber] == 0)
		{
			for(i = 0; i < bufferSize; i++)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
				procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = 0.0000;
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = 0.0000;
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);
			}
		}
		else
#endif
		{
			for(i = 0; i < bufferSize; i++)
			{
				//for(j = 0; j < NUMBER_OF_BANDS; j++)
				{
					if(inputCouplingMode == 0)
					{
						tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
					}
					else if(inputCouplingMode == 1)
					{
						couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
						couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}
					else if(inputCouplingMode == 2)
					{
						tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					}

					/************************** FIRST BAND DIVIDER (LOW/MID) *************************/
					j = 0;
					{


						lp_x[j][0] = tempInput; // get inputBuffer[0] data for BD0 from process input
						hp_x[j][0] = tempInput; // get input data for BD0 from process input

						lp_y[j][0] = lp_a[j][0]*lp_x[j][0] + lp_a[j][1]*lp_x[j][1] + lp_a[j][2]*lp_x[j][2]/* + lp_a[j][3]*lp_x[j][3]*/ - lp_b[j][1]*lp_y[j][1] - lp_b[j][2]*lp_y[j][2]/* - lp_b[j][3]*lp_y[j][3]*/;
						procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[j][0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
						outputSum[0] += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
						//lp_x[j][3] = lp_x[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						//lp_y[j][3] = lp_y[j][2];
						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

						hp_y[j][0] = hp_a[j][0]*hp_x[j][0] + hp_a[j][1]*hp_x[j][1] + hp_a[j][2]*hp_x[j][2]/* + hp_a[j][3]*hp_x[j][3]*/ - hp_b[j][1]*hp_y[j][1] - hp_b[j][2]*hp_y[j][2];// - hp_b[j][3]*hp_y[j][3];

						//hp_x[j][4] = hp_x[j][3];
						//hp_x[j][3] = hp_x[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						//hp_y[j][4] = hp_y[j][3];
						//hp_y[j][3] = hp_y[j][2];
						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];
					}
					//else
					j = 1;
					/************************** SECOND BAND DIVIDER (MID/HIGH) *************************/
					{
						lp_x[j][0] = hp_y[0][0];
						lp_y[j][0] = lp_a[j][0]*lp_x[j][0] + lp_a[j][1]*lp_x[j][1] + lp_a[j][2]*lp_x[j][2]/* + lp_a[j][3]*lp_x[j][3]*/ - lp_b[j][1]*lp_y[j][1] - lp_b[j][2]*lp_y[j][2]/* - lp_b[j][3]*lp_y[j][3]*/;
						//lp_x[j][0] = procOutputBuffer[j-1].buffer[i];//hp_y[j-1][0]; // get input data for higher BDs from previous BD output

						procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = lp_y[j][0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
						outputSum[1] += procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i];

						//lp_x[j][3] = lp_x[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						//lp_y[j][3] = lp_y[j][2];
						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

						hp_x[j][0] = tempInput;
						hp_y[j][0] = hp_a[j][0]*hp_x[j][0] + hp_a[j][1]*hp_x[j][1] + hp_a[j][2]*hp_x[j][2]/* + hp_a[j][3]*hp_x[j][3]*/ - hp_b[j][1]*hp_y[j][1] - hp_b[j][2]*hp_y[j][2];// - hp_b[j][3]*hp_y[j][3];

						//hp_x[j][4] = hp_x[j][3];
						//hp_x[j][3] = hp_x[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						//hp_y[j][4] = hp_y[j][3];
						//hp_y[j][3] = hp_y[j][2];
						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];

						noiseFilter_x[0] = hp_y[j][0];
						noiseFilter_y[0] = 0.01870016*noiseFilter_x[0] + 0.00304999*noiseFilter_x[1] + 0.01870016*noiseFilter_x[2] - (-1.69729152)*noiseFilter_y[1] - 0.73774183*noiseFilter_y[2];

						procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = hp_y[j][0];
						//procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = noiseFilter_y[0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);

						noiseFilter_x[2] = noiseFilter_x[1];
						noiseFilter_x[1] = noiseFilter_x[0];

						noiseFilter_y[2] = noiseFilter_y[1];
						noiseFilter_y[1] = noiseFilter_y[0];
						//outputSum[2] += procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i];

					}
				}
			}
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[2]]);
		contextIndex = 0;


//		for(j = 0; j < 3; j++)
//		{
//			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
//			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
//		}
//		for(i = 0; i < NUMBER_OF_BANDS; i++)
//		{
//			for(j = 0; j < 4; j++)
//			{
//				((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = lp_x[i][j];
//				//std::cout << "end: lp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(15) << procEvent->processContext[i*8+j*4] << std::endl;
//				((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = lp_y[i][j];
//				//std::cout << "end: lp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << lp_y[i][j] << std::endl;
//				((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++] = hp_x[i][j];
//				//std::cout << "end: hp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_x[i][j] << std::endl;
//				((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++] = hp_y[i][j];
//				//std::cout << "end: hp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_y[i][j] << std::endl;
//			}
//		}
//		for(j = 0; j < 3; j++)
//		{
//			 ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = noiseFilter_x[j];
//			 ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = noiseFilter_y[j];
//		}

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] = couplingFilter_y[j];
		}
		for(i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for(j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = lp_x[i][j];
				context[procEvent->processTypeIndex].lp_y[i][j] = lp_y[i][j];
				context[procEvent->processTypeIndex].hp_x[i][j] = hp_x[i][j];
				context[procEvent->processTypeIndex].hp_y[i][j] = hp_y[i][j];
			}
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = noiseFilter_x[j];
			context[procEvent->processTypeIndex].noiseFilter_y[j] = noiseFilter_y[j];
		}

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[2]].ready = 1;
		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{
		/*if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "filter3bb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated filter3bb." << std::endl;
			free(procEvent->processContext);
		}*/
		filter3bbCount--;
		status = 0;
	}
	else
	{
		std::cout << "filter3bb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "***** EXITING: Effects2::filter3bb: " << status << endl;
#endif
	return status;
}


#define dbg 0
int filter3bb2(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	static Filter3bb2Context context[20];
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::filter3bb2" << endl;
	cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(string(filter3bb2Symbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);

		/*procEvent->processContext = (double *)calloc(50, sizeof(double));
		if(procEvent->processContext == NULL)
		{
			std::cout << "filter3bb2 calloc failed." << std::endl;
		}
		else
		{
			std::cout << "filter3bb2 calloc succeeded." << std::endl;
			for(unsigned int i = 0; i < 50; i++)
			{
				((double *)procEvent->processContext)[i] = 0.00;
			}
		}*/
		procEvent->processTypeIndex = filter3bb2Count;
		filter3bb2Count++;
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].lp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].lp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 5; j++)
		{
			context[procEvent->processTypeIndex].bp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].bp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].hp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].hp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].noiseFilter_y[j] = 0.00000;
		}

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		status = 0;
	}
	else if(action == 'r')
	{
		unsigned int i,j;
		unsigned int contextIndex = 0;
		//double internalPosPeak = 0.0;
		//double internalNegPeak = 0.0;
		double tempInput;
		double outputSum[3];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;
		outputSum[2] = 0.00000;

		double lp_a[4], lp_b[4];
		double bp_a[5], bp_b[5];
		double hp_a[4], hp_b[4];
		//ProcessBuffer procOutputBuffer[NUMBER_OF_BANDS]; // transfers data from HP output of lower band to LP input higher band

		double lp_y[4], lp_x[4]; // needs to be static to retain data from previous processing
		double bp_y[5], bp_x[5]; // needs to be static to retain data from previous processing
		double hp_y[4], hp_x[4]; // needs to be static to retain data from previous processing
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];

		/*if(procEvent->processContext == NULL)
		{
			cout << "audioCallback filter3bb2 context not allocated" << endl;
			status = -1;
		}*/

		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			lp_x[j] = context[procEvent->processTypeIndex].lp_x[j];
			lp_y[j] = context[procEvent->processTypeIndex].lp_y[j];
		}
		for(j = 0; j < 5; j++)
		{
			bp_x[j] = context[procEvent->processTypeIndex].bp_x[j];
			bp_y[j] = context[procEvent->processTypeIndex].bp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			hp_x[j] = context[procEvent->processTypeIndex].hp_x[j];
			hp_y[j] = context[procEvent->processTypeIndex].hp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = context[procEvent->processTypeIndex].noiseFilter_x[j];
			noiseFilter_y[j] = context[procEvent->processTypeIndex].noiseFilter_y[j];
		}


#if(dbg == 1)
		cout << "filter3bb2 procEvent->parameters: ";
#endif

#if(dbg == 1)
				cout << procEvent->parameters[i] << ", ";
#endif
		unsigned int tempIndex = procEvent->parameters[0];
		lp_a[0] = lp2[tempIndex][0];
		lp_a[1] = lp2[tempIndex][1];
		lp_a[2] = lp2[tempIndex][2];
		lp_b[1] = lp2[tempIndex][4];
		lp_b[2] = lp2[tempIndex][5];

		bp_a[0] = bp2[tempIndex][0];
		bp_a[1] = bp2[tempIndex][1];
		bp_a[2] = bp2[tempIndex][2];
		bp_a[3] = bp2[tempIndex][3];
		bp_a[4] = bp2[tempIndex][4];
		bp_b[1] = bp2[tempIndex][6];
		bp_b[2] = bp2[tempIndex][7];
		bp_b[3] = bp2[tempIndex][8];
		bp_b[4] = bp2[tempIndex][9];

		hp_a[0] = hp2[tempIndex][0];
		hp_a[1] = hp2[tempIndex][1];
		hp_a[2] = hp2[tempIndex][2];
		hp_b[1] = hp2[tempIndex][4];
		hp_b[2] = hp2[tempIndex][5];



#if(dbg == 1)
		cout << endl;
#endif
		//footswitchStatus = 1;

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[2]]);

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if(footswitchStatus[procEvent->footswitchNumber] == 0)
		{
			for(i = 0; i < bufferSize; i++)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
				procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = 0.0000;
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = 0.0000;
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);
			}
		}
		else
#endif
		{
			for(i = 0; i < bufferSize; i++)
			{

				if(inputCouplingMode == 0)
				{
					tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
				}
				else if(inputCouplingMode == 1)
				{
					couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];

					tempInput = couplingFilter_y[0];

					couplingFilter_x[2] = couplingFilter_x[1];
					couplingFilter_x[1] = couplingFilter_x[0];

					couplingFilter_y[2] = couplingFilter_y[1];
					couplingFilter_y[1] = couplingFilter_y[0];
				}
				else if(inputCouplingMode == 2)
				{
					tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}

					lp_x[0] = tempInput;
					bp_x[0] = tempInput;
					hp_x[0] = tempInput;
				//*********************Band Divider (BD): Lowpass filter ********************

				lp_y[0] = lp_a[0]*lp_x[0] + lp_a[1]*lp_x[1] + lp_a[2]*lp_x[2] - lp_b[1]*lp_y[1] - lp_b[2]*lp_y[2];

				lp_x[2] = lp_x[1];
				lp_x[1] = lp_x[0];
				lp_y[2] = lp_y[1];
				lp_y[1] = lp_y[0];


				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[0];
				//*********************** Bandpass filter ****************************
				bp_y[0] = bp_a[0]*bp_x[0] + bp_a[1]*bp_x[1] + bp_a[2]*bp_x[2] + bp_a[3]*bp_x[3] + bp_a[4]*bp_x[4] - bp_b[1]*bp_y[1] - bp_b[2]*bp_y[2] - bp_b[3]*bp_y[3] - bp_b[4]*bp_y[4];
				procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = bp_y[0];

				bp_x[4] = bp_x[3];
				bp_x[3] = bp_x[2];
				bp_x[2] = bp_x[1];
				bp_x[1] = bp_x[0];

				bp_y[4] = bp_y[3];
				bp_y[3] = bp_y[2];
				bp_y[2] = bp_y[1];
				bp_y[1] = bp_y[0];

				//*********************Band Divider (BD): Highpass filter ********************
				//hp_x[0] = inputBuffer[0][i];

				hp_y[0] = hp_a[0]*hp_x[0] + hp_a[1]*hp_x[1] + hp_a[2]*hp_x[2] - hp_b[1]*hp_y[1] - hp_b[2]*hp_y[2];

				hp_x[2] = hp_x[1];
				hp_x[1] = hp_x[0];
				hp_y[2] = hp_y[1];
				hp_y[1] = hp_y[0];



				//procOutputBuffer.buffer[i] = hp_y[0];
				// j = 0 case is processed above

				procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = hp_y[0];


				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);

			}

		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[2]]);


		/*if(procEvent->processContext == NULL) return -1;*/

		contextIndex = 0;

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] = couplingFilter_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].lp_x[j] = lp_x[j];
			context[procEvent->processTypeIndex].lp_y[j] = lp_y[j];
		}
		for(j = 0; j < 5; j++)
		{
			context[procEvent->processTypeIndex].bp_x[j] = bp_x[j];
			context[procEvent->processTypeIndex].bp_y[j] = bp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].hp_x[j] = hp_x[j];
			context[procEvent->processTypeIndex].hp_y[j] = hp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = noiseFilter_x[j];
			context[procEvent->processTypeIndex].noiseFilter_y[j] = noiseFilter_y[j];
		}



		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[2]].ready = 1;
		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{
		/*if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "filter3bb2 processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated filter3bb2." << std::endl;
			free(procEvent->processContext);
		}*/
		filter3bb2Count--;
		status = 0;
	}
	else
	{
		std::cout << "filter3bb2: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	cout << "***** EXITING: Effects2::filter3bb2: " << status << endl;
#endif
	return status;

}


#define dbg 0
int lohifilterb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;
	static LohifilterbContext context[20];
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::lohifilterb" << endl;
	cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(string(lohifilterbSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);

		/*procEvent->processContext = (double *)calloc(50, sizeof(double));
		if(procEvent->processContext == NULL)
		{
			std::cout << "lohifilterb calloc failed." << std::endl;
		}
		else
		{
			std::cout << "lohifilterb calloc succeeded." << std::endl;
			for(i = 0; i < 50; i++)
			{
				((double *)procEvent->processContext)[i] = 0.00;
			}
		}*/
		procEvent->processTypeIndex = lohifilterbCount;
		lohifilterbCount++;

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		i = 0;
		{
			for(j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].lp_y[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_y[i][j] = 0.00000;
			}
		}

		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}


		status = 0;
	}
	else if(action == 'r')
	{

		int contextIndex = 0;
		double lp_a[NUMBER_OF_BANDS][4], lp_b[NUMBER_OF_BANDS][4];
		double hp_a[NUMBER_OF_BANDS][4], hp_b[NUMBER_OF_BANDS][4];

		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];
		double tempInput;
		double outputSum[2];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;


		/*if(procEvent->processContext == NULL)
		{
			cout << "audioCallback lohifilterb context not allocated" << endl;
			status = -1;
		}*/
		//for(int i = 0; i < 1; i++)
//		for(j = 0; j < 3; j++)
//		{
//			couplingFilter_x[j] = context[procEvent->processTypeIndex];
//			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
//		}
//		i = 0;
//		{
//			for(j = 0; j < 4; j++)
//			{
//				lp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
//				//std::cout << "start: lp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(15) << procEvent->processContext[i*8+j*4] << std::endl;
//				lp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
//				//std::cout << "start: lp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << lp_y[i][j] << std::endl;
//				hp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++];
//				//std::cout << "start: hp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_x[i][j] << std::endl;
//				hp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++];
//				//std::cout << "start: hp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_y[i][j] << std::endl;
//			}
//		}
		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		i = 0;
		{
			for(j = 0; j < 4; j++)
			{
				lp_x[i][j] = context[procEvent->processTypeIndex].lp_x[i][j];
				lp_y[i][j] = context[procEvent->processTypeIndex].lp_y[i][j];
				hp_x[i][j] = context[procEvent->processTypeIndex].hp_x[i][j];
				hp_y[i][j] = context[procEvent->processTypeIndex].hp_y[i][j];
			}
		}


		int tempIndex = procEvent->parameters[0];

		//for(i = 0; i < NUMBER_OF_BANDS; i++)
#if(dbg == 1)
		cout << "lohifilterb procEvent->parameters: ";
#endif

		i = 0;
		{
#if(dbg == 1)
				cout << procEvent->parameters[i] << ", ";
#endif
			lp_a[i][0] = lp3[tempIndex][0];
			lp_a[i][1] = lp3[tempIndex][1];
			lp_a[i][2] = lp3[tempIndex][2];
			lp_a[i][3] = lp3[tempIndex][3];
			//lp_a[i][4] = lp[tempIndex][4];
			lp_b[i][1] = lp3[tempIndex][5];
			lp_b[i][2] = lp3[tempIndex][6];
			lp_b[i][3] = lp3[tempIndex][7];
			//lp_b[i][4] = lp[tempIndex][9];

			hp_a[i][0] = hp3[tempIndex][0];
			hp_a[i][1] = hp3[tempIndex][1];
			hp_a[i][2] = hp3[tempIndex][2];
			hp_a[i][3] = hp3[tempIndex][3];
			//hp_a[i][4] = hp[tempIndex][4];
			hp_b[i][1] = hp3[tempIndex][5];
			hp_b[i][2] = hp3[tempIndex][6];
			hp_b[i][3] = hp3[tempIndex][7];
			//hp_b[i][4] = hp[tempIndex][9];
		}
#if(dbg == 1)
		cout << endl;
#endif

		//for(j = 0; j < NUMBER_OF_BANDS; j++)
		j = 0;
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);

			for(i = 0; i < bufferSize; i++)
			{
#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == 0)//(input[i] > 0.00000)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
					procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = 0.00;
					processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				}
			    else
#endif
			    {
					if(inputCouplingMode == 0)
					{
						tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
					}
					else if(inputCouplingMode == 1)
					{
						couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
						couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}
					else if(inputCouplingMode == 2)
					{
						tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					}

					/********************* Lowpass filter ********************/
					lp_x[j][0] = tempInput;

					lp_y[j][0] = lp_a[j][0]*lp_x[j][0] + lp_a[j][1]*lp_x[j][1] + lp_a[j][2]*lp_x[j][2] + lp_a[j][3]*lp_x[j][3] - lp_b[j][1]*lp_y[j][1] - lp_b[j][2]*lp_y[j][2] - lp_b[j][3]*lp_y[j][3];

					if(j == 0)
					{
						procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[j][0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
					}

					//lp_x[j][4] = lp_x[j][3];
					lp_x[j][3] = lp_x[j][2];
					lp_x[j][2] = lp_x[j][1];
					lp_x[j][1] = lp_x[j][0];

					//lp_y[j][4] = lp_y[j][3];
					lp_y[j][3] = lp_y[j][2];
					lp_y[j][2] = lp_y[j][1];
					lp_y[j][1] = lp_y[j][0];

					//low_output[i] = input[i];
					/********************* Highpass filter ********************/
					hp_x[j][0] = tempInput;

					hp_y[j][0] = hp_a[j][0]*hp_x[j][0] + hp_a[j][1]*hp_x[j][1] + hp_a[j][2]*hp_x[j][2] + hp_a[j][3]*hp_x[j][3] - hp_b[j][1]*hp_y[j][1] - hp_b[j][2]*hp_y[j][2] - hp_b[j][3]*hp_y[j][3];

					if(j == 0)
					{
						procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = hp_y[j][0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
					}

					hp_x[j][3] = hp_x[j][2];
					hp_x[j][2] = hp_x[j][1];
					hp_x[j][1] = hp_x[j][0];

					hp_y[j][3] = hp_y[j][2];
					hp_y[j][2] = hp_y[j][1];
					hp_y[j][1] = hp_y[j][0];
					//high_output[i] = input[i];
			    }
			}
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);

		contextIndex = 0;
		/*procEvent->internalData[0] = procBufferArray[procEvent->outputBufferIndexes[0]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[0]].ampNegativePeak;
		procEvent->internalData[1] = procBufferArray[procEvent->outputBufferIndexes[1]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[1]].ampNegativePeak;*/

		//for(int i = 0; i < NUMBER_OF_BANDS; i++)
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] = couplingFilter_y[j];
		}
		i = 0;
		{
			for(j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = lp_x[i][j];
				context[procEvent->processTypeIndex].lp_y[i][j] = lp_y[i][j];
				context[procEvent->processTypeIndex].hp_x[i][j] = hp_x[i][j];
				context[procEvent->processTypeIndex].hp_y[i][j] = hp_y[i][j];
			}
		}

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;

		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{
		/*if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "lohifilterb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated lohifilterb." << std::endl;
			free(procEvent->processContext);
		}*/
		lohifilterbCount--;
		status = 0;
	}
	else
	{
		std::cout << "lohifilterb: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	cout << "***** EXITING: Effects2::lohifilterb: " << status << endl;
#endif
	return status;
}

int mixerb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::mixerb" << endl;
	cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(string(mixerbSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		status = 0;
	}
	else if(action == 'r')
	{
		double level1 = logAmp[procEvent->parameters[0]];
		double level2 =  logAmp[procEvent->parameters[1]];
		double level3 =  logAmp[procEvent->parameters[2]];
		double levelOut =  logAmp[procEvent->parameters[3]];
		double outputSum = 0.00000;

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		for(i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if(footswitchStatus[procEvent->footswitchNumber] == 0)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
			}
			else
#endif
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = levelOut*(level1*(procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i]) +
						level2*(procBufferArray[procEvent->inputBufferIndexes[1]].buffer[i]) +
						level3*(procBufferArray[procEvent->inputBufferIndexes[2]].buffer[i]));
			}

			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}
		//procBufferArray[procEvent->outputBufferIndexes[0]].average = outputSum/bufferSize;

		//getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;


		/*procEvent->internalData[0] = procBufferArray[procEvent->outputBufferIndexes[0]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[0]].ampNegativePeak;*/

		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{

		status = 0;
	}
	else
	{
		std::cout << "mixerb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "***** EXITING: Effects2::mixerb: " << status << endl;
#endif
	return status;
}

int volumeb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::volumeb" << endl;
	cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(string(volumebSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		status = 0;
	}
	else if(action == 'r')
	{
		double vol = logAmp[procEvent->parameters[0]];
		double outputSum = 0.00000;

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if(footswitchStatus[procEvent->footswitchNumber] == 0)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
			}
			else
#endif
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = vol*(procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset);
				//outputSum += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
			}
			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;

		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{


		status = 0;
	}
	else
	{
		std::cout << "volumeb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "***** EXITING: Effects2::volumeb: " << status << endl;
#endif
	return status;
}

int waveshaperb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;

	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: Effects2::waveshaperb" << endl;
	cout << "action: " << action << endl;
#endif

	const double r1 = 1000.0;
	const double r2 = 2000.0;
	const double r3 = 500.0;
	const double r4 = 120.0;
	const double r5 = 50.0;
	static WaveshaperbContext context[20];

	if(action == 'c')
	{
		componentVector.push_back(string(waveshaperbSymbol));
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);


		procEvent->processTypeIndex = waveshaperbCount;
		waveshaperbCount++;

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.0000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.0000;
		}
		context[procEvent->processTypeIndex].outMeasure = 0.0000; // outMeasure

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = 0.0000;
			context[procEvent->processTypeIndex].noiseFilter_y[j] = 0.0000;
		}

		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		status = 0;
	}
	else if(action == 'r')
	{
		double k[5][5];
		double v[4];
		double x[6],y[6];
		double m[5],b[5];
		double tempInput;
		double distAveArray[4];
		double distAve;
		int contextIndex = 0;
		double outMeasure;
		double outputSum = 0.00000;
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];



		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		outMeasure = context[procEvent->processTypeIndex].outMeasure;
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = context[procEvent->processTypeIndex].noiseFilter_x[j];
			noiseFilter_y[j] = context[procEvent->processTypeIndex].noiseFilter_y[j];
		}

		if(waveshaperMode == 0)
		{
			k[4][0] = r2*r3*r4*r5;
			k[4][1] = r1*r3*r4*r5;
			k[4][2] = r1*r2*r4*r5;
			k[4][3] = r1*r2*r3*r5;
			k[4][4] = r1*r2*r3*r4;

			k[3][0] = r2*r3*r4;
			k[3][1] = r1*r3*r4;
			k[3][2] = r1*r2*r4;
			k[3][3] = r1*r2*r3;
			k[3][4] = 0.000;

			k[2][0] = r2*r3;
			k[2][1] = r1*r3;
			k[2][2] = r1*r2;
			k[2][3] = 0.000;
			k[2][4] = 0.000;

			k[1][0] = r2;
			k[1][1] = r2;
			k[1][2] = 0.000;
			k[1][3] = 0.000;
			k[1][4] = 0.000;

			k[0][0] = 1.000;
			k[0][1] = 0.000;
			k[0][2] = 0.000;
			k[0][3] = 0.000;
			k[0][4] = 0.000;

		}
		else
		{
			for(j = 0; j < 6; j++)
			{
				x[j] = context[procEvent->processTypeIndex].x[j];
				y[j] = context[procEvent->processTypeIndex].y[j];
			}
			for(j = 0; j < 5; j++)
			{
			    m[j] = (y[j+1]-y[j])/(x[j+1]-y[j]);
			    b[j] = y[j]-m[j]*x[j];
			    cout << "m[" << j << "] = " << m[j] << "\tb[" << j << "] = " << b[j] << endl;
				m[j] = context[procEvent->processTypeIndex].m[j];
				b[j] = context[procEvent->processTypeIndex].b[j];
			}
		}


		int edge = procEvent->parameters[1];
		v[0] = brkpt[edge][0];
		v[1] = brkpt[edge][1];
		v[2] = brkpt[edge][2];
		v[3] = brkpt[edge][3];

		double gain = logAmp[procEvent->parameters[0]]*5.0;
		double mix = linAmp[procEvent->parameters[2]]*0.1;
		double out = logAmp[procEvent->parameters[3]]*0.1;
		double k1,k2,k3,k4,k5;
		double v1,v2,v3,v4;
		double clean, dist;

		int kIndex = 0;
		//double out[2];
		//static double lastOut;
		//double doubleIndex = 0.0;
		//int dataIndex = 0;

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if(footswitchStatus[procEvent->footswitchNumber] == 0)//(input[i] > 0.00000)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
			}
			else
#endif
			{
				if(inputCouplingMode == 0)
				{
					tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
				}
				else if(inputCouplingMode == 1)
				{
					couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];

					tempInput = couplingFilter_y[0];

					couplingFilter_x[2] = couplingFilter_x[1];
					couplingFilter_x[1] = couplingFilter_x[0];

					couplingFilter_y[2] = couplingFilter_y[1];
					couplingFilter_y[1] = couplingFilter_y[0];
				}
				else if(inputCouplingMode == 2)
				{
					tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}

				if(waveshaperMode == 0)
				{
			        if(0.000 <= tempInput)
					{
			        	v1 = v[0];
			            v2 = v[1];
			            v3 = v[2];
			            v4 = v[3];
			       		while(true)
			            {
			                k1 = k[kIndex][0];
			                k2 = k[kIndex][1];
			                k3 = k[kIndex][2];
			                k4 = k[kIndex][3];
			                k5 = k[kIndex][4];
			                dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5)/(k1 + k2 + k3 + k4 + k5);

			                if(kIndex == 0)
			                {
			                    if(dist > v[kIndex])
			                        kIndex++;
			                    else
			                        break;
			                }
			                else if(kIndex == 4)
			                {
			                    if(dist < v[kIndex-1])
			                        kIndex--;
			                    else
			                        break;
			                }
			                else
			                {
			                    if(dist > v[kIndex])
			                        kIndex++;
			                    else if(dist < v[kIndex-1])
			                        kIndex--;
			                    else
			                        break;
			                }
			            }
					}
			        else
			        {
			            v1 = -v[0];
			            v2 = -v[1];
			            v3 = -v[2];
			            v4 = -v[3];
			            while(true)
			            {
			                k1 = k[kIndex][0];
			                k2 = k[kIndex][1];
			                k3 = k[kIndex][2];
			                k4 = k[kIndex][3];
			                k5 = k[kIndex][4];

			                dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5)/(k1 + k2 + k3 + k4 + k5);
			                if(kIndex == 0)
			                {
			                    if(dist < -v[kIndex])
			                        kIndex++;
			                    else
			                        break;
			                }
			                else if(kIndex == 4)
			                {
			                    if(dist > -v[kIndex-1])
			                        kIndex--;
			                    else
			                        break;
			                }
			                else
			                {
			                    if(dist < -v[kIndex])
			                        kIndex++;
			                    else if(dist > -v[kIndex-1])
			                        kIndex--;
			                    else
			                        break;
			                }
			            }
			        }
				}
				else
				{
			        if(tempInput*gain >= 0.0000)
			        {
			            if(tempInput*gain <= x[1])
			            	dist = tempInput*gain*m[0] + b[0];
			            else if(tempInput*gain <= x[2])
			            	dist = tempInput*gain*m[1] + b[1];
			            else if(tempInput*gain <= x[3])
			            	dist = tempInput*gain*m[2] + b[2];
			            else if(tempInput*gain <= x[4])
			            	dist = tempInput*gain*m[3] + b[3];
			            else
			            	dist = tempInput*gain*m[4] + b[4];
			        }
			        else
			        {
						if(tempInput*gain >= -x[1])
							dist = tempInput*gain*m[0] - b[0];
						else if(tempInput*gain >= -x[2])
							dist = tempInput*gain*m[1] - b[1];
						else if(tempInput*gain >= -x[3])
							dist = tempInput*gain*m[2] - b[2];
						else if(tempInput*gain >= -x[4])
							dist = tempInput*gain*m[3] - b[3];
						else
							dist = tempInput*gain*m[4] - b[4];
			        }
				}

				clean = tempInput;

				//procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = (1.0 - mix)*clean + mix*dist;
				noiseFilter_x[0] = /*out**/((1.0 - mix)*clean + mix*dist);
				noiseFilter_y[0] = 0.01870016*noiseFilter_x[0] + 0.00304999*noiseFilter_x[1] + 0.01870016*noiseFilter_x[2] - (-1.69729152)*noiseFilter_y[1] - 0.73774183*noiseFilter_y[2];
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = noiseFilter_y[0];
				noiseFilter_x[2] = noiseFilter_x[1];
				noiseFilter_x[1] = noiseFilter_x[0];

				noiseFilter_y[2] = noiseFilter_y[1];
				noiseFilter_y[1] = noiseFilter_y[0];
			}

			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);


		contextIndex = 0;

//		for(j = 0; j < 3; j++)
//		{
//			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
//			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
//		}
//		((double *)procEvent->processContext)[contextIndex++] = outMeasure; // outMeasure
//
//		for(j = 0; j < 3; j++)
//		{
//			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_x[j];
//			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_y[j];
//		}

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] = couplingFilter_y[j];
		}
		context[procEvent->processTypeIndex].outMeasure = outMeasure; // outMeasure

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] = noiseFilter_x[j];
			context[procEvent->processTypeIndex].noiseFilter_y[j] = noiseFilter_y[j];
		}

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;

		status = 0;
	}
	else if(action == 'd')
	{
		status = 0;
	}
	else if(action == 's')
	{
		/*if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "waveshaperb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated waveshaperb." << std::endl;
			free(procEvent->processContext); // for some reason, this causes a SIGABRT after a third combo selection
		}*/
		waveshaperbCount--;
		status = 0;
	}
	else
	{
		std::cout << "waveshaperb: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	cout << "***** EXITING: Effects2::waveshaperb: " << status << endl;
#endif
	return status;
}



