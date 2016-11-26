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
#include <vector>
#include <sys/types.h>
//#include <stdbool.h>
/*#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <linux/types.h>*/

#define BUFFER_SIZE 1024
#define DELAY_BUFFER_LENGTH 40000

/*************** CONTROL CONTEXTS ************************/
struct EnvGenContext{
	int envStage; //0:attack, 1:decay, 2:sustain, 3:release
	float slewRate;
	int stageTimeValue;
};


struct LfoContext{
	int  cycleTimeValueIndex; // let byte automatically recycle when it 255.
	float cyclePositionValue;
	int int_cyclePositionValue;
	float waveValue;
};


/****************** EFFECTS CONTEXTS *********************/
struct DelayContext{
	unsigned int inputPtr;
	unsigned int outputPtr;
	float delayBuffer[DELAY_BUFFER_LENGTH];
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
typedef  int (*ProcessSample)(float sample, struct ProcessBuffer *);
typedef  void (*GetAvgs)(struct ProcessBuffer *);


struct ProcessBuffer{
	string processName;
	string portName;
	float ampPositiveSum;
	float ampPositivePeak;
	int ampPositiveCount;
	float ampPositiveAvg;
	float ampPositiveMeasWindow[8];
	int ampPositiveMeasWindowIndex;
	float ampNegativeSum;
	float ampNegativePeak;
	int ampNegativeCount;
	float ampNegativeAvg;
	float ampNegativeMeasWindow[8];
	int ampNegativeMeasWindowIndex;
	float buffer[BUFFER_SIZE]; // for real-time processing
	int ready;
	int processed;
};

struct ProcessEvent{
	int processType;  //used to identify process type, not position in processing sequence
	string processName;
	int processTypeIndex;
	int footswitchNumber;
	int parameters[10];
	float internalData[256]; // use to store curves, etc.
	int dataReadReady; // data buffer ready to be read by outside process
	int dataReadDone; // outside process finished reading data
	int processInputCount;
	int processOutputCount;
	int *inputBufferIndexes;
	vector<string> inputBufferNames;
	int *outputBufferIndexes;
	vector<string> outputBufferNames;
	void *processContext;
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
	void *controlContext;
	bool gateStatus;
	bool envTriggerStatus;
	float output;
	unsigned int int_output;
};

#endif /* STRUCTS_H_ */
