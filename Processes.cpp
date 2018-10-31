/*
 * Processes.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */
#include "config.h"
#include "Processes.h"
#include "valueArrays.h"
#include "ComponentSymbols.h"

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

extern bool debugOutput;
extern vector<string> componentVector;

#define dbg 0

#define dbg 0
int
clearProcBuffer (struct ProcessSignalBuffer *bufferArray)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: clearProcBuffer" << endl;
	if(debugOutput) cout << "bufferArray name: " << bufferArray->processName << endl;
#endif

	for (auto & bufferSample : bufferArray->buffer)
	{
		bufferSample = 0.0000;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: clearProcBuffer" << endl;
#endif

	return status;

}


void
printProcessData (struct ProcessEvent *procEvent)
{
	cout << " processName: " << procEvent->processName;
	cout << "\t\tprocessTypeInt: " << procEvent->processTypeInt;
	cout << "\t\tprocessTypeIndex: " << procEvent->processTypeIndex;
	cout << "\t\tfootswitchNumber: " << procEvent->footswitchNumber << endl;
	cout << " parameterCount: " << procEvent->parameterCount << endl;

	for (int i = 0; i < procEvent->parameterCount; i++)
	{
		cout << "\t\tparameterArray[" << i << "].internalIndexValue: "
						<< procEvent->parameterArray[i].internalIndexValue
						<< endl;
		cout << "\t\tparameterArray[" << i << "].paramContBufferIndex: "
						<< procEvent->parameterArray[i].paramContBufferIndex
						<< endl;
		cout << "\t\tparameterArray[" << i << "].parameterName: "
						<< procEvent->parameterArray[i].parameterName << endl;
	}

	cout << " processInputCount: " << procEvent->processInputCount << endl;

	for (int i = 0; i < procEvent->processInputCount; i++)
	{
		cout << "\t\tinputConnectedBufferIndexArray[" << i << "]: "
						<< procEvent->inputConnectedBufferIndexArray[i] << endl;
	}

	cout << " processOutputCount: " << procEvent->processOutputCount << endl;

	for (int i = 0; i < procEvent->processOutputCount; i++)
	{
		cout << "\t\toutputConnectedBufferIndexArray[" << i << "]: "
						<< procEvent->outputConnectedBufferIndexArray[i]
						<< endl;
	}
}

static void
printSignalBufferData (ProcessSignalBuffer procSigBuffer)
{
	cout << "process signal data: " << procSigBuffer.srcProcess.objectName
					<< ":  ";
	for (int i = 0; i < 10; i++)
	{
		cout << procSigBuffer.buffer[i] << ",";
	}
	cout << endl;
}

#define dbg 0
int delayTimingTestCount = 0;
int
delayb (char action, struct ProcessEvent *procEvent,
		array<ProcessSignalBuffer, 60> &procBufferArray,
		array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
		bool *footswitchStatus)

{
	static DelayContext context[10];
	int bufferSize = procEvent->bufferSize;
	int status = 0;

	if (action == 'c')
	{
		componentVector.push_back (delaybSymbol);
	}
	else if (action == 'l')
	{
#if(dbg >= 1)
		if(debugOutput) cout << "***** loading delayb: " << procEvent->processTypeIndex << endl;
#endif
		context[procEvent->processTypeIndex].inputPtr = 0;
		context[procEvent->processTypeIndex].outputPtr = 0;
		for (auto & delaySample : context[procEvent->processTypeIndex].delayBuffer)
		{
			delaySample = 0.0;
		}

		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0] =
						0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1] =
						0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2] =
						0.000;
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3] =
						0.000;

		status = 0;

	}
	else if (action == 'r')
	{
		double tempInput = 0;
		int delay = 0;

		int i;

		int inputPtr = context[procEvent->processTypeIndex].inputPtr;
		int outputPtr = context[procEvent->processTypeIndex].outputPtr;
		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;
		int param2 =
						paramContBufferArray[procEvent->parameterArray[1].paramContBufferIndex].parameterValueIndex;
		int delayCoarse = delayTimeCoarse[param1];
		int delayFine = delayTimeFine[param2];
#if(dbg >= 2)
		if(debugOutput) cout << "param1: " << param1;
		if(debugOutput) cout << "delayCoarse: " << delayCoarse;
#endif

#if(dbg >= 2)
		if(debugOutput) cout << "param2: " << param2;
		if(debugOutput) cout << "delayFine: " << delayFine << endl;
#endif

		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3] =
						context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2] =
						context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1] =
						context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0];
		context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0] =
						delayFine;

		int delayFineAveraged =
						(context[procEvent->processTypeIndex].delayTimeAveragingBuffer[0]
										+ context[procEvent->processTypeIndex].delayTimeAveragingBuffer[1]
										+ context[procEvent->processTypeIndex].delayTimeAveragingBuffer[2]
										+ context[procEvent->processTypeIndex].delayTimeAveragingBuffer[3])
										/ 4;

		delay = (delayCoarse + delayFineAveraged);

		if (inputPtr >= delay)
			outputPtr = inputPtr - delay;
		else
			outputPtr = DELAY_BUFFER_LENGTH - (delay - inputPtr);

		if (inputPtr >= DELAY_BUFFER_LENGTH)
			inputPtr = 0;
		if (outputPtr >= DELAY_BUFFER_LENGTH)
			outputPtr = 0;

#if(dbg >= 3)
		if(debugOutput) cout << "delay: " << delay;
		if(debugOutput) cout << "inputPtr: " << inputPtr;
		if(debugOutput) cout << "outputPtr: " << outputPtr << endl;
#endif

		for (i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if (footswitchStatus[procEvent->footswitchNumber] == false)
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				if (inputPtr >= DELAY_BUFFER_LENGTH)
					inputPtr = 0;
				context[procEvent->processTypeIndex].delayBuffer[inputPtr++] =
								0.0;
			}
			else
#endif
			{
				tempInput =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];

				context[procEvent->processTypeIndex].delayBuffer[inputPtr] =
								tempInput;
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								context[procEvent->processTypeIndex].delayBuffer[outputPtr];

				if (inputPtr >= (DELAY_BUFFER_LENGTH))
					inputPtr = 0;
				else
					inputPtr++;

				if (outputPtr >= (DELAY_BUFFER_LENGTH))
					outputPtr = 0;
				else
				{
					outputPtr++;
				}
			}
		}

		context[procEvent->processTypeIndex].inputPtr = inputPtr;
		context[procEvent->processTypeIndex].outputPtr = outputPtr;
		status = 0;
	}

	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{
		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "delayb: invalid actiond: " << action << std::endl;
		status = 0;
	}

	return status;
}

