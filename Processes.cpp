/*
 * Processes.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */
#include "Processes.h"

#include "config.h"
#include "valueArrays.h"
#include "ComponentSymbols.h"
#include "PedalUtilityData.h"
extern int delayFineDivisor;


#define NUMBER_OF_BANDS 2
#define CONTROL_VALUE_INDEX_MAX 100
#define AVE_BUFFER_SIZE 16
#define DIST_AVE_BUFFER_SIZE 3
#define FOOTSWITCH_ALWAYS_ON 0
#define AVERAGE_OFFSET_ON 1
#define COUPLING_FILTER_ON 0
#define NOISE_FILTER_ON 1


/* actions:
 * 		0: load the ProcessEvent data
 * 		1: run the process
 * 		2: read data from process??
 * 		3: stop the process
 *
 */

extern PedalUtilityData pedalUtilityData;
extern bool debugOutput;
extern vector<string> componentVector;
extern vector<string> controlTypeVector;

#define dbg 0


#define dbg 0
int clearProcBuffer(struct ProcessBuffer *procBuffer)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: clearProcBuffer" << endl;
	if(debugOutput) cout << "procBuffer name: " << procBuffer->processName << endl;
#endif

#if(dbg >= 2)
#endif

#if(dbg >= 2)
	if(debugOutput) cout << "clearing: " << procBuffer->processName << ":" << procBuffer->portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		if(debugOutput) cout << procBuffer->buffer[i] << ",";
	}
	if(debugOutput) cout << endl;
#endif

	for(unsigned int i = 0; i < BUFFER_SIZE; i++)
	{
		procBuffer->buffer[i] = 0.000;
	}

#if(dbg >= 2)
	if(debugOutput) cout << "cleared: " << procBuffer->processName << ":" << procBuffer->portName;
	for(unsigned int i = 0; i < 20; i++)
	{
		if(debugOutput) cout << procBuffer->buffer[i] << ",";
	}
	if(debugOutput) cout << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: clearProcBuffer" << endl;
#endif

	return status;

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
extern int signalMarkCounter;
int delayTimingTestCount = 0;
int delayb( char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	static DelayContext context[10];
	static int bufferSize = 256;
	int status = 0;

	if(action == 'c')
	{
		componentVector.push_back(delaybSymbol);
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
#if(dbg >= 1)
		if(debugOutput) cout << "***** loading delayb: " << procEvent->processTypeIndex << endl;
#endif
		context[procEvent->processTypeIndex].inputPtr = 0;
		context[procEvent->processTypeIndex].outputPtr = 0;
		for(unsigned int delayBufferIndex = 0; delayBufferIndex < DELAY_BUFFER_LENGTH+10; delayBufferIndex++)
		{
			context[procEvent->processTypeIndex].delayBuffer[delayBufferIndex]=0.0;
		}

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[0] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[1] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[2] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[3] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[4] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[5] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[6] = 0.000;
		context[procEvent->processTypeIndex].delaySignalAveragingBuffer[7] = 0.000;
		context[procEvent->processTypeIndex].previousDelay = 0;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0] = 0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1] = 0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2] = 0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3] = 0.000;
		bufferSize = pedalUtilityData.getBufferSize();
		status = 0;
	}
	else if(action == 'r')
	{
		double outputSum = 0.00000;
		double tempInput = 0;
		unsigned int delayCoarse = 0;
		unsigned int delayFine = 0;
		unsigned int delay = 0;


		unsigned int i;
		unsigned int inputPtr = context[procEvent->processTypeIndex].inputPtr;
		unsigned int outputPtr = context[procEvent->processTypeIndex].outputPtr;


		delayCoarse = delayTimeCoarse[procEvent->parameters[0]];
		delayFine = delayTimeFine[procEvent->parameters[1]];

		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3] = context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2] = context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1] = context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0] = delayFine;

		unsigned int delayFineAveraged =
				(context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0] +
				context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1] +
				context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2] +
				context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3])/4;

		delay = (delayCoarse + delayFineAveraged);


		if(inputPtr >= delay) outputPtr = inputPtr - delay;
		else outputPtr = DELAY_BUFFER_LENGTH - (delay - inputPtr);

		if(inputPtr >= DELAY_BUFFER_LENGTH) inputPtr = 0;
		if(outputPtr >= DELAY_BUFFER_LENGTH) outputPtr = 0;

#if(dbg >= 2)
		if(debugOutput) cout << "procEvent->parameters[0]: " << procEvent->parameters[0];
		if(debugOutput) cout << "procEvent->parameters[1]: " << procEvent->parameters[1];
		if(debugOutput) cout << "delayCoarse: " << delayCoarse;
		if(debugOutput) cout << "delayFine: " << delayFine;
		if(debugOutput) cout << "delay: " << delay;
		if(debugOutput) cout << "inputPtr: " << inputPtr;
		if(debugOutput) cout << "outputPtr: " << outputPtr << endl;
#endif

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		unsigned int extraSkipCount = 0;
		for(i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if(footswitchStatus[procEvent->footswitchNumber] == false)
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

				if(inputPtr >= (DELAY_BUFFER_LENGTH)) inputPtr = 0;
				else inputPtr++;

				if(outputPtr >= (DELAY_BUFFER_LENGTH)) outputPtr = 0;
				else
				{
					outputPtr++;
				}
			}
			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		context[procEvent->processTypeIndex].inputPtr = inputPtr;
		context[procEvent->processTypeIndex].outputPtr = outputPtr;
		context[procEvent->processTypeIndex].previousDelay = delayFineAveraged;
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
		if(debugOutput) std::cout << "delayb: invalid actiond: " << action << std::endl;
		status = 0;
	}

	return status;
}

#define dbg 0
int filter3bb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	static Filter3bbContext context[20];
	int status = 0;
	static int bufferSize = 256;
	static int inputCouplingMode = 1;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::filter3bb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif


	if(action == 'c')
	{
		componentVector.push_back(filter3bbSymbol);
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);


#if(dbg >= 0)
		if(debugOutput) cout << "***** loading filter3bb: " << procEvent->processTypeIndex << endl;
#endif

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
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = 0.00000;
		}


		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		bufferSize = pedalUtilityData.getBufferSize();
		inputCouplingMode = pedalUtilityData.getInputCouplingMode();
		status = 0;

	}
	else if(action == 'r')
	{
		unsigned int i,j;
		unsigned int contextIndex = 0;
		double tempInput;
		double outputSum[3];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;
		outputSum[2] = 0.00000;

		double lp_b[NUMBER_OF_BANDS][4], lp_a[NUMBER_OF_BANDS][4];
		double hp_b[NUMBER_OF_BANDS][4], hp_a[NUMBER_OF_BANDS][4];
		ProcessBuffer procOutputBuffer[NUMBER_OF_BANDS]; // transfers data from HP output of lower band to LP input higher band

		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
		double couplingFilter_y[4], couplingFilter_x[4];
		double couplingFilter_b[3], couplingFilter_a[3];
		double rolloffFilter_y[4], rolloffFilter_x[4];
		double rolloffFilter_b[3], rolloffFilter_a[3];
		int lpOutputPhaseInversion = 0;
		int hpOutputPhaseInversion = 0;


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
			rolloffFilter_x[j] = context[procEvent->processTypeIndex].rolloffFilter_x[j];
			rolloffFilter_y[j] = context[procEvent->processTypeIndex].rolloffFilter_y[j];
		}


