/*
 * Combo.h
 *
 *  Created on: Jun 28, 2016
 *      Author: mike
 */

#ifndef COMBO_H_
#define COMBO_H_

#include <iostream>

#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>
#include <jack/jack.h>
//#include "Process.h"
#include "Effects2.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"
//#include "jackaudioio.h"
#define BUFFER_SIZE 1024

/*typedef struct _processingParams{
	double lowGateThres;
	double highGateThres;
	double gatedGain;
};*/




class Combo{
private:
	//Process *process[20];
	int initProcBufferArray(struct ProcessBuffer *bufferArray, vector<Json::Value> connectionsJson);


public:
	/*struct */ProcessEvent processSequence[20]; // do these 5 variables/structs need to be public ??
	ControlEvent controlSequence[20];
	ProcessBuffer procBufferArray[60];
	int footswitchStatus[10];
	int inputProcBufferIndex[2];
	int outputProcBufferIndex[2];
	int processCount;
	int controlCount;
	int bufferCount;
	Combo(Json::Value comboJson);
	~Combo();

	int loadEffects();

};

#endif /* COMBO_H_ */