#define dbg 0
int
filter3bb (char action, struct ProcessEvent *procEvent,
		   array<ProcessSignalBuffer, 60> &procBufferArray,
		   array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
		   bool *footswitchStatus)
{
	static Filter3bbContext context[20];
	int status = 0;
	int bufferSize = procEvent->bufferSize;
	int inputCouplingMode = procEvent->inputCouplingMode;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::filter3bb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if (action == 'c')
	{
		componentVector.push_back (filter3bbSymbol);
	}
	else if (action == 'l')
	{

#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading filter3bb: " << procEvent->processTypeIndex
							<< endl;
#endif

		for (int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for (int i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].lp_y[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_y[i][j] = 0.00000;
			}
		}
		for (int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = 0.00000;
		}

		status = 0;
		cout << "source process name: "
						<< procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].srcProcess.objectName;
		cout << "source port name: "
						<< procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].srcProcess.portName
						<< endl;

	}
	else if (action == 'r')
	{
		int i, j;
		double tempInput = 0;
		double outputSum[3];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;
		outputSum[2] = 0.00000;

		double lp_b[NUMBER_OF_BANDS][4], lp_a[NUMBER_OF_BANDS][4];
		double hp_b[NUMBER_OF_BANDS][4], hp_a[NUMBER_OF_BANDS][4];
		ProcessSignalBuffer procOutputBuffer[NUMBER_OF_BANDS]; // transfers data from HP output of lower band to LP input higher band

		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
		double couplingFilter_y[4], couplingFilter_x[4];
		double couplingFilter_b[3], couplingFilter_a[3];
		double rolloffFilter_y[4], rolloffFilter_x[4];
		double rolloffFilter_b[3], rolloffFilter_a[3];
		int lpOutputPhaseInversion = 0;
		int hpOutputPhaseInversion = 0;

		for (j = 0; j < 3; j++)
		{
			couplingFilter_x[j] =
							context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] =
							context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		for (i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for (j = 0; j < 4; j++)
			{
				lp_x[i][j] = context[procEvent->processTypeIndex].lp_x[i][j];
				lp_y[i][j] = context[procEvent->processTypeIndex].lp_y[i][j];
				hp_x[i][j] = context[procEvent->processTypeIndex].hp_x[i][j];
				hp_y[i][j] = context[procEvent->processTypeIndex].hp_y[i][j];
			}
		}
		for (j = 0; j < 3; j++)
		{
			rolloffFilter_x[j] =
							context[procEvent->processTypeIndex].rolloffFilter_x[j];
			rolloffFilter_y[j] =
							context[procEvent->processTypeIndex].rolloffFilter_y[j];
		}

#if(dbg >= 1)
		if(debugOutput) cout << "filter3bb procEvent->parameters: ";
#endif

		{
			{
#if(dbg >= 1)
				if(debugOutput) cout << procEvent->parameterArray[i] << ", ";
#endif

				for (j = 0; j < 3; j++)
				{
					couplingFilter_b[j] = couplingFilter[j];
				}
				for (j = 0; j < 3; j++)
				{
					couplingFilter_a[j] = couplingFilter[j + 3];
				}
				int param1 =
								paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;

				int param2 =
								paramContBufferArray[procEvent->parameterArray[1].paramContBufferIndex].parameterValueIndex;
				int param3 =
								paramContBufferArray[procEvent->parameterArray[2].paramContBufferIndex].parameterValueIndex;
#if(FILTER_ORDER == 2)
				lp_b[0][0] = lp[param1][0];
				lp_b[0][1] = lp[param1][1];
				lp_b[0][2] = lp[param1][2];
				lp_a[0][1] = lp[param1][4];
				lp_a[0][2] = lp[param1][5];

				hp_b[0][0] = hp[param1][0];
				hp_b[0][1] = hp[param1][1];
				hp_b[0][2] = hp[param1][2];
				hp_a[0][1] = hp[param1][4];
				hp_a[0][2] = hp[param1][5];

				lp_b[1][0] = lp[param2][0];
				lp_b[1][1] = lp[param2][1];
				lp_b[1][2] = lp[param2][2];
				lp_a[1][1] = lp[param2][4];
				lp_a[1][2] = lp[param2][5];

				hp_b[1][0] = hp[param2][0];
				hp_b[1][1] = hp[param2][1];
				hp_b[1][2] = hp[param2][2];
				hp_a[1][1] = hp[param2][4];
				hp_a[1][2] = hp[param2][5];

				rolloffFilter_b[0] = lp[param3][0];
				rolloffFilter_b[1] = lp[param3][1];
				rolloffFilter_b[2] = lp[param3][2];
				rolloffFilter_a[1] = lp[param3][4];
				rolloffFilter_a[2] = lp[param3][5];

#elif(FILTER_ORDER == 3)
				lp_b[0][0] = lp[param1][0];
				lp_b[0][1] = lp[param1][1];
				lp_b[0][2] = lp[param1][2];
				lp_b[0][3] = lp[param1][3];
				lp_a[0][1] = lp[param1][5];
				lp_a[0][2] = lp[param1][6];
				lp_a[0][3] = lp[param1][7];

				hp_b[0][0] = hp[param1][0];
				hp_b[0][1] = hp[param1][1];
				hp_b[0][2] = hp[param1][2];
				hp_b[0][3] = hp[param1][3];
				hp_a[0][1] = hp[param1][5];
				hp_a[0][2] = hp[param1][6];
				hp_a[0][3] = hp[param1][7];

				lp_b[1][0] = lp[param2][0];
				lp_b[1][1] = lp[param2][1];
				lp_b[1][2] = lp[param2][2];
				lp_b[1][3] = lp[param2][3];
				lp_a[1][1] = lp[param2][5];
				lp_a[1][2] = lp[param2][6];
				lp_a[1][3] = lp[param2][7];

				hp_b[1][0] = hp[param2][0];
				hp_b[1][1] = hp[param2][1];
				hp_b[1][2] = hp[param2][2];
				hp_b[1][3] = hp[param2][3];
				hp_a[1][1] = hp[param2][5];
				hp_a[1][2] = hp[param2][6];
				hp_a[1][3] = hp[param2][7];

				rolloffFilter_b[0] = hp[param3][0];
				rolloffFilter_b[1] = hp[param3][1];
				rolloffFilter_b[2] = hp[param3][2];
				rolloffFilter_b[3] = hp[param3][3];
				rolloffFilter_a[1] = hp[param3][5];
				rolloffFilter_a[2] = hp[param3][6];
				rolloffFilter_a[3] = hp[param3][7];

#endif
				lpOutputPhaseInversion = 0;
				hpOutputPhaseInversion = 0;
			}
		}
#if(dbg >= 1)
		if(debugOutput) cout << endl;
#endif

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if (footswitchStatus[procEvent->footswitchNumber] == false)
		{
			for (i = 0; i < bufferSize; i++)
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
								0.0000;
				procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
								0.0000;
			}
		}
		else
