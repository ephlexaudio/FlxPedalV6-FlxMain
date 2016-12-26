/*
 * DatabaseInt.h
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

using namespace std;

//#include "ConnectionCont.h"
#include "structs.h"
#include "utilityFunctions.h"



/*typedef struct ProcessParams{
	string name;
	int type;
	int value;
};

typedef struct Process{
	string name;
	string type;
	int footswitchType;
	int footswitchNumber;
	vector<string> inputs;
	vector<string> outputs;
	vector<ProcessParams> params;

};*/

struct effectIO{
	string effectName;
	string inputProcess[2];
	string inputPort[2];
	string outputProcess[2];
	string outputPort[2];
};

#define JSON_BUFFER_LENGTH 32000
class ComboDataInt
{

private:

	/*struct Connector{
		string process;
		string port;
	};*/

	int comboFD;
	//FILE * comboFD;
	Json::Reader comboReader;
	Json::FastWriter comboWriter;
	char jsonBuffer[JSON_BUFFER_LENGTH];
	char fileNameBuffer[20];
	char fileNamePathBuffer[40];

	std::vector<Connector> dataReadyList;
	string currentTargetProcess;
	/**** Tasks for sequencing processes *******/
	int getTargetProcessIndex(string processName);
	std::vector<string> getProcessInputs(string processName);
	std::vector<string> getProcessOutputs(string processName);
	int getConnectionDestinationIndex(string destinationProcessName);
	int fillUnsequencedProcessList();
	string getFirstProcess();
	string getNextProcess();
	std::vector<IndexedParameter> tempParameterArray;

	//int addFirstConnectionToConnectionList();
	//int addNextConnectionToConnectionList(string srcProcess, string srcPort);
	//bool checkConnectionListDestinationsAgainstUnsequencedProcessInputs(string processName);
	bool areDataBuffersReadyForProcessInputs(string processName);
	int transferProcessToSequencedProcessList(string processName);
	//void deleteConnectionsFromConnectionListContainingProcessInputs(string processName);
	int addOutputConnectionsToDataReadyList(string processName);
	bool isUnsequencedProcessListEmpty();
	bool isOutputInDataReadyList(Connector output);
	//int getParameterArray();
	Json::Value mergeConnections(Json::Value srcConn, Json::Value destConn);
	bool compareConnectionsSrc2Dest(Json::Value conn1, Json::Value conn2);
	int transferConnection(Json::Value conn, vector<Json::Value> *srcConnArray, vector<Json::Value> *destConnArray);
public:
	ComboDataInt();
	~ComboDataInt();

	Json::Value effectComboJson;
	Json::Value pedalUiJson;
	std::vector<Json::Value> unsequencedProcessListJson;
	std::vector<Process> unsequencedProcessListStruct;
	std::vector<Json::Value> unsequencedConnectionListJson;

	std::vector<Json::Value> connectionsJson;
	std::vector<Json::Value> processesJson;
	std::vector<Process> processesStruct;
	std::vector<IndexedParameter> unsortedParameterArray;
	std::vector<IndexedParameter> sortedParameterArray;
	std::vector<IndexedControlParameter> controlParameterArray;
	std::vector<Control> controlsStruct;
	std::vector<ControlConnection> controlConnectionsStruct;
	//std::vector<Parameter> parameterArray;
	PedalUI pedalUiStruct;
	void printSequencedConnectionList();
	void printUnsequencedProcessList();
	void printSequencedProcessList();
	void printUnsequencedConnectionList();
	void printUnsortedParameters();
	void printProcessParameter(int controlParameterIndex);
	void printSortedParameters();
	void printDataReadyList(void);
	void printControlList(void);
	void printControlParameterList();
	void printControlConnectionList(void);
	void printControlParameter(int controlParameterIndex);
	int getCombo(char *comboName); //get JSON data and parse into effectComboJson
	int getPedalUi(void); // setup pedalUiJson using effectComboJson
	int getConnections(void); // setup connectionsJson using effectComboJson
	int getConnections2(void);
	int getProcesses(void); // setup processesStruct and unsortedParameterArray using effectComboJson
	int getControls(void);	// setup controlsStruct and controlParameterArray using effectComboJson
	int getControlConnections(void);  // setup controlConnectionsStruct using effectComboJson
	void getProcParameters(int procIndex, int params[10]);
	int saveCombo(void/*Json::Value combo*/);
	int updateJson(int paramIndex, int paramValue);
	int updateJson(char *processKey, char *paramKey, int paramValue);
	//int readJson(char *effectKey, char *paramKey);
	void listParameters(void);
	int getProcessIndex(int absParameterIndex);
	int getControlIndex(string targetProcessName, string targetParameterName);
	int getProcessParameterIndex(string processName, string parameterName);
	int getControlParameterIndex(string controlName, string parameterName);
	int updateProcess(int absParamIndex, int valueIndex);
	int updateControl(int absParamIndex, int valueIndex);
};



#endif /* DATABASEINT_H_ */
