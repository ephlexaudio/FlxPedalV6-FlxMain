/*
 * structs.h
 *
 *  Created on: Jul 16, 2016
 *      Author: mike
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

using namespace std;

#include "config.h"
#include <vector>
#include <sys/types.h>
#include <json/json.h>
#include "indexMapStructs.h"
#define AVE_ARRAY_SIZE 16



//****************** GET RID OF CONTEXTS*************************************8


/*************** CONTROL CONTEXTS ************************/
struct EnvGenContext{
	int envStage; //0:attack, 1:decay, 2:sustain, 3:release
	int stageTimeValue;
	double slewRate;
};


struct LfoContext{
	int  cycleTimeValueIndex;
	double cyclePositionValue;
	int int_cyclePositionValue;
	double waveValue;
};


/****************** EFFECTS CONTEXTS *********************/
struct DelayContext{
	unsigned long inputPtr;
	unsigned long outputPtr;
	double delayBuffer[DELAY_BUFFER_LENGTH+10];
	unsigned long delayTimeAveragingBuffer[4];
};

#define NUMBER_OF_BANDS 2
struct Filter3bbContext{
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
	double couplingFilter_y[4], couplingFilter_x[4];
	double rolloffFilter_y[4], rolloffFilter_x[4];
};

struct Filter3bb2Context{
	double lp_y[4], lp_x[4];
	double bp_y[7], bp_x[7];
	double hp_y[4], hp_x[4];
	double couplingFilter_y[3], couplingFilter_x[3];
	double rolloffFilter_y[3], rolloffFilter_x[3];
};

struct LohifilterbContext{
	double hp_y[NUMBER_OF_BANDS][4], hp_x[NUMBER_OF_BANDS][4];
	double lp_y[NUMBER_OF_BANDS][4], lp_x[NUMBER_OF_BANDS][4];
	double couplingFilter_y[3], couplingFilter_x[3];
	double noiseFilter_y[3], noiseFilter_x[3];
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

/**********************************************************************************************/

struct Connector{
	string objectName; // connector "Parent".  Was "process", needed something more generic to cover "effect" and "control"
	string portName;
	int portIndex; // position of connector WRT other like connectors (input,output, parameter)
	int connectedBufferIndex; // index of connection to pub/sub data structure arrays, if applicable
};




struct ProcessSignalConnection{
	Connector src; // source process output port
	Connector dest; // destination process input port
};

struct ProcessParameterControlConnection{
	Connector src; // source parameter control output port
	Connector dest; // destination parameter port
};

struct ProcessParameter{
	Connector param; // contains parent process, param name, param index, paramControlBuffer index.
	int paramType;   //  both ProcessParams type and ControlParameter type use the same type system.
	string paramControlType;
	int valueIndex;
};

struct ControlParameter{
	string name;
	string alias;  // alias and abbr are for Pedal UI
	string abbr;
	int index;
	string parentControl;  // create parent control index in getPedalUi
	int paramType;
	int controlledParamType;
	bool inheritControlledParamType;
	bool cvEnabled;
	int valueIndex;
};



struct Process{
	string processName;
	int processSequenceIndex;
	string processTypeString;
	int processTypeInt;
	int processTypeIndex; //used for process contexts.  Will be obsolete
	string parentEffect;
	int parentEffectIndex;
	string footswitchType;
	int footswitchNumber;
	int inputCount;
	int outputCount;
	int paramCount;
	vector<Connector> inputVector;	//