#endif
		{
			for (i = 0; i < bufferSize; i++)
			{
				{
					if (inputCouplingMode == 0)
					{
						tempInput =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					}
					else if (inputCouplingMode == 1)
					{
						couplingFilter_x[0] =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]
										* couplingFilter_x[0]
										+ couplingFilter_b[1]
														* couplingFilter_x[1]
										+ couplingFilter_b[2]
														* couplingFilter_x[2]
										- couplingFilter_a[1]
														* couplingFilter_y[1]
										- couplingFilter_a[2]
														* couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}

					/************************** FIRST BAND DIVIDER (LOW/MID) *************************/
					j = 0;
					{

						lp_x[j][0] = tempInput; // get inputBuffer[0] data for BD0 from process input
						hp_x[j][0] = tempInput; // get input data for BD0 from process input

#if(FILTER_ORDER == 2)
						lp_y[j][0] = lp_b[j][0] * lp_x[j][0]
										+ lp_b[j][1] * lp_x[j][1]
										+ lp_b[j][2] * lp_x[j][2]
										- lp_a[j][1] * lp_y[j][1]
										- lp_a[j][2] * lp_y[j][2];
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

						if (lpOutputPhaseInversion < 50)
							procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
											-lp_y[j][0];
						else
							procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
											lp_y[j][0];

						outputSum[0] +=
										procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i];

#if(FILTER_ORDER == 2)
						hp_y[j][0] = hp_b[j][0] * hp_x[j][0]
										+ hp_b[j][1] * hp_x[j][1]
										+ hp_b[j][2] * hp_x[j][2]
										- hp_a[j][1] * hp_y[j][1]
										- hp_a[j][2] * hp_y[j][2];
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
						lp_y[j][0] = lp_b[j][0] * lp_x[j][0]
										+ lp_b[j][1] * lp_x[j][1]
										+ lp_b[j][2] * lp_x[j][2]
										- lp_a[j][1] * lp_y[j][1]
										- lp_a[j][2] * lp_y[j][2];

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

						procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
										lp_y[j][0];
						outputSum[1] +=
										procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i];

						hp_x[j][0] = tempInput;
#if(FILTER_ORDER == 2)
						hp_y[j][0] = hp_b[j][0] * hp_x[j][0]
										+ hp_b[j][1] * hp_x[j][1]
										+ hp_b[j][2] * hp_x[j][2]
										- hp_a[j][1] * hp_y[j][1]
										- hp_a[j][2] * hp_y[j][2];
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
						rolloffFilter_y[0] = rolloffFilter_b[0]
										* rolloffFilter_x[0]
										+ rolloffFilter_b[1]
														* rolloffFilter_x[1]
										+ rolloffFilter_b[2]
														* rolloffFilter_x[2]
										- rolloffFilter_a[1]
														* rolloffFilter_y[1]
										- rolloffFilter_a[2]
														* rolloffFilter_y[2];

						if (hpOutputPhaseInversion < 50)
							procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
											-rolloffFilter_y[0];
						else
							procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
											rolloffFilter_y[0];

						rolloffFilter_x[2] = rolloffFilter_x[1];
						rolloffFilter_x[1] = rolloffFilter_x[0];

						rolloffFilter_y[2] = rolloffFilter_y[1];
						rolloffFilter_y[1] = rolloffFilter_y[0];

					}
				}
			}
		}

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] =
							couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] =
							couplingFilter_y[j];
		}
		for (i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for (j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = lp_x[i][j];
				context[procEvent->processTypeIndex].lp_y[i][j] = lp_y[i][j];
				context[procEvent->processTypeIndex].hp_x[i][j] = hp_x[i][j];
				context[procEvent->processTypeIndex].hp_y[i][j] = hp_y[i][j];
			}
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].rolloffFilter_x[j] =
							rolloffFilter_x[j];
			context[procEvent->processTypeIndex].rolloffFilter_y[j] =
							rolloffFilter_y[j];
		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{
		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "filter3bb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::filter3bb: " << status << endl;
#endif
	return status;
}

#define dbg 0
int
filter3bb2 (char action, struct ProcessEvent *procEvent,
			array<ProcessSignalBuffer, 60> &procBufferArray,
			array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
			bool *footswitchStatus)
{
	static Filter3bb2Context context[20];
	int bufferSize = procEvent->bufferSize;
	int inputCouplingMode = procEvent->inputCouplingMode;
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::filter3bb2" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if (action == 'c')
	{
		componentVector.push_back (filter3bb2Symbol);
	}
	else if (action == 'l')
	{

#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading filter3bb2: " << procEvent->processTypeIndex
							<< endl;
#endif
		for (int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for (int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		for (int j = 0; j < 4; j++)
		{
			context[procEvent->processTypeIndex].lp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].lp_y[j] = 0.00000;
		}
		for (int j = 0; j < 7; j++)
		{
			context[procEvent->processTypeIndex].bp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].bp_y[j] = 0.00000;
		}
		for (int j = 0; j < 4; j++)
		{
			context[procEvent->processTypeIndex].hp_x[j] = 0.00000;
			context[procEvent->processTypeIndex].hp_y[j] = 0.00000;
		}
		for (int j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].rolloffFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].rolloffFilter_y[j] = 0.00000;
		}

		status = 0;
	}
	else if (action == 'r')
	{
		int i, j;
		double tempInput = 0;

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

		for (j = 0; j < 3; j++)
		{
			couplingFilter_x[j] =
							context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] =
							context[procEvent->processTypeIndex].couplingFilter_y[j];
		}

		for (j = 0; j < 4; j++)
		{
			lp_x[j] = context[procEvent->processTypeIndex].lp_x[j];
			lp_y[j] = context[procEvent->processTypeIndex].lp_y[j];
		}
		for (j = 0; j < 7; j++)
		{
			bp_x[j] = context[procEvent->processTypeIndex].bp_x[j];
			bp_y[j] = context[procEvent->processTypeIndex].bp_y[j];
		}
		for (j = 0; j < 4; j++)
		{
			hp_x[j] = context[procEvent->processTypeIndex].hp_x[j];
			hp_y[j] = context[procEvent->processTypeIndex].hp_y[j];
		}
		for (j = 0; j < 3; j++)
		{
			rolloffFilter_x[j] =
							context[procEvent->processTypeIndex].rolloffFilter_x[j];
			rolloffFilter_y[j] =
							context[procEvent->processTypeIndex].rolloffFilter_y[j];
		}