#if(dbg >= 1)
		if(debugOutput) cout << "filter3bb procEvent->parameters: ";
#endif

		{
			{
#if(dbg >= 1)
				if(debugOutput) cout << procEvent->parameters[i] << ", ";
#endif

				for(j = 0; j < 3; j++)
				{
					couplingFilter_b[j] = couplingFilter[j];
				}
				for(j = 0; j < 3; j++)
				{
					couplingFilter_a[j] = couplingFilter[j+3];
				}

#if(FILTER_ORDER == 2)
				lp_b[0][0] = lp[procEvent->parameters[0]][0];
				lp_b[0][1] = lp[procEvent->parameters[0]][1];
				lp_b[0][2] = lp[procEvent->parameters[0]][2];
				lp_a[0][1] = lp[procEvent->parameters[0]][4];
				lp_a[0][2] = lp[procEvent->parameters[0]][5];

				hp_b[0][0] = hp[procEvent->parameters[0]][0];
				hp_b[0][1] = hp[procEvent->parameters[0]][1];
				hp_b[0][2] = hp[procEvent->parameters[0]][2];
				hp_a[0][1] = hp[procEvent->parameters[0]][4];
				hp_a[0][2] = hp[procEvent->parameters[0]][5];

				lp_b[1][0] = lp[procEvent->parameters[1]][0];
				lp_b[1][1] = lp[procEvent->parameters[1]][1];
				lp_b[1][2] = lp[procEvent->parameters[1]][2];
				lp_a[1][1] = lp[procEvent->parameters[1]][4];
				lp_a[1][2] = lp[procEvent->parameters[1]][5];

				hp_b[1][0] = hp[procEvent->parameters[1]][0];
				hp_b[1][1] = hp[procEvent->parameters[1]][1];
				hp_b[1][2] = hp[procEvent->parameters[1]][2];
				hp_a[1][1] = hp[procEvent->parameters[1]][4];
				hp_a[1][2] = hp[procEvent->parameters[1]][5];

				rolloffFilter_b[0] = lp[procEvent->parameters[2]][0];
				rolloffFilter_b[1] = lp[procEvent->parameters[2]][1];
				rolloffFilter_b[2] = lp[procEvent->parameters[2]][2];
				rolloffFilter_a[1] = lp[procEvent->parameters[2]][4];
				rolloffFilter_a[2] = lp[procEvent->parameters[2]][5];


#elif(FILTER_ORDER == 3)
				lp_b[0][0] = lp[procEvent->parameters[0]][0];
				lp_b[0][1] = lp[procEvent->parameters[0]][1];
				lp_b[0][2] = lp[procEvent->parameters[0]][2];
				lp_b[0][3] = lp[procEvent->parameters[0]][3];
				lp_a[0][1] = lp[procEvent->parameters[0]][5];
				lp_a[0][2] = lp[procEvent->parameters[0]][6];
				lp_a[0][3] = lp[procEvent->parameters[0]][7];

				hp_b[0][0] = hp[procEvent->parameters[0]][0];
				hp_b[0][1] = hp[procEvent->parameters[0]][1];
				hp_b[0][2] = hp[procEvent->parameters[0]][2];
				hp_b[0][3] = hp[procEvent->parameters[0]][3];
				hp_a[0][1] = hp[procEvent->parameters[0]][5];
				hp_a[0][2] = hp[procEvent->parameters[0]][6];
				hp_a[0][3] = hp[procEvent->parameters[0]][7];

				lp_b[1][0] = lp[procEvent->parameters[1]][0];
				lp_b[1][1] = lp[procEvent->parameters[1]][1];
				lp_b[1][2] = lp[procEvent->parameters[1]][2];
				lp_b[1][3] = lp[procEvent->parameters[1]][3];
				lp_a[1][1] = lp[procEvent->parameters[1]][5];
				lp_a[1][2] = lp[procEvent->parameters[1]][6];
				lp_a[1][3] = lp[procEvent->parameters[1]][7];

				hp_b[1][0] = hp[procEvent->parameters[1]][0];
				hp_b[1][1] = hp[procEvent->parameters[1]][1];
				hp_b[1][2] = hp[procEvent->parameters[1]][2];
				hp_b[1][3] = hp[procEvent->parameters[1]][3];
				hp_a[1][1] = hp[procEvent->parameters[1]][5];
				hp_a[1][2] = hp[procEvent->parameters[1]][6];
				hp_a[1][3] = hp[procEvent->parameters[1]][7];

				rolloffFilter_b[0] = hp[procEvent->parameters[2]][0];
				rolloffFilter_b[1] = hp[procEvent->parameters[2]][1];
				rolloffFilter_b[2] = hp[procEvent->parameters[2]][2];
				rolloffFilter_b[3] = hp[procEvent->parameters[2]][3];
				rolloffFilter_a[1] = hp[procEvent->parameters[2]][5];
				rolloffFilter_a[2] = hp[procEvent->parameters[2]][6];
				rolloffFilter_a[3] = hp[procEvent->parameters[2]][7];

				#endif
				lpOutputPhaseInversion = 0;
				hpOutputPhaseInversion = 0;
			}
		}
#if(dbg >= 1)
		if(debugOutput) cout << endl;
#endif

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[2]]);

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if(footswitchStatus[procEvent->footswitchNumber] == false)
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
				{
					if(inputCouplingMode == 0)
					{
						tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
					}
					else if(inputCouplingMode == 1)
					{
						couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]*couplingFilter_x[0] + couplingFilter_b[1]*couplingFilter_x[1] +couplingFilter_b[2]*couplingFilter_x[2] - couplingFilter_a[1]*couplingFilter_y[1] - couplingFilter_a[2]*couplingFilter_y[2];

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

#if(FILTER_ORDER == 2)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

#elif(FILTER_ORDER == 3)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] + lp_b[j][3]*lp_x[j][3] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2] - lp_a[j][3]*lp_y[j][3];

						lp_x[j][3] = lp_x[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][3] = lp_y[j][2];
						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

#endif

						if(lpOutputPhaseInversion < 50)
							procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = -lp_y[j][0];
						else
							procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[j][0];

						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
						outputSum[0] += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];

