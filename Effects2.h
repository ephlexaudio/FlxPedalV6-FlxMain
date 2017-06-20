/*
 * Effects.h
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */

#ifndef EFFECTS2_H_
#define EFFECTS2_H_

//#define BUFFER_SIZE 1024
#include "config.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>

#include "structs.h"
/*#include "filterValues.h"
#include "volumeValues.h"
#include "delayValues.h"
#include "lfoFreqValues.h"
#include "envGenTimeValues.h"*/
#include "ComponentSymbols.h"

using namespace std;

/*struct Connector{
	string process;
	string port;
};*/



/*struct ProcessBuffer{
	string processName;
	string portName;
	double buffer[BUFFER_SIZE];
	int ready;
	int processed;

};

struct ProcessEvent{
	int processType;  //used to identify process type, not position in processing sequence
	string processName;
	int processTypeIndex;
	int parameters[10];
	int processInputCount;
	int processOutputCount;
	int *inputBufferIndexes;
	vector<string> inputBufferNames;
	int *outputBufferIndexes;
	vector<string> outputBufferNames;
	void *processContext;
	bool processFinished;
};*/



//***************** functions for loading process data ***************************
int initProcBuffers(struct ProcessBuffer *procBufferArray);
int setProcBuffer(struct ProcessBuffer procBufferArray, int processed, int ready);
int resetProcBuffer(struct ProcessBuffer procBufferArray);
int clearProcBuffer(struct ProcessBuffer *procBuffer);
int clearProcBuffer(struct ProcessBuffer procBuffer);
int setProcData(struct ProcessEvent *procEvent, Process processStruct);

int setProcParameters(struct ProcessEvent *procEvent, Process processStruct);

int initProcInputBufferIndexes(struct ProcessEvent *procEvent);
int initProcOutputBufferIndexes(struct ProcessEvent *procEvent);

int setProcInputBufferIndex(struct ProcessEvent *procEvent, int processInputIndex, int inputBufferIndex, struct ProcessBuffer *procBufferArray);
int setProcOutputBufferIndex(struct ProcessEvent *procEvent, int processOutputIndex, int outputBufferIndex, struct ProcessBuffer *procBufferArray);

//************************ processing *************************************************
void initBufferAveParameters(struct ProcessBuffer *bufferArray);
void resetBufferAve(struct ProcessBuffer *bufferArray);
int processBufferAveSample(double sample, struct ProcessBuffer *bufferArray);
void updateBufferOffset(struct ProcessBuffer *bufferArray);

int delayb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int filter3bb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int filter3bb2(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int lohifilterb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int mixerb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int volumeb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int reverbb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int waveshaperb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int samplerb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int oscillatorb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);

int blankb(/*int*/ char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, int *footswitchStatus);



#endif /* EFFECTS_H_ */