#if(dbg >= 1)
		if(debugOutput) cout << "filter3bb2 procEvent->parameters: ";
#endif

#if(dbg >= 1)
		if(debugOutput) cout << procEvent->parameterArray[i] << ", ";
#endif

		for (j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for (j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j + 3];
		}
		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;
		int param2 =
						paramContBufferArray[procEvent->parameterArray[1].paramContBufferIndex].parameterValueIndex;

#if(FILTER_ORDER == 2)
		lp_b[0] = lp2[param1][0];
		lp_b[1] = lp2[param1][1];
		lp_b[2] = lp2[param1][2];
		lp_a[1] = lp2[param1][4];
		lp_a[2] = lp2[param1][5];

		bp_b[0] = bp2[param1][0];
		bp_b[1] = bp2[param1][1];
		bp_b[2] = bp2[param1][2];
		bp_b[3] = bp2[param1][3];
		bp_b[4] = bp2[param1][4];
		bp_a[1] = bp2[param1][6];
		bp_a[2] = bp2[param1][7];
		bp_a[3] = bp2[param1][8];
		bp_a[4] = bp2[param1][9];

		hp_b[0] = hp2[param1][0];
		hp_b[1] = hp2[param1][1];
		hp_b[2] = hp2[param1][2];
		hp_a[1] = hp2[param1][4];
		hp_a[2] = hp2[param1][5];

		rolloffFilter_b[0] = lp2[param2][0];
		rolloffFilter_b[1] = lp2[param2][1];
		rolloffFilter_b[2] = lp2[param2][2];
		rolloffFilter_a[1] = lp2[param2][4];
		rolloffFilter_a[2] = lp2[param2][5];

#elif(FILTER_ORDER == 3)
		lp_b[0] = lp2[param1][0];
		lp_b[1] = lp2[param1][1];
		lp_b[2] = lp2[param1][2];
		lp_b[3] = lp2[param1][3];
		lp_a[1] = lp2[param1][5];
		lp_a[2] = lp2[param1][6];
		lp_a[3] = lp2[param1][7];

		bp_b[0] = bp2[param1][0];
		bp_b[1] = bp2[param1][1];
		bp_b[2] = bp2[param1][2];
		bp_b[3] = bp2[param1][3];
		bp_b[4] = bp2[param1][4];
		bp_b[5] = bp2[param1][5];
		bp_b[6] = bp2[param1][6];
		bp_a[1] = bp2[param1][8];
		bp_a[2] = bp2[param1][9];
		bp_a[3] = bp2[param1][10];
		bp_a[4] = bp2[param1][11];
		bp_a[5] = bp2[param1][12];
		bp_a[6] = bp2[param1][13];

		hp_b[0] = hp2[param1][0];
		hp_b[1] = hp2[param1][1];
		hp_b[2] = hp2[param1][2];
		hp_b[3] = hp2[param1][3];
		hp_a[1] = hp2[param1][5];
		hp_a[2] = hp2[param1][6];
		hp_a[3] = hp2[param1][7];

		rolloffFilter_b[0] = hp2[param2][0];
		rolloffFilter_b[1] = hp2[param2][1];
		rolloffFilter_b[2] = hp2[param2][2];
		rolloffFilter_b[3] = hp2[param2][3];
		rolloffFilter_a[1] = hp2[param2][5];
		rolloffFilter_a[2] = hp2[param2][6];
		rolloffFilter_a[3] = hp2[param2][7];

#endif

		lpOutputPhaseInversion = 0;
		hpOutputPhaseInversion = 0;

#if(dbg >= 1)
		if(debugOutput) cout << endl;
#endif

#if(FOOTSWITCH_ALWAYS_ON == 0)
		if (footswitchStatus[procEvent->footswitchNumber] == false)
		{
			for (i = 0; i < bufferSize; i++)
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
								0.0000;
				procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
								0.0000;
			}
		}
		else