#if(FILTER_ORDER == 2)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];
#elif(FILTER_ORDER == 3)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] + hp_b[j][3]*hp_x[j][3] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2] - hp_a[j][3]*hp_y[j][3];
						hp_x[j][3] = hp_x[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][3] = hp_y[j][2];
						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];
#endif

					}

					j = 1;
					/************************** SECOND BAND DIVIDER (MID/HIGH) *************************/
					{
						lp_x[j][0] = hp_y[0][0];
#if(FILTER_ORDER == 2)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2];

						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];
#elif(FILTER_ORDER == 3)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] + lp_b[j][3]*lp_x[j][3] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2] - lp_a[j][3]*lp_y[j][3];

						lp_x[j][3] = lp_x[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][3] = lp_y[j][2];
						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];
#endif

						procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = lp_y[j][0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
						outputSum[1] += procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i];


						hp_x[j][0] = tempInput;
#if(FILTER_ORDER == 2)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];

#elif(FILTER_ORDER == 3)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] + hp_b[j][3]*hp_x[j][3] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2] - hp_a[j][3]*hp_y[j][3];

						hp_x[j][3] = hp_x[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][3] = hp_y[j][2];
						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];
#endif


						rolloffFilter_x[0] = hp_y[j][0];
						rolloffFilter_y[0] = rolloffFilter_b[0]*rolloffFilter_x[0] + rolloffFilter_b[1]*rolloffFilter_x[1] + rolloffFilter_b[2]*rolloffFilter_x[2] - rolloffFilter_a[1]*rolloffFilter_y[1] - rolloffFilter_a[2]*rolloffFilter_y[2];

						if(hpOutputPhaseInversion < 50)
							procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = -rolloffFilter_y[0];
						else
							procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = rolloffFilter_y[0];
						processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);

						rolloffFilter_x[2] = rolloffFilter_x[1];
						rolloffFilter_x[1] = rolloffFilter_x[0];

						rolloffFilter_y[2] = rolloffFilter_y[1];
						rolloffFilter_y[1] = rolloffFilter_y[0];

					}
				}
			}
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[2]]);
		contextIndex = 0;

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
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = rolloffFilter_x[j];
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = rolloffFilter_y[j];
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
		status = 0;
	}
	else
	{
		if(debugOutput) std::cout << "filter3bb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::filter3bb: " << status << endl;
#endif
	return status;
}


#define dbg 0
int filter3bb2(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	static Filter3bb2Context context[20];
	static int bufferSize = 256;
	static int inputCouplingMode = 1;
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::filter3bb2" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(filter3bb2Symbol);
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);

#if(dbg >= 0)
		if(debugOutput) cout << "***** loading filter3bb2: " << procEvent->processTypeIndex << endl;
#endif
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 4; j++)
		{
			context[procEvent->processTypeIndex].lp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].lp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 7; j++)
		{
			context[procEvent->processTypeIndex].bp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].bp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 4; j++)
		{
			context[procEvent->processTypeIndex].hp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].hp_y[j] = 0.00000;
		}
		for(unsigned int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = 0.00000;
		}

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		bufferSize = pedalUtilityData.getBufferSize();
		status = 0;
	}
	else if(action == 'r')
	{
		unsigned int i,j;
		unsigned int contextIndex = 0;
		double tempInput = 0;
		double outputSum[3];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;
		outputSum[2] = 0.00000;

		double lp_b[4], lp_a[4];
		double bp_b[7], bp_a[7];
		double hp_b[4], hp_a[4];

		double lp_y[4], lp_x[4];
		double bp_y[7], bp_x[7];
		double hp_y[4], hp_x[4];
		double couplingFilter_y[3], couplingFilter_x[3];
		double couplingFilter_b[3], couplingFilter_a[3];
		double rolloffFilter_y[3], rolloffFilter_x[3];
		double rolloffFilter_b[3], rolloffFilter_a[3];

		int lpOutputPhaseInversion = 0;
		int hpOutputPhaseInversion = 0;


		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}


		for(j = 0; j < 4; j++)
		{
			lp_x[j] = context[procEvent->processTypeIndex].lp_x[j];
			lp_y[j] = context[procEvent->processTypeIndex].lp_y[j];
		}
		for(j = 0; j < 7; j++)
		{
			bp_x[j] = context[procEvent->processTypeIndex].bp_x[j];
			bp_y[j] = context[procEvent->processTypeIndex].bp_y[j];
		}
		for(j = 0; j < 4; j++)
		{
			hp_x[j] = context[procEvent->processTypeIndex].hp_x[j];
			hp_y[j] = context[procEvent->processTypeIndex].hp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			rolloffFilter_x[j] = context[procEvent->processTypeIndex].rolloffFilter_x[j];
			rolloffFilter_y[j] = context[procEvent->processTypeIndex].rolloffFilter_y[j];
		}


#if(dbg >= 1)
		if(debugOutput) cout << "filter3bb2 procEvent->parameters: ";
#endif

#if(dbg >= 1)
				if(debugOutput) cout << procEvent->parameters[i] << ", ";
#endif

		for(j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j+3];
		}

#if(FILTER_ORDER == 2)
		lp_b[0] = lp2[procEvent->parameters[0]][0];
		lp_b[1] = lp2[procEvent->parameters[0]][1];
		lp_b[2] = lp2[procEvent->parameters[0]][2];
		lp_a[1] = lp2[procEvent->parameters[0]][4];
		lp_a[2] = lp2[procEvent->parameters[0]][5];

		bp_b[0] = bp2[procEvent->parameters[0]][0];
		bp_b[1] = bp2[procEvent->parameters[0]][1];
		bp_b[2] = bp2[procEvent->parameters[0]][2];
		bp_b[3] = bp2[procEvent->parameters[0]][3];
		bp_b[4] = bp2[procEvent->parameters[0]][4];
		bp_a[1] = bp2[procEvent->parameters[0]][6];
		bp_a[2] = bp2[procEvent->parameters[0]][7];
		bp_a[3] = bp2[procEvent->parameters[0]][8];
		bp_a[4] = bp2[procEvent->parameters[0]][9];

		hp_b[0] = hp2[procEvent->parameters[0]][0];
		hp_b[1] = hp2[procEvent->parameters[0]][1];
		hp_b[2] = hp2[procEvent->parameters[0]][2];
		hp_a[1] = hp2[procEvent->parameters[0]][4];
		hp_a[2] = hp2[procEvent->parameters[0]][5];

		rolloffFilter_b[0] = lp2[procEvent->parameters[1]][0];
		rolloffFilter_b[1] = lp2[procEvent->parameters[1]][1];
		rolloffFilter_b[2] = lp2[procEvent->parameters[1]][2];
		rolloffFilter_a[1] = lp2[procEvent->parameters[1]][4];
		rolloffFilter_a[2] = lp2[procEvent->parameters[1]][5];