	vector<Connector> outputVector;  // use ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray
									 // to put bufferArray index into outputVector[].connectedBufferIndex
	vector<ProcessParameter> paramVector;
};


struct Control{
	string name;  // make the name more descriptive (ie. control:mixerb_0:level1)
	int index;
	int controlTypeIndex;
	string parentEffect;
	int parentEffectIndex;
	string conType;
	int conTypeInt;
	vector<ControlParameter> paramVector;
	Connector output;
	Connector outputInv;
	vector<Connector> targetProcessParamVector;
	vector<Connector> targetProcessParamInvVector;
};

struct Effect{
	string name;
	string abbr;
	int index;
	vector<Process> processVector;
	vector<ProcessSignalConnection> processConnectionVector;
	vector<Control> processParamControlVector;
	vector<ProcessParameterControlConnection> processParamControlConnectionVector;
};

struct EffectConnection{
	Connector src;
	Connector dest;
};


struct ComboJsonFileStruct
{
	string name;
	Effect effectArray[2];
	vector<EffectConnection> effectConnectionVector;
};


// KEEP AS STRUCT
struct ProcessSignalBuffer{
	Connector srcProcess;
	vector<Connector> destProcessVector;
	double buffer[BUFFER_SIZE]; // for real-time processing

};

struct ProcessParameterControlBuffer{
	Connector srcControl;
	Connector destProcessParameter;
	int parameterValueIndex;
};



// START "CLASSIFYING" STRUCTS FROM HERE







//CLASS "PROCESS" (BASE OR "INTERFACE" CLASS FOR CHILD PROCESSES: DELAY, WAVESHAPER, ETC)
struct ProcessEvent{
	int processTypeInt;  //used to identify process type, not position in processing sequence
	int processSequenceIndex;
	string processName;
	int processTypeIndex;
	int footswitchNumber;
	int parameterCount;
	struct{
		int internalIndexValue;
		int paramContBufferIndex;
		bool controlConnected;
		string parameterName;
	}parameterArray[10];

	int processInputCount;
	int processOutputCount;
	int inputConnectedBufferIndexArray[5];
	int outputConnectedBufferIndexArray[5];
	bool processFinished;
	int bufferSize;
	int inputCouplingMode;
	int antiAliasingNumber;
	int waveshaperMode;

};



//CLASS "CONTROL" (BASE OR "INTERFACE" CLASS FOR CHILD CONTROLS: Normal, EnvelopeGenerator, LowFreqOsc)
struct ControlEvent{
	string controlName;
	int conType;
	int parameterCount;
	struct {
		int value;
		bool cvEnabled;
		string parameterName;
	}parameter[10];
							// Normal: 	parameter[0].value=parameter value

							// EnvGen: 	parameter[0].value=attack time
							//			parameter[1].value=decay time
							//			parameter[2].value=sustain time
							//			parameter[3].value=release time
							//			parameter[4].value=peak value index;
							//			parameter[5].value=sustain value index;

							// LFO:		parameter[0].value=frequency
							//			parameter[1].value=amplitude
							//			parameter[2].value=offset


	int outputConnectionCount;
	int outputInvConnectionCount;
	int outputToParamControlBufferIndex[5];
	int outputInvToParamControlBufferIndex[5];
	int controlTypeIndex;
	bool envTrigger;
	double output;
	double outputInv;
	int int_output;
	int int_outputInv;
};


//CLASS "COMBO"
struct ComboStruct {
	string name;
	ProcessEvent processSequence[20]; // vector of Process child classes
	ControlEvent controlSequence[20]; // vector of Control child classes
	ProcessSignalBuffer processSignalBufferArray[60]; // keep this as struct
	ProcessParameterControlBuffer processParamControlBufferArray[60]; // keep this as struct
	map<string, ProcessIndexing>  processIndexMap;
	map<string, ControlIndexing>  controlIndexMap;
	int footswitchStatus[10];
	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int paramControlBufferCount;
	bool controlVoltageEnabled;
	int bufferSize;
	Json::Value pedalUi;
};







//********************* Utility Structs *******************************************

struct ProcessUtility {
	int antiAliasingNumber;
	int inputCouplingMode;
	int waveshaperMode;
	int bufferSize;
};


struct JackUtility {
	int period;
	int buffer;
};

struct NoiseGateUtility {
	double closeThres;
	double openThres;
	double gain;
};

struct EnvTriggerUtility {
	double highThres;
	double lowThres;
};


struct ProcessingUtility {
	NoiseGateUtility noiseGateUtil;
	EnvTriggerUtility triggerUtil;
	ProcessUtility procUtil;
};


//************************************************************************************************













#endif /* STRUCTS_H_ */
