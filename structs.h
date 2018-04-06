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
	unsigned int  cycleTimeValueIndex;
	double cyclePositionValue;
	unsigned int int_cyclePositionValue;
	double waveValue;
};


/****************** EFFECTS CONTEXTS *********************/
struct DelayContext{
	//double dummy[100];
	unsigned int inputPtr;
	unsigned int outputPtr;
	double delayBuffer[DELAY_BUFFER_LENGTH+10];
	//double dummy1[100];
	double delayTimeAveragingBuffer[4];
	double delaySignalAveragingBuffer[8];
	unsigned int previousDelay;
};

#define NUMBER_OF_BANDS 2
struct Filter3bbContext{
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[4], couplingFilter_x[4];
	double rolloffFilter_y[4], rolloffFilter_x[4];
};

struct Filter3bb2Context{
	double lp_y[4], lp_x[4]; // needs to be static to retain data from previous processing
	double bp_y[7], bp_x[7]; // needs to be static to retain data from previous processing
	double hp_y[4], hp_x[4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[3], couplingFilter_x[3];
	double rolloffFilter_y[3], rolloffFilter_x[3];
};

struct LohifilterbContext{
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4]; // needs to be static to retain data from previous processing
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
};

struct ReverbbContext{
	unsigned int inputPtr;
	unsigned int outputPtr[REVERB_TAP_COUNT];
	double delayBuffer[REVERB_TAP_COUNT][DELAY_BUFFER_LENGTH];
};


struct WaveshaperbContext{
	double v[5];
	double x[6],y[6];
	double m[5],b[5];
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
	double antiAliasingFilter_y[2][3], antiAliasingFilter_x[2][3];
	double outMeasure;
};

struct BlankbContext{
	int blankInt;
};

struct SamplerbContext{
	int blankInt;
};

struct OscillatorbContext{
	int blankInt;
};


struct ProcessParams{
	string name;
	int paramType;   //  both ProcessParams type and ControlParameter type use the same type system.
	int value;
};

struct ControlParameter{
	string name;
	string alias;
	string abbr;
	int paramType;
	int controlledParamType;
	bool inheritControlledParamType;
	bool cvEnabled;
	int value;
};

struct Process{
	string name;
	string procType;
	int footswitchType;
	int footswitchNumber;
	vector<string> inputs;
	vector<string> outputs;
	vector<ProcessParams> params;
};

struct Control{
	string name;
	string parentEffect;
	string conType;
	vector<ControlParameter> params;
	vector<unsigned int> absProcessParameterIndexes;  // index from sortedParameterArray
	vector<unsigned int> absProcessParameterIndexesInv;  // index from sortedParameterArray
	//int targetParameterType;   // using same type system as ProcessParams and ControlParameter
	//int targetParameterTypeInv;   // using same type system as ProcessParams and ControlParameter
};



struct Connector{
	string object; // was "process", needed something more generic to cover "effect" and "control"
	string port;
};

struct ControlConnection{
	Connector src;
	Connector dest;
};

struct ProcessConnection{
	Connector src;
	Connector dest;
};



// use indexed parameter structures to enable faster access to parameters for updating, etc.
struct IndexedProcessParameter{
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





/*struct Connector{
	string process;
	string port;
};*/

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
	int procType;  //used to identify process type, not position in processing sequence
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
	int conType;
	string name;
	struct {
		int value;
		bool cvEnabled;
	}parameter[10];
	//int parameter[10];		// Normal: 	parameter[0]=parameter value

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


	//int absProcessParameterIndex[5];  // from unsortedParameterArray, using variable absParamIndex
								// get process:parameter from ControlConnection.dest, then
								// get index using getProcessParameterIndex(dest.process, dest.parameter)
	ParameterControlConnection paramContConnection[5];
	int paramContConnectionCount; // number of process parameters being written to by this control
	ParameterControlConnection paramContConnectionInv[5];
	int paramContConnectionCountInv; // number of process parameters being written to by this control
	//void *controlContext;
	int controlTypeIndex;
	bool gateOpen;
	bool envTriggerStatus;
	double output;
	double outputInv;
	unsigned int int_output;
	unsigned int int_outputInv;
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
	bool controlVoltageEnabled;
	Json::Value pedalUi;
};

struct UtilityDataStruct {
	bool usbEnable;
	int antiAliasingNumber;
	string inputCoupling;
	string waveshaperMode;
	int jack_Period;
	int jack_Buffer;
	float noiseGate_LowThres;
	float noiseGate_HighThres;
	float noiseGate_Gain;
	float trigger_HighThres;
	float trigger_LowThres;
};
#endif /* STRUCTS_H_ */
