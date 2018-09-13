/*
 * Effects.h
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */

#ifndef PROCESSES_H_
#define PROCESSES_H_

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



using namespace std;


int clearProcBuffer(struct ProcessBuffer *procBuffer);


//************************ processing *************************************************
void initBufferAveParameters(struct ProcessBuffer *bufferArray);
void resetBufferAve(struct ProcessBuffer *bufferArray);
int processBufferAveSample(double sample, struct ProcessBuffer *bufferArray);
void updateBufferOffset(struct ProcessBuffer *bufferArray);

int delayb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int filter3bb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int filter3bb2(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int lohifilterb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int mixerb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int volumeb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int reverbb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int waveshaperb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int samplerb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int oscillatorb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);

int blankb(char action, struct ProcessEvent *procEvent, struct ProcessBuffer *bufferArray, bool *footswitchStatus);



#endif /* PROCESSES_H_ */