#elif(FILTER_ORDER == 3)
		lp_b[0] = lp2[procEvent->parameters[0]][0];
		lp_b[1] = lp2[procEvent->parameters[0]][1];
		lp_b[2] = lp2[procEvent->parameters[0]][2];
		lp_b[3] = lp2[procEvent->parameters[0]][3];
		lp_a[1] = lp2[procEvent->parameters[0]][5];
		lp_a[2] = lp2[procEvent->parameters[0]][6];
		lp_a[3] = lp2[procEvent->parameters[0]][7];

		bp_b[0] = bp2[procEvent->parameters[0]][0];
		bp_b[1] = bp2[procEvent->parameters[0]][1];
		bp_b[2] = bp2[procEvent->parameters[0]][2];
		bp_b[3] = bp2[procEvent->parameters[0]][3];
		bp_b[4] = bp2[procEvent->parameters[0]][4];
		bp_b[5] = bp2[procEvent->parameters[0]][5];
		bp_b[6] = bp2[procEvent->parameters[0]][6];
		bp_a[1] = bp2[procEvent->parameters[0]][8];
		bp_a[2] = bp2[procEvent->parameters[0]][9];
		bp_a[3] = bp2[procEvent->parameters[0]][10];
		bp_a[4] = bp2[procEvent->parameters[0]][11];
		bp_a[5] = bp2[procEvent->parameters[0]][12];
		bp_a[6] = bp2[procEvent->parameters[0]][13];

		hp_b[0] = hp2[procEvent->parameters[0]][0];
		hp_b[1] = hp2[procEvent->parameters[0]][1];
		hp_b[2] = hp2[procEvent->parameters[0]][2];
		hp_b[3] = hp2[procEvent->parameters[0]][3];
		hp_a[1] = hp2[procEvent->parameters[0]][5];
		hp_a[2] = hp2[procEvent->parameters[0]][6];
		hp_a[3] = hp2[procEvent->parameters[0]][7];

		rolloffFilter_b[0] = hp2[procEvent->parameters[1]][0];
		rolloffFilter_b[1] = hp2[procEvent->parameters[1]][1];
		rolloffFilter_b[2] = hp2[procEvent->parameters[1]][2];
		rolloffFilter_b[3] = hp2[procEvent->parameters[1]][3];
		rolloffFilter_a[1] = hp2[procEvent->parameters[1]][5];
		rolloffFilter_a[2] = hp2[procEvent->parameters[1]][6];
		rolloffFilter_a[3] = hp2[procEvent->parameters[1]][7];

#endif

		lpOutputPhaseInversion = 0;
		hpOutputPhaseInversion = 0;


#if(dbg >= 1)
		if(debugOutput) cout << endl;
#endif

		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[2]]);

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if(footswitchStatus[procEvent->footswitchNumber] == false)
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
					tempInput = 0;
				if(inputCouplingMode == 0)
				{
					tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
				}
				else if(inputCouplingMode == 1)
				{
					couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					couplingFilter_y[0] = couplingFilter_b[0]*couplingFilter_x[0] + couplingFilter_b[1]*couplingFilter_x[1] +couplingFilter_b[2]*couplingFilter_x[2] - couplingFilter_a[1]*couplingFilter_y[1] - couplingFilter_a[2]*couplingFilter_y[2];

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

#if(FILTER_ORDER == 2)
					lp_y[0] = lp_b[0]*lp_x[0] + lp_b[1]*lp_x[1] + lp_b[2]*lp_x[2] - lp_a[1]*lp_y[1] - lp_a[2]*lp_y[2];

					lp_x[2] = lp_x[1];
					lp_x[1] = lp_x[0];
					lp_y[2] = lp_y[1];
					lp_y[1] = lp_y[0];

#elif(FILTER_ORDER == 3)
					lp_y[0] = lp_b[0]*lp_x[0] + lp_b[1]*lp_x[1] + lp_b[2]*lp_x[2] + lp_b[3]*lp_x[3] - lp_a[1]*lp_y[1] - lp_a[2]*lp_y[2] - lp_a[3]*lp_y[3];

					lp_x[3] = lp_x[2];
					lp_x[2] = lp_x[1];
					lp_x[1] = lp_x[0];
					lp_y[3] = lp_y[2];
					lp_y[2] = lp_y[1];
					lp_y[1] = lp_y[0];

#endif


				if(lpOutputPhaseInversion < 50)
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = -lp_y[0];
				else
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[0];

				//*********************** Bandpass filter ****************************
#if(FILTER_ORDER == 2)
				bp_y[0] = bp_b[0]*bp_x[0] + bp_b[1]*bp_x[1] + bp_b[2]*bp_x[2] + bp_b[3]*bp_x[3] + bp_b[4]*bp_x[4] - bp_a[1]*bp_y[1] - bp_a[2]*bp_y[2] - bp_a[3]*bp_y[3] - bp_a[4]*bp_y[4];

				bp_x[4] = bp_x[3];
				bp_x[3] = bp_x[2];
				bp_x[2] = bp_x[1];
				bp_x[1] = bp_x[0];

				bp_y[4] = bp_y[3];
				bp_y[3] = bp_y[2];
				bp_y[2] = bp_y[1];
				bp_y[1] = bp_y[0];
#elif(FILTER_ORDER == 3)
				bp_y[0] = bp_b[0]*bp_x[0] + bp_b[1]*bp_x[1] + bp_b[2]*bp_x[2] + bp_b[3]*bp_x[3] + bp_b[4]*bp_x[4] + bp_b[5]*bp_x[5] + bp_b[6]*bp_x[6] - bp_a[1]*bp_y[1] - bp_a[2]*bp_y[2] - bp_a[3]*bp_y[3] - bp_a[4]*bp_y[4] - bp_a[5]*bp_y[5] - bp_a[6]*bp_y[6];

				bp_x[6] = bp_x[5];
				bp_x[5] = bp_x[4];
				bp_x[4] = bp_x[3];
				bp_x[3] = bp_x[2];
				bp_x[2] = bp_x[1];
				bp_x[1] = bp_x[0];

				bp_y[6] = bp_y[5];
				bp_y[5] = bp_y[4];
				bp_y[4] = bp_y[3];
				bp_y[3] = bp_y[2];
				bp_y[2] = bp_y[1];
				bp_y[1] = bp_y[0];
#endif
				procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = bp_y[0];


				//*********************Band Divider (BD): Highpass filter ********************

#if(FILTER_ORDER == 2)
				hp_y[0] = hp_b[0]*hp_x[0] + hp_b[1]*hp_x[1] + hp_b[2]*hp_x[2] - hp_a[1]*hp_y[1] - hp_a[2]*hp_y[2];

				hp_x[2] = hp_x[1];
				hp_x[1] = hp_x[0];
				hp_y[2] = hp_y[1];
				hp_y[1] = hp_y[0];

#elif(FILTER_ORDER == 3)
				hp_y[0] = hp_b[0]*hp_x[0] + hp_b[1]*hp_x[1] + hp_b[2]*hp_x[2] + hp_b[3]*hp_x[3] - hp_a[1]*hp_y[1] - hp_a[2]*hp_y[2] - hp_a[3]*hp_y[3];

				hp_x[3] = hp_x[2];
				hp_x[2] = hp_x[1];
				hp_x[1] = hp_x[0];
				hp_y[3] = hp_y[2];
				hp_y[2] = hp_y[1];
				hp_y[1] = hp_y[0];

#endif
				// j = 0 case is processed above

				rolloffFilter_x[0] = hp_y[0];
				rolloffFilter_y[0] = rolloffFilter_b[0]*rolloffFilter_x[0] + rolloffFilter_b[1]*rolloffFilter_x[1] + rolloffFilter_b[2]*rolloffFilter_x[2] - rolloffFilter_a[1]*rolloffFilter_y[1] - rolloffFilter_a[2]*rolloffFilter_y[2];

				if(hpOutputPhaseInversion < 50)
					procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = -rolloffFilter_y[0];
				else
					procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = rolloffFilter_y[0];
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);

				rolloffFilter_x[2] = rolloffFilter_x[1];
				rolloffFilter_x[1] = rolloffFilter_x[0];

				rolloffFilter_y[2] = rolloffFilter_y[1];
				rolloffFilter_y[1] = rolloffFilter_y[0];

				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);
			}
		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[2]]);

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
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = rolloffFilter_x[j];
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = rolloffFilter_y[j];
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
		status = 0;
	}
	else
	{
		if(debugOutput) std::cout << "filter3bb2: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::filter3bb2: " << status << endl;
#endif
	return status;

}