#endif
		{
			for (i = 0; i < bufferSize; i++)
			{
				tempInput = 0;
				if (inputCouplingMode == 0)
				{
					tempInput =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				}
				else if (inputCouplingMode == 1)
				{
					couplingFilter_x[0] =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					couplingFilter_y[0] = couplingFilter_b[0]
									* couplingFilter_x[0]
									+ couplingFilter_b[1] * couplingFilter_x[1]
									+ couplingFilter_b[2] * couplingFilter_x[2]
									- couplingFilter_a[1] * couplingFilter_y[1]
									- couplingFilter_a[2] * couplingFilter_y[2];

					tempInput = couplingFilter_y[0];

					couplingFilter_x[2] = couplingFilter_x[1];
					couplingFilter_x[1] = couplingFilter_x[0];

					couplingFilter_y[2] = couplingFilter_y[1];
					couplingFilter_y[1] = couplingFilter_y[0];
				}

				lp_x[0] = tempInput;
				bp_x[0] = tempInput;
				hp_x[0] = tempInput;
				//*********************Band Divider (BD): Lowpass filter ********************

#if(FILTER_ORDER == 2)
				lp_y[0] = lp_b[0] * lp_x[0] + lp_b[1] * lp_x[1]
								+ lp_b[2] * lp_x[2] - lp_a[1] * lp_y[1]
								- lp_a[2] * lp_y[2];

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

				if (lpOutputPhaseInversion < 50)
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									-lp_y[0];
				else
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									lp_y[0];

				//*********************** Bandpass filter ****************************
#if(FILTER_ORDER == 2)
				bp_y[0] = bp_b[0] * bp_x[0] + bp_b[1] * bp_x[1]
								+ bp_b[2] * bp_x[2] + bp_b[3] * bp_x[3]
								+ bp_b[4] * bp_x[4] - bp_a[1] * bp_y[1]
								- bp_a[2] * bp_y[2] - bp_a[3] * bp_y[3]
								- bp_a[4] * bp_y[4];

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
				procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
								bp_y[0];

				//*********************Band Divider (BD): Highpass filter ********************

#if(FILTER_ORDER == 2)
				hp_y[0] = hp_b[0] * hp_x[0] + hp_b[1] * hp_x[1]
								+ hp_b[2] * hp_x[2] - hp_a[1] * hp_y[1]
								- hp_a[2] * hp_y[2];

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
				rolloffFilter_y[0] = rolloffFilter_b[0] * rolloffFilter_x[0]
								+ rolloffFilter_b[1] * rolloffFilter_x[1]
								+ rolloffFilter_b[2] * rolloffFilter_x[2]
								- rolloffFilter_a[1] * rolloffFilter_y[1]
								- rolloffFilter_a[2] * rolloffFilter_y[2];

				if (hpOutputPhaseInversion < 50)
					procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
									-rolloffFilter_y[0];
				else
					procBufferArray[procEvent->outputConnectedBufferIndexArray[2]].buffer[i] =
									rolloffFilter_y[0];

				rolloffFilter_x[2] = rolloffFilter_x[1];
				rolloffFilter_x[1] = rolloffFilter_x[0];

				rolloffFilter_y[2] = rolloffFilter_y[1];
				rolloffFilter_y[1] = rolloffFilter_y[0];

			}
		}

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] =
							couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] =
							couplingFilter_y[j];
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].lp_x[j] = lp_x[j];
			context[procEvent->processTypeIndex].lp_y[j] = lp_y[j];
		}
		for (j = 0; j < 5; j++)
		{
			context[procEvent->processTypeIndex].bp_x[j] = bp_x[j];
			context[procEvent->processTypeIndex].bp_y[j] = bp_y[j];
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].hp_x[j] = hp_x[j];
			context[procEvent->processTypeIndex].hp_y[j] = hp_y[j];
		}

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].rolloffFilter_x[j] =
							rolloffFilter_x[j];
			context[procEvent->processTypeIndex].rolloffFilter_y[j] =
							rolloffFilter_y[j];
		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{
		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "filter3bb2: invalid actiond: " << action << std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::filter3bb2: " << status << endl;
#endif
	return status;

}

