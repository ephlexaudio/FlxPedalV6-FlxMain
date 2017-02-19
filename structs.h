/*
 * structs.h
 *
 *  Created on: Jul 16, 2016
 *      Author: mike
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

using namespace std;

//#include <string>
#include "config.h"
#include <vector>
#include <sys/types.h>
//#include <stdbool.h>
/*#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <linux/types.h>*/

//#define BUFFER_SIZE 1024
#define AVE_ARRAY_SIZE 16
/*************** CONTROL CONTEXTS ************************/
struct EnvGenContext{
	int envStage; //0:attack, 1:decay, 2:sustain, 3:release
	double slewRate;
	int stageTimeValue;
};


struct LfoContext{
	int  cycleTimeValueIndex; // let byte automatically recycle when it 255.
	double cyclePositionValue;
	int int_cyclePositionValue;
	double waveValue;
};


/****************** EFFECTS CONTEXTS *********************/
struct DelayContext{
	double dummy[100];
	unsigned int inputPtr;
	unsigned int outputPtr;
	double delayBuffer[DELAY_BUFFER_LENGTH];
	double dummy1[100];
};

#define NUMBER_OF_BANDS 2
struct Filter3bbContext{
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[4], couplingFilter_x[4];
	double noiseFilter_y[4], noiseFilter_x[4];
};

struct Filter3bb2Context{
	double lp_y[4], lp_x[4]; // needs to be static to retain data from previous processing
	double bp_y[5], bp_x[5]; // needs to be static to retain data from previous processing
	double hp_y[4], hp_x[4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
};

struct LohifilterbContext{
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
};

struct WaveshaperbContext{
	double v[4];
	double x[6],y[6];
	double m[5],b[5];
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
	double outMeasure;
};



struct ProcessParams{
	string name;
	int type;   //  both ProcessParams type and ControlParameter type use the same type system.
	int value;
};

struct ControlParameter{
	string name;
	string alias;
	string abbr;
	int type;
	int value;
};

struct Process{
	string name;
	string type;
	int footswitchType;
	int footswitchNumber;
	vector<string> inputs;
	vector<string> outputs;
	vector<ProcessParams> params;
};

struct Control{
	string name;
	string parentEffect;
	string type;
	vector<ControlParameter> params;
	vector<unsigned int> absProcessParameterIndexes;
	int targetParameterType;   // using same type system as ProcessParams and ControlParameter
};


struct ControlConnection{
	string src;
	string dest;
};




// use indexed parameter structures to enable faster access to parameters for updating, etc.
struct IndexedParameter{
	string effectName;
	int effectIndex;
	string processName;
	int effectProcessIndex;
	int absProcessIndex;
	string paramName;
	int processParamIndex;
	int effectParamIndex;
	int absParamIndex;
	int paramValue;
};
struct IndexedControlParameter{
	string effectName;
	int effectIndex;
	string controlName;
	int effectControlIndex;
	int absControlIndex;
	string controlParamName;
	int controlParamIndex;
	int effectParamIndex;
	//vector<int> absProcessParamIndex;
	int controlParamValue;
};



struct PedalUI{
	string title;
	vector<Control> effect[2];
};





struct Connector{
	string process;
	string port;
};

struct ParameterControlConnection{
	int processIndex;
	int processParamIndex;
};


typedef  void (*ClearParameters)(struct ProcessBuffer *);
typedef  int (*ProcessSample)(double sample, struct ProcessBuffer *);
typedef  void (*GetAvgs)(struct ProcessBuffer *);


struct ProcessBuffer{
	string processName;
	string portName;
	double bufferSum;
	double aveArray[AVE_ARRAY_SIZE];
	double offset;
	int aveArrayIndex;
	double buffer[BUFFER_SIZE]; // for real-time processing
	int ready;
	int processed;
};

struct ProcessEvent{
	int processType;  //used to identify process type, not position in processing sequence
	string processName;
	int processTypeIndex;
	int footswitchNumber;
	int parameterCount;
	int parameters[10];
	double internalData[256]; // use to store curves, etc.
	int dataReadReady; // data buffer ready to be read by outside process
	int dataReadDone; // outside process finished reading data
	int processInputCount;
	int processOutputCount;
	int inputBufferIndexes[5];
	vector<string> inputBufferNames;
	int outputBufferIndexes[5];
	vector<string> outputBufferNames;
	//void *processContext;
	bool processFinished;
};

struct ControlEvent{
	int type;
	string name;
	int parameter[10];		// Normal: 	parameter[0]=parameter value

							// EnvGen: 	parameter[0]=attack time
							//			parameter[1]=decay time
							//			parameter[2]=sustain time
							//			parameter[3]=release time
							//			parameter[4]=peak value index;
							//			parameter[5]=sustain value index;

							// LFO:		parameter[0]=frequency
							//			parameter[1]=amplitude
							//			parameter[2]=offset
							//			parameter[3]=
							//			parameter[4]=


	ParameterControlConnection paramContConnection[5];
	//int absProcessParameterIndex[5];  // from unsortedParameterArray, using variable absParamIndex
								// get process:parameter from ControlConnection.dest, then
								// get index using getProcessParameterIndex(dest.process, dest.parameter)
	int paramContConnectionCount; // number of process parameters being written to by this control
	//void *controlContext;
	int controlTypeIndex;
	bool gateStatus;
	bool envTriggerStatus;
	double output;
	unsigned int int_output;
};



struct ComboStruct {
	string name;
	ProcessEvent processSequence[20];
	ControlEvent controlSequence[20];
	ProcessBuffer procBufferArray[60];
	int footswitchStatus[10];
	int inputProcBufferIndex[2];
	int outputProcBufferIndex[2];
	int processCount;
	int controlCount;
	int bufferCount;
	Json::Value pedalUi;
};


#endif /* STRUCTS_H_ */