#define dbg 0
int lohifilterb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	unsigned int i,j;
	static LohifilterbContext context[20];
	static int bufferSize = 256;
	static int inputCouplingMode = 1;
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::lohifilterb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(lohifilterbSymbol);
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);

#if(dbg >= 0)
		if(debugOutput) cout << "***** loading lohifilterb: " << procEvent->processTypeIndex << endl;
#endif

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

		bufferSize = pedalUtilityData.getBufferSize();
		status = 0;
	}
	else if(action == 'r')
	{

		int contextIndex = 0;
		double lp_b[NUMBER_OF_BANDS][4], lp_a[NUMBER_OF_BANDS][4];
		double hp_b[NUMBER_OF_BANDS][4], hp_a[NUMBER_OF_BANDS][4];

		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
		double couplingFilter_y[3], couplingFilter_x[3];
		double couplingFilter_b[3], couplingFilter_a[3];
		double noiseFilter_y[3], noiseFilter_x[3];
		double noiseFilter_b[3], noiseFilter_a[3];
		double tempInput;
		double outputSum[2];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;



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

		for(j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j+3];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_b[j] = noiseFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_a[j] = noiseFilter[j+3];
		}

#if(dbg >= 1)
		if(debugOutput) cout << "lohifilterb procEvent->parameters: ";
#endif

		i = 0;
		{
#if(dbg >= 1)
				if(debugOutput) cout << procEvent->parameters[i] << ", ";
#endif


				lp_b[i][0] = lp[tempIndex][0];
				lp_b[i][1] = lp[tempIndex][1];
				lp_b[i][2] = lp[tempIndex][2];
				lp_a[i][1] = lp[tempIndex][4];
				lp_a[i][2] = lp[tempIndex][5];

				hp_b[i][0] = hp[tempIndex][0];
				hp_b[i][1] = hp[tempIndex][1];
				hp_b[i][2] = hp[tempIndex][2];
				hp_a[i][1] = hp[tempIndex][4];
				hp_a[i][2] = hp[tempIndex][5];
		}
#if(dbg >= 1)
		if(debugOutput) cout << endl;
#endif

		j = 0;
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[1]]);

			for(i = 0; i < bufferSize; i++)
			{
#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == false)
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
						couplingFilter_y[0] = couplingFilter_b[0]*couplingFilter_x[0] + couplingFilter_b[1]*couplingFilter_x[1] +couplingFilter_b[2]*couplingFilter_x[2] - couplingFilter_a[1]*couplingFilter_y[1] - couplingFilter_a[2]*couplingFilter_y[2];

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
#if(FILTER_ORDER == 2)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

#elif(FILTER_ORDER == 3)
						lp_y[j][0] = lp_b[j][0]*lp_x[j][0] + lp_b[j][1]*lp_x[j][1] + lp_b[j][2]*lp_x[j][2] + lp_b[j][3]*lp_x[j][3] - lp_a[j][1]*lp_y[j][1] - lp_a[j][2]*lp_y[j][2] - lp_a[j][3]*lp_y[j][3];

						lp_x[j][3] = lp_x[j][2];
						lp_x[j][2] = lp_x[j][1];
						lp_x[j][1] = lp_x[j][0];

						lp_y[j][3] = lp_y[j][2];
						lp_y[j][2] = lp_y[j][1];
						lp_y[j][1] = lp_y[j][0];

#endif
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = lp_y[j][0];
					processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);

					/********************* Highpass filter ********************/
					hp_x[j][0] = tempInput;


#if(FILTER_ORDER == 2)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];

#elif(FILTER_ORDER == 3)
						hp_y[j][0] = hp_b[j][0]*hp_x[j][0] + hp_b[j][1]*hp_x[j][1] + hp_b[j][2]*hp_x[j][2] + hp_b[j][3]*hp_x[j][3] - hp_a[j][1]*hp_y[j][1] - hp_a[j][2]*hp_y[j][2] - hp_a[j][3]*hp_y[j][3];

						hp_x[j][3] = hp_x[j][2];
						hp_x[j][2] = hp_x[j][1];
						hp_x[j][1] = hp_x[j][0];

						hp_y[j][3] = hp_y[j][2];
						hp_y[j][2] = hp_y[j][1];
						hp_y[j][1] = hp_y[j][0];
#endif

					procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i] = hp_y[j][0];
					processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
			    }
			}
		}
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);

		contextIndex = 0;


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
		status = 0;
	}
	else
	{
		if(debugOutput) std::cout << "lohifilterb: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::lohifilterb: " << status << endl;
#endif
	return status;
}