#define dbg 0
int
lohifilterb (char action, struct ProcessEvent *procEvent,
			 array<ProcessSignalBuffer, 60> &procBufferArray,
			 array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
			 bool *footswitchStatus)
{
	int i, j;
	static LohifilterbContext context[20];
	int bufferSize = procEvent->bufferSize;
	int inputCouplingMode = procEvent->inputCouplingMode;
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::lohifilterb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if (action == 'c')
	{
		componentVector.push_back (lohifilterbSymbol);
	}
	else if (action == 'l')
	{

#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading lohifilterb: " << procEvent->processTypeIndex
							<< endl;
#endif

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.00000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.00000;
		}
		i = 0;
		{
			for (j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].lp_y[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_x[i][j] = 0.00000;
				context[procEvent->processTypeIndex].hp_y[i][j] = 0.00000;
			}
		}

		status = 0;
	}
	else if (action == 'r')
	{

		double lp_b[NUMBER_OF_BANDS][4], lp_a[NUMBER_OF_BANDS][4];
		double hp_b[NUMBER_OF_BANDS][4], hp_a[NUMBER_OF_BANDS][4];

		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
		double couplingFilter_y[3], couplingFilter_x[3];
		double couplingFilter_b[3], couplingFilter_a[3];
		double tempInput = 0;

		for (j = 0; j < 3; j++)
		{
			couplingFilter_x[j] =
							context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] =
							context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		i = 0;
		{
			for (j = 0; j < 4; j++)
			{
				lp_x[i][j] = context[procEvent->processTypeIndex].lp_x[i][j];
				lp_y[i][j] = context[procEvent->processTypeIndex].lp_y[i][j];
				hp_x[i][j] = context[procEvent->processTypeIndex].hp_x[i][j];
				hp_y[i][j] = context[procEvent->processTypeIndex].hp_y[i][j];
			}
		}

		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;

		int tempIndex = param1;

		for (j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for (j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j + 3];
		}

#if(dbg >= 1)
		if(debugOutput) cout << "lohifilterb procEvent->parameters: ";
#endif

		i = 0;
		{
#if(dbg >= 1)
			if(debugOutput) cout << procEvent->parameterArray[i] << ", ";
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

			for (i = 0; i < bufferSize; i++)
			{
#if(FOOTSWITCH_ALWAYS_ON == 0)
				if (footswitchStatus[procEvent->footswitchNumber] == false)
				{
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				}
				else
#endif
				{
					if (inputCouplingMode == 0)
					{
						tempInput =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					}
					else if (inputCouplingMode == 1)
					{
						couplingFilter_x[0] =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]
										* couplingFilter_x[0]
										+ couplingFilter_b[1]
														* couplingFilter_x[1]
										+ couplingFilter_b[2]
														* couplingFilter_x[2]
										- couplingFilter_a[1]
														* couplingFilter_y[1]
										- couplingFilter_a[2]
														* couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}

					/********************* Lowpass filter ********************/
					lp_x[j][0] = tempInput;
#if(FILTER_ORDER == 2)
					lp_y[j][0] = lp_b[j][0] * lp_x[j][0]
									+ lp_b[j][1] * lp_x[j][1]
									+ lp_b[j][2] * lp_x[j][2]
									- lp_a[j][1] * lp_y[j][1]
									- lp_a[j][2] * lp_y[j][2];
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
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									lp_y[j][0];

					/********************* Highpass filter ********************/
					hp_x[j][0] = tempInput;

#if(FILTER_ORDER == 2)
					hp_y[j][0] = hp_b[j][0] * hp_x[j][0]
									+ hp_b[j][1] * hp_x[j][1]
									+ hp_b[j][2] * hp_x[j][2]
									- hp_a[j][1] * hp_y[j][1]
									- hp_a[j][2] * hp_y[j][2];
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

					procBufferArray[procEvent->outputConnectedBufferIndexArray[1]].buffer[i] =
									hp_y[j][0];
				}
			}
		}

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] =
							couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] =
							couplingFilter_y[j];
		}
		i = 0;
		{
			for (j = 0; j < 4; j++)
			{
				context[procEvent->processTypeIndex].lp_x[i][j] = lp_x[i][j];
				context[procEvent->processTypeIndex].lp_y[i][j] = lp_y[i][j];
				context[procEvent->processTypeIndex].hp_x[i][j] = hp_x[i][j];
				context[procEvent->processTypeIndex].hp_y[i][j] = hp_y[i][j];
			}
		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{
		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "lohifilterb: invalid actiond: " << action
							<< std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::lohifilterb: " << status << endl;
#endif
	return status;
}

#define dbg 0
int
mixerb (char action, struct ProcessEvent *procEvent,
		array<ProcessSignalBuffer, 60> &procBufferArray,
		array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
		bool *footswitchStatus)
{
	int i;
	int status = 0;
	int bufferSize = procEvent->bufferSize;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::mixerb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if (action == 'c')
	{
		componentVector.push_back (mixerbSymbol);
	}
	else if (action == 'l')
	{
#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading mixerb: " << procEvent->processTypeIndex << endl;
#endif

		status = 0;
	}
	else if (action == 'r')
	{

		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;
		int param2 =
						paramContBufferArray[procEvent->parameterArray[1].paramContBufferIndex].parameterValueIndex;
		int param3 =
						paramContBufferArray[procEvent->parameterArray[2].paramContBufferIndex].parameterValueIndex;
		int param4 =
						paramContBufferArray[procEvent->parameterArray[3].paramContBufferIndex].parameterValueIndex;
		double level1 = logAmp[param1];
		double level2 = logAmp[param2];
		double level3 = logAmp[param3];
		double levelOut = logAmp[param4];

		for (i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if (footswitchStatus[procEvent->footswitchNumber] == false)
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
			}
			else
#endif
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								levelOut
												* (level1
																* (procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i])
																+ level2
																				* (procBufferArray[procEvent->inputConnectedBufferIndexArray[1]].buffer[i])
																+ level3
																				* (procBufferArray[procEvent->inputConnectedBufferIndexArray[2]].buffer[i]));
			}

		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{

		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "mixerb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::mixerb: " << status << endl;
#endif
	return status;
}

int
volumeb (char action, struct ProcessEvent *procEvent,
		 array<ProcessSignalBuffer, 60> &procBufferArray,
		 array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
		 bool *footswitchStatus)
{
	int i;
	int status = 0;
	int bufferSize = procEvent->bufferSize;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::volumeb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	if (action == 'c')
	{
		componentVector.push_back (volumebSymbol);
	}
	else if (action == 'l')
	{
#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading volumeb: " << procEvent->processTypeIndex
							<< endl;
#endif

		status = 0;
	}
	else if (action == 'r')
	{
		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;
		double vol = logAmp[param1];

		for (i = 0; i < bufferSize; i++)
		{
#if(FOOTSWITCH_ALWAYS_ON == 0)
			if (footswitchStatus[procEvent->footswitchNumber] == false)
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
			}
			else
#endif
			{
				procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
								vol
												* (procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i]);
			}
		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{

		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "volumeb: invalid actiond: " << action << std::endl;
		status = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::volumeb: " << status << endl;
#endif
	return status;
}

int
waveshaperb (char action, struct ProcessEvent *procEvent,
			 array<ProcessSignalBuffer, 60> &procBufferArray,
			 array<ProcessParameterControlBuffer, 60> &paramContBufferArray,
			 bool *footswitchStatus)

{
	int i, j;

	int status = 0;
	int bufferSize = procEvent->bufferSize;
	int antiAliasingNumber = procEvent->antiAliasingNumber;
	int waveshaperMode = procEvent->waveshaperMode;
	int inputCouplingMode = procEvent->inputCouplingMode;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: Effects::waveshaperb" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	const double divider = 5.5;
	const double r1 = 1000.0;
	const double r2 = 2000.0;
	double r3 = r2 / divider;
	double r4 = r3 / divider;
	double r5 = r4 / divider;
	double r6 = r5 / divider;

	static WaveshaperbContext context[20];

	if (action == 'c')
	{
		componentVector.push_back (waveshaperbSymbol);
	}
	else if (action == 'l')
	{

#if(dbg >= 0)
		if (debugOutput)
			cout << "***** loading waveshaperb: " << procEvent->processTypeIndex
							<< endl;
#endif

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] = 0.0000;
			context[procEvent->processTypeIndex].couplingFilter_y[j] = 0.0000;
		}
		context[procEvent->processTypeIndex].outMeasure = 0.0000; // outMeasure

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j] =
							0.0000;
			context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j] =
							0.0000;
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j] =
							0.0000;
			context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j] =
							0.0000;
		}

		status = 0;
	}
	else if (action == 'r')
	{
		double k[6][6];
		double v[5];
		double x[6], y[6];
		double m[5], b[5];
		double tempInput = 0;
		double outMeasure;

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
		int upSampleBufferSize = antiAliasingNumber * bufferSize;

		for (j = 0; j < 3; j++)
		{
			couplingFilter_x[j] =
							context[procEvent->processTypeIndex].couplingFilter_x[j];
			couplingFilter_y[j] =
							context[procEvent->processTypeIndex].couplingFilter_y[j];
		}
		outMeasure = context[procEvent->processTypeIndex].outMeasure;

		for (j = 0; j < 3; j++)
		{
			antiAliasingFilter_x[0][j] =
							context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j];
			antiAliasingFilter_y[0][j] =
							context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j];
		}
		for (j = 0; j < 3; j++)
		{
			antiAliasingFilter_x[1][j] =
							context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j];
			antiAliasingFilter_y[1][j] =
							context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j];
		}
		for (j = 0; j < 3; j++)
		{
			noiseFilter_x[j] =
							context[procEvent->processTypeIndex].noiseFilter_x[j];
			noiseFilter_y[j] =
							context[procEvent->processTypeIndex].noiseFilter_y[j];
		}

		if (waveshaperMode == 0)
		{
			k[5][0] = r2 * r3 * r4 * r5 * r6;
			k[5][1] = r1 * r3 * r4 * r5 * r6;
			k[5][2] = r1 * r2 * r4 * r5 * r6;
			k[5][3] = r1 * r2 * r3 * r5 * r6;
			k[5][4] = r1 * r2 * r3 * r4 * r6;
			k[5][5] = r1 * r2 * r3 * r4 * r5;

			k[4][0] = r2 * r3 * r4 * r5;
			k[4][1] = r1 * r3 * r4 * r5;
			k[4][2] = r1 * r2 * r4 * r5;
			k[4][3] = r1 * r2 * r3 * r5;
			k[4][4] = r1 * r2 * r3 * r4;
			k[4][5] = 0.0000;

			k[3][0] = r2 * r3 * r4;
			k[3][1] = r1 * r3 * r4;
			k[3][2] = r1 * r2 * r4;
			k[3][3] = r1 * r2 * r3;
			k[3][4] = 0.000;
			k[3][5] = 0.000;

			k[2][0] = r2 * r3;
			k[2][1] = r1 * r3;
			k[2][2] = r1 * r2;
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

			for (j = 0; j < 5; j++)
			{
				m[j] = (y[j + 1] - y[j]) / (x[j + 1] - x[j]);
				b[j] = y[j] - m[j] * x[j];
#if(dbg >= 2)
				if(debugOutput) cout << "m[" << j << "] = " << m[j] << "\tb[" << j << "] = " << b[j] << endl;
#endif
			}
		}

		for (j = 0; j < 3; j++)
		{
			couplingFilter_b[j] = couplingFilter[j];
		}
		for (j = 0; j < 3; j++)
		{
			couplingFilter_a[j] = couplingFilter[j + 3];
		}
		for (j = 0; j < 3; j++)
		{
			antiAliasingFilter_b[j] = antiAliasingFilter[j];
		}
		for (j = 0; j < 3; j++)
		{
			antiAliasingFilter_a[j] = antiAliasingFilter[j + 3];
		}
		for (j = 0; j < 3; j++)
		{
			noiseFilter_b[j] = noiseFilter[j];
		}
		for (j = 0; j < 3; j++)
		{
			noiseFilter_a[j] = noiseFilter[j + 3];
		}

		int param1 =
						paramContBufferArray[procEvent->parameterArray[0].paramContBufferIndex].parameterValueIndex;
		int param2 =
						paramContBufferArray[procEvent->parameterArray[1].paramContBufferIndex].parameterValueIndex;
		int param3 =
						paramContBufferArray[procEvent->parameterArray[2].paramContBufferIndex].parameterValueIndex;

		int edge = param2;
		v[0] = brkpt[edge][0];
		v[1] = brkpt[edge][1];
		v[2] = brkpt[edge][2];
		v[3] = brkpt[edge][3];
		v[4] = brkpt[edge][4];

		double gain = logAmp[param1] * 5.0;
		double mix = linAmp[param3] * 0.1;
		double k1, k2, k3, k4, k5, k6;
		double v1, v2, v3, v4, v5;
		double clean , dist;
		bool kIndexFound = false;
		int kIndex = 0;

		if (antiAliasingNumber > 1)
		{
			if (footswitchStatus[procEvent->footswitchNumber] == false)
			{
				for (i = 0; i < bufferSize; i++)
				{
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				}
			}
			else
			{
				for (i = 0; i < bufferSize; i++)
				{
					if (inputCouplingMode == 0)
					{
						tempInput =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					}
					else if (inputCouplingMode == 1)
					{
						couplingFilter_x[0] =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]
										* couplingFilter_x[0]
										+ couplingFilter_b[1]
														* couplingFilter_x[1]
										+ couplingFilter_b[2]
														* couplingFilter_x[2]
										- couplingFilter_a[1]
														* couplingFilter_y[1]
										- couplingFilter_a[2]
														* couplingFilter_y[2];

						coupledBuffer[i] = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}
				}

				for (i = 0; i < bufferSize; i++)
				{
					upSampleBuffer[antiAliasingNumber * i] = coupledBuffer[i]
									* antiAliasingNumber;
					for (j = 1; j < antiAliasingNumber; j++)
						upSampleBuffer[antiAliasingNumber * i + j] = 0.00000000;
				}

#if(UP_SAMPLE_FILTER == 1)
				for (i = 0; i < upSampleBufferSize; i++)
				{

					antiAliasingFilter_x[0][0] = upSampleBuffer[i];
					antiAliasingFilter_y[0][0] =
									antiAliasingFilter_b[0]
													* antiAliasingFilter_x[0][0]
													+ antiAliasingFilter_b[1]
																	* antiAliasingFilter_x[0][1]
													+ antiAliasingFilter_b[2]
																	* antiAliasingFilter_x[0][2]
													- antiAliasingFilter_a[1]
																	* antiAliasingFilter_y[0][1]
													- antiAliasingFilter_a[2]
																	* antiAliasingFilter_y[0][2];
					filteredUpSampleBuffer[i] = antiAliasingFilter_y[0][0];
					antiAliasingFilter_x[0][2] = antiAliasingFilter_x[0][1];
					antiAliasingFilter_x[0][1] = antiAliasingFilter_x[0][0];

					antiAliasingFilter_y[0][2] = antiAliasingFilter_y[0][1];
					antiAliasingFilter_y[0][1] = antiAliasingFilter_y[0][0];
				}
#endif

				for (i = 0; i < upSampleBufferSize; i++)
				{
#if(UP_SAMPLE_FILTER == 1)
					tempInput = filteredUpSampleBuffer[i];
#else
					tempInput = upSampleBuffer[i];
#endif

					if (waveshaperMode == 0)
					{
						kIndexFound = false;
						if (0.000 <= tempInput)
						{
							v1 = v[0];
							v2 = v[1];
							v3 = v[2];
							v4 = v[3];
							v5 = v[4];

							while (kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];
								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);

								if (kIndex == 0)
								{
									if (dist > v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if (kIndex == 5)
								{
									if (dist < v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if (dist > v[kIndex])
										kIndex++;
									else if (dist < v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);
							}
						}
						else
						{
							v1 = -v[0];
							v2 = -v[1];
							v3 = -v[2];
							v4 = -v[3];
							v5 = -v[4];
							while (kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];

								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);

								if (kIndex == 0)
								{
									if (dist < -v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if (kIndex == 5)
								{
									if (dist > -v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if (dist < -v[kIndex])
										kIndex++;
									else if (dist > -v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);
							}
						}
					}

					clean = tempInput;

#if(DOWN_SAMPLE_FILTER == 1)
					antiAliasingFilter_x[1][0] = ((1.0 - mix)*clean + mix*dist);
					antiAliasingFilter_y[1][0] = antiAliasingFilter_b[0]*antiAliasingFilter_x[1][0] + antiAliasingFilter_b[1]*antiAliasingFilter_x[1][1] + antiAliasingFilter_b[2]*antiAliasingFilter_x[1][2] - antiAliasingFilter_a[1]*antiAliasingFilter_y[1][1] - antiAliasingFilter_a[2]*antiAliasingFilter_y[1][2];
					downSampleBuffer[i] = antiAliasingFilter_y[1][0];

					antiAliasingFilter_x[1][2] = antiAliasingFilter_x[1][1];
					antiAliasingFilter_x[1][1] = antiAliasingFilter_x[1][0];
					antiAliasingFilter_y[1][2] = antiAliasingFilter_y[1][1];
					antiAliasingFilter_y[1][1] = antiAliasingFilter_y[1][0];
#else
					downSampleBuffer[i] = ((1.0 - mix) * clean + mix * dist);
#endif
				}

				for (i = 0; i < bufferSize; i++)
				{
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									downSampleBuffer[antiAliasingNumber * i];
				}

			}
		}
		else
		{
			for (i = 0; i < bufferSize; i++)
			{
#if(FOOTSWITCH_ALWAYS_ON == 0)
				if (footswitchStatus[procEvent->footswitchNumber] == false)
				{
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
				}
				else
#endif
				{
					if (inputCouplingMode == 0)
					{
						tempInput =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
					}
					else if (inputCouplingMode == 1)
					{
						couplingFilter_x[0] =
										procBufferArray[procEvent->inputConnectedBufferIndexArray[0]].buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0]
										* couplingFilter_x[0]
										+ couplingFilter_b[1]
														* couplingFilter_x[1]
										+ couplingFilter_b[2]
														* couplingFilter_x[2]
										- couplingFilter_a[1]
														* couplingFilter_y[1]
										- couplingFilter_a[2]
														* couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}

					if (waveshaperMode == 0)
					{
						kIndexFound = false;
						if (0.000 <= tempInput)
						{
							v1 = v[0];
							v2 = v[1];
							v3 = v[2];
							v4 = v[3];
							v5 = v[4];

							while (kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];
								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);

								if (kIndex == 0)
								{
									if (dist > v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if (kIndex == 5)
								{
									if (dist < v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if (dist > v[kIndex])
										kIndex++;
									else if (dist < v[kIndex - 1])
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
							while (kIndexFound == false)
							{
								k1 = k[kIndex][0];
								k2 = k[kIndex][1];
								k3 = k[kIndex][2];
								k4 = k[kIndex][3];
								k5 = k[kIndex][4];
								k6 = k[kIndex][5];

								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);

								if (kIndex == 0)
								{
									if (dist < -v[kIndex])
										kIndex++;
									else
										kIndexFound = true;
								}
								else if (kIndex == 5)
								{
									if (dist > -v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								else
								{
									if (dist < -v[kIndex])
										kIndex++;
									else if (dist > -v[kIndex - 1])
										kIndex--;
									else
										kIndexFound = true;
								}
								dist = (tempInput * gain * k1 + v1 * k2
												+ v2 * k3 + v3 * k4 + v4 * k5
												+ v5 * k6)
												/ (k1 + k2 + k3 + k4 + k5 + k6);
							}
						}
					}
					else
					{
						if (tempInput * gain >= 0.0000)
						{
							if (tempInput * gain <= x[1])
								dist = tempInput * gain * m[0] + b[0];
							else if (tempInput * gain <= x[2])
								dist = tempInput * gain * m[1] + b[1];
							else if (tempInput * gain <= x[3])
								dist = tempInput * gain * m[2] + b[2];
							else if (tempInput * gain <= x[4])
								dist = tempInput * gain * m[3] + b[3];
							else
								dist = tempInput * gain * m[4] + b[4];
						}
						else
						{
							if (tempInput * gain >= -x[1])
								dist = tempInput * gain * m[0] - b[0];
							else if (tempInput * gain >= -x[2])
								dist = tempInput * gain * m[1] - b[1];
							else if (tempInput * gain >= -x[3])
								dist = tempInput * gain * m[2] - b[2];
							else if (tempInput * gain >= -x[4])
								dist = tempInput * gain * m[3] - b[3];
							else
								dist = tempInput * gain * m[4] - b[4];
						}
					}

					clean = tempInput;

					noiseFilter_x[0] = 0.1 * ((1.0 - mix) * clean + mix * dist);
					noiseFilter_y[0] = noiseFilter_b[0] * noiseFilter_x[0]
									+ noiseFilter_b[1] * noiseFilter_x[1]
									+ noiseFilter_b[2] * noiseFilter_x[2]
									- noiseFilter_a[1] * noiseFilter_y[1]
									- noiseFilter_a[2] * noiseFilter_y[2];
					procBufferArray[procEvent->outputConnectedBufferIndexArray[0]].buffer[i] =
									noiseFilter_y[0];

					noiseFilter_x[2] = noiseFilter_x[1];
					noiseFilter_x[1] = noiseFilter_x[0];

					noiseFilter_y[2] = noiseFilter_y[1];
					noiseFilter_y[1] = noiseFilter_y[0];

				}

			}
		}

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].couplingFilter_x[j] =
							couplingFilter_x[j];
			context[procEvent->processTypeIndex].couplingFilter_y[j] =
							couplingFilter_y[j];
		}
		context[procEvent->processTypeIndex].outMeasure = outMeasure; // outMeasure

		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[0][j] =
							antiAliasingFilter_x[0][j];
			context[procEvent->processTypeIndex].antiAliasingFilter_y[0][j] =
							antiAliasingFilter_y[0][j];
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].antiAliasingFilter_x[1][j] =
							antiAliasingFilter_x[1][j];
			context[procEvent->processTypeIndex].antiAliasingFilter_y[1][j] =
							antiAliasingFilter_y[1][j];
		}
		for (j = 0; j < 3; j++)
		{
			context[procEvent->processTypeIndex].noiseFilter_x[j] =
							noiseFilter_x[j];
			context[procEvent->processTypeIndex].noiseFilter_y[j] =
							noiseFilter_y[j];
		}

		status = 0;
	}
	else if (action == 'd')
	{
		status = 0;
	}
	else if (action == 's')
	{
		status = 0;
	}
	else
	{
		if (debugOutput)
			std::cout << "waveshaperb: invalid actiond: " << action
							<< std::endl;
		status = 0;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: Effects::waveshaperb: " << status << endl;
#endif
	return status;
}
