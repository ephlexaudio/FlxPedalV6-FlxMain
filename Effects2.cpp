/*
 * Effects.cpp
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */

#include "Effects2.h"


#define BUFFER_SIZE 1024
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

// set

extern unsigned int bufferSize;
extern int inputCouplingMode;
#define dbg 1

int resetProcBuffer(struct ProcessBuffer procBufferArray)
{
	int status = 0;

	procBufferArray.processed = 0;
	procBufferArray.ready = 0;

	return status;
}


int setProcBuffer(struct ProcessBuffer procBufferArray, int processed, int ready)
{
	int status = 0;

	procBufferArray.processed = processed;
	procBufferArray.ready = ready;

	return status;
}

int initProcBuffers(struct ProcessBuffer *procBufferArray)
{
	int status = 0;

	for(int i = 0; i < 60; i++)
	{
		resetProcBuffer(procBufferArray[i]);
	}

	return status;
}

#define dbg 1
int setProcData(struct ProcessEvent *procEvent, Process processStruct)
{
	int status = 0;
	string processName;
	volatile int processType = 0;
	//volatile int processIndex = 0;
	volatile int footswitchNumber = 0;
	volatile int processInputCount = 0;
	volatile int processOutputCount = 0;

	if(processStruct.type.compare("delayb") == 0) processType = 0;
	else if(processStruct.type.compare("filter3bb") == 0) processType = 1;
	else if(processStruct.type.compare("filter3bb2") == 0) processType = 2;
	else if(processStruct.type.compare("lohifilterb") == 0) processType = 3;
	else if(processStruct.type.compare("mixerb") == 0) processType = 4;
	else if(processStruct.type.compare("volumeb") == 0) processType = 5;
	else if(processStruct.type.compare("waveshaperb") == 0) processType = 6;

	processName = processStruct.name;
	footswitchNumber = processStruct.footswitchNumber;
	processInputCount = processStruct.inputs.size();
	processOutputCount = processStruct.outputs.size();
#if(dbg >= 2)
	std::cout << "ProcessingControl process name: " << processName << std::endl;
	//std::cout << "processType: " << processType << std::endl;
	std::cout << "footswitch number: " << footswitchNumber << std::endl;
	std::cout << "process type index: " << atoi(processName.substr(processName.find("_")+1).c_str()) << std::endl;
	std::cout << "processInputCount: " << processInputCount << std::endl;
	std::cout << "processOutputCount: " << processOutputCount << std::endl;
	std::cout <<  std::endl;
#endif


	procEvent->processName = processName;
	procEvent->processType = processType;
	procEvent->footswitchNumber = footswitchNumber-1;
	procEvent->processInputCount = processInputCount;

#if(dbg >= 2)
	cout << "procEvent->inputBufferNames: ";
#endif
	for(int processInputIndex = 0; processInputIndex < procEvent->processInputCount; processInputIndex++)
	{
		procEvent->inputBufferNames.push_back(processStruct.inputs[processInputIndex].c_str());
#if(dbg >= 2)
		cout << procEvent->inputBufferNames[processInputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	cout << endl;
	cout << "procEvent->outputBufferNames: ";
#endif

	procEvent->processOutputCount = processOutputCount;

	for(int processOutputIndex = 0; processOutputIndex < procEvent->processOutputCount; processOutputIndex++)
	{
		procEvent->outputBufferNames.push_back(processStruct.outputs[processOutputIndex].c_str());
#if(dbg >= 2)
		cout << procEvent->outputBufferNames[processOutputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	cout << endl;
#endif

	return status;
}

#define dbg 0
int setProcParameters(struct ProcessEvent *procEvent, Process processStruct)
{
	int status = 0;


	volatile int paramArrayCount = processStruct.params.size();

	for(int paramArrayIndex = 0; paramArrayIndex < paramArrayCount; paramArrayIndex++)
	{
		procEvent->parameters[paramArrayIndex] = processStruct.params[paramArrayIndex].value;

#if(dbg == 1)
		std::cout << "parameter[" << paramArrayIndex << "]: " << procEvent->parameters[paramArrayIndex] << std::endl;
#endif
	}

	return status;
}



int initProcInputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;

	for(int procInputIndex = 0; procInputIndex < procEvent->processInputCount; procInputIndex++)
	{
		procEvent->inputBufferIndexes[procInputIndex] = 58;
	}

	return status;
}

int initProcOutputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;

	for(int procOutputIndex = 0; procOutputIndex < procEvent->processOutputCount; procOutputIndex++)
	{
		procEvent->outputBufferIndexes[procOutputIndex] = 59;
	}

	return status;
}

#define dbg 0
/*int setProcInputBufferIndex(struct ProcessEvent *procEvent, int processInputIndex, int inputBufferIndex, struct ProcessBuffer *procBufferArray)
{
	int status = 0;

	// set the process input to read from the buffer
	procEvent->inputBufferIndexes[processInputIndex] = inputBufferIndex;

#if(dbg == 1)
	std::cout << procEvent->processName << " input buffer: ";
	std::cout << procEvent->inputBufferIndexes[processInputIndex] << endl;
#endif


	return status;
}*/


/*#define dbg 0
int setProcOutputBufferIndex(struct ProcessEvent *procEvent, int processOutputIndex, int outputBufferIndex, struct ProcessBuffer *procBufferArray)
{
	int status = 0;

	// set the process output to write into the buffer
	procEvent->outputBufferIndexes[processOutputIndex] = outputBufferIndex;
	procEvent->outputBufferNames.push_back(procBufferArray[outputBufferIndex].portName);

#if(dbg == 1)
	std::cout << procEvent->processName << " output buffer: ";
	std::cout << procEvent->outputBufferIndexes[processOutputIndex] << endl;
#endif

	return status;
}*/

#define dbg 1

int control(int action, bool gate, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent)
{
	int targetParameterValueIndex = 0;

	if(action == 0)
	{
		if(controlEvent->type == 0)
		{

		}
		else if(controlEvent->type == 1)
		{
			//int attack = controlEvent->parameter[0];
			//int peakValueIndex = 50;//controlEvent->parameter[4];

			controlEvent->controlContext = (EnvGenContext *)calloc(1, sizeof(EnvGenContext));
			if(controlEvent->controlContext == NULL)
			{
				std::cout << "EnvGenContext control calloc failed." << std::endl;
			}
			else
			{
				std::cout << "EnvGenContext control calloc succeeded." << std::endl;
				((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
			}
		}
		else if(controlEvent->type == 2)
		{
			controlEvent->controlContext = (LfoContext *)calloc(1, sizeof(LfoContext));
			if(controlEvent->controlContext == NULL)
			{
				std::cout << "LfoContext control calloc failed." << std::endl;
			}
			else
			{
				std::cout << "LfoContext control calloc succeeded." << std::endl;
				((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = 0;
			}
		}
	}
	else if(action == 1)
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
			if(controlEvent->controlContext == NULL)
			{
				cout << "audioCallback control context not allocated" << endl;
				return -1;
			}

			if(gate == true && controlEvent->envTriggerStatus == false) // pick detected
			{
				((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
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


			int stageTimeValue = ((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue;
			double slewRate = ((EnvGenContext *)(controlEvent->controlContext))->slewRate;
			int attack = controlEvent->parameter[0];
			int decay = controlEvent->parameter[1];
			//int sustain = controlEvent->parameter[2];
			//int release = controlEvent->parameter[3];
			int attackPeakValueIndex = 100;//controlEvent->parameter[4];
			int decayBottomValueIndex = 10;//controlEvent->parameter[5];
			//int internalOutput = 50*controlEvent->output;

			switch(((EnvGenContext *)(controlEvent->controlContext))->envStage)
			{
			case 0: // idle
				controlEvent->output = 0.0;
				// pick detected, go to attack
				if(controlEvent->envTriggerStatus == true)
				{
					((EnvGenContext *)(controlEvent->controlContext))->envStage++;
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
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 2;
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
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 1;
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
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
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



			((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = stageTimeValue;
			((EnvGenContext *)(controlEvent->controlContext))->slewRate = slewRate;
		}
		else if(controlEvent->type == 2) // LFO
		{
			if(controlEvent->controlContext == NULL)
			{
				cout << "audioCallback control context not allocated" << endl;
				return -1;
			}

			unsigned int cycleTimeValueIndex = ((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex;
			double cyclePositionValue = ((LfoContext *)(controlEvent->controlContext))->cyclePositionValue;
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
			((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = cycleTimeValueIndex;
			((LfoContext *)(controlEvent->controlContext))->cyclePositionValue = cyclePositionValue;
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
	else if(action == 2)
	{

	}
	else if(action == 3)
	{
		free(controlEvent->controlContext);
	}
	else
	{

	}

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

int delayb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processContext = (DelayContext *)calloc(1, sizeof(DelayContext));
		if(procEvent->processContext == NULL)
		{
			std::cout << "delayb calloc failed." << std::endl;
		}
		else
		{
			std::cout << "delayb calloc succeeded." << std::endl;
			((DelayContext *)(procEvent->processContext))->inputPtr = 0;
			((DelayContext *)(procEvent->processContext))->outputPtr = 0;
			((DelayContext *)(procEvent->processContext))->delayBuffer[0]=0.0;

		}
		procEvent->processFinished = false;

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		return 0;
	}
	else if(action == 1)
	{
		//double internalPosPeak = 0.0;
		//double internalNegPeak = 0.0;
		double outputSum = 0.00000;
		double tempInput;
		unsigned int delayCoarse = 0 ;
		unsigned int delayFine = 0 ;
		unsigned int delay = 0;

		//double gain, breakpoint, edge;

		if(procEvent->processContext == NULL)
		{
			cout << "audioCallback delayb context not allocated" << endl;
			return -1;
		}

		unsigned int i;
		unsigned int inputPtr = ((DelayContext *)(procEvent->processContext))->inputPtr;
		unsigned int outputPtr = ((DelayContext *)(procEvent->processContext))->outputPtr;


		delayCoarse = delayTime[procEvent->parameters[0]];
		delayFine = delayTime[procEvent->parameters[1]]/100;
		delay = delayCoarse + delayFine;

		if(inputPtr >= delay) outputPtr = inputPtr - delay;
		else outputPtr = DELAY_BUFFER_LENGTH - (delay - inputPtr);

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
				((DelayContext *)(procEvent->processContext))->delayBuffer[inputPtr++] = 0.0;
			}
			else
#endif
			{
				/*if(inputCouplingMode == 0)
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
				}*/

				tempInput = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];

				((DelayContext *)(procEvent->processContext))->delayBuffer[inputPtr] = tempInput;

				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = ((DelayContext *)(procEvent->processContext))->delayBuffer[outputPtr];

				if(inputPtr >= DELAY_BUFFER_LENGTH) inputPtr = 0;
				else inputPtr++;

				if(outputPtr >= DELAY_BUFFER_LENGTH) outputPtr = 0;
				else outputPtr++;
				//outputSum += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
			}
			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processFinished = true;

		((DelayContext *)(procEvent->processContext))->inputPtr = inputPtr;
		((DelayContext *)(procEvent->processContext))->outputPtr = outputPtr;

		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		return 0;
	}

	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{
		if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "delayb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated delayb." << std::endl;
			free(procEvent->processContext);
			procEvent->processFinished = false;
		}

		return 0;
	}
	else
	{
		std::cout << "delayb: invalid actiond: " << action << std::endl;
		return 0;
	}
}

#define dbg 0
int filter3bb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);

		procEvent->processContext = (double *)calloc(50, sizeof(double));
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
		}
		procEvent->processFinished = false;

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		return 0;

	}
	else if(action == 1)
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

		if(procEvent->processContext == NULL)
		{
			cout << "audioCallback filter3bb context not allocated" << endl;
			return -1;
		}


		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		for(i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for(j = 0; j < 4; j++)
			{
				lp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
				lp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
				hp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++];
				hp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++];
			}
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			noiseFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}


		//int footswitchStatus = 0;
		//int filter3bValueIndex[10];
		//filter3bValueIndex[0] = 30;
		//filter3bValueIndex[1] = 50;
		/*filter3bValueIndex[2] = 20;
		filter3bValueIndex[3] = 25;
		filter3bValueIndex[4] = 30;
		filter3bValueIndex[5] = 35;
		filter3bValueIndex[6] = 40;
		filter3bValueIndex[7] = 45;
		filter3bValueIndex[8] = 50;
		filter3bValueIndex[9] = 55;
		filter3bValueIndex[10] = 60;
		filter3bValueIndex[11] = 65;
		filter3bValueIndex[12] = 70;
		filter3bValueIndex[13] = 72;
		filter3bValueIndex[14] = 74;*/
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
//			else
//			{
//				//cout << procEvent->parameters[i] << ", ";
//				lp_a[i][0] = lp[65/*procEvent->parameters[i]*/][0];
//				lp_a[i][1] = lp[65/*procEvent->parameters[i]*/][1];
//				lp_a[i][2] = lp[65/*procEvent->parameters[i]*/][2];
//				lp_a[i][3] = lp[65/*procEvent->parameters[i]*/][3];
//				//lp_a[i][4] = lp[tempIndex][4];
//				lp_b[i][1] = lp[65/*procEvent->parameters[i]*/][5];
//				lp_b[i][2] = lp[65/*procEvent->parameters[i]*/][6];
//				lp_b[i][3] = lp[65/*procEvent->parameters[i]*/][7];
//				//lp_b[i][4] = lp[tempIndex][9];
//
//				hp_a[i][0] = hp[65/*procEvent->parameters[i]*/][0];
//				hp_a[i][1] = hp[65/*procEvent->parameters[i]*/][1];
//				hp_a[i][2] = hp[65/*procEvent->parameters[i]*/][2];
//				hp_a[i][3] = hp[65/*procEvent->parameters[i]*/][3];
//				//hp_a[i][4] = hp[tempIndex][4];
//				hp_b[i][1] = hp[65/*procEvent->parameters[i]*/][5];
//				hp_b[i][2] = hp[65/*procEvent->parameters[i]*/][6];
//				hp_b[i][3] = hp[65/*procEvent->parameters[i]*/][7];
//				//hp_b[i][4] = hp[tempIndex][9];
//
//			}
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
						/*couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
						couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];


						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];*/

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


		for(j = 0; j < 3; j++)
		{
			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
		}
		for(i = 0; i < NUMBER_OF_BANDS; i++)
		{
			for(j = 0; j < 4; j++)
			{
				((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = lp_x[i][j];
				//std::cout << "end: lp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(15) << procEvent->processContext[i*8+j*4] << std::endl;
				((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = lp_y[i][j];
				//std::cout << "end: lp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << lp_y[i][j] << std::endl;
				((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++] = hp_x[i][j];
				//std::cout << "end: hp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_x[i][j] << std::endl;
				((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++] = hp_y[i][j];
				//std::cout << "end: hp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_y[i][j] << std::endl;
			}
		}
		for(j = 0; j < 3; j++)
		{
			 ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = noiseFilter_x[j];
			 ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = noiseFilter_y[j];
		}

		/*getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[2]]);*/

		/*procEvent->internalData[0] = procBufferArray[procEvent->outputBufferIndexes[0]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[0]].ampNegativePeak;
		procEvent->internalData[1] = procBufferArray[procEvent->outputBufferIndexes[1]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[1]].ampNegativePeak;
		procEvent->internalData[2] = procBufferArray[procEvent->outputBufferIndexes[2]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[2]].ampNegativePeak;*/

		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[2]].ready = 1;
		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{
		if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "filter3bb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated filter3bb." << std::endl;
			free(procEvent->processContext);
			procEvent->processFinished = false;
		}
		return 0;
	}
	else
	{
		std::cout << "filter3bb: invalid actiond: " << action << std::endl;
		return 0;
	}
}


#define dbg 0
int filter3bb2(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[2]]);

		procEvent->processContext = (double *)calloc(50, sizeof(double));
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
		}
		procEvent->processFinished = false;

		for(unsigned int i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		return 0;
	}
	else if(action == 1)
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

		if(procEvent->processContext == NULL)
		{
			cout << "audioCallback filter3bb2 context not allocated" << endl;
			return -1;
		}

		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		for(j = 0; j < 3; j++)
		{
			lp_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			lp_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		for(j = 0; j < 5; j++)
		{
			bp_x[j] = ((double *)procEvent->processContext)[contextIndex++];
			bp_y[j] = ((double *)procEvent->processContext)[contextIndex++];
		}
		for(j = 0; j < 3; j++)
		{
			hp_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			hp_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = ((double *)procEvent->processContext)[contextIndex++];
			noiseFilter_y[j] = ((double *)procEvent->processContext)[contextIndex++];
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
					/*couplingFilter_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset;
					couplingFilter_y[0] = 0.993509026691*couplingFilter_x[0] + (-1.987017202207)*couplingFilter_x[1] +0.993509026691*couplingFilter_x[2] - (-1.986975069020)*couplingFilter_y[1] - 0.987060186570*couplingFilter_y[2];

					lp_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];//couplingFilter_y[0]; // get inputBuffer[0] data for BD0 from process input
					bp_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];//couplingFilter_y[0]; // get inputBuffer[0] data for BD0 from process input
					hp_x[0] = procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i];//couplingFilter_y[0]; // get input data for BD0 from process input

					couplingFilter_x[2] = couplingFilter_x[1];
					couplingFilter_x[1] = couplingFilter_x[0];

					couplingFilter_y[2] = couplingFilter_y[1];
					couplingFilter_y[1] = couplingFilter_y[0];*/


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

//				lp_y[0] = lp_a[0]*lp_x[0] + lp_a[1]*lp_x[1] + lp_a[2]*lp_x[2] + lp_a[3]*lp_x[3] - lp_b[1]*lp_y[1] - lp_b[2]*lp_y[2] - lp_b[3]*lp_y[3];
//
//				lp_x[3] = lp_x[2];
//				lp_x[2] = lp_x[1];
//				lp_x[1] = lp_x[0];
//				lp_y[3] = lp_y[2];
//				lp_y[2] = lp_y[1];
//				lp_y[1] = lp_y[0];

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

//				hp_y[0] = hp_a[0]*hp_x[0] + hp_a[1]*hp_x[1] + hp_a[2]*hp_x[2] + hp_a[3]*hp_x[3] - hp_b[1]*hp_y[1] - hp_b[2]*hp_y[2] - hp_b[3]*hp_y[3];
//
//				hp_x[3] = hp_x[2];
//				hp_x[2] = hp_x[1];
//				hp_x[1] = hp_x[0];
//				hp_y[3] = hp_y[2];
//				hp_y[2] = hp_y[1];
//				hp_y[1] = hp_y[0];


				//procOutputBuffer.buffer[i] = hp_y[0];
				// j = 0 case is processed above

				procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = hp_y[0];

				/*noiseFilter_x[0] = hp_y[0];
				noiseFilter_y[0] = 0.01870016*noiseFilter_x[0] + 0.00304999*noiseFilter_x[1] + 0.01870016*noiseFilter_x[2] - (-1.69729152)*noiseFilter_y[1] - 0.73774183*noiseFilter_y[2];
				procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i] = noiseFilter_y[0];
				noiseFilter_x[2] = noiseFilter_x[1];
				noiseFilter_x[1] = noiseFilter_x[0];

				noiseFilter_y[2] = noiseFilter_y[1];
				noiseFilter_y[1] = noiseFilter_y[0];*/

				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[1]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[1]]);
				processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[2]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[2]]);

			}

		}

		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[2]]);


		if(procEvent->processContext == NULL) return -1;

		contextIndex = 0;

		for(j = 0; j < 3; j++)
		{
			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			((double *)procEvent->processContext)[contextIndex++] = lp_x[j];
			((double *)procEvent->processContext)[contextIndex++] = lp_y[j];
		}
		for(j = 0; j < 5; j++)
		{
			((double *)procEvent->processContext)[contextIndex++] = bp_x[j];
			((double *)procEvent->processContext)[contextIndex++] = bp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			((double *)procEvent->processContext)[contextIndex++] = hp_x[j];
			((double *)procEvent->processContext)[contextIndex++] = hp_y[j];
		}
		for(j = 0; j < 3; j++)
		{
			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_x[j];
			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_y[j];
		}

		/*getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[1]]);
		getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[2]]);*/

		/*procEvent->internalData[0] = procBufferArray[procEvent->outputBufferIndexes[0]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[0]].ampNegativePeak;
		procEvent->internalData[1] = procBufferArray[procEvent->outputBufferIndexes[1]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[1]].ampNegativePeak;
		procEvent->internalData[2] = procBufferArray[procEvent->outputBufferIndexes[2]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[2]].ampNegativePeak;*/


		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[2]].ready = 1;
		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{
		if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "filter3bb2 processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated filter3bb2." << std::endl;
			free(procEvent->processContext);
			procEvent->processFinished = false;
		}
		return 0;
	}
	else
	{
		std::cout << "filter3bb2: invalid actiond: " << action << std::endl;
		return 0;
	}


}


#define dbg 0
int lohifilterb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;

	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);

		procEvent->processContext = (double *)calloc(50, sizeof(double));
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
		}
		procEvent->processFinished = false;
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}

		return 0;
	}
	else if(action == 1)
	{

		int contextIndex = 0;
		double lp_a[NUMBER_OF_BANDS][4], lp_b[NUMBER_OF_BANDS][4];
		double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing

		double hp_a[NUMBER_OF_BANDS][4], hp_b[NUMBER_OF_BANDS][4];
		double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];
		double tempInput;
		double outputSum[2];
		outputSum[0] = 0.00000;
		outputSum[1] = 0.00000;


		if(procEvent->processContext == NULL)
		{
			cout << "audioCallback lohifilterb context not allocated" << endl;
			return -1;
		}
		//for(int i = 0; i < 1; i++)
		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		i = 0;
		{
			for(j = 0; j < 4; j++)
			{
				lp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
				//std::cout << "start: lp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(15) << procEvent->processContext[i*8+j*4] << std::endl;
				lp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
				//std::cout << "start: lp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << lp_y[i][j] << std::endl;
				hp_x[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++];
				//std::cout << "start: hp_x[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_x[i][j] << std::endl;
				hp_y[i][j] = ((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++];
				//std::cout << "start: hp_y[" << i << "][" << j << "]: " << std::fixed << std::setprecision(3) << hp_y[i][j] << std::endl;
			}
		}

		/*clearBufferParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		clearBufferParameters(&procBufferArray[procEvent->outputBufferIndexes[1]]);*/

		int tempIndex = procEvent->parameters[0];
		/*cout << "procEvent->parameters[0]: " << procEvent->parameters[0];
		cout << "procEvent->parameters[1]: " << procEvent->parameters[1];
		cout << "procEvent->parameters[2]: " << procEvent->parameters[2] << endl;*/

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
			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
		}
		i = 0;
		{
			for(j = 0; j < 4; j++)
			{
				((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = lp_x[i][j];
				((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = lp_y[i][j];
				((double *)procEvent->processContext)[/*i*8+j*4+2*/contextIndex++] = hp_x[i][j];
				((double *)procEvent->processContext)[/*i*8+j*4+3*/contextIndex++] = hp_y[i][j];
			}
		}

		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;
		procBufferArray[procEvent->outputBufferIndexes[1]].ready = 1;

		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{
		if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "lohifilterb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated lohifilterb." << std::endl;
			free(procEvent->processContext);
			procEvent->processFinished = false;
		}
		return 0;
	}
	else
	{
		std::cout << "lohifilterb: invalid actiond: " << action << std::endl;
		return 0;
	}
}

int mixerb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;
	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processFinished = false;
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		return 0;
	}
	else if(action == 1)
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
				procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i] = levelOut*(level1*(procBufferArray[procEvent->inputBufferIndexes[0]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[0]].offset) +
						level2*(procBufferArray[procEvent->inputBufferIndexes[1]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[1]].offset) +
						level3*(procBufferArray[procEvent->inputBufferIndexes[2]].buffer[i] - procBufferArray[procEvent->inputBufferIndexes[2]].offset));
			}

			processBufferAveSample(procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i], &procBufferArray[procEvent->outputBufferIndexes[0]]);
		}
		//procBufferArray[procEvent->outputBufferIndexes[0]].average = outputSum/bufferSize;

		//getBufferAvgs(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;


		/*procEvent->internalData[0] = procBufferArray[procEvent->outputBufferIndexes[0]].ampPositivePeak
				- procBufferArray[procEvent->outputBufferIndexes[0]].ampNegativePeak;*/

		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{

		return 0;
	}
	else
	{
		std::cout << "mixerb: invalid actiond: " << action << std::endl;
		return 0;
	}
}