int mixerb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	unsigned int i,j;
	int status = 0;
	static int bufferSize = 256;
	static int inputCouplingMode = 1;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::mixerb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(mixerbSymbol);
	}
	else if(action == 'l')
	{
#if(dbg >= 0)
		if(debugOutput) cout << "***** loading mixerb: " << procEvent->processTypeIndex << endl;
#endif
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		bufferSize = pedalUtilityData.getBufferSize();
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
			if(footswitchStatus[procEvent->footswitchNumber] == false)
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
		if(debugOutput) std::cout << "mixerb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::mixerb: " << status << endl;
#endif
	return status;
}

int volumeb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	unsigned int i,j;
	int status = 0;
	static int bufferSize = 256;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::volumeb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if(action == 'c')
	{
		componentVector.push_back(volumebSymbol);
	}
	else if(action == 'l')
	{
#if(dbg >= 0)
		if(debugOutput) cout << "***** loading volumeb: " << procEvent->processTypeIndex << endl;
#endif
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		bufferSize = pedalUtilityData.getBufferSize();
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
			if(footswitchStatus[procEvent->footswitchNumber] == false)
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
			}
			else
#endif
			{
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = vol*(procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset);
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
		if(debugOutput) std::cout << "volumeb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::volumeb: " << status << endl;
#endif
	return status;
}



int waveshaperb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
	unsigned int i,j;

	int status = 0;
	static int bufferSize = 256;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::waveshaperb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	static int antiAliasingNumber = 1;
	static int waveshaperMode = 1;
	static int inputCouplingMode = 1;
	const double divider = 5.5;
	const double r1 = 1000.0;
	const double r2 = 2000.0;
	double r3 = r2/divider;
	double r4 = r3/divider;
	double r5 = r4/divider;
	double r6 = r5/divider;

	static WaveshaperbContext context[20];

	if(action == 'c')
	{
		componentVector.push_back(waveshaperbSymbol);
	}
	else if(action == 'l')
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);


#if(dbg >= 0)
		if(debugOutput) cout << "***** loading waveshaperb: " << procEvent->processTypeIndex << endl;
