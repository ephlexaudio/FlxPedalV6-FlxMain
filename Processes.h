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
#include <array>
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


int clearProcBuffer(struct ProcessSignalBuffer *procBuffer);


int delayb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
		   array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);


int filter3bb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
			  array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);

int filter3bb2(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
			   array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);

int lohifilterb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
				array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);

int mixerb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
		   array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);

int volumeb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
			array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);

int waveshaperb(char action, struct ProcessEvent *procEvent, array<ProcessSignalBuffer,60> &bufferArray,
				array<ProcessParameterControlBuffer,60> &paramContBufferArray, bool *footswitchStatus);






#endif /* PROCESSES_H_ */