int volumeb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;

	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processFinished = false;
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		return 0;
	}
	else if(action == 1)
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
		//procBufferArray[procEvent->outputBufferIndexes[0]].offset = outputSum/bufferSize;
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;

		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{


		return 0;
	}
	else
	{
		std::cout << "volumeb: invalid actiond: " << action << std::endl;
		return 0;
	}
}

int waveshaperb(int action, struct ProcessEvent *procEvent, struct ProcessBuffer *procBufferArray, int *footswitchStatus)
{
	unsigned int i,j;

	if(action == 0)
	{
		initBufferAveParameters(&procBufferArray[procEvent->outputBufferIndexes[0]]);
		procEvent->processContext = (double *)calloc(50, sizeof(double));
		if(procEvent->processContext == NULL)
		{
			std::cout << "waveshaperb calloc failed." << std::endl;
		}
		else
		{
			std::cout << "waveshaperb calloc succeeded." << std::endl;
			for(i = 0; i < 50; i++)
			{
				((double *)procEvent->processContext)[i] = 0.00;
			}
			const double r1 = 1000.0;
			const double r2 = 2000.0;
			const double r3 = 500.0;
			const double r4 = 120.0;
			const double r5 = 50.0;
			int contextIndex = 0;
			/* Calculate constants for waveshaper algorithm */

			for(j = 0; j < 3; j++) // initialize coupling filter
			{
				 ((double *)procEvent->processContext)[contextIndex++] = 0.00000;
				 ((double *)procEvent->processContext)[contextIndex++] = 0.00000;
			}

			((double *)procEvent->processContext)[contextIndex++] = 0.0; // outMeasure
			/*((double *)procEvent->processContext)[contextIndex++] = 0.0; // dist average 0
			((double *)procEvent->processContext)[contextIndex++] = 0.0; // dist average 1
			((double *)procEvent->processContext)[contextIndex++] = 0.0; // dist average 2
			((double *)procEvent->processContext)[contextIndex++] = 0.0; // dist average 3*/

			for(j = 0; j < 3; j++) // initialize noise filter
			{
				 ((double *)procEvent->processContext)[contextIndex++] = 0.00000;
				 ((double *)procEvent->processContext)[contextIndex++] = 0.00000;
			}

			((double *)procEvent->processContext)[contextIndex++] = r2*r3*r4*r5;
			((double *)procEvent->processContext)[contextIndex++] = r1*r3*r4*r5;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2*r4*r5;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2*r3*r5;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2*r3*r4;

			((double *)procEvent->processContext)[contextIndex++] = r2*r3*r4;
			((double *)procEvent->processContext)[contextIndex++] = r1*r3*r4;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2*r4;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2*r3;
			((double *)procEvent->processContext)[contextIndex++] = 0;

			((double *)procEvent->processContext)[contextIndex++] = r2*r3;
			((double *)procEvent->processContext)[contextIndex++] = r1*r3;
			((double *)procEvent->processContext)[contextIndex++] = r1*r2;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;

			((double *)procEvent->processContext)[contextIndex++] = r2;
			((double *)procEvent->processContext)[contextIndex++] = r1;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;

			((double *)procEvent->processContext)[contextIndex++] = 1;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;
			((double *)procEvent->processContext)[contextIndex++] = 0;

			((double *)procEvent->processContext)[contextIndex++] = 0.200;
			((double *)procEvent->processContext)[contextIndex++] = 0.350;
			((double *)procEvent->processContext)[contextIndex++] = 0.60;
			((double *)procEvent->processContext)[contextIndex++] = 0.80;

		}

		procEvent->processFinished = false;
		for(i = 0; i < 256; i++)
		{
			procEvent->internalData[i] = 0.0;
		}
		procEvent->dataReadDone = true;
		procEvent->dataReadReady = false;

		return 0;
	}
	else if(action == 1)
	{
		double k[5][5];
		double v[4];
		double tempInput;
		double distAveArray[4];
		double distAve;
		int contextIndex = 0;
		double outMeasure;
		double outputSum = 0.00000;
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];

		if(procEvent->processContext == NULL)
		{
			cout << "audioCallback waveshaperb context not allocated" << endl;
			return -1;
		}


		for(j = 0; j < 3; j++)
		{
			couplingFilter_x[j] = ((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++];
			couplingFilter_y[j] = ((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++];
		}
		outMeasure = ((double *)procEvent->processContext)[contextIndex++];
		/*distAveArray[0] = ((double *)procEvent->processContext)[contextIndex++];
		distAveArray[1] = ((double *)procEvent->processContext)[contextIndex++];
		distAveArray[2] = ((double *)procEvent->processContext)[contextIndex++];
		distAveArray[3] = ((double *)procEvent->processContext)[contextIndex++];*/
		for(j = 0; j < 3; j++)
		{
			noiseFilter_x[j] = ((double *)procEvent->processContext)[contextIndex++];
			noiseFilter_y[j] = ((double *)procEvent->processContext)[contextIndex++];
		}

		k[4][0] = ((double *)procEvent->processContext)[contextIndex++];
		k[4][1] = ((double *)procEvent->processContext)[contextIndex++];
		k[4][2] = ((double *)procEvent->processContext)[contextIndex++];
		k[4][3] = ((double *)procEvent->processContext)[contextIndex++];
		k[4][4] = ((double *)procEvent->processContext)[contextIndex++];

		k[3][0] = ((double *)procEvent->processContext)[contextIndex++];
		k[3][1] = ((double *)procEvent->processContext)[contextIndex++];
		k[3][2] = ((double *)procEvent->processContext)[contextIndex++];
		k[3][3] = ((double *)procEvent->processContext)[contextIndex++];
		k[3][4] = ((double *)procEvent->processContext)[contextIndex++];

		k[2][0] = ((double *)procEvent->processContext)[contextIndex++];
		k[2][1] = ((double *)procEvent->processContext)[contextIndex++];
		k[2][2] = ((double *)procEvent->processContext)[contextIndex++];
		k[2][3] = ((double *)procEvent->processContext)[contextIndex++];
		k[2][4] = ((double *)procEvent->processContext)[contextIndex++];

		k[1][0] = ((double *)procEvent->processContext)[contextIndex++];
		k[1][1] = ((double *)procEvent->processContext)[contextIndex++];
		k[1][2] = ((double *)procEvent->processContext)[contextIndex++];
		k[1][3] = ((double *)procEvent->processContext)[contextIndex++];
		k[1][4] = ((double *)procEvent->processContext)[contextIndex++];

		k[0][0] = ((double *)procEvent->processContext)[contextIndex++];
		k[0][1] = ((double *)procEvent->processContext)[contextIndex++];
		k[0][2] = ((double *)procEvent->processContext)[contextIndex++];
		k[0][3] = ((double *)procEvent->processContext)[contextIndex++];
		k[0][4] = ((double *)procEvent->processContext)[contextIndex++];


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
		procEvent->dataReadReady = false;

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

				if (v[3] < outMeasure) kIndex = 4;
				else if (v[2] < outMeasure && outMeasure <= v[3]) kIndex = 3;
				else if (v[1] < outMeasure && outMeasure <= v[2]) kIndex = 2;
				else if (v[0] < outMeasure && outMeasure <= v[1]) kIndex = 1;
				else if (-v[0] < outMeasure && outMeasure <= v[0]) kIndex = 0;
				else if (-v[1] < outMeasure && outMeasure <= -v[0]) kIndex = 1;
				else if (-v[2] < outMeasure && outMeasure <= -v[1]) kIndex = 2;
				else if (-v[3] < outMeasure && outMeasure <= -v[2]) kIndex = 3;
				else if (outMeasure <=-v[3]) kIndex = 4;

				if(0.000 <= outMeasure)
				{
					v1 = v[0]; v2 = v[1]; v3 = v[2]; v4 = v[3];
				}
				else
				{
					v1 = -v[0]; v2 = -v[1]; v3 = -v[2]; v4 = -v[3];
				}
				k1 = k[kIndex][0]; k2 = k[kIndex][1]; k3 = k[kIndex][2]; k4 = k[kIndex][3]; k5 = k[kIndex][4];

				dist = (tempInput*gain*k1 + v1*k2 + v2*k3 + v3*k4 + v4*k5)/(k1 + k2 + k3 + k4 + k5);
				clean = tempInput;
				//outputSum += procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
				outMeasure = dist;
				/*distAveArray[3] = distAveArray[2];
				distAveArray[2] = distAveArray[1];
				distAveArray[1] = distAveArray[0];
				distAveArray[0] = dist;
				outMeasure = distAveArray[0];//procBufferArray[procEvent->outputBufferIndexes[0]].buffer[i];
				distAve = (distAveArray[0] + distAveArray[1] + distAveArray[2])/3;*/

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
		//procBufferArray[procEvent->outputBufferIndexes[0]].offset = outputSum/bufferSize;
		updateBufferOffset(&procBufferArray[procEvent->outputBufferIndexes[0]]);

		procEvent->dataReadReady = true;
		procEvent->dataReadDone = false;

		contextIndex = 0;

		for(j = 0; j < 3; j++)
		{
			((double *)procEvent->processContext)[/*i*8+j*4*/contextIndex++] = couplingFilter_x[j];
			((double *)procEvent->processContext)[/*i*8+j*4+1*/contextIndex++] = couplingFilter_y[j];
		}
		((double *)procEvent->processContext)[contextIndex++] = outMeasure; // outMeasure
		/*((double *)procEvent->processContext)[contextIndex++] = distAveArray[0]; // dist average 0
		((double *)procEvent->processContext)[contextIndex++] = distAveArray[1]; // dist average 1
		((double *)procEvent->processContext)[contextIndex++] = distAveArray[2]; // dist average 2
		((double *)procEvent->processContext)[contextIndex++] = distAveArray[3]; // dist average 3*/

		for(j = 0; j < 3; j++)
		{
			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_x[j];
			 ((double *)procEvent->processContext)[contextIndex++] = noiseFilter_y[j];
		}

		procEvent->processFinished = true;
		procBufferArray[procEvent->outputBufferIndexes[0]].ready = 1;

		return 0;
	}
	else if(action == 2)
	{
		return 0;
	}
	else if(action == 3)
	{
		if(procEvent->processContext == NULL || procEvent == NULL)
		{
			std::cout << "waveshaperb processContext missing." << std::endl;
		}
		else
		{
			std::cout << "freeing allocated waveshaperb." << std::endl;
			procEvent->processFinished = false;
			free(procEvent->processContext); // for some reason, this causes a SIGABRT after a third combo selection
		}
		return 0;
	}
	else
	{
		std::cout << "waveshaperb: invalid actiond: " << action << std::endl;
		return 0;
	}
}