#endif

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.0000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.0000;
		}
		context[procEvent->processTypeIndex].outMeasure = 0.0000; // outMeasure

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j] = 0.0000;
			context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j] = 0.0000;
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j] = 0.0000;
			context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j] = 0.0000;
		}

		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		antiAliasingNumber = pedalUtilityData.getAntiAliasingNumber();
		waveshaperMode = pedalUtilityData.getWaveshaperMode();
		bufferSize = pedalUtilityData.getBufferSize();
		status = 0;
	}
	else if(action == 'r')
	{
		double k[6][6];
		double v[5];
		double x[6],y[6];
		double m[5],b[5];
		double tempInput;
		double distAveArray[4];
		double distAve;
		int contextIndex = 0;
		double outMeasure;
		double outputSum = 0.00000;

		double coupledBuffer[BUFFER_SIZE];
		double couplingFilter_y[3], couplingFilter_x[3];
		double couplingFilter_b[3], couplingFilter_a[3];

		double upSampleBuffer[10000];
		double noiseFilter_b[3], noiseFilter_a[3];
		double noiseFilter_y[3], noiseFilter_x[3];

		double filteredUpSampleBuffer[10000];
		double antiAliasingFilter_y[2][3], antiAliasingFilter_x[2][3];
		double antiAliasingFilter_b[3], antiAliasingFilter_a[3];

		double downSampleBuffer[10000];
		int upSampleBufferIndex = 0;
		int upSampleBufferSize = antiAliasingNumber*bufferSize;

		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] = context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		outMeasure = context[procEvent->processTypeIndex].outMeasure;

		for(j = 0; j < 3; j++)
		{
			antiAliasingFilter_x[0][j] = context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j];
			antiAliasingFilter_y[0][j] = context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j];
		}
		for(j = 0; j < 3; j++)
		{
			antiAliasingFilter_x[1][j] = context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j];
			antiAliasingFilter_y[1][j] = context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = context[procEvent->processTypeIndex].noiseFilter_x[j];
			noiseFilter_y[j] = context[procEvent->processTypeIndex].noiseFilter_y[j];
		}

		if(waveshaperMode == 0)
		{
			k[5][0] = r2*r3*r4*r5*r6;
			k[5][1] = r1*r3*r4*r5*r6;
			k[5][2] = r1*r2*r4*r5*r6;
			k[5][3] = r1*r2*r3*r5*r6;
			k[5][4] = r1*r2*r3*r4*r6;
			k[5][5] = r1*r2*r3*r4*r5;

			k[4][0] = r2*r3*r4*r5;
			k[4][1] = r1*r3*r4*r5;
			k[4][2] = r1*r2*r4*r5;
			k[4][3] = r1*r2*r3*r5;
			k[4][4] = r1*r2*r3*r4;
			k[4][5] = 0.0000;

			k[3][0] = r2*r3*r4;
			k[3][1] = r1*r3*r4;
			k[3][2] = r1*r2*r4;
			k[3][3] = r1*r2*r3;
			k[3][4] = 0.000;
			k[3][5] = 0.000;

			k[2][0] = r2*r3;
			k[2][1] = r1*r3;
			k[2][2] = r1*r2;
			k[2][3] = 0.000;
			k[2][4] = 0.000;
			k[2][5] = 0.000;

			k[1][0] = r2;
			k[1][1] = r1;
			k[1][2] = 0.000;
			k[1][3] = 0.000;
			k[1][4] = 0.000;
			k[1][5] = 0.000;

			k[0][0] = 1.000;
			k[0][1] = 0.000;
			k[0][2] = 0.000;
			k[0][3] = 0.000;
			k[0][4] = 0.000;
			k[0][5] = 0.000;

		}
		else
		{
			x[0] = 0.000;
			y[0] = 0.000;
			x[1] = 0.100;
			y[1] = 0.400;
			x[2] = 0.200;
			y[2] = 0.800;
			x[3] = 0.300;
			y[3] = 1.200;
			x[4] = 0.400;
			y[4] = 1.200;
			x[5] = 0.500;
			y[5] = 1.200;
			x[6] = 0.600;
			y[6] = 1.200;

			for(j = 0; j < 5; j++)
			{
			    m[j] = (y[j+1]-y[j])/(x[j+1]-x[j]);
			    b[j] = y[j]-m[j]*x[j];
#if(dbg >= 2)
			    if(debugOutput) cout << "m[" << j << "] = " << m[j] << "\tb[" << j << "] = " << b[j] << endl;
#endif
			}
		}

		for(j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j+3];
		}
		for(j = 0; j < 3; j++)
		{
			antiAliasingFilter_b[j] = antiAliasingFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			antiAliasingFilter_a[j] = antiAliasingFilter[j+3];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_b[j] = noiseFilter[j];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_a[j] = noiseFilter[j+3];
		}


		int edge = procEvent->parameters[1];
		v[0] = brkpt[edge][0];
		v[1] = brkpt[edge][1];
		v[2] = brkpt[edge][2];
		v[3] = brkpt[edge][3];
		v[4] = brkpt[edge][4];

		double gain = logAmp[procEvent->parameters[0]]*5.0;
		double mix = linAmp[procEvent->parameters[2]]*0.1;
		double out = logAmp[procEvent->parameters[3]]*0.1;
		double k1,k2,k3,k4,k5,k6;
		double v1,v2,v3,v4,v5;
		double clean, dist;
		bool kIndexFound = false;
		int kIndex = 0;

		if(antiAliasingNumber > 1)
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			if(footswitchStatus[procEvent->footswitchNumber] == false)
			{
				for(i = 0; i < bufferSize; i++)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}
			}
			else
			{
				double sample, nextSample;



				for(i = 0; i < bufferSize; i++)
				{
					if(inputCouplingMode == 0)
					{
						coupledBuffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
					}
					else if(inputCouplingMode == 1)
					{
						couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]*couplingFilter_x[0] + couplingFilter_b[1]*couplingFilter_x[1] +couplingFilter_b[2]*couplingFilter_x[2] - couplingFilter_a[1]*couplingFilter_y[1] - couplingFilter_a[2]*couplingFilter_y[2];

						coupledBuffer[i] = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}
					else if(inputCouplingMode == 2)
					{
						coupledBuffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
					}
				}

				for(i = 0; i < bufferSize; i++)
				{
					sample = coupledBuffer[i];
					nextSample = coupledBuffer[i+1];
					if(i < bufferSize - 1)
					{
						upSampleBuffer[antiAliasingNumber*i] = sample*antiAliasingNumber;
						upSampleBuffer[antiAliasingNumber*i+1] = 0.00000000;
						upSampleBuffer[antiAliasingNumber*i+2] = 0.00000000;
						upSampleBuffer[antiAliasingNumber*i+3] = 0.00000000;
					}
					else
					{
						upSampleBuffer[antiAliasingNumber*i] = sample*antiAliasingNumber;
						upSampleBuffer[antiAliasingNumber*i+1] = 0.00000000;
						upSampleBuffer[antiAliasingNumber*i+2] = 0.00000000;
						upSampleBuffer[antiAliasingNumber*i+3] = 0.00000000;
					}
				}

				for(i = 0; i < upSampleBufferSize; i++)
				{

					antiAliasingFilter_x[0][0] = upSampleBuffer[i];
					antiAliasingFilter_y[0][0] = antiAliasingFilter_b[0]*antiAliasingFilter_x[0][0] + antiAliasingFilter_b[1]*antiAliasingFilter_x[0][1] + antiAliasingFilter_b[2]*antiAliasingFilter_x[0][2] - antiAliasingFilter_a[1]*antiAliasingFilter_y[0][1] - antiAliasingFilter_a[2]*antiAliasingFilter_y[0][2];
					filteredUpSampleBuffer[i] = antiAliasingFilter_y[0][0];
					antiAliasingFilter_x[0][2] = antiAliasingFilter_x[0][1];
					antiAliasingFilter_x[0][1] = antiAliasingFilter_x[0][0];

					antiAliasingFilter_y[0][2] = antiAliasingFilter_y[0][1];
					antiAliasingFilter_y[0][1] = antiAliasingFilter_y[0][0];
				}


				for(i = 0; i < upSampleBufferSize; i++)
				{

					tempInput = filteredUpSampleBuffer[i];

					if(waveshaperMode == 0)
					{
						kIndexFound = false;
						if(0.000 <= tempInput)
						{
							v1 = v[0];
							v2 = v[1];
							v3 = v[2];
							v4 = v[3];
							v5 = v[4];

							while(kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];
								dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);

								if(kIndex == 0)
								{
									if(dist > v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if(kIndex == 5)
								{
									if(dist < v[kIndex-1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if(dist > v[kIndex])
										kIndex++;
									else if(dist < v[kIndex-1])
										kIndex--;
									else
										kIndexFound = true;
								}
								dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);
							}
						}
						else
						{
							v1 = -v[0];
							v2 = -v[1];
							v3 = -v[2];
							v4 = -v[3];
							v5 = -v[4];
							while(kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];

								dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);

								if(kIndex == 0)
								{
									if(dist < -v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if(kIndex == 5)
								{
									if(dist > -v[kIndex-1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if(dist < -v[kIndex])
										kIndex++;
									else if(dist > -v[kIndex-1])
										kIndex--;
									else
										kIndexFound = true;
								}
								dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);
							}
						}
					}

					clean = tempInput;
					antiAliasingFilter_x[1][0] = ((1.0 - mix)*clean + mix*dist);
					antiAliasingFilter_y[1][0] = antiAliasingFilter_b[0]*antiAliasingFilter_x[1][0] + antiAliasingFilter_b[1]*antiAliasingFilter_x[1][1] + antiAliasingFilter_b[2]*antiAliasingFilter_x[1][2] - antiAliasingFilter_a[1]*antiAliasingFilter_y[1][1] - antiAliasingFilter_a[2]*antiAliasingFilter_y[1][2];
					downSampleBuffer[i] = antiAliasingFilter_y[1][0];

					antiAliasingFilter_x[1][2] = antiAliasingFilter_x[1][1];
					antiAliasingFilter_x[1][1] = antiAliasingFilter_x[1][0];

					antiAliasingFilter_y[1][2] = antiAliasingFilter_y[1][1];
					antiAliasingFilter_y[1][1] = antiAliasingFilter_y[1][0];
				}

				for(i = 0; i < bufferSize; i++)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = downSampleBuffer[antiAliasingNumber*i];
				}

				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
			}
			updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		}
		else
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			for(i = 0; i < bufferSize; i++)
			{
	#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == false)
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
						couplingFilter_y[0] = couplingFilter_b[0]*couplingFilter_x[0] + couplingFilter_b[1]*couplingFilter_x[1] +couplingFilter_b[2]*couplingFilter_x[2] - couplingFilter_a[1]*couplingFilter_y[1] - couplingFilter_a[2]*couplingFilter_y[2];

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
						kIndexFound = false;
				        if(0.000 <= tempInput)
						{
				        	v1 = v[0];
				            v2 = v[1];
				            v3 = v[2];
				            v4 = v[3];
				            v5 = v[4];

					       	while(kIndexFound == false)
				            {
				                k1 = k[kIndex][0];
				                k2 = k[kIndex][1];
				                k3 = k[kIndex][2];
				                k4 = k[kIndex][3];
				                k5 = k[kIndex][4];
				                k6 = k[kIndex][5];
				                dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);

				                if(kIndex == 0)
				                {
				                    if(dist > v[kIndex])
				                        kIndex++;
				                    else
				                    	kIndexFound = true;
				                }
				                else if(kIndex == 5)
				                {
				                    if(dist < v[kIndex-1])
				                        kIndex--;
				                    else
				                    	kIndexFound = true;
				                }
				                else
				                {
				                    if(dist > v[kIndex])
				                        kIndex++;
				                    else if(dist < v[kIndex-1])
				                        kIndex--;
				                    else
				                    	kIndexFound = true;
				                }
				            }
						}
				        else
				        {
				            v1 = -v[0];
				            v2 = -v[1];
				            v3 = -v[2];
				            v4 = -v[3];
				            v5 = -v[4];
				            while(kIndexFound == false)
				            {
				                k1 = k[kIndex][0];
				                k2 = k[kIndex][1];
				                k3 = k[kIndex][2];
				                k4 = k[kIndex][3];
				                k5 = k[kIndex][4];
				                k6 = k[kIndex][5];

				                dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);

				                if(kIndex == 0)
				                {
				                    if(dist < -v[kIndex])
				                        kIndex++;
				                    else
				                    	kIndexFound = true;
				                }
				                else if(kIndex == 5)
				                {
				                    if(dist > -v[kIndex-1])
				                        kIndex--;
				                    else
				                    	kIndexFound = true;
				                }
				                else
				                {
				                    if(dist < -v[kIndex])
				                        kIndex++;
				                    else if(dist > -v[kIndex-1])
				                        kIndex--;
				                    else
				                    	kIndexFound = true;
				                }
				                dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5 + v5*k6)/(k1 + k2 + k3 + k4 + k5 + k6);
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

					noiseFilter_x[0] = 0.1*((1.0 - mix)*clean + mix*dist);
					noiseFilter_y[0] = noiseFilter_b[0]*noiseFilter_x[0] + noiseFilter_b[1]*noiseFilter_x[1] + noiseFilter_b[2]*noiseFilter_x[2] - noiseFilter_a[1]*noiseFilter_y[1] - noiseFilter_a[2]*noiseFilter_y[2];
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = noiseFilter_y[0];

					noiseFilter_x[2] = noiseFilter_x[1];
					noiseFilter_x[1] = noiseFilter_x[0];

					noiseFilter_y[2] = noiseFilter_y[1];
					noiseFilter_y[1] = noiseFilter_y[0];


				}

				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
			}
			updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		}


		contextIndex = 0;


		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] = couplingFilter_y[j];
		}
		context[procEvent->processTypeIndex].outMeasure = outMeasure; // outMeasure

		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j] = antiAliasingFilter_x[0][j];
			context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j] = antiAliasingFilter_y[0][j];
		}
		for(j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j] = antiAliasingFilter_x[1][j];
			context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j] = antiAliasingFilter_y[1][j];
		}
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
		status = 0;
	}
	else
	{
		if(debugOutput) std::cout << "waveshaperb: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::waveshaperb: " << status << endl;
#endif
	return status;
}


