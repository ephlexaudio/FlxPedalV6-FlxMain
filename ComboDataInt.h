/*
 * ComboDataInt.h
 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#ifndef DATABASEINT_H_
#define DATABASEINT_H_


#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>
#include <json/json.h>
#include <algorithm>

using namespace std;

#include "structs.h"
#include "utilityFunctions.h"


#define JSON_BUFFER_LENGTH 32000
class ComboDataInt
{

private:


	int comboFD;
	Json::Reader comboReader;
	Json::FastWriter comboWriter;
	char jsonBuffer[JSON_BUFFER_LENGTH];
	char fileNameBuffer[20];
	char fileNamePathBuffer[40];
	int inputProcBufferIndex[2];
	int outputProcBufferIndex[2];

	std::vector<Connector> dataReadyList;
	string currentTargetProcess;
	string comboName;
	Json::Value effectComboJson;
	Json::Value pedalUiJson;
	std::vector<Json::Value> unsequencedProcessListJson;
	std::vector<Process> unsequencedProcessListStruct;
	std::vector<Json::Value> unsequencedConnectionListJson;
	std::vector<ProcessConnection> unsequencedConnectionListStruct;

	std::vector<Json::Value> connectionsJson;
	std::vector<Json::Value> processesJson;

	std::vector<Process> processesStruct; // process here are in order
	std::vector<ProcessConnection> connectionsStruct;
	std::vector<Control> controlsStruct;
	std::vector<ControlConnection> controlConnectionsStruct;

	std::vector<IndexedProcessParameter> unsortedProcessParameterArray;
	std::vector<IndexedProcessParameter> sortedProcessParameterArray;
	std::vector<IndexedControlParameter> sortedControlParameterArray;

	ProcessEvent processSequence[20];
	ControlEvent controlSequence[20];
	ProcessBuffer procBufferArray[60];
	int footswitchStatus[10];
	int processCount;
	int effectCount;
	int controlCount;
	int bufferCount;
	/**** Tasks for sequencing processes *******/

	void readUnsequencedConnectionListJsonIntoUnsequencedConnectionListStruct();
	int getTargetProcessIndex(string processName);
	std::vector<string> getProcessInputs(string processName);
	std::vector<string> getProcessOutputs(string processName);
	int getConnectionDestinationIndex(string destinationProcessName);
	int fillUnsequencedProcessList();
	string getFirstProcess();
	std::vector<string> getFirstProcesses();
	string getNextProcess();
	std::vector<string> getNextProcesses();
	std::vector<IndexedProcessParameter> tempParameterArray;

	int areDataBuffersReadyForProcessInputs(string processName);
	int areAllProcessInputsSatisfied(string processName);
	int transferProcessToSequencedProcessList(string processName);
	int addOutputConnectionsToDataReadyList(string processName);
	int addOutputConnectionsToDataReadyList(vector<string> processNames);
	int transferProcessesToSequencedProcessList(vector<string> processNames);
	bool isUnsequencedProcessListEmpty();
	bool isOutputInDataReadyList(Connector output);
	Json::Value getJsonValueFromUnorderedJsonValueList(int index, string arrayName, Json::Value arrayContainer);
	Json::Value getJsonValueFromUnorderedJsonValueList(int index, Json::Value array);
	Json::Value mergeConnections(Json::Value srcConn, Json::Value destConn);
	ProcessConnection mergeConnections(ProcessConnection srcConn, ProcessConnection destConn);
	bool compareConnectionsSrc2Dest(Json::Value conn1, Json::Value conn2);
	bool compareConnectionsSrc2Dest(ProcessConnection srcConn, ProcessConnection destConn);
	int transferConnection(Json::Value conn, vector<Json::Value> *srcConnArray, vector<Json::Value> *destConnArray);
	//************************ previously in public space ******************************

	int getProcessSequenceIndex(string processName);
	int getControlSequenceIndex(string controlName);
	int getInputProcBufferIndex(int LRindex);
	int getOutputProcBufferIndex(int LRindex);
	void setInputProcBufferIndex(int LRindex, int bufferIndex);
	void setOutputProcBufferIndex(int LRindex, int bufferIndex);
	int getCombo(char *comboName); //get JSON data from file and parse into effectComboJson
	int getConnections2(void);
	int getProcesses(void); // setup processesStruct and unsortedParameterArray using effectComboJson
	int getControls(void);	// setup controlsStruct and controlParameterArray using effectComboJson
	int getControlConnections(void);  // setup controlConnectionsStruct using effectComboJson
	int setProcData(struct ProcessEvent *procEvent, Process processStruct);

	int setProcParameters(struct ProcessEvent *procEvent, Process processStruct);

	int initProcInputBufferIndexes(struct ProcessEvent *procEvent);
	int initProcOutputBufferIndexes(struct ProcessEvent *procEvent);
	int initProcBufferArray(struct ProcessBuffer *bufferArray, vector<ProcessConnection> connectionsStruct);
	int connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray();
	int connectProcessInputsToProcessOutputBuffersUsingConnectionsStruct();
	int initializeProcessDataIntoProcessEventElement();
	int initializeControlDataIntoControlEventElement();
	int setProcessSequenceParameter(string processName, int parameterIndex, int valueIndex);
	int setControlSequenceParameter(string controlName, int parameterIndex, int valueIndex);


public:
	ComboDataInt();
	~ComboDataInt();


	void printSequencedConnectionJsonList();
	void printSequencedConnectionStructList();
	void printUnsequencedProcessList();
	void printSequencedProcessList();
	void printUnsequencedConnectionJsonList();
	void printUnsequencedConnectionStructList();
	void printUnsortedParameters();
	void printProcessParameter(int controlParameterIndex);
	void printSortedParameters();
	void printDataReadyList(void);
	void printControlList(void);
	void printSequencedControlList(void);
	void printControlParameterList();
	void printControlConnectionList(void);
	void printControlParameter(int controlParameterIndex);
	void printBufferList(void);
	int getProcessCount();
	int getControlCount();
	int getBufferCount();

	int getPedalUi(void); // setup pedalUiJson using effectComboJson
	Json::Value getPedalUiJson(void);

	int loadComboStructFromName(char *comboName);

	int saveCombo(void);
	void listParameters(void);
	int getProcessParameterIndex(string processName, string parameterName);
	int getControlParameterIndex(string controlName, string parameterName);

	int updateProcess(int absParamIndex, int valueIndex);
	int updateControl(int absParamIndex, int valueIndex);
	string getName();
	ComboStruct getComboStruct();
	IndexedProcessParameter getProcessParameter(int arrayIndex);
	IndexedControlParameter getControlParameter(int arrayIndex);
	int getProcessParameterArraySize();
	int getControlParameterArraySize();

};




#endif /* DATABASEINT_H_ */