int samplerb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
		unsigned int i,j;
		int status = 0;
		static int bufferSize = 256;
	#if(dbg >= 1)
		if(debugOutput) cout << "***** ENTERING: Effects::samplerb" << endl;
		if(debugOutput) cout << "action: " << action << endl;
	#endif

		static SamplerbContext context[20];

		if(action == 'c')
		{
			componentVector.push_back(samplerbSymbol);
		}
		else if(action == 'l')
		{
			initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			context[procEvent->processTypeIndex].blankInt = 0;
#if(dbg >= 0)
		if(debugOutput) cout << "***** loading samplerb: " << procEvent->processTypeIndex << endl;
#endif

			for(i = 0; i < 256; i++)
			{
				procEvent->internalData[i] = 0.0;
			}
			bufferSize = pedalUtilityData.getBufferSize();
			status = 0;
		}
		else if(action == 'r')
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			for(i = 0; i < bufferSize; i++)
			{
	#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == false)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}
				else
	#endif
				{
					// PROCESS HERE
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
			if(debugOutput) std::cout << "samplerb: invalid action: " << action << std::endl;
			status = 0;
		}

	#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING: Effects::samplerb: " << status << endl;
	#endif
		return status;
}

int oscillatorb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
		unsigned int i,j;
		int status = 0;
		static int bufferSize = 256;
	#if(dbg >= 1)
		if(debugOutput) cout << "***** ENTERING: Effects::oscillatorb" << endl;
		if(debugOutput) cout << "action: " << action << endl;
	#endif

		static OscillatorbContext context[20];

		if(action == 'c')
		{
			componentVector.push_back(oscillatorbSymbol);
		}
		else if(action == 'l')
		{
#if(dbg >= 0)
		if(debugOutput) cout << "***** loading oscillatorb: " << procEvent->processTypeIndex << endl;
#endif
			initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			context[procEvent->processTypeIndex].blankInt = 0;

			for(i = 0; i < 256; i++)
			{
				procEvent->internalData[i] = 0.0;
			}
			bufferSize = pedalUtilityData.getBufferSize();
			status = 0;
		}
		else if(action == 'r')
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			for(i = 0; i < bufferSize; i++)
			{
	#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == false)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}
				else
	#endif
				{
					// PROCESS HERE
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
			if(debugOutput) std::cout << "oscillatorb: invalid actiond: " << action << std::endl;
			status = 0;
		}

	#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING: Effects::oscillatorb: " << status << endl;
	#endif
		return status;
}

int blankb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, bool *footswitchStatus)
{
		unsigned int i,j;
		int status = 0;
		static int bufferSize = 256;
	#if(dbg >= 1)
		if(debugOutput) cout << "***** ENTERING: Effects::blankb" << endl;
		if(debugOutput) cout << "action: " << action << endl;
	#endif

		static BlankbContext context[20];

		if(action == 'c')
		{
			componentVector.push_back(blankbSymbol);
		}
		else if(action == 'l')
		{
#if(dbg >= 0)
		if(debugOutput) cout << "***** loading blankb: " << procEvent->processTypeIndex << endl;
#endif
			initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			context[procEvent->processTypeIndex].blankInt = 0;

			for(i = 0; i < 256; i++)
			{
				procEvent->internalData[i] = 0.0;
			}
			bufferSize = pedalUtilityData.getBufferSize();
			status = 0;
		}
		else if(action == 'r')
		{
			resetBufferAve(&procBufferArray[procEvent->outputBufferIndexes[0]]);
			for(i = 0; i < bufferSize; i++)
			{
	#if(FOOTSWITCH_ALWAYS_ON == 0)
				if(footswitchStatus[procEvent->footswitchNumber] == false)
				{
					procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];
				}
				else
	#endif
				{
					// PROCESS HERE
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
			if(debugOutput) std::cout << "blankb: invalid actiond: " << action << std::endl;
			status = 0;
		}

	#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING: Effects::blankb: " << status << endl;
	#endif
		return status;
}
