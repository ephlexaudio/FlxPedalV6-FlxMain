/*
 * DatabaseInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */
#include "config.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"
#include "ProcessingControl.h"
#include "FileSystemFuncts.h"

/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 1
#define COMBO_DATA_MAP 0*/

/*******
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::" << endl;
	if(debugOutput) cout << ": " <<  << endl;
#endif
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::: " << status << endl;
#endif
***********/
extern bool debugOutput;
extern int procCount;
extern int effectCount;
extern int globalComboIndex;
#if(COMBO_DATA_VECTOR == 1)
	extern std::vector<string> comboList;//comboVectorList;
#elif(COMBO_DATA_ARRAY == 1)
	extern std::vector<string> comboList;
#elif(COMBO_DATA_MAP == 1)
	extern std::vector<string> comboList;
#endif

#define JSON_BUFFER_LENGTH 32000
extern int validateJsonBuffer(char *jsonBuffer);
extern int getComboIndex(string comboName);

#if(COMBO_DATA_VECTOR == 1)
	extern vector<ComboDataInt> comboDataVector;
#elif(COMBO_DATA_ARRAY == 1)
	extern ComboDataInt comboDataArray[15];
#elif(COMBO_DATA_MAP == 1)
	extern map<string, ComboDataInt> comboDataMap;
#endif

extern ComboStruct combo;
extern int currentComboStructIndex;
extern int oldComboStructIndex;

struct _segFaultInfo {
	string function;
	int line;
};



ComboDataInt::ComboDataInt()
{
	this->comboFD = 0;
	this->bufferCount = 0;
	this->controlCount = 0;
	this->processCount = 0;
	this->inputProcBufferIndex[0] = 0;
	this->inputProcBufferIndex[1] = 1;
	this->outputProcBufferIndex[0] = 0;
	this->outputProcBufferIndex[1] = 1;

}

ComboDataInt::~ComboDataInt()
{

}

void ComboDataInt::printUnsequencedProcessList()
{
	if(debugOutput) cout << "UNSEQUENCED PROCESSES" << endl;

	for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
	{
		if(debugOutput) cout << this->unsequencedProcessListStruct[i].name << endl;
	}
}

void ComboDataInt::printSequencedProcessList()
{
	if(debugOutput) cout << "SEQUENCED PROCESSES" << endl;

	/*for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
	{
		if(debugOutput) cout << this->processesStruct[i].name << endl;
	}*/
	for(int i = 0; i < 20; i++)
	{
		if(this->processSequence[i].processName.empty() == false)
		{
			if(debugOutput) cout << this->processSequence[i].processName << endl;
			for(int j = 0; j < this->processSequence[i].processInputCount; j++)
			{
				if(debugOutput) cout << "\t" << this->processSequence[i].inputBufferIndexes[j] << ":" << this->processSequence[i].inputBufferNames[j] << endl;
			}
			for(int j = 0; j < this->processSequence[i].parameterCount; j++)
			{
				if(debugOutput) cout << "\t" << this->processSequence[i].parameters[j] << ":" << this->processSequence[i].parameters[j] << endl;
			}
			for(int j = 0; j < this->processSequence[i].processOutputCount; j++)
			{
				if(debugOutput) cout << "\t" << this->processSequence[i].outputBufferIndexes[j] << ":" << this->processSequence[i].outputBufferNames[j] << endl;
			}
		}
		else break;
	}

}

void ComboDataInt::printUnsequencedConnectionJsonList()
{
	if(debugOutput) cout << "UNSEQUENCED JSON CONNECTIONS: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->unsequencedConnectionListJson.size(); i++)
	{
		if(debugOutput) cout << "unsequencedConnectionListJson[" << i << "]: " << this->unsequencedConnectionListJson[i]["src"]["object"] << ":";
		if(debugOutput) cout << this->unsequencedConnectionListJson[i]["src"]["port"] << ">" << this->unsequencedConnectionListJson[i]["dest"]["object"] << ":";
		if(debugOutput) cout << this->unsequencedConnectionListJson[i]["dest"]["port"] << endl;
	}
}

void ComboDataInt::printUnsequencedConnectionStructList()
{
	if(debugOutput) cout << "UNSEQUENCED STRUCT CONNECTIONS: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->unsequencedConnectionListStruct.size(); i++)
	{
		if(debugOutput) cout << "unsequencedConnectionListStruct[" << i << "]: " << this->unsequencedConnectionListStruct[i].src.object << ":";
		if(debugOutput) cout << this->unsequencedConnectionListStruct[i].src.port << ">" << this->unsequencedConnectionListStruct[i].dest.object << ":";
		if(debugOutput) cout << this->unsequencedConnectionListStruct[i].dest.port << endl;
	}
}

void ComboDataInt::printSequencedConnectionJsonList()
{
	if(debugOutput) cout << "SEQUENCED CONNECTIONS JSON: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->connectionsJson.size(); i++)
	{
		if(debugOutput) cout << "connectionListJson[" << i << "]: " << this->connectionsJson[i]["src"]["object"] << ":";
		if(debugOutput) cout << this->connectionsJson[i]["src"]["port"] << ">" << this->connectionsJson[i]["dest"]["object"] << ":";
		if(debugOutput) cout << this->connectionsJson[i]["dest"]["port"] << endl;
	}
}

void ComboDataInt::printSequencedConnectionStructList()
{
	if(debugOutput) cout << "SEQUENCED CONNECTIONS STRUCT: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->connectionsStruct.size(); i++)
	{
		if(debugOutput) cout << "connectionListStruct[" << i << "]: " << this->connectionsStruct[i].src.object << ":";
		if(debugOutput) cout << this->connectionsStruct[i].src.port << ">" << this->connectionsStruct[i].dest.object << ":";
		if(debugOutput) cout << this->connectionsStruct[i].dest.port << endl;
	}
}


void ComboDataInt::printUnsortedParameters(void)
{
	if(debugOutput) cout << "UNSORTED PARAMETERS" << endl;

    for(std::vector<IndexedProcessParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
    {
    	if(debugOutput) cout << "effectName: " << this->unsortedParameterArray[parameterArrayIndex].effectName
    			<< "\t\tprocessName: " << this->unsortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tparamName: " << this->unsortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->unsortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
}

void ComboDataInt::printSortedParameters(void)
{
	if(debugOutput) cout << "SORTED PARAMETERS" << endl;

    for(std::vector<IndexedProcessParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->sortedParameterArray.size(); parameterArrayIndex++)
    {
    	if(debugOutput) cout << "effectName: " << this->sortedParameterArray[parameterArrayIndex].effectName
    			<< "\t\tabsProcessIndex: " << this->sortedParameterArray[parameterArrayIndex].absProcessIndex
    			<< "processName: " << this->sortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tprocessParamIndex: " << this->sortedParameterArray[parameterArrayIndex].processParamIndex
    			<< "paramName: " << this->sortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->sortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
}

void ComboDataInt::printDataReadyList(void)
{
	if(debugOutput) cout << "DATA READY LIST:" << endl;
	for(std::vector<Connector>::size_type i = 0; i < this->dataReadyList.size(); i++)
	{
		if(debugOutput) cout << "dataReadyList[" << i <<"]: " << this->dataReadyList[i].object << ":" << this->dataReadyList[i].port << endl;
	}
}

void ComboDataInt::printControlList(void)
{
	if(debugOutput) cout << "CONTROL LIST:" << endl;
	for(std::vector<Control>::size_type i = 0; i < this->controlsStruct.size(); i++)
	{
		if(debugOutput) cout << "control[" << i << "].name: " << this->controlsStruct[i].name << endl;
		if(debugOutput) cout << "control[" << i << "].parentEffect: " << this->controlsStruct[i].parentEffect << endl;
		if(debugOutput) cout << "control[" << i << "].conType: " << this->controlsStruct[i].conType << endl;
		for(std::vector<Control>::size_type j = 0; j < this->controlsStruct[i].params.size(); j++)
		{
			if(debugOutput) cout << "\tcontrol[" << i << "].params:[" << j << "].name: " << this->controlsStruct[i].params[j].name << endl;
			if(debugOutput) cout << "\tcontrol[" << i << "].params:[" << j << "].alias: " << this->controlsStruct[i].params[j].alias << endl;
			if(debugOutput) cout << "\tcontrol[" << i << "].params:[" << j << "].abbr: " << this->controlsStruct[i].params[j].abbr << endl;
			if(debugOutput) cout << "\tcontrol[" << i << "].params:[" << j << "].value: " << this->controlsStruct[i].params[j].value << endl;
		}
		for(std::vector<unsigned int>::size_type j = 0; j < this->controlsStruct[i].absProcessParameterIndexes.size(); j++)
		{
			if(debugOutput) cout << "\tcontrol[" << i << "].absProcessParameterIndexes:[" << j << "]: " << this->controlsStruct[i].absProcessParameterIndexes[j] << endl;
		}
		for(std::vector<unsigned int>::size_type j = 0; j < this->controlsStruct[i].absProcessParameterIndexesInv.size(); j++)
		{
			if(debugOutput) cout << "\tcontrol[" << i << "].absProcessParameterIndexesInv:[" << j << "]: " << this->controlsStruct[i].absProcessParameterIndexesInv[j] << endl;
		}

	if(debugOutput) cout << endl;
	}
}

void ComboDataInt::printSequencedControlList(void)
{
	if(debugOutput) cout << "SEQUENCED CONTROL LIST:" << endl;
	for(int i = 0; i < 20; i++)
	{
		if(this->controlSequence[i].name.empty() == false)
		{
			if(debugOutput) cout << "controlSequence[" << i << "].name: " << this->controlSequence[i].name << endl;
			if(debugOutput) cout << "controlSequence[" << i << "].conType: " << this->controlSequence[i].conType << endl;
			if(debugOutput) cout << "controlSequence[" << i << "].paramContConnectionCount: " << this->controlSequence[i].paramContConnectionCount << endl;

			for(int j = 0; j < this->controlSequence[i].paramContConnectionCount; j++)
			{
				if(debugOutput) cout << "\tcontrolSequence[" << i << "].paramContConnection:[" << j << "].processIndex: " << this->controlSequence[i].paramContConnection[j].processIndex << endl;
				if(debugOutput) cout << "\tcontrolSequence[" << i << "].paramContConnection:[" << j << "].processParamIndex: " << this->controlSequence[i].paramContConnection[j].processParamIndex << endl;
			}
			if(debugOutput) cout << "controlSequence[" << i << "].paramContConnectionCountInv: " << this->controlSequence[i].paramContConnectionCountInv << endl;

			for(int j = 0; j < this->controlSequence[i].paramContConnectionCountInv; j++)
			{
				if(debugOutput) cout << "\tcontrolSequence[" << i << "].paramContConnectionInv:[" << j << "].processIndex: " << this->controlSequence[i].paramContConnectionInv[j].processIndex << endl;
				if(debugOutput) cout << "\tcontrolSequence[" << i << "].paramContConnectionInv:[" << j << "].processParamIndex: " << this->controlSequence[i].paramContConnectionInv[j].processParamIndex << endl;
			}
		}
		else break;

		if(debugOutput) cout << endl;
	}
}


void ComboDataInt::printControlParameterList()
{
	if(debugOutput) cout << "CONTROL PARAMETER LIST:" << endl;

    for(std::vector<IndexedControlParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->controlParameterArray.size(); parameterArrayIndex++)
    {
    	if(debugOutput) cout << "controlName: " << this->controlParameterArray[parameterArrayIndex].controlName
    			<< "\t\tabsControlIndex: " << this->controlParameterArray[parameterArrayIndex].absControlIndex
    			<< "\t\tcontrolParamIndex: " << this->controlParameterArray[parameterArrayIndex].controlParamIndex
    			/*<< "\t\tabsProcessParamIndex: " << this->controlParameterArray[parameterArrayIndex].absProcessParamIndex*/
    			<< "\t\tcontrolParamName: " << this->controlParameterArray[parameterArrayIndex].controlParamName
    			<< "\t\tcontrolParamValue: " << this->controlParameterArray[parameterArrayIndex].controlParamValue << endl;
    }

}


void ComboDataInt::printProcessParameter(int processParameterIndex)
{
	if(debugOutput) cout << "PROCESS PARAMETER: ";

	if(debugOutput) cout << "processName: " << this->sortedParameterArray[processParameterIndex].processName;
	if(debugOutput) cout << "\tabsProcessIndex: " << this->sortedParameterArray[processParameterIndex].absProcessIndex
			<< "\tprocessParamIndex: " << this->sortedParameterArray[processParameterIndex].processParamIndex
			<< "\tabsParamIndex: " << this->sortedParameterArray[processParameterIndex].absParamIndex
			<< "\tparamName: " << this->sortedParameterArray[processParameterIndex].paramName
			<< "\tparamValue: " << this->sortedParameterArray[processParameterIndex].paramValue << endl;
}


void ComboDataInt::printControlParameter(int controlParameterIndex)
{
	if(debugOutput) cout << "CONTROL PARAMETER: " << controlParameterIndex << endl;
	if(debugOutput) cout << "CONTROL PARAMETER ARRAY SIZE: " << this->controlParameterArray.size() << endl;

	if(debugOutput) cout << "controlName: " << this->controlParameterArray[controlParameterIndex].controlName
			<< "\t\tabsControlIndex: " << this->controlParameterArray[controlParameterIndex].absControlIndex
			<< "\t\tcontrolParamIndex: " << this->controlParameterArray[controlParameterIndex].controlParamIndex
			/*<< "\t\tabsProcessParamIndex: " << this->controlParameterArray[controlParameterIndex].absProcessParamIndex*/
			<< "\t\tcontrolParamName: " << this->controlParameterArray[controlParameterIndex].controlParamName
			<< "\t\tcontrolParamValue: " << this->controlParameterArray[controlParameterIndex].controlParamValue << endl;
}


void ComboDataInt::printControlConnectionList(void)
{
	if(debugOutput) cout << "CONTROL CONNECTION LIST:" << endl;

	for(std::vector<ControlConnection>::size_type i = 0; i < this->controlConnectionsStruct.size(); i++)
	{
		if(debugOutput) cout << "controlConnection[" << i << "]: " << this->controlConnectionsStruct[i].src.object << ":" << this->controlConnectionsStruct[i].src.port << ">" << this->controlConnectionsStruct[i].dest.object << ":" << this->controlConnectionsStruct[i].dest.port << endl;
	}
	if(debugOutput) cout << endl;
}

void ComboDataInt::printBufferList(void)
{
	if(debugOutput) cout << "PROCESS BUFFER LIST:" << endl;
	for(std::vector<ProcessBuffer>::size_type i = 0; i < 60; i++)
	{
		if(this->procBufferArray[i].processName.empty() == false)
		{
			if(debugOutput) cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
		}
		else break;
	}
	if(debugOutput) cout << endl;

}

void ComboDataInt::readUnsequencedConnectionListJsonIntoUnsequencedConnectionListStruct()
{
	for(std::vector<Json::Value>::size_type connectionIndex = 0; connectionIndex < this->unsequencedConnectionListJson.size(); connectionIndex++)
	{
		ProcessConnection procConn = ProcessConnection();
		procConn.src.object = this->unsequencedConnectionListJson[connectionIndex]["src"]["object"].asString();
		procConn.src.port = this->unsequencedConnectionListJson[connectionIndex]["src"]["port"].asString();
		procConn.dest.object = this->unsequencedConnectionListJson[connectionIndex]["dest"]["object"].asString();
		procConn.dest.port = this->unsequencedConnectionListJson[connectionIndex]["dest"]["port"].asString();
		this->unsequencedConnectionListStruct.push_back(procConn);
	}
}


#define dbg 0
int ComboDataInt::getTargetProcessIndex(string processName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getTargetProcessIndex" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif
	int targetProcessIndex = 0;
	// get index for target process
#if(dbg >=2)
	if(debugOutput) cout << "process to search for index of: " << processName << endl;
#endif
	try
	{
		for(std::vector<Process>::size_type processIndex = 0; processIndex < this->unsequencedProcessListStruct.size(); processIndex++)
		{
#if(dbg >=2)
			if(debugOutput) cout << "comparing: " << processName << " & " << this->unsequencedProcessListStruct[processIndex].name << endl;
#endif

			if(processName.compare(this->unsequencedProcessListStruct[processIndex].name) == 0)
			{
				targetProcessIndex = processIndex;
				break;
			}
			if(processIndex == this->unsequencedProcessListStruct.size() - 1) //end of connection array reach, but no match found
			{
				targetProcessIndex = -1;
			}
		}
	}
	catch(std::exception &e)
	{
		 if(debugOutput) cout << "exception in ? " << e.what() <<  endl;
	}
#if(dbg >= 2)
	if(targetProcessIndex >= 0)
		if(debugOutput) cout << "target process index: " << targetProcessIndex << ":" << this->unsequencedProcessListStruct[targetProcessIndex].name << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getTargetProcessIndex: " << targetProcessIndex << endl;
#endif
	return targetProcessIndex;
}

#define dbg 0
std::vector<string> ComboDataInt::getProcessInputs(string processName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getProcessInputs" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif
	std::vector<string> inputs;
	int targetIndex = this->getTargetProcessIndex(processName);

	for(std::vector<Process>::size_type inputIndex = 0; inputIndex < this->unsequencedProcessListStruct[targetIndex].inputs.size(); inputIndex++)
	{
		inputs.push_back(this->unsequencedProcessListStruct[targetIndex].inputs[inputIndex]);
	}

#if(dbg >= 2)
	for(std::vector<string>::size_type i = 0; i < inputs.size(); i++)
	{
		if(debugOutput) cout << "inputs[" << i <<"]: " << inputs[i] << endl;
	}
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getProcessInputs: " << inputs.size() << endl;
#endif

	return inputs;
}


#define dbg 0
int ComboDataInt::fillUnsequencedProcessList()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::fillUnsequencedProcessList" << endl;
#endif
	int status = 0;


	try
	{
		/*if(this->unsequencedProcessListStruct.empty() == false)
		{
			this->unsequencedProcessListStruct.clear();// = new std::vector<Json::Value>;
		}*/


		Json::Value tempEffects = this->effectComboJson["effectArray"];
		int tempEffectCount = tempEffects.size();
#if(dbg >= 1)
		if(debugOutput) cout << "TRANSFERRING PROCESS DATA FROM JSON TO STRUCT..." << endl;
#endif
		for(int effectIndex = 0; (effectIndex < tempEffectCount); effectIndex++)
	    {
	    	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
	        int tempProcCount = tempProcs.size();
	    	for(int procIndex = 0; procIndex < tempProcCount; procIndex++)
	    	{

	    		struct Process tempProc;
	    		tempProc.name = tempProcs[procIndex]["name"].asString();
	    		tempProc.procType = tempProcs[procIndex]["procType"].asString();

				try
				{
					tempProc.footswitchType = tempProcs[procIndex]["footswitchType"].asInt();
				}
				catch(std::exception &e)
				{
					tempProc.footswitchType =  atoi(tempProcs[procIndex]["footswitchType"].asString().c_str());
				}


				try
				{
					tempProc.footswitchNumber = tempProcs[procIndex]["footswitchNumber"].asInt();
				}
				catch(std::exception &e)
				{
					tempProc.footswitchNumber =  atoi(tempProcs[procIndex]["footswitchNumber"].asString().c_str());
				}

				for(std::vector<Json::Value>::size_type inputIndex = 0; inputIndex < tempProcs[procIndex]["inputArray"].size(); inputIndex++)
				{
					Json::Value input = this->getJsonValueFromUnorderedJsonValueList(inputIndex, "inputArray", tempProcs[procIndex]);
					tempProc.inputs.push_back(input["name"].asString());
				}

				for(std::vector<Json::Value>::size_type outputIndex = 0; outputIndex < tempProcs[procIndex]["outputArray"].size(); outputIndex++)
				{
					Json::Value output = this->getJsonValueFromUnorderedJsonValueList(outputIndex, "outputArray", tempProcs[procIndex]);
					tempProc.outputs.push_back(output["name"].asString());
				}

				for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < tempProcs[procIndex]["paramArray"].size(); paramIndex++)
				{
					Json::Value tempParameter = this->getJsonValueFromUnorderedJsonValueList(paramIndex, "paramArray", tempProcs[procIndex]);
					ProcessParams tempProcParam;
					tempProcParam.name = tempParameter["name"].asString();

					try
					{
						tempProcParam.paramType = tempParameter/*tempProcs[procIndex]["paramArray"][paramIndex]*/["procType"].asInt();
					}
					catch(std::exception &e)
					{
						tempProcParam.paramType =  atoi(tempParameter/*tempProcs[procIndex]["paramArray"][paramIndex]*/["procType"].asString().c_str());
					}

					try
					{
						tempProcParam.value = tempParameter/*tempProcs[procIndex]["paramArray"][paramIndex]*/["value"].asInt();
					}
					catch(std::exception &e)
					{
						tempProcParam.value =  atoi(tempParameter/*tempProcs[procIndex]["paramArray"][paramIndex]*/["value"].asString().c_str());
					}

		    		tempProc.params.push_back(tempProcParam);
				}
	    		this->unsequencedProcessListJson.push_back(tempProcs[procIndex]);
	    		this->unsequencedProcessListStruct.push_back(tempProc);
	    	}
	    }
	}
	catch(std::exception &e)
	{
		 cerr << "exception: " << e.what() <<  endl;
		 status = -1;
	}


#if(dbg >= 2)
    for(int procIndex = 0; procIndex < this->unsequencedProcessListStruct.size(); procIndex++)
    {
		if(debugOutput) cout << "name: " << this->unsequencedProcessListStruct[procIndex].name << endl;
		if(debugOutput) cout << "procType: " << this->unsequencedProcessListStruct[procIndex].procType << endl;
    	if(debugOutput) cout << "footswitchType: " << this->unsequencedProcessListStruct[procIndex].footswitchType << endl;
		if(debugOutput) cout << "footswitchNumber: " << this->unsequencedProcessListStruct[procIndex].footswitchNumber << endl;

		for(int inputIndex = 0; inputIndex < this->unsequencedProcessListStruct[procIndex].inputs.size(); inputIndex++)
		{
    		if(debugOutput) cout << "inputs[" << inputIndex << "]: " << this->unsequencedProcessListStruct[procIndex].inputs[inputIndex] << endl;
		}

		for(int outputIndex = 0; outputIndex < this->unsequencedProcessListStruct[procIndex].outputs.size(); outputIndex++)
		{
    		if(debugOutput) cout << "outputs[" << outputIndex << "]: " << this->unsequencedProcessListStruct[procIndex].outputs[outputIndex] << endl;
		}

		for(int paramIndex = 0; paramIndex < this->unsequencedProcessListStruct[procIndex].params.size(); paramIndex++)
		{
			if(debugOutput) cout << "params[" << paramIndex << "].name: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].name;
			if(debugOutput) cout << "params[" << paramIndex << "].paramType: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].paramType;
			if(debugOutput) cout << "params[" << paramIndex << "].value: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].value;
    		if(debugOutput) cout << endl;
		}
		if(debugOutput) cout << endl;
    }
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::fillUnsequencedProcessList: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::areDataBuffersReadyForProcessInputs(string processName)
{
	// since data buffers are fed by process outputs, the dataReadyList contains the output process:port names

	// get process outputs that feed data buffers using process inputs and connection list.
	// Start at known process inputs and use connection list to work backwards toward outputs of previous processes.

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::areDataBuffersReadyForProcessInputs" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif
	//bool allProcessOutputsContainedInConnectionList = true;
	//bool dataReadyMatches[10] = {true,true,true,true,true,true,true,true,true,true};
	bool allProcessOutputsContainedInConnectionList = false;
	//bool dataReadyMatches[10] = {false,false,false,false,false,false,false,false,false,false};
	vector<int> dataReadyMatches;
	int dataReadyMatchSum = 0;

	std::vector<string> inputs = getProcessInputs(processName);
	std::vector<Connector> procOutputs;
#if(dbg >= 2)
	if(debugOutput) cout << "target process: " << processName << endl;
	this->printDataReadyList();
	this->printUnsequencedConnectionList();
#endif
	for(std::vector<string>::size_type inputIndex = 0; inputIndex < inputs.size(); inputIndex++)
	{

		//for(std::vector<Connector>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListJson.size(); connIndex++)
		for(std::vector<Connector>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListStruct.size(); connIndex++)
		{
			ProcessConnection tempProcConn;
			tempProcConn = this->unsequencedConnectionListStruct[connIndex];
			/*tempProcConn.src.object = this->unsequencedConnectionListStruct[connIndex].src.object;
			tempProcConn.src.port = this->unsequencedConnectionListStruct[connIndex].src.port;
			tempProcConn.dest.object = this->unsequencedConnectionListStruct[connIndex].dest.object;
			tempProcConn.dest.port = this->unsequencedConnectionListStruct[connIndex].dest.port;*/

			// if connection dest process:port matches process input, put src process:port in procOutputs
			if(processName.compare(tempProcConn.dest.object) == 0 && inputs[inputIndex].compare(tempProcConn.dest.port)==0)
			{
				Connector tempConn;
				tempConn.object = tempProcConn.src.object;
				tempConn.port = tempProcConn.src.port;
				procOutputs.push_back(tempConn);
			}
		}
	}

	if(procOutputs.empty() == false)
	{
		// are output process:ports that feed relevant data buffers in dataReadyList
		for(std::vector<Connector>::size_type dataReadyIndex = 0; dataReadyIndex < this->dataReadyList.size(); dataReadyIndex++)
		{
			for(std::vector<Connector>::size_type outputIndex = 0; outputIndex < procOutputs.size(); outputIndex++)
			{
				if(procOutputs[outputIndex].object.compare(this->dataReadyList[dataReadyIndex].object) == 0 &&
						procOutputs[outputIndex].port.compare(this->dataReadyList[dataReadyIndex].port) == 0)
				{
					dataReadyMatches.push_back(1);//[dataReadyIndex] = true;
					break;
				}
				if(dataReadyIndex == this->dataReadyList.size() - 1) //end of connection array reach, but no match found
				{
					dataReadyMatches.push_back(0);//[dataReadyIndex] = false;
				}
			}
		}
	}
	else
	{
		dataReadyMatches.push_back(0);//[0] = 0;
	}

#if(dbg >= 2)
	if(debugOutput) cout << "dataReadyMatches: ";
	for(int i = 0; i < dataReadyMatches.size(); i++)
	{
		if(debugOutput) cout << dataReadyMatches[i] << ",";
	}
	if(debugOutput) cout << endl;
#endif
	for(std::vector<int>::size_type dataReadyIndex = 0; dataReadyIndex < dataReadyMatches.size(); dataReadyIndex++)
	{
		dataReadyMatchSum += dataReadyMatches[dataReadyIndex];
		//allProcessOutputsContainedInConnectionList &= dataReadyMatches[dataReadyIndex];
	}

	if(dataReadyMatchSum > 0) allProcessOutputsContainedInConnectionList = true;
	//else if()
	else allProcessOutputsContainedInConnectionList = false;

#if(dbg >= 1)
	//if(debugOutput) cout << "allProcessOutputsContainedInConnectionList: " << allProcessOutputsContainedInConnectionList << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::areDataBuffersReadyForProcessInputs:" << allProcessOutputsContainedInConnectionList << endl;
#endif

	return allProcessOutputsContainedInConnectionList;
}

#define dbg 0
int ComboDataInt::areAllProcessInputsSatisfied(string processName)
{
	/*	status = 2: all inputs satisfied
	 * status = 1: some inputs satisfied
	 * status = 0: no inputs satisfied
	 */
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::areAllProcessInputsSatisfied" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif

	int status = 0;
	std::vector<string> inputs = getProcessInputs(processName);
	std::vector<ProcessConnection> connections;
	int inputPortCount = inputs.size();
	int inputPortSatisfiedCount = 0;

	// Get all connections connecting to the inputs of process "processName"
	for(int inputPortIndex = 0; inputPortIndex < inputPortCount; inputPortIndex++)
	{
		for(int connIndex = 0; connIndex < this->unsequencedConnectionListStruct.size(); connIndex++)
		{
			if((this->unsequencedConnectionListStruct[connIndex].dest.object.compare(processName) == 0) &&
					(this->unsequencedConnectionListStruct[connIndex].dest.port.compare(inputs[inputPortIndex]) == 0))
			{
				connections.push_back(this->unsequencedConnectionListStruct[connIndex]);
			}
		}
	}

	int connectionsCount = connections.size();

	for(int connectionsIndex = 0; connectionsIndex < connectionsCount; connectionsIndex++)
	{
		for(int dataReadyListIndex = 0; dataReadyListIndex < this->dataReadyList.size(); dataReadyListIndex++)
		{
#if(dbg >= 2)
				if(debugOutput) cout << "comparing: " << connections[connectionsIndex].src.object << ":" << connections[connectionsIndex].src.port << " to " << this->dataReadyList[dataReadyListIndex].object << ":" << this->dataReadyList[dataReadyListIndex].port << endl;
#endif

			if((this->dataReadyList[dataReadyListIndex].object.compare(connections[connectionsIndex].src.object) == 0) &&
					(this->dataReadyList[dataReadyListIndex].port.compare(connections[connectionsIndex].src.port) == 0))
			{
#if(dbg >= 2)
				if(debugOutput) cout << "inputPortSatisfied: " << connections[connectionsIndex].src.object << ":" << connections[connectionsIndex].src.port << endl;
#endif
				inputPortSatisfiedCount++;
			}
		}
	}

	if(inputPortSatisfiedCount == inputPortCount) status = 2;
	else if(inputPortSatisfiedCount > 0) status = 1;
	//else status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::areAllProcessInputsSatisfied:" << status << endl;
#endif

	return status;
}

#define dbg 0
bool ComboDataInt::isUnsequencedProcessListEmpty()
{
	bool isListEmpty;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::isUnsequencedProcessListEmpty" << endl;
#endif

	if(this->unsequencedProcessListStruct.size() == 0) isListEmpty = true;
	else isListEmpty = false;
#if(dbg >= 2)
	if(debugOutput) cout << "unsequenced processes: " << endl;
	this->printUnsequencedProcessList();
#endif

#if(dbg >= 1)
	//if(debugOutput) cout << "isListEmpty: " << isListEmpty << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::isUnsequencedProcessListEmpty: " << isListEmpty << endl;
#endif
	return isListEmpty;
}

#define dbg 0
bool ComboDataInt::isOutputInDataReadyList(Connector output)
{
	bool inList = false;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::isOutputInDataReadyList" << endl;
	if(debugOutput) cout << "Connector process: " << output.object << "\t port: " << output.port << endl;
#endif

	for(unsigned int listedOutputIndex = 0; listedOutputIndex < this->dataReadyList.size(); listedOutputIndex++)
	{
		string listedOutputProcess = this->dataReadyList[listedOutputIndex].object;
		string listedOutputPort = this->dataReadyList[listedOutputIndex].port;
		if(output.object.compare(listedOutputProcess) == 0 && output.port.compare(listedOutputPort) == 0)
		{
			inList = true;
			break;
		}
	}

#if(dbg >= 1)
	//if(debugOutput) cout << "isListEmpty: " << isListEmpty << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::isOutputInDataReadyList: " << inList << endl;
#endif

	return inList;
}


#define dbg 0
Json::Value ComboDataInt::getJsonValueFromUnorderedJsonValueList(int index, string arrayName, Json::Value unorderedListContainer)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getJsonValueFromUnorderedJsonValueList" << endl;
	if(debugOutput) cout << "index: " << index << "\tarrayName: " << arrayName << endl;
#endif

	Json::Value listItem;
	Json::Value unorderedList = unorderedListContainer[arrayName];
	int listItemIndex = 0;

	for(std::vector<Json::Value>::size_type unorderedListIndex = 0; unorderedListIndex < unorderedList.size(); unorderedListIndex++)
	{
#if(dbg >= 2)
		if(debugOutput) cout << unorderedList[unorderedListIndex]["index"].asString() << ":";
		if(unorderedList[unorderedListIndex]["alias"].isNull() == false)
		{
			if(debugOutput) cout << unorderedList[unorderedListIndex]["alias"].asString() << endl;
		}
		else
		{
			if(debugOutput) cout << unorderedList[unorderedListIndex]["name"].asString() << endl;
		}

#endif

		if(atoi(unorderedList[unorderedListIndex]["index"].asString().c_str()) == index)
		{
			listItemIndex = unorderedListIndex;
			listItem = unorderedList[unorderedListIndex];
			break;
		}
	}
#if(dbg >= 1)

	if(debugOutput) cout << "***** EXITING: ComboDataInt::getJsonValueFromUnorderedJsonValueList: ";
	if(debugOutput) cout << listItem["index"].asString() << ":";
	if(listItem["alias"].isNull() == false)
	{
		if(debugOutput) cout << listItem["alias"].asString() << endl;
	}
	else
	{
		if(debugOutput) cout << listItem["name"].asString() << endl;
	}
#endif

	return listItem;
}

#define dbg 0
Json::Value ComboDataInt::getJsonValueFromUnorderedJsonValueList(int index, Json::Value unorderedList)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getJsonValueFromUnorderedJsonValueList2" << endl;
	if(debugOutput) cout << "index: " << index << "\tarrayName: " << arrayName << endl;
#endif

	Json::Value listItem;
	//Json::Value unorderedList = unorderedListContainer[arrayName];

	for(std::vector<Json::Value>::size_type unorderedListIndex = 0; unorderedListIndex < unorderedList.size(); unorderedListIndex++)
	{
		if(atoi(unorderedList[unorderedListIndex]["index"].asString().c_str()) == index)
		{
			listItem = unorderedList[unorderedListIndex];
			break;
		}
	}
#if(dbg >= 1)

	if(debugOutput) cout << "***** EXITING: ComboDataInt::getJsonValueFromUnorderedJsonValueList2: " <<  endl;
#endif

	return listItem;
}


#define dbg 0
string ComboDataInt::getFirstProcess()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getFirstProcess" << endl;
#endif
	Connector start;
	start.object/*string srcProcess*/ = "system";
	start.port/*string srcPort*/ = "capture_1";

	string firstProcess;
	//bool foundNextProcess = false;


	for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListStruct.size(); connIndex++)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "comparing: ";
		if(debugOutput) cout << start.object << " vs " << this->unsequencedConnectionListStruct[connIndex].src.object;//["src"]["object"];
		if(debugOutput) cout << "\t and \t";
		if(debugOutput) cout << start.port << " vs " << this->unsequencedConnectionListStruct[connIndex].src.port/*["src"]["port"]*/ << endl;
#endif
		if((start.object.compare(this->unsequencedConnectionListStruct[connIndex].src.object/*["src"]["object"].asString()*/) == 0) /*||
				(start.object.compare(this->unsequencedConnectionListStruct[connIndex]["srcEffect"].asString()) == 0))*/ &&
				(start.port.compare(this->unsequencedConnectionListStruct[connIndex].src.port/*["src"]["port"].asString()*/) == 0))
		{
			/*Connector tempConn;
			tempConn.object = this->unsequencedConnectionListStruct[connIndex]["src"]["object"].asString();
			tempConn.port = this->unsequencedConnectionListStruct[connIndex]["src"]["port"].asString();
			//if(foundNextProcess == false)*/
			{
				firstProcess = this->unsequencedConnectionListStruct[connIndex].dest.object;
				//this->dataReadyList.push_back(tempConn);
				break;//foundNextProcess = true;
			}
		}
	}

#if(dbg >= 2)
	for(std::vector<Connector>::size_type i = 0; i < this->dataReadyList.size(); i++)
	{
		if(debugOutput) cout << "dataReadyList[" << i <<"]: " << dataReadyList[i].object << ":" << dataReadyList[i].port << endl;
	}
	//if(debugOutput) cout << "firstProcess: " << firstProcess << endl;
#endif

#if(dbg >= 1)

	if(debugOutput) cout << "***** EXITING: ComboDataInt::getFirstProcess: " << firstProcess << endl;
#endif

	return firstProcess;
}

#define dbg 0
std::vector<string> ComboDataInt::getFirstProcesses()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getFirstProcess" << endl;
#endif
	Connector start;
	start.object/*string srcProcess*/ = "system";
	start.port/*string srcPort*/ = "capture_1";

	string firstProcess;
	std::vector<string> firstProcesses;
	//bool foundNextProcess = false;


	for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListStruct.size(); connIndex++)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "comparing: ";
		if(debugOutput) cout << start.object << " vs " << this->unsequencedConnectionListStruct[connIndex].src.object;//["src"]["object"];
		if(debugOutput) cout << "\t and \t";
		if(debugOutput) cout << start.port << " vs " << this->unsequencedConnectionListStruct[connIndex].src.port/*["src"]["port"]*/ << endl;
#endif
		if((start.object.compare(this->unsequencedConnectionListStruct[connIndex].src.object/*["src"]["object"].asString()*/) == 0) /*||
				(start.object.compare(this->unsequencedConnectionListStruct[connIndex]["srcEffect"].asString()) == 0))*/ &&
				(start.port.compare(this->unsequencedConnectionListStruct[connIndex].src.port/*["src"]["port"].asString()*/) == 0))
		{
			/*Connector tempConn;
			tempConn.object = this->unsequencedConnectionListStruct[connIndex]["src"]["object"].asString();
			tempConn.port = this->unsequencedConnectionListStruct[connIndex]["src"]["port"].asString();
			//if(foundNextProcess == false)
			{
				firstProcess = this->unsequencedConnectionListStruct[connIndex].dest.object;
				//this->dataReadyList.push_back(tempConn);
				break;//foundNextProcess = true;
			}*/
			firstProcess = this->unsequencedConnectionListStruct[connIndex].dest.object;
			if(std::find(firstProcesses.begin(),firstProcesses.end(),firstProcess)==firstProcesses.end()) // procName not in nextProcesses
			{
				firstProcesses.push_back(firstProcess);
			}

		}
	}

#if(dbg >= 2)
	for(std::vector<Connector>::size_type i = 0; i < this->dataReadyList.size(); i++)
	{
		if(debugOutput) cout << "dataReadyList[" << i <<"]: " << dataReadyList[i].object << ":" << dataReadyList[i].port << endl;
	}
	//if(debugOutput) cout << "firstProcess: " << firstProcess << endl;
#endif

#if(dbg >= 1)
	//if(debugOutput) cout << "nextProcess: " << nextProcess << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getFirstProcesses: ";
	for(std::vector<string>::size_type i = 0; i < firstProcesses.size(); i++)
	{
		if(debugOutput) cout << firstProcesses[i] << ", ";
	}
	if(debugOutput) cout << endl;

#endif

	return firstProcesses;
}

#define dbg 0
string ComboDataInt::getNextProcess()
{
	string nextProcess;
	bool foundNextProcess = false;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getNextProcess" << endl;
#endif

	for(std::vector<Process>::size_type procIndex = 0; procIndex < this->unsequencedProcessListStruct.size(); procIndex++)
	{
		string procName = this->unsequencedProcessListStruct[procIndex].name;
		if(this->areDataBuffersReadyForProcessInputs(procName) == true)
		{
			nextProcess = procName;
			break;
		}
	}

#if(dbg >= 2)
	for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
	{
		if(debugOutput) cout << "unsequencedProcessListStruct[" << i <<"]: " << unsequencedProcessListStruct[i].name << endl;
	}

#endif

#if(dbg >= 1)
	//if(debugOutput) cout << "nextProcess: " << nextProcess << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getNextProcess: " << nextProcess << endl;
#endif

	return nextProcess;
}


#define dbg 0
std::vector<string> ComboDataInt::getNextProcesses()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getNextProcesses" << endl;
#endif
	std::vector<string> nextProcesses;
	bool foundNextProcesses = false;


	for(int inputSatisfactionLevel = 2; (inputSatisfactionLevel > 0) && (foundNextProcesses == false); inputSatisfactionLevel--)
	{
		for(std::vector<Process>::size_type procIndex = 0; procIndex < this->unsequencedProcessListStruct.size(); procIndex++)
		{
			string procName = this->unsequencedProcessListStruct[procIndex].name;
			/*if(this->areDataBuffersReadyForProcessInputs(procName) == true)
			{
				if(std::find(nextProcesses.begin(),nextProcesses.end(),procName)==nextProcesses.end()) // procName not in nextProcesses
				{
					nextProcesses.push_back(procName);
				}
			}*/
			if(this->areAllProcessInputsSatisfied(procName) == inputSatisfactionLevel) // all inputs satisfied.  Check for this first.
			{
				if(std::find(nextProcesses.begin(),nextProcesses.end(),procName)==nextProcesses.end()) // procName not in nextProcesses
				{
					nextProcesses.push_back(procName);
					foundNextProcesses = true;
				}
			}
			/*else if(this->areAllProcessInputsSatisfied(procName) == 1) // some inputs satisfied
			{
				if(std::find(nextProcesses.begin(),nextProcesses.end(),procName)==nextProcesses.end()) // procName not in nextProcesses
				{
					nextProcesses.push_back(procName);
				}
			}*/
		}
	}

#if(dbg >= 2)
	for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
	{
		if(debugOutput) cout << "unsequencedProcessListStruct[" << i <<"]: " << unsequencedProcessListStruct[i].name << endl;
	}

#endif

#if(dbg >= 1)
	//if(debugOutput) cout << "nextProcess: " << nextProcess << endl;
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getNextProcesses: ";
	for(std::vector<string>::size_type i = 0; i < nextProcesses.size(); i++)
	{
		if(debugOutput) cout << nextProcesses[i] << ", ";
	}
	if(debugOutput) cout << endl;

#endif

	return nextProcesses;
}

/*#define dbg 0
int ComboDataInt::addFirstConnectionToConnectionList()
{
	int status = 0;

	string srcProcess = "system";
	string srcPort = "capture_1";

	this->addNextConnectionToConnectionList(srcProcess, srcPort);

	return status;
}

#define dbg 0
int ComboDataInt::addNextConnectionToConnectionList(string srcProcess, string srcPort)
{
	int status = 0;

	for(int connIndex = 0; connIndex < this->connectionsJson.size(); connIndex++)
	{
		if(srcProcess.compare(this->connectionsJson[connIndex]["src"]["object"].asString()) == 0 &&
				srcPort.compare(this->connectionsJson[connIndex]["src"]["port"].asString()) == 0)
		{
			this->connectionListJson.push_back(this->connectionsJson[connIndex]);
		}
	}

	return status;
}*/


#define dbg 0
int ComboDataInt::transferProcessToSequencedProcessList(string processName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::transferProcessToSequencedProcessList" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif
	int status = 0;

	try
	{
		int targetProcessIndex = 0;
		// get index for target process
		targetProcessIndex = this->getTargetProcessIndex(processName);
		if(targetProcessIndex >= 0)
		{
			this->processesStruct.push_back(this->unsequencedProcessListStruct[targetProcessIndex]);
			this->unsequencedProcessListStruct.erase(this->unsequencedProcessListStruct.begin() + targetProcessIndex);
		}
		else
		{
			status = -1;
		}
#if(dbg >= 2)
		if(debugOutput) cout << "targetProcessIndex: " << targetProcessIndex << endl;
		if(debugOutput) cout << "unsequenced processes: " << endl;
		for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
		{
			if(debugOutput) cout << this->unsequencedProcessListStruct[i].name << endl;
		}

		if(debugOutput) cout << "sequenced processes: " << endl;
		for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
		{
			if(debugOutput) cout << this->processesStruct[i].name << endl;
		}
	#endif

	}
	catch(std::exception &e)
	{
		 if(debugOutput) cout << "exception in ? section " << e.what() <<  endl;
		 status = -1;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::transferProcessToSequencedProcessList: " << status << endl;
#endif
	return status;
}


#define dbg 0
int ComboDataInt::transferProcessesToSequencedProcessList(vector<string> processNames)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::transferProcessesToSequencedProcessList" << endl;
	for(std::vector<string>::size_type i = 0; i < processNames.size(); i++)
	{
		if(debugOutput) cout << processNames[i] << ", ";
	}
#endif
	int status = 0;

	try
	{
		for(std::vector<string>::size_type i = 0; i < processNames.size(); i++)
		{
			int targetProcessIndex = 0;
			// get index for target process
			targetProcessIndex = this->getTargetProcessIndex(processNames[i]);
			if(targetProcessIndex >= 0)
			{
				this->processesStruct.push_back(this->unsequencedProcessListStruct[targetProcessIndex]);
				this->unsequencedProcessListStruct.erase(this->unsequencedProcessListStruct.begin() + targetProcessIndex);
			}
			else
			{
				status = -1;
				break;
			}
	#if(dbg >= 2)
			if(debugOutput) cout << "targetProcessIndex: " << targetProcessIndex << endl;
			if(debugOutput) cout << "unsequenced processes: " << endl;
			for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
			{
				if(debugOutput) cout << this->unsequencedProcessListStruct[i].name << endl;
			}

			if(debugOutput) cout << "sequenced processes: " << endl;
			for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
			{
				if(debugOutput) cout << this->processesStruct[i].name << endl;
			}
		#endif
		}
	}
	catch(std::exception &e)
	{
		 if(debugOutput) cout << "exception in ? section " << e.what() <<  endl;
		 status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::transferProcessesToSequencedProcessList: " << status << endl;
#endif
	return status;
}



/*#define dbg 0
void ComboDataInt::deleteConnectionsFromConnectionListContainingProcessInputs(string processName)
{
	int targetConnectionIndex = 0;

	for(int connIndex = 0; connIndex < this->connectionListJson.size(); connIndex++)
	{
		targetConnectionIndex = this->getConnectionDestinationIndex(processName);
		if(targetConnectionIndex >= 0)
		{
			this->connectionListJson.erase(this->connectionListJson.begin()+connIndex);
#if(dbg >= 1)
			if(debugOutput) cout << "deleting index: " << connIndex << endl;
#endif
		}
#if(dbg >= 1)

		for(int i = 0; i < this->connectionListJson.size(); i++)
		{
			if(debugOutput) cout << "connectionListJson[" << i << "]: " << this->connectionListJson[i]["src"]["object"] << ":";
			if(debugOutput) cout << this->connectionListJson[i]["src"]["port"] << ">" << this->connectionListJson[i]["dest"]["object"] << ":";
			if(debugOutput) cout << this->connectionListJson[i]["dest"]["port"] << endl;
		}
#endif
	}
}*/

#define dbg 0
int ComboDataInt::addOutputConnectionsToDataReadyList(string processName)
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::addOutputConnectionsToDataReadyList" << endl;
	if(debugOutput) cout << "processName: " << processName << endl;
#endif
	int status = 0;

	try
	{
		int procIndex = this->getTargetProcessIndex(processName);

		if(procIndex >= 0)
		{
			for(std::vector<Process>::size_type outputIndex = 0; outputIndex < this->unsequencedProcessListStruct[procIndex].outputs.size(); outputIndex++)
			{
				Connector tempConn;
				tempConn.object = processName;
				tempConn.port = this->unsequencedProcessListStruct[procIndex].outputs[outputIndex];
				if(this->isOutputInDataReadyList(tempConn) == false)
					this->dataReadyList.push_back(tempConn);
			}
		}
		else
		{
			status = -1;
		}
	}
	catch(std::exception &e)
	{
		 if(debugOutput) cout << "exception in getTargetProcessIndex(processName) section " << e.what() <<  endl;
		 status = -1;
	}

#if(dbg >= 2)
	this->printDataReadyList();
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::addOutputConnectionsToDataReadyList: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::addOutputConnectionsToDataReadyList(vector<string> processNames)
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::addOutputConnectionsToDataReadyList" << endl;
	for(std::vector<string>::size_type i = 0; i < processNames.size(); i++)
	{
		if(debugOutput) cout << processNames[i] << ", ";
	}
	if(debugOutput) cout << endl;
#endif
	int status = 0;

	try
	{
		for(std::vector<string>::size_type i = 0; i < processNames.size(); i++)
		{
			int procIndex = this->getTargetProcessIndex(processNames[i]);

			if(procIndex >= 0)
			{
				for(std::vector<Process>::size_type outputIndex = 0; outputIndex < this->unsequencedProcessListStruct[procIndex].outputs.size(); outputIndex++)
				{
					Connector tempConn;
					tempConn.object = processNames[i];
					tempConn.port = this->unsequencedProcessListStruct[procIndex].outputs[outputIndex];
					if(this->isOutputInDataReadyList(tempConn) == false)
						this->dataReadyList.push_back(tempConn);
				}
			}
			else
			{
				status = -1;
				break;
			}
		}
	}
	catch(std::exception &e)
	{
		 if(debugOutput) cout << "exception in getTargetProcessIndex(processName) section " << e.what() <<  endl;
		 status = -1;
	}

#if(dbg >= 2)
	this->printDataReadyList();
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::addOutputConnectionsToDataReadyList: " << status << endl;
#endif
	return status;
}





#define dbg 0
ProcessConnection ComboDataInt::mergeConnections(ProcessConnection srcConn, ProcessConnection destConn)
{
	ProcessConnection mergedConnection;
#if(dbg >= 1)
	if(debugOutput) cout << "ComboDataInt::mergeConnections" << endl;
	if(debugOutput) cout << "srcConn: " << srcConn.src.object << ":" << srcConn.src.port << ">"
			<< srcConn.dest.object << ":" << srcConn.dest.port << " & ";
	if(debugOutput) cout << "destConn: " << destConn.src.object << ":" << destConn.src.port << ">"
			<< destConn.dest.object << ":" << destConn.dest.port << endl;
#endif

	mergedConnection.src.object = srcConn.src.object;
	mergedConnection.src.port = srcConn.src.port;

	mergedConnection.dest.object = destConn.dest.object;
	mergedConnection.dest.port = destConn.dest.port;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::mergeConnections: ";// << status << endl;
	if(debugOutput) cout << "mergeConnection: " << mergedConnection.src.object << ":" << mergedConnection.src.port << ">"
			<< mergedConnection.dest.object << ":" << mergedConnection.dest.port << endl;
#endif
	return mergedConnection;
}



#define dbg 0
bool ComboDataInt::compareConnectionsSrc2Dest(ProcessConnection srcConn, ProcessConnection destConn)
{
	bool result = false;

#if(dbg >= 1)
	if(debugOutput) cout << "ComboDataInt::compareConnectionsSrc2Dest" << endl;
	if(debugOutput) cout << "\tsrcConn: " << srcConn.src.object << ":" << srcConn.src.port << ">"
			<<  srcConn.dest.object << ":" << srcConn.dest.port << " & ";
	if(debugOutput) cout << "destConn: " << destConn.src.object << ":" << destConn.src.port << ">"
			<<  destConn.dest.object << ":" << destConn.dest.port << endl;
#endif

	bool objectMatch = destConn.src.object.compare(srcConn.dest.object)==0
			/*&& destConn["srcEffect"].empty() && srcConn["destEffect"].empty()*/;
	/*bool effectMatch = destConn["srcEffect"].asString().compare(srcConn["destEffect"].asString())==0
			&& destConn["src"]["object"].empty() && srcConn["dest"]["object"].empty();*/
	bool portMatch = destConn.src.port.compare(srcConn.dest.port)==0;

	if(debugOutput) cout << /*"effectMatch: " << effectMatch <<*/ "objectMatch: " << objectMatch << "\tportMatch: " << portMatch << endl;
	result = ((objectMatch /*|| effectMatch*/) && portMatch);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::compareConnectionsSrc2Dest: " << result << endl;
#endif

	return result;
}


#define dbg 0
int ComboDataInt::getProcessSequenceIndex(string processName)
{
	int index = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getProcessSequenceIndex" << endl;
	if(debugOutput) cout << "/tprocessName: " << processName <<  endl;
#endif

	for (int i = 0; i < 20; i++)
	{
		if(this->processSequence[i].processName.compare(processName) == 0)
		{
			index = i;
			break;
		}
		if(i == 19) index = -1; // couldn't find processName
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::getProcessSequenceIndex: " << index << endl;
#endif
	return index;
}

#define dbg 0
int ComboDataInt::getControlSequenceIndex(string controlName)
{
	int index = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getControlSequenceIndex" << endl;
	if(debugOutput) cout << "/tcontrolName: " << controlName << endl;
#endif

	for (int i = 0; i < 20; i++)
	{
		if(this->controlSequence[i].name.compare(controlName) == 0)
		{
			index = i;
			break;
		}
		if(i == 19) index = -1; // couldn't find processName
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::getControlSequenceIndex: " << index << endl;
#endif
	return index;

}


//***************************** PUBLIC *************************************


#define dbg 1
int ComboDataInt::getCombo(char *comboName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getCombo" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif
    int status = 0;
    bool boolStatus = true;
    procCount = 0;

    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string procString;
    //int absProcessIndex = 0;
    string paramString;
    //int effectParamIndex = 0;
    int absParamIndex = 0;
    //int paramArrayIndex = 0;

    //this->tempParameterArray.clear();

#if(dbg >= 1)
    if(debugOutput) cout << "reading JSON file into jsonString." << endl;
#endif
    //char jsonString[JSON_BUFFER_LENGTH];
    clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
    clearBuffer(this->fileNameBuffer,20);
	/* open combo file */
    strncpy(this->fileNameBuffer, comboName, 19);
    sprintf(this->fileNamePathBuffer,"/home/Combos/%s.txt", this->fileNameBuffer);
    this->comboFD = open(fileNamePathBuffer,O_RDONLY);
	/* read file into temp string */
    if(this->comboFD >= 0)
    {
    	if(read(this->comboFD, this->jsonBuffer, JSON_BUFFER_LENGTH) >= 0)
    	{
#if(dbg >= 1)
    		if(debugOutput) cout << "parsing jsonString in effectComboJson" << endl;

#endif
    		int result = validateJsonBuffer(this->jsonBuffer);
    		if(result == 0) // file needed to cleaned, so replacing file
    		{
#if(dbg >= 3)
    			if(debugOutput) cout << "file needed cleaning, so replacing with cleaned file" << endl;
    			if(debugOutput) cout << "new file: " << this->jsonBuffer << endl;
#endif
    			close(this->comboFD);

    			/*char command[50];
    			sprintf(command,"rm %s", fileNamePathBuffer);
    			if(debugOutput) cout << "command: " << command << endl;
    			system(command);*/
    			this->comboFD = open(fileNamePathBuffer, O_WRONLY|O_CREAT|O_TRUNC, 0666);

    			if(write(this->comboFD,this->jsonBuffer,strlen(this->jsonBuffer)) == -1)
    			{
    				clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
    				if(debugOutput) cout << "error writing jsonBufferString back to combo file." << endl;
    			}
    		}
    		if(result >= 0)
    		{
    			if(this->effectComboJson.empty() == false)
    			{
    				this->effectComboJson.clear();
    			}

    			boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
#if(dbg >= 1)
    			if(debugOutput) cout << "getting combo index" << endl;
#endif
    			string compCombo = this->effectComboJson["name"].asString();
    			this->comboName = this->effectComboJson["name"].asString();

#if(dbg >= 2)
    			if(debugOutput) cout << this->effectComboJson.toStyledString() << endl;
#endif
    			/*for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
    			{
    				if(comboList.at(i).compare(compCombo)==0)
    				{
    					globalComboIndex = i;
    					break;
    				}
    			}*/
    			absParamIndex = 0;


    			//procCount++; // add one more for wrapper

    			if(boolStatus == false)
    			{
    				status = -1;
    				if(debugOutput) cout << "JSON parse failed." << endl;
    			}
    		}
    		else
    		{
				status = -1;
				if(debugOutput) cout << "JSON parse failed." << endl;
    		}
    	}
    	else
    	{
    		if(debugOutput) cout << "failed to read file: " << fileNamePathBuffer << endl;
    		status = -1;
    	}
    }
    else
    {
    	if(debugOutput) cout << "failed to open file: " << fileNamePathBuffer << endl;
    	status = -1;
    }

	if(this->comboFD >= 0)
	{
		close(this->comboFD);
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::getCombo: " << status << endl;
#endif

    //return this->effectComboJson;
	return status;
}


#define dbg 0
int ComboDataInt::getCombo(string comboJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getCombo" << endl;
	if(debugOutput) cout << "comboJson: " << comboJson << endl;
#endif

    int status = 0;
    bool boolStatus = true;
    procCount = 0;

    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string procString;
    //int absProcessIndex = 0;
    string paramString;
    //int effectParamIndex = 0;
    int absParamIndex = 0;
    //int paramArrayIndex = 0;

    //this->tempParameterArray.clear();

#if(dbg >= 1)
    if(debugOutput) cout << "reading JSON file into jsonString." << endl;
#endif
    //char jsonString[JSON_BUFFER_LENGTH];
    clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
	/* read file into temp string */
    if(comboJson.empty() == false)
    {
    	strncpy(this->jsonBuffer, comboJson.c_str(), JSON_BUFFER_LENGTH);
#if(dbg >= 1)
    		if(debugOutput) cout << "parsing jsonString in effectComboJson" << endl;

#endif
		int result = validateJsonBuffer(this->jsonBuffer);
		if(result == 0) // file needed to cleaned, so replacing file
		{
			// not sure if this could apply or not
		}
		else if(result >= 0)
		{
			if(this->effectComboJson.empty() == false)
			{
				this->effectComboJson.clear();
			}


			boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
	#if(dbg >= 1)
			if(debugOutput) cout << "getting combo index" << endl;
	#endif
			string compCombo = this->effectComboJson["name"].asString();
			this->comboName = this->effectComboJson["name"].asString();
			/*for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
			{
				if(comboList.at(i).compare(compCombo)==0)
				{
					globalComboIndex = i;
					break;
				}
			}*/
			absParamIndex = 0;


			//procCount++; // add one more for wrapper

			if(boolStatus == false)
			{
				status = -1;
				if(debugOutput) cout << "JSON parse failed." << endl;
			}
		}
		else
		{
			status = -1;
			if(debugOutput) cout << "JSON parse failed." << endl;
		}
    }
    else
    {
    	if(debugOutput) cout << "string is empty." << endl;
    	status = -1;
    }

	if(this->comboFD >= 0)
	{
		close(this->comboFD);
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::getCombo: " << status << endl;
#endif

    //return this->effectComboJson;
	return status;
}

#define dbg 0
int ComboDataInt::getPedalUi(void)
{
    int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getPedalUi" << endl;
#endif

    //PedalUI pedalUiStruct;
    char effectAbbr[5];
    char compStr[10];
    //bool newEffect = true;
    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string controlString;
    //int controlIndex = 0;
    int controlCount = 0;
    string paramString;
    std::vector<Json::Value>::size_type paramIndex = 0;
    int effectParamArrayIndex = 0;
    int absParamArrayIndex = 0;

    for(int i = 0; i < 5; i++) effectAbbr[i]=0;
    for(int i = 0; i < 10; i++) compStr[i]=0;

    if(this->effectComboJson.isNull() == false)
    {
    	this->pedalUiJson = Json::Value(); // clear any previous data

    	this->pedalUiJson["title" ] = this->effectComboJson["name"];
#if(dbg>=2)
    	if(debugOutput) cout << "title: " << this->pedalUiJson["title"].asString() << endl;
#endif

       	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamArrayIndex = 0;
			this->pedalUiJson["effects"][effectIndex]["abbr"] = tempEffects[effectIndex]["abbr"];
			this->pedalUiJson["effects"][effectIndex]["name"] = tempEffects[effectIndex]["name"];
#if(dbg>=2)
			if(debugOutput) cout << "\teffect: " << this->pedalUiJson["effects"][effectIndex]["abbr"].asString();
			if(debugOutput) cout << "\t" << this->pedalUiJson["effects"][effectIndex]["name"].asString() << endl;
#endif

        	effectString = tempEffects[effectIndex]["name"].asString();
        	//Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            //procCount = tempProcs.size();

        	Json::Value tempControls = tempEffects[effectIndex]["controlArray"];

        	controlCount = tempControls.size();
#if(dbg>=2)
			if(debugOutput) cout << "\tcontrolCount: " << controlCount << endl;
#endif

        	for(int controlIndex = 0; controlIndex < controlCount; controlIndex++)
        	{
        		//Json::Value tempIndexedControl = this->getJsonValueFromUnorderedJsonValueList(controlIndex,tempControls);
        		Json::Value tempIndexedControl = this->getJsonValueFromUnorderedJsonValueList(controlIndex, "controlArray", tempEffects[effectIndex]);
            	controlString = /*tempControls[controlIndex]*/tempIndexedControl["name"].asString();
            	Json::Value params = /*tempControls[controlIndex]*/tempIndexedControl["conParamArray"];
            	//controlString = tempControls[controlIndex]["name"].asString();
            	//Json::Value params = tempControls[controlIndex]["conParamArray"];
        		for(paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
        			//Json::Value tempIndexedParam = this->getJsonValueFromUnorderedJsonValueList(paramIndex,params);
            		Json::Value tempIndexedParam = this->getJsonValueFromUnorderedJsonValueList(paramIndex, "conParamArray", tempIndexedControl);
        			paramString = /*params[paramIndex]*/tempIndexedParam["name"].asString();

//        			if(params[paramIndex]["alias"].asString().size() > 1
//        					&& params[paramIndex]["alias"].asString().compare("none") != 0)
        			{
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"] =
        						/*params[paramIndex]*/tempIndexedParam["alias"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"] =
        						/*params[paramIndex]*/tempIndexedParam["abbr"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] =
        						/*params[paramIndex]*/tempIndexedParam["value"];
        				if(tempIndexedParam["inheritControlledParamType"] == true)
        				{
#if(dbg>=2)
        					if(debugOutput) cout << tempIndexedParam["alias"] << "inheriting controlled parameter: " << tempIndexedParam["paramType"] << " becoming " << tempIndexedParam["controlledParamType"] << endl;
#endif
        					this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"] = tempIndexedParam["controlledParamType"];
        				}
        				else
        				{
        					this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"] = tempIndexedParam["paramType"];
        				}
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"] = absParamArrayIndex;
#if(dbg>=2)
						if(debugOutput) cout << "\t\tparam: " << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"].asString();
						if(debugOutput) cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"].asString();
						if(debugOutput) cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"].asString();
						if(debugOutput) cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"].asInt();
						if(debugOutput) cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"].asInt() << endl;
#endif
        			}

       			effectParamArrayIndex++;
       			absParamArrayIndex++;
        		}
        	}
        }
#if(dbg>=2)
        if(debugOutput) cout << "effectParamArrayIndex: " << effectParamArrayIndex << endl;
    	if(debugOutput) cout << "param count: " << this->pedalUiJson["effects"][0]["params"].size() << endl;
#endif
    	status = 0;//return this->pedalUiJson;

    }
    else
    {
    	status = -1;//return NULL;
    }

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::getPedalUi: " << status << endl;
#endif
#if(dbg >= 2)
	if(debugOutput) cout << "***** " << this->pedalUiJson.toStyledString() << endl;
#endif
    return status;
}


#define dbg 0
int ComboDataInt::getConnections2(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getConnections2" << endl;
#endif
	int status = 0;

	struct _procInput {
		string procName;
		string procInput;
	};

    this->connectionsJson.clear();
    int loopCount = 0;
    if(this->effectComboJson.isNull() == false)
    {
    	try
    	{
        	this->unsequencedConnectionListStruct.clear();// = new std::vector<Json::Value>;

        	std::vector<Json::Value> procConnectionsJson; // put JSON data here then transform/transfer struct data into procConnectionsStruct
        	std::vector<ProcessConnection> procConnectionsStruct;
        	std::vector<Connector> procInputs;
        	//std::vector<std::vector<int>> connectionStrings;
        	std::vector<ProcessConnection> mergedConnections;
        	ProcessConnection tempProcConnection;
        	mergedConnections.clear();
        	procInputs.clear();
        	procConnectionsJson.clear();
        	procConnectionsStruct.clear();
    		//*************  Read the effect process intra-connections into proc vector  *************
        	Json::Value tempEffects = this->effectComboJson["effectArray"];
            effectCount = tempEffects.size();
#if(dbg >= 2)
            if(debugOutput) cout << "Reading connectionArray data into procConnectionsJson." << endl;
#endif
            for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
            {
            	Json::Value tempProcConns = tempEffects[effectIndex]["connectionArray"];
            	int procConnCount = tempProcConns.size();
            	for(int procConnIndex = 0; procConnIndex < procConnCount; procConnIndex++)
            	{
#if(dbg >= 2)
            		if(debugOutput) cout << tempProcConns[procConnIndex]["src"]["object"].asString() << ":";
            		if(debugOutput) cout << tempProcConns[procConnIndex]["src"]["port"].asString() << ">";
            		if(debugOutput) cout << tempProcConns[procConnIndex]["dest"]["object"].asString() << ":";
            		if(debugOutput) cout << tempProcConns[procConnIndex]["dest"]["port"].asString() << endl;
#endif
            		procConnectionsJson.push_back(tempProcConns[procConnIndex]);
            	}
            }

#if(dbg >= 2)
            if(debugOutput) cout << "Reading effectConnectionArray data into procConnectionsJson." << endl;
#endif
            Json::Value tempEffectConns = this->effectComboJson["effectConnectionArray"];
            int effectConnCount = tempEffectConns.size();
            for(int effectConnIndex = 0; effectConnIndex < effectConnCount; effectConnIndex++)
            {
#if(dbg >= 2)
        		if(debugOutput) cout << tempEffectConns[effectConnIndex]["src"]["object"].asString() << ":";
        		if(debugOutput) cout << tempEffectConns[effectConnIndex]["src"]["port"].asString() << ">";
        		if(debugOutput) cout << tempEffectConns[effectConnIndex]["dest"]["object"].asString() << ":";
        		if(debugOutput) cout << tempEffectConns[effectConnIndex]["dest"]["port"].asString() << endl;
#endif
            	procConnectionsJson.push_back(tempEffectConns[effectConnIndex]);
            }

        	for(std::vector<Json::Value>::size_type connectionIndex = 0; connectionIndex < procConnectionsJson.size(); connectionIndex++)
        	{
        		ProcessConnection procConn = ProcessConnection();
        		procConn.src.object = procConnectionsJson[connectionIndex]["src"]["object"].asString();
        		procConn.src.port = procConnectionsJson[connectionIndex]["src"]["port"].asString();
        		procConn.dest.object = procConnectionsJson[connectionIndex]["dest"]["object"].asString();
        		procConn.dest.port = procConnectionsJson[connectionIndex]["dest"]["port"].asString();
        		procConnectionsStruct.push_back(procConn);
        	}
#if(dbg >= 2)
        	this->printUnsequencedConnectionStructList();
#endif
            //*********** Put straight proc2proc connections in mergedConnections ***************
//            for(int procConnIndex = 0; procConnIndex < procConnections.size(); procConnIndex++)
//            {
//            	if(procConnections[procConnIndex]["src"]["object"].isNull() == false &&
//            			procConnections[procConnIndex]["dest"]["object"].isNull() == false)
//            	{
//            		mergedConnections.push_back(procConnections[procConnIndex])
//            	}
//            }
            // ************ Gather inputs for all processes ********************
            Connector tempProcInput;
            tempProcInput.object = string("system");
            tempProcInput.port = string("playback_1");
            procInputs.push_back(tempProcInput);
            tempProcInput.object = string("system");
            tempProcInput.port = string("playback_2");
            procInputs.push_back(tempProcInput);

        	for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
            {
            	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            	int procCount = tempProcs.size();
            	for(int procIndex = 0; procIndex < procCount; procIndex++)
            	{
            		Json::Value tempInputs = tempProcs[procIndex]["inputArray"];
            		//int tempInputCount = tempInputs.size();

					for(int procInputIndex = 0; procInputIndex < tempInputs.size(); procInputIndex++)
					{
						Json::Value input = this->getJsonValueFromUnorderedJsonValueList(procInputIndex,"inputArray", tempProcs[procIndex]);
						tempProcInput.object = tempProcs[procIndex]["name"].asString();
						tempProcInput.port = input/*tempInputs[procInputIndex]*/["name"].asString();
						procInputs.push_back(tempProcInput);
						//break;
					}
            	}
            }

        	// ASSUMPTION: EACH PROCESS INPUT WILL BE CONNECTED TO ONLY ONE OUTPUT.
//            For each process/system input, create connectionString integer vector,
//             with each integer in the vector being the index for a connection
//              in procConnections.  A connectionString is done when the last integer
//              points to the process/system output to which the original process input
//              was connected.
//

            for(std::vector<_procInput>::size_type procInputIndex = 0; procInputIndex < procInputs.size(); procInputIndex++)
            {
    #if(dbg >=2)
            	if(debugOutput) cout << "procInput[" << procInputIndex << "]: " << procInputs[procInputIndex].object << ":" << procInputs[procInputIndex].port << endl;
            	if(debugOutput) cout << "PROCESS CONNECTIONS:" << endl;
    #endif
            	Json::Value inputConn;
            	// For each input, find the connection that directly feeds into it.

        		for(std::vector</*Json::Value*/ProcessConnection>::size_type connIndex = 0; connIndex < procConnectionsStruct.size(); connIndex++)
        		{
    				if(((procConnectionsStruct[connIndex].dest.object.compare(procInputs[procInputIndex].object) == 0) ) &&
        					(procConnectionsStruct[connIndex].dest.port.compare(procInputs[procInputIndex].port) == 0))
        			{
        				tempProcConnection = procConnectionsStruct[connIndex];
        				bool exit = false;

    #if(dbg >= 2)
        				if(debugOutput) cout << "BASE CONNECTION FOUND....." << endl;
        				if(debugOutput) cout << tempProcConnection.src.object << ":";
        				if(debugOutput) cout << tempProcConnection.src.port << ">";
        				if(debugOutput) cout << tempProcConnection.dest.object << ":";
        				if(debugOutput) cout << tempProcConnection.dest.port << endl;
        				if(debugOutput) cout << "************************************" << endl;
    #endif
        	    		for(int loopNum = 0; loopNum < 5 && exit == false; loopNum++)
        	        	{
        	        		for(std::vector</*Json::Value*/ProcessConnection>::size_type connIndex = 0; connIndex < procConnectionsStruct.size(); connIndex++)
        	        		{
    #if(dbg >= 2)
    	        				if(debugOutput) cout << procConnectionsStruct[connIndex].src.object << ":";
    	        				if(debugOutput) cout << procConnectionsStruct[connIndex].src.port << ">";
    	        				if(debugOutput) cout << procConnectionsStruct[connIndex].dest.object << ":";
    	        				if(debugOutput) cout << procConnectionsStruct[connIndex].dest.port << endl;
    #endif

        	        			if(((procConnectionsStruct[connIndex].dest.object.compare(tempProcConnection.src.object) == 0)) &&
        	        					(procConnectionsStruct[connIndex].dest.port.compare(tempProcConnection.src.port) == 0)
        	        			)
        	        			{
        	        				tempProcConnection = this->mergeConnections(procConnectionsStruct[connIndex],tempProcConnection);
        	        				exit = (tempProcConnection.src.object.find("(") == string::npos) || (tempProcConnection.src.object.compare("system") == 0); // object is process output or "system"
        	        				if(exit)
        	        				{
        	        					mergedConnections.push_back(tempProcConnection);
        	        				}
    #if(dbg >= 2)
        	        				if(debugOutput) cout << "MATCH....." << endl;
        	        				if(debugOutput) cout << tempProcConnection.src.object << ":";
        	        				if(debugOutput) cout << tempProcConnection.src.port << ">";
        	        				if(debugOutput) cout << tempProcConnection.dest.object << ":";
        	        				if(debugOutput) cout << tempProcConnection.dest.port << endl;
    #endif

        	        				break;
        	        			}
        	        			else if((tempProcConnection.src.object.find("(") == string::npos) || (tempProcConnection.src.object.compare("system") == 0))
        	        			// original connection can't be merged further
    	        				{
    	        					mergedConnections.push_back(tempProcConnection);
    	        					exit = true;
    	        					break;
    	        				}
        	        			else if(connIndex == procConnectionsStruct.size()-1) // no matches found, so no connections exist
        	        			{
        	        				exit = true;
        	        				break;
        	        			}
        	        		}
        	        	}
        				break;
        			}
        		}
            }

            for(std::vector</*Json::Value*/ProcessConnection>::size_type mergedConnIndex = 0; mergedConnIndex < mergedConnections.size(); mergedConnIndex++)
            {
    #if(dbg >= 2)
            	if(debugOutput) cout << "mergedConnections[" << mergedConnIndex << "]: ";
    			if(debugOutput) cout << mergedConnections[mergedConnIndex].src.object << ":";
    			if(debugOutput) cout << mergedConnections[mergedConnIndex].src.port << ">";
    			if(debugOutput) cout << mergedConnections[mergedConnIndex].dest.object << ":";
    			if(debugOutput) cout << mergedConnections[mergedConnIndex].dest.port << endl;
    #endif
    			ProcessConnection tempConn;
//    			if(mergedConnections[mergedConnIndex].src.object.compare("system") == 0)
//    			{
//    				tempConn.src.process = mergedConnections[mergedConnIndex]["srcEffect"];
//    			}
//    			else
    			{
    				tempConn.src.object = mergedConnections[mergedConnIndex].src.object;
    			}

    			tempConn.src.port = mergedConnections[mergedConnIndex].src.port;

//    			if(mergedConnections[mergedConnIndex]["destEffect"].compare("system") == 0)
//    			{
//    				tempConn["dest"]["object"] = mergedConnections[mergedConnIndex]["destEffect"];
//    			}
//    			else
    			{
    				tempConn.dest.object = mergedConnections[mergedConnIndex].dest.object;
    			}

    			tempConn.dest.port = mergedConnections[mergedConnIndex].dest.port;

    			this->unsequencedConnectionListStruct.push_back(tempConn);
            }
    	}
    	catch (int e)
    	{

    	}
    }
    else status = -1;//return NULL;


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getConnections2: " << status << endl;
#endif
	return status;
}


#define dbg 2
int ComboDataInt::getProcesses(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getProcesses" << endl;
#endif

	int status = 0;
    int absProcIndex = 0;
    string process;
    vector<string> processes;
    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string procString;
    int absProcessIndex = 0;
    string paramString;
    int effectParamIndex = 0;
    int absParamIndex = 0;
    int paramArrayIndex = 0;
    int breakLoopCount = 0;


    this->processesStruct.clear();
    this->unsortedParameterArray.clear();
    this->sortedParameterArray.clear();
    this->unsequencedProcessListStruct.clear();// = new std::vector<Json::Value>;

    /****************** Create unsortedParameterArray *******************************/
    if(this->effectComboJson.isNull() == false)
    {
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
    #if(dbg >= 2)
        if(debugOutput) cout << "getting effects" << endl;
    #endif
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamIndex = 0;
        	effectString = tempEffects[effectIndex]["name"].asString();
        	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            int tempProcCount = tempProcs.size();
            procCount += tempProcCount;
    #if(dbg >= 2)
            if(debugOutput) cout << "getting processes" << endl;
    #endif

        	for(int procIndex = 0; procIndex < tempProcCount; procIndex++)
        	{
            	procString = tempProcs[procIndex]["name"].asString();
            	Json::Value params = tempProcs[procIndex]["paramArray"];
    #if(dbg >= 2)
                if(debugOutput) cout << "getting parameters" << endl;
    #endif
        		for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
        			paramString = params[paramIndex]["name"].asString();
        			IndexedProcessParameter tempParam;
        			tempParam.effectName = effectString;
        			tempParam.effectIndex = effectIndex;
        			tempParam.processName = procString;
        			tempParam.effectProcessIndex = procIndex;
        			tempParam.absProcessIndex = absProcessIndex;
        			tempParam.paramName = paramString;
        			tempParam.processParamIndex = paramIndex;
        			tempParam.effectParamIndex = effectParamIndex;
        			tempParam.absParamIndex = absParamIndex;
        			try
        			{
        				tempParam.paramValue = params[paramIndex]["value"].asInt();
        			}
        			catch(std::exception &e)
        			{
        				tempParam.paramValue = atoi(params[paramIndex]["value"].asString().c_str());
        			}
#if(dbg>=2)
        			if(debugOutput) cout << "[" << effectIndex << "]:" << effectString << "[" << procIndex << "]:" << procString << "[" << paramIndex << "]:" << paramString << ":" << paramArrayIndex << endl;
#endif
        			effectParamIndex++;
        			absParamIndex++;
        			this->unsortedParameterArray.push_back(tempParam);
        		}
        		absProcessIndex++;
        	}
        }

        //processesStruct is created in transferProcessToSequencedProcessList



        /************** Sort processes ********************************/
        bool sequencingStart = true;
        if(status == 0)
        {
            try
            {
#if(dbg>=2)
    			if(debugOutput) cout << "fillUnsequencedProcessList" << endl;
#endif
    			this->dataReadyList.clear();

    			Connector tempConn;
    			tempConn.object = string("system");
    			tempConn.port = string("capture_1");
    			this->dataReadyList.push_back(tempConn);
    			tempConn.object = string("system");
    			tempConn.port = string("capture_2");
    			this->dataReadyList.push_back(tempConn);
    			if(this->fillUnsequencedProcessList() != 0)
    			{
    				status = -1;
    			}
            }
        	catch(std::exception &e)
        	{
        		 if(debugOutput) cout << "exception in fillUnsequencedProcessList section: " << e.what() <<  endl;
        		 status = -1;
        	}
        }

#if(dbg>=2)
        this->printUnsequencedConnectionStructList();
        this->printUnsequencedProcessList();
#endif

        if(status == 0)
        {
			try
			{
				breakLoopCount = 0;
				while(this->isUnsequencedProcessListEmpty() == false)
				{
					if(sequencingStart)
					{
						/*process = this->getFirstProcess();
						if(process.empty() == true)*/
						processes = this->getFirstProcesses();
						if(processes.empty() == true)
						{
							status = -1;
							if(debugOutput) cout << "first process string vector is blank." << endl;
							break;
						}
						else
						{
							sequencingStart = false;
						}

					}
					else
					{
						/*process = this->getNextProcess();
						if(process.empty() == true)*/
						processes = this->getNextProcesses();
						if(processes.empty() == true)
						{
							status = -1;
							if(debugOutput) cout << "next process string vector is empty." << endl;
							break;
						}
					}
					//if(process.length() > 1)
					{
						//if(this->addOutputConnectionsToDataReadyList(process) != 0)
						if(this->addOutputConnectionsToDataReadyList(processes) != 0)
						{
							status = -1;
							if(debugOutput) cout << "addOutputConnectionsToDataReadyList failed." << endl;
							break;
						}
						//if(this->transferProcessToSequencedProcessList(process) != 0)
						if(this->transferProcessesToSequencedProcessList(processes) != 0)
						{
							status = -1;
							break;
						}
					}
					if(breakLoopCount++ >= 100)
					{
						status = -1;
						if(debugOutput) cout << "Couldn't sort processes." << endl;
						break;
					}
				}
			}
			catch(std::exception &e)
			{
				 if(debugOutput) cout << "exception in isUnsequencedProcessListEmpty while loop: " << e.what() <<  endl;
				 return -1;
			}
		}


        if(status == 0)
        {
            try
            {
            	for(std::vector<Connector>::size_type dataReadyIndex = 0; dataReadyIndex < this->dataReadyList.size(); dataReadyIndex++)
                {
                	Connector dataReadyItem = this->dataReadyList[dataReadyIndex];
                	for(int i = 0; i < 2; i++)
                	{
                		for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListStruct/*Json*/.size(); connIndex++)
                		{
                			/*Json::Value*/ProcessConnection connection = this->unsequencedConnectionListStruct/*Json*/[connIndex];

                			if((dataReadyItem.object.compare(connection.src.object/*.asString()*/) == 0) &&
                					(dataReadyItem.port.compare(connection.src.port/*.asString()*/) == 0))
                			{
                				this->connectionsStruct/*Json*/.push_back(connection);
                				this->unsequencedConnectionListStruct/*Json*/.erase(this->unsequencedConnectionListStruct/*Json*/.begin() + connIndex);
                			}

                		}
                	}
                }
            }
        	catch(std::exception &e)
        	{
        		if(debugOutput) cout << "exception in this->dataReadyList.size for loop: " << e.what() <<  endl;
        		status = -1;
        	}
        }


        /***************************
 	 	 *	Create sorted parameter list.
 	 	 *		sort according to processSequence, and change process indexing
 	 	 *		to reflect new process order
 	 	 *
 	 	 *
         ************************/
        // loop through processStruct
#if(dbg >= 2)
        if(debugOutput) cout << "Create sorted parameter list." << endl;
#endif

        if(status == 0)
        {
            try
            {
    			for(std::vector<Process>::size_type processIndex = 0; processIndex < this->processesStruct.size(); processIndex++)
    			{
    				// for each process, get process name
    				string procName = this->processesStruct[processIndex].name;
    				// loop through unsortedParameterArray
    				for(std::vector<IndexedProcessParameter>::size_type parameterIndex = 0; parameterIndex < this->unsortedParameterArray.size(); parameterIndex++)
    				{
    					// for each parameter, compare process name with parameter.processName
    					if(procName.compare(this->unsortedParameterArray[parameterIndex].processName) == 0)
    					{
    						// if a match is found, push parameter into sortedParameterArray
    						IndexedProcessParameter tempParam;
    						tempParam.effectName = this->unsortedParameterArray[parameterIndex].effectName;
    						tempParam.effectIndex = this->unsortedParameterArray[parameterIndex].effectIndex;
    						tempParam.processName = this->unsortedParameterArray[parameterIndex].processName;
    						tempParam.effectProcessIndex = this->unsortedParameterArray[parameterIndex].effectProcessIndex;
    						tempParam.absProcessIndex = processIndex;
    						tempParam.paramName = this->unsortedParameterArray[parameterIndex].paramName;
    						tempParam.processParamIndex = this->unsortedParameterArray[parameterIndex].processParamIndex;
    						tempParam.effectParamIndex = this->unsortedParameterArray[parameterIndex].effectParamIndex;
    						tempParam.absParamIndex = this->unsortedParameterArray[parameterIndex].absParamIndex;
    						tempParam.paramValue = this->unsortedParameterArray[parameterIndex].paramValue;

    						this->sortedParameterArray.push_back(tempParam);
    					}
    				}
    			}
            }
        	catch(std::exception &e)
        	{
        		if(debugOutput) cout << "exception in this->processesStruct.size for loop: " << e.what() <<  endl;
        		status = -1;
        	}
        }

#if(dbg >= 2)
        if(status == 0)
        {
            try
            {
                if(debugOutput) cout << "SEQUENCED PROCESSES: " << endl;
        		this->printSequencedProcessList();
        		this->printSortedParameters();
                if(debugOutput) cout << "SEQUENCED CONNECTIONS: " << endl;
                this->printSequencedConnectionStructList();
                this->printSequencedConnectionJsonList();
            }
        	catch(std::exception &e)
        	{
        		if(debugOutput) cout << "exception in dbg >= 2  section: " << e.what() <<  endl;
        		status = -1;
        	}
        }
#endif
#if(dbg >= 2)
    if(debugOutput) cout << "getProcesses end: procCount = " << procCount << endl;
#endif
    }
    else status = -1;
	//this->getParameterArray();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getProcesses: " << status << endl;
#endif

    return status;
}

#define dbg 2
int ComboDataInt::getControls(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getControls" << endl;
#endif

    //int absProcIndex = 0;
    string control;
    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string controlString;
    int absControlIndex = 0;
    string paramString;
    int effectParamIndex = 0;
    int absParamIndex = 0;
    int paramArrayIndex = 0;

	this->controlsStruct.clear();
	this->controlParameterArray.clear();

#if(dbg >= 2)
	this->printControlList();
	this->printControlParameterList();
	this->printUnsortedParameters();
#endif
    if(this->effectComboJson.isNull() == false)
    {
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
    #if(dbg >= 2)
        if(debugOutput) cout << "getting effects" << endl;
    #endif
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamIndex = 0;
        	effectString = tempEffects[effectIndex]["name"].asString();

        	Json::Value tempControls = tempEffects[effectIndex]["controlArray"];
            int tempControlCount = tempControls.size();

        	for(int controlIndex = 0; controlIndex < tempControlCount; controlIndex++)
        	{
        		int unorderedControlIndex = tempControls[controlIndex]["index"].asInt();
        		Json::Value tempIndexedControl = this->getJsonValueFromUnorderedJsonValueList(controlIndex, "controlArray", tempEffects[effectIndex]);
        		controlString = tempIndexedControl/*tempControls[controlIndex]*/["name"].asString();
            	Json::Value params = tempIndexedControl/*tempControls[controlIndex]*/["conParamArray"/*"controlParameterArray"*/];
    #if(dbg >= 2)
                if(debugOutput) cout << "getting parameters" << endl;
    #endif
        		for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
            		int unorderedParamIndex = tempIndexedControl["conParamArray"][paramIndex]["index"].asInt();
        			Json::Value tempIndexedParameter = this->getJsonValueFromUnorderedJsonValueList(paramIndex, "conParamArray", tempIndexedControl);
        			paramString = /*params[paramIndex]*/tempIndexedParameter["name"].asString();
        			IndexedControlParameter tempParam;
#if(dbg >= 2)
        			if(debugOutput) cout << "unorderedParamIndex: " << unorderedParamIndex << "\tparamString: " << paramString << endl;
#endif
        			tempParam.effectName = effectString;
        			tempParam.effectIndex = effectIndex;
        			tempParam.controlName = controlString;
        			tempParam.effectControlIndex = unorderedControlIndex;//controlIndex;
        			tempParam.absControlIndex = absControlIndex;
        			tempParam.controlParamName = paramString;
        			tempParam.controlParamIndex = unorderedParamIndex;//paramIndex;
        			tempParam.effectParamIndex = effectParamIndex;
        			//tempParam.absProcessParamIndex = absParamIndex;
        			try
        			{
        				tempParam.controlParamValue = /*params[paramIndex]*/tempIndexedParameter["value"].asInt();
        			}
        			catch(std::exception &e)
        			{
        				tempParam.controlParamValue = atoi(/*params[paramIndex]*/tempIndexedParameter["value"].asString().c_str());
        			}
       #if(dbg >= 2)
        			if(debugOutput) cout << "[" << effectIndex << "]:" << effectString << "[" << controlIndex << "]:" << controlString << "[" << paramIndex << "]:" << paramString << ":" << paramArrayIndex << endl;
        #endif
        			effectParamIndex++;
        			absParamIndex++;
        			this->controlParameterArray.push_back(tempParam);
        		}
        		absControlIndex++;
        	}
        }
#if(dbg >= 2)
    	this->printControlParameterList();
    	this->printUnsortedParameters();
#endif

		for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
		{
			effectParamIndex = 0;
			effectString = tempEffects[effectIndex]["name"].asString();

			Json::Value tempControls = tempEffects[effectIndex]["controlArray"];
			int tempControlCount = tempControls.size();

        	for(int controlIndex = 0; controlIndex < tempControlCount; controlIndex++)
        	{
        		Json::Value tempIndexedControl = this->getJsonValueFromUnorderedJsonValueList(controlIndex, "controlArray", tempEffects[effectIndex]);
        		Control tempControl;

        		int paramArraySize = /*tempControls[controlIndex]*/tempIndexedControl["conParamArray"].size();
        		tempControl.name = /*tempControls[controlIndex]*/tempIndexedControl["name"].asString();
        		tempControl.parentEffect = effectString;
        		tempControl.conType = /*tempControls[controlIndex]*/tempIndexedControl["conType"].asString();
#if(dbg >= 2)
					if(debugOutput) cout << tempIndexedControl.toStyledString() << endl;
#endif

        		for(int paramArrayIndex = 0; paramArrayIndex < paramArraySize; paramArrayIndex++)
        		{
        			Json::Value tempIndexedParam = this->getJsonValueFromUnorderedJsonValueList(paramArrayIndex, "conParamArray", tempIndexedControl);
        			ControlParameter tempParam;
        			// Comment: tempIndexedParam = tempControls[controlIndex]["conParamArray"][paramArrayIndex] ?
           			tempParam.name = tempIndexedParam["name"].asString();
           			tempParam.alias = tempIndexedParam["alias"].asString();
           			tempParam.abbr = tempIndexedParam["abbr"].asString();
           			tempParam.cvEnabled = tempIndexedParam["controlVoltageEnabled"].asBool();
           			tempParam.paramType = tempIndexedParam["paramType"].asInt();
           			tempParam.controlledParamType = tempIndexedParam["controlledParamType"].asInt();
           			tempParam.inheritControlledParamType = tempIndexedParam["inheritControlledParamType"].asBool();

           			if(tempParam.inheritControlledParamType)
           			{
           				tempParam.paramType = tempParam.inheritControlledParamType;
           			}



           			try
        			{
        				tempParam.value = tempIndexedParam["value"].asInt();
					}
					catch(std::exception &e)
					{
							tempParam.value = atoi(tempIndexedParam["value"].asString().c_str());
					}
#if(dbg >= 2)
					if(debugOutput) cout << tempControl.parentEffect << ":" /*<< tempControl.conType*/ << "Control Parameter[" << paramArrayIndex << "]: " << tempControl.conType << endl;
					if(debugOutput) cout << tempIndexedParam.toStyledString() << endl;
#endif
       			tempControl.params.push_back(tempParam);
        		}
        		this->controlsStruct.push_back(tempControl);
        	}
        }


#if(dbg >= 2)
		this->printControlList();
		this->printControlParameterList();
		this->printControlConnectionList();
#endif


		int controlCount = this->controlsStruct.size();
		int connCount = this->controlConnectionsStruct.size();
		// *******************  enter the absolute parameter indexes of parameters controlled by a parameter controller

		//loop through controlsStruct vector
		controlCount = this->controlsStruct.size();
		for(int controlIndex = 0; controlIndex < controlCount; controlIndex++)
		{
			//for each controlsStruct, get the name
			this->controlsStruct[controlIndex].absProcessParameterIndexes.clear();
			this->controlsStruct[controlIndex].absProcessParameterIndexesInv.clear();
			//loop through controlConnectionsStruct vector
			for(int connIndex = 0; connIndex < connCount; connIndex++)
			{
				//for each controlConnectionsStruct, compare the src name to the controlsStruct name
#if(dbg >= 2)
				if(debugOutput) cout << "controlConnectionsStruct: " << this->controlConnectionsStruct[connIndex].src.object << ":" << this->controlConnectionsStruct[connIndex].src.port << endl;
#endif
				//int delimiter = this->controlConnectionsStruct[connIndex].src.find(":");
				//if(delimiter != string::npos)
				{
					string controlName = this->controlConnectionsStruct[connIndex].src.object;//substr(0,delimiter);
					string controlOutput = this->controlConnectionsStruct[connIndex].src.port;//substr(delimiter+1);
#if(dbg >= 2)
					if(debugOutput) cout << "controlsStruct[" << controlIndex << "].name" << this->controlsStruct[controlIndex].name << endl;
					if(debugOutput) cout << "controlName:  " << controlName << endl;
					if(debugOutput) cout << "controlOutput:  " << controlOutput << endl;
#endif
					if(controlName.compare(this->controlsStruct[controlIndex].name) == 0)
					{
					//if a match is found, get the absolute parameter index of the dest process:parameter
						//uint8_t colonPosition = this->controlConnectionsStruct[connIndex].dest.object;//find(":");
						string process = this->controlConnectionsStruct[connIndex].dest.object;
						string parameter = this->controlConnectionsStruct[connIndex].dest.port;

						/*if(0 < colonPosition && colonPosition < this->controlConnectionsStruct[connIndex].dest.length())
						{
							process = this->controlConnectionsStruct[connIndex].dest.substr(0,colonPosition);
							parameter = this->controlConnectionsStruct[connIndex].dest.substr(colonPosition+1);
						}
						else
							if(debugOutput) cout << "invalid syntax in control connection: " << this->controlConnectionsStruct[connIndex].dest << endl;
						*/
						int paramIndex = this->getProcessParameterIndex(process, parameter);
						//if valid, enter the parameter index into the target parameter index array in the controlsStruct
						if(0 <= paramIndex && paramIndex < 100)
						{
							if(controlOutput.compare("output") == 0)
							{
	#if(dbg >= 2)
								if(debugOutput) cout << "pushing paramIndex " << paramIndex << " for " << process << ":" << parameter << " into absProcessParameterIndexes" << endl;
	#endif
								this->controlsStruct[controlIndex].absProcessParameterIndexes.push_back(paramIndex);
							}
							else if(controlOutput.compare("outputInv") == 0)
							{
	#if(dbg >= 2)
								if(debugOutput) cout << "pushing paramIndex " << paramIndex << " for " << process << ":" << parameter << " into absProcessParameterIndexesInv" << endl;
	#endif
								this->controlsStruct[controlIndex].absProcessParameterIndexesInv.push_back(paramIndex);
							}
							else
								if(debugOutput) cout << "invalid control output name." << endl;

						}
						else
							if(debugOutput) cout << "invalid control output name." << endl;
					}
				}
			}
		}

#if(dbg >= 2)
		this->printControlList();
#endif
    }

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getControls: " << status << endl;
#endif

	return status;
}


#define dbg 0
int ComboDataInt::getControlConnections(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::getControlConnections" << endl;
#endif
    int absProcIndex = 0;
    string process;
    string effectString;
    string currentEffectString;
    //int effectIndex = 0;
    string procString;
    int absProcessIndex = 0;
    string paramString;
    int effectParamIndex = 0;
    int absParamIndex = 0;
    int paramArrayIndex = 0;


	this->controlConnectionsStruct.clear();

	Json::Value tempEffects = this->effectComboJson["effectArray"];
    effectCount = tempEffects.size();
#if(dbg >= 1)
    if(debugOutput) cout << "getting effects" << endl;
#endif
    for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
    {
    	effectParamIndex = 0;
    	effectString = tempEffects[effectIndex]["name"].asString();

    	Json::Value tempControlConnections = tempEffects[effectIndex]["controlConnectionArray"];
        int tempControlConnectionCount = tempControlConnections.size();
    	for(int controlConnectionIndex = 0; controlConnectionIndex < tempControlConnectionCount; controlConnectionIndex++)
    	{
    		ControlConnection tempContConn;
    		tempContConn.src.object = tempControlConnections[controlConnectionIndex]["src"]["object"].asString();
    		tempContConn.src.port = tempControlConnections[controlConnectionIndex]["src"]["port"].asString();
    		tempContConn.dest.object = tempControlConnections[controlConnectionIndex]["dest"]["object"].asString();
    		tempContConn.dest.port = tempControlConnections[controlConnectionIndex]["dest"]["port"].asString();
#if(dbg >= 2)
    		if(debugOutput) cout << tempControlConnections[controlConnectionIndex]["src"]["object"].asString() << ":";
    		if(debugOutput) cout << tempControlConnections[controlConnectionIndex]["src"]["port"].asString() << endl;
    		if(debugOutput) cout << tempControlConnections[controlConnectionIndex]["dest"]["object"].asString() << ":";
    		if(debugOutput) cout << tempControlConnections[controlConnectionIndex]["dest"]["port"].asString() << endl;
#endif
    		//tempContConn.dest = tempControlConnections[controlConnectionIndex]["dest"]["name"].asString();
    		this->controlConnectionsStruct.push_back(tempContConn);
    	}
    }

#if(dbg >= 1)
	this->printControlConnectionList();
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::getControlConnections: " << status << endl;
#endif
	return status;
}



#define dbg 0
int ComboDataInt::initProcBufferArray(struct ProcessBuffer *bufferArray, vector</*Json::Value*/ProcessConnection> connectionsStruct)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::initProcBufferArray" << endl;
#endif
	int procBufferIndex = 0;
	int procBufferCount = 0;
	bool bufferAlreadyInList = false;
	vector<Connector> bufferNameArray;
	vector<Connector>::iterator it;
	Connector tempConn;

	tempConn.object = string("system");
	tempConn.port = string("capture_1");
	bufferNameArray.push_back(tempConn);
	tempConn.object = string("system");
	tempConn.port = string("capture_2");
	bufferNameArray.push_back(tempConn);


	// loop through connectionsStruct and create list of process output buffers
	//for(std::vector<Json::Value>::size_type connectionsJsonIndex = 0; connectionsJsonIndex < connectionsJson.size(); connectionsJsonIndex++)
	for(std::vector<ProcessConnection>::size_type connectionsStructIndex = 0; connectionsStructIndex < this->connectionsStruct.size(); connectionsStructIndex++)
	{

		/*string compConnectionProc = connectionsJson[connectionsJsonIndex]["src"]["object"].asString();
		string compConnectionPort = connectionsJson[connectionsJsonIndex]["src"]["port"].asString();*/
		string compConnectionProc = this->connectionsStruct[connectionsStructIndex].src.object;
		string compConnectionPort = this->connectionsStruct[connectionsStructIndex].src.port;

#if(dbg >= 2)
		if(debugOutput) cout << "[" << connectionsJsonIndex << "]:" << compConnectionProc << ":" << compConnectionPort << endl;
		if(debugOutput) cout << endl;
		if(debugOutput) cout << "OUTPUT BUFFERS: " << endl;
		if(debugOutput) cout << "bufferNameArray size: " << bufferNameArray.size() << endl;
		for(std::vector<Connector>::size_type bufferNameArrayIndex = 0; bufferNameArrayIndex < bufferNameArray.size(); bufferNameArrayIndex++)
		{
			if(debugOutput) cout << "procBuffer[" << bufferNameArrayIndex << "]: " << bufferNameArray[bufferNameArrayIndex].object;
			if(debugOutput) cout << ":" << bufferNameArray[bufferNameArrayIndex].port << endl;
		}
		if(debugOutput) cout << endl;
#endif

		// for each connection, loop through current list of process buffers

		tempConn.object = compConnectionProc;
		tempConn.port = compConnectionPort;



		bufferAlreadyInList = false;
		for(std::vector<Connector>::size_type bufferNameArrayIndex = 0; bufferNameArrayIndex < bufferNameArray.size(); bufferNameArrayIndex++)
		{
			//tempConn.object = bufferNameArray[bufferNameArrayIndex].object;
			//tempConn.port = bufferNameArray[bufferNameArrayIndex].port;

			// compare connection output (source) process:port to listed buffer process:port
#if(dbg >=2)
			if(debugOutput) cout << "comparing: " << bufferNameArray[bufferNameArrayIndex].object << ":" << bufferNameArray[bufferNameArrayIndex].port;
			if(debugOutput) cout << " & " << compConnectionProc << ":" << compConnectionPort << endl;
#endif

			if(compConnectionProc.compare(bufferNameArray[bufferNameArrayIndex].object) == 0 &&
					compConnectionPort.compare(bufferNameArray[bufferNameArrayIndex].port) == 0)
			{
#if(dbg >= 2)
				if(debugOutput) cout << "MATCH: " << compConnectionProc << ":" << compConnectionPort << " already in list." << endl;
#endif
				bufferAlreadyInList = true;
				break;  // connection output already listed in buffer list
			}
		}
		if(bufferAlreadyInList == false) // connection output was not in list, so add to list
		{
			bufferNameArray.push_back(tempConn);
#if(dbg >= 2)
			if(debugOutput) cout << "NO MATCH: adding " << tempConn.object << ":" << tempConn.port << " to list." << endl;
#endif
		}
	}

#if(dbg >= 2)
	if(debugOutput) cout << endl;
	if(debugOutput) cout << "OUTPUT BUFFERS: " << endl;
	if(debugOutput) cout << "bufferNameArray size: " << bufferNameArray.size() << endl;
#endif
	this->bufferCount = bufferNameArray.size();
	for(int bufferNameArrayIndex = 0; bufferNameArrayIndex < this->bufferCount; bufferNameArrayIndex++)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "procBuffer[" << bufferNameArrayIndex << "]: " << bufferNameArray[bufferNameArrayIndex].object;
		if(debugOutput) cout << ":" << bufferNameArray[bufferNameArrayIndex].port << endl;
#endif
		bufferArray[bufferNameArrayIndex].processName = bufferNameArray[bufferNameArrayIndex].object;
		bufferArray[bufferNameArrayIndex].portName = bufferNameArray[bufferNameArrayIndex].port;
	}
#if(dbg >= 2)
	if(debugOutput) cout << endl;
#endif
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::initProcBufferArray: " << status << endl;
#endif

	return status;
}

/*int ComboDataInt::resetProcBuffer(struct ProcessBuffer procBufferArray)
{
	int status = 0;

	procBufferArray.processed = 0;
	procBufferArray.ready = 0;

	return status;
}*/


/*int ComboDataInt::setProcBuffer(struct ProcessBuffer procBufferArray, int processed, int ready)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::setProcBuffer" << endl;
#endif
	int status = 0;

	procBufferArray.processed = processed;
	procBufferArray.ready = ready;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::setProcBuffer: " << status << endl;
#endif

	return status;
}*/

/*int ComboDataInt::initProcBuffers(struct ProcessBuffer *procBufferArray)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::initProcBuffers" << endl;
#endif
	int status = 0;

	for(int i = 0; i < 60; i++)
	{
		resetProcBuffer(procBufferArray[i]);
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::initProcBuffers: " << status << endl;
#endif

	return status;
}*/

#define dbg 0
int ComboDataInt::setProcData(struct ProcessEvent *procEvent, Process processStruct)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::setProcData" << endl;
#endif
	int status = 0;
	string processName;
	int procType = 0;
	//int processIndex = 0;
	int footswitchNumber = 0;
	int processInputCount = 0;
	int processOutputCount = 0;


	if(processStruct.procType.compare("delayb") == 0) procType = 0;
	else if(processStruct.procType.compare("filter3bb") == 0) procType = 1;
	else if(processStruct.procType.compare("filter3bb2") == 0) procType = 2;
	else if(processStruct.procType.compare("lohifilterb") == 0) procType = 3;
	else if(processStruct.procType.compare("mixerb") == 0) procType = 4;
	else if(processStruct.procType.compare("volumeb") == 0) procType = 5;
	else if(processStruct.procType.compare("waveshaperb") == 0) procType = 6;
	else if(processStruct.procType.compare("reverbb") == 0) procType = 7;
	else if(processStruct.procType.compare("samplerb") == 0) procType = 8;
	else if(processStruct.procType.compare("oscillatorb") == 0) procType = 9;
	//else if(processStruct.procType.compare("blankb") == 0) procType = ?;

	processName = processStruct.name;
	footswitchNumber = processStruct.footswitchNumber;
	processInputCount = processStruct.inputs.size();
	processOutputCount = processStruct.outputs.size();

#if(dbg >= 2)
	if(debugOutput) std::cout << "ProcessingControl2 process name: " << processName << std::endl;
	//if(debugOutput) std::cout << "procType: " << procType << std::endl;
	if(debugOutput) std::cout << "footswitch number: " << footswitchNumber << std::endl;
	if(debugOutput) std::cout << "process type index: " << atoi(processName.substr(processName.find("_")+1).c_str()) << std::endl;
	if(debugOutput) std::cout << "processInputCount: " << processInputCount << std::endl;
	if(debugOutput) std::cout << "processOutputCount: " << processOutputCount << std::endl;
#endif


	procEvent->processName = processName;
	procEvent->procType = procType;
	procEvent->footswitchNumber = footswitchNumber-1;
	procEvent->processInputCount = processInputCount;

#if(dbg >= 2)
	if(debugOutput) cout << "procEvent->inputBufferNames: ";
#endif
	for(int processInputIndex = 0; processInputIndex < procEvent->processInputCount; processInputIndex++)
	{
		procEvent->inputBufferNames.push_back(processStruct.inputs[processInputIndex].c_str());
#if(dbg >= 2)
		if(debugOutput) cout << procEvent->inputBufferNames[processInputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	if(debugOutput) cout << endl;
	if(debugOutput) cout << "procEvent->outputBufferNames: ";
#endif

	procEvent->processOutputCount = processOutputCount;
	for(int processOutputIndex = 0; processOutputIndex < procEvent->processOutputCount; processOutputIndex++)
	{
		procEvent->outputBufferNames.push_back(processStruct.outputs[processOutputIndex].c_str());
#if(dbg >= 2)
		if(debugOutput) cout << procEvent->outputBufferNames[processOutputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	if(debugOutput) cout << endl;
	if(debugOutput) cout <<  endl;

#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::setProcData: " << status << endl;
#endif

	return status;
}

#define dbg 0
int ComboDataInt::setProcParameters(struct ProcessEvent *procEvent, Process processStruct)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::setProcParameters" << endl;
#endif

	int parameterCount = processStruct.params.size();
	procEvent->parameterCount = parameterCount;

	for(int paramArrayIndex = 0; paramArrayIndex < procEvent->parameterCount; paramArrayIndex++)
	{
		procEvent->parameters[paramArrayIndex] = processStruct.params[paramArrayIndex].value;

#if(dbg >= 1)
		if(debugOutput) std::cout << "parameter[" << paramArrayIndex << "]: " << procEvent->parameters[paramArrayIndex] << std::endl;
#endif
	}

	for(int paramArrayIndex = procEvent->parameterCount; paramArrayIndex < 10; paramArrayIndex++)
	{
		procEvent->parameters[paramArrayIndex] = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::setProcParameters: " << status << endl;
#endif

	return status;
}


int ComboDataInt::initProcInputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::initProcInputBufferIndexes" << endl;
#endif

	for(int procInputIndex = 0; procInputIndex < procEvent->processInputCount; procInputIndex++)
	{
		procEvent->inputBufferIndexes[procInputIndex] = 58;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::initProcInputBufferIndexes: " << status << endl;
#endif
	return status;
}

int ComboDataInt::initProcOutputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::initProcOutputBufferIndexes" << endl;
#endif

	for(int procOutputIndex = 0; procOutputIndex < procEvent->processOutputCount; procOutputIndex++)
	{
		procEvent->outputBufferIndexes[procOutputIndex] = 59;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::initProcOutputBufferIndexes: " << status << endl;
#endif

	return status;
}

#define dbg 0
int ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray" << endl;
	if(debugOutput) cout << "bufferCount: " << this->bufferCount << endl;
#endif
	// loop through connectionsJson
	//for(int bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
	//for(unsigned int connIndex = 0; connIndex < this->connectionsJson.size(); connIndex++)
	for(unsigned int connIndex = 0; connIndex < this->connectionsStruct.size(); connIndex++)
	{
		// 		for each procBufferArray element, get the buffer index and output (source) process:port

		Connector conn, targetConn;

		/*conn.object = this->connectionsJson[connIndex]["src"]["object"].asString();
		conn.port = this->connectionsJson[connIndex]["src"]["port"].asString();
		targetConn.object = this->connectionsJson[connIndex]["dest"]["object"].asString();
		targetConn.port = this->connectionsJson[connIndex]["dest"]["port"].asString();*/
		conn.object = this->connectionsStruct[connIndex].src.object;
		conn.port = this->connectionsStruct[connIndex].src.port;
		targetConn.object = this->connectionsStruct[connIndex].dest.object;
		targetConn.port = this->connectionsStruct[connIndex].dest.port;


#if(dbg >= 2)
		if(debugOutput) cout << "************************************************************" << endl;
		if(debugOutput) cout << "conn: " << conn.object << ":" << conn.port << endl;
		if(debugOutput) cout << "targetConn: " << targetConn.object << ":" << targetConn.port << endl;
#endif
		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
		{
			if(this->procBufferArray[bufferIndex].processName.compare(conn.object) == 0 &&
					this->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{
#if(dbg >= 2)
				if(debugOutput) cout << "found buffer index for " << conn.object << ": " << bufferIndex << endl;
#endif
				break; // found input process:port index
			}
		}

		//*********** connect relevant system input to output buffer  ********************************

		//this->setInputProcBufferIndex(0, 0);
		//this->setInputProcBufferIndex(1,1);
		/*if(conn.object.compare("system") == 0 && conn.port.compare("capture_1") == 0)
		{
			this->inputProcBufferIndex[0] = bufferIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "Connected to input1: procBuffer[" << bufferIndex << "]: ";
			if(debugOutput) cout << conn.object << ":" << conn.port << ">";
			if(debugOutput) cout <<  targetConn.object << ":" << targetConn.port << endl;
#endif
		}
		else if(conn.object.compare("system") == 0 && conn.port.compare("capture_2") == 0)
		{
			this->inputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "Connected to input2: procBuffer[" << bufferIndex << "]: ";
			if(debugOutput) cout << conn.object << ":" << conn.port << ">";
			if(debugOutput) cout <<  targetConn.object << ":" << targetConn.port << endl;
#endif
		}

		else*/
		{
			// 		loop through processSequence
			for(int procIndex = 0; procIndex < this->processCount; procIndex++)
			{
				// 		for each processSequence element, loop through the outputs
				for(int outputIndex = 0; outputIndex < this->processSequence[procIndex].processOutputCount; outputIndex++)
				{
					//			for each output, get the process:port
					Connector procSeqOutputConn;
					procSeqOutputConn.object = this->processSequence[procIndex].processName;
					procSeqOutputConn.port = this->processSequence[procIndex].outputBufferNames[outputIndex];
					// 				compare the procesSequence output process:port to the connectionsJson process:port
					// 				if there is a match, set the processSequence output element index to the output process buffer index

					if(procSeqOutputConn.object.compare(conn.object) == 0 &&
							procSeqOutputConn.port.compare(conn.port) == 0)

						this->processSequence[procIndex].outputBufferIndexes[outputIndex] = bufferIndex;
#if(dbg >= 2)
						if(debugOutput) cout << "Connected output : procBuffer[" << bufferIndex << "]: ";
						if(debugOutput) cout <<  procSeqOutputConn.object << ":" << procSeqOutputConn.port << ">";
						if(debugOutput) cout << targetConn.object << ":" << targetConn.port  << endl;
#endif

				}
			}
		}
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray: " << status << endl;
#endif
	return status;
}

#define dbg 2
int ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsStruct/*Json*/()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsJson" << endl;
#endif

	// initialize outputProcBufferIndexes
	//this->setOutputProcBufferIndex(0,59);//outputProcBufferIndex[0] = 59;
	//this->setOutputProcBufferIndex(1,59);//this->outputProcBufferIndex[1] = 59;

	// loop through connectionsJson
	//for(unsigned int connIndex = 0; connIndex < this->connectionsJson.size(); connIndex++)
	for(unsigned int connIndex = 0; connIndex < this->connectionsStruct.size(); connIndex++)
	{
		// 		for each connectionsJson element, get the buffer index, output (source) process:port and the
		// 		target (dest) process:port
		Connector conn, targetConn;
		/*conn.object = this->connectionsJson[connIndex]["src"]["object"].asString();
		conn.port = this->connectionsJson[connIndex]["src"]["port"].asString();
		targetConn.object = this->connectionsJson[connIndex]["dest"]["object"].asString();
		targetConn.port = this->connectionsJson[connIndex]["dest"]["port"].asString();*/
		conn.object = this->connectionsStruct[connIndex].src.object;
		conn.port = this->connectionsStruct[connIndex].src.port;
		targetConn.object = this->connectionsStruct[connIndex].dest.object;
		targetConn.port = this->connectionsStruct[connIndex].dest.port;

		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
		{
			if(this->procBufferArray[bufferIndex].processName.compare(conn.object) == 0 &&
					this->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{
				break; // found output process:port index
			}
		}

		//*********** connect relevant output buffer to system output ********************************
		if(targetConn.object.compare("system") == 0 && targetConn.port.compare("playback_1") == 0)
		{
			this->setOutputProcBufferIndex(0, bufferIndex);
#if(dbg >= 2)
			if(debugOutput) cout << "Connected output1: procBuffer[" << bufferIndex << "]: ";
			if(debugOutput) cout << conn.object << ":" << conn.port << ">";
			if(debugOutput) cout <<  targetConn.object << ":" << targetConn.port << endl;
#endif
		}
		else if(targetConn.object.compare("system") == 0 && targetConn.port.compare("playback_2") == 0)
		{
			this->setOutputProcBufferIndex(1,bufferIndex);//this->outputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "Connected output2: procBuffer[" << bufferIndex << "]: ";
			if(debugOutput) cout << conn.object << ":" << conn.port << ">";
			if(debugOutput) cout <<  targetConn.object << ":" << targetConn.port << endl;
#endif
		}
		else
		{
			// 		loop through processSequence
			for(int procIndex = 0; procIndex < this->processCount; procIndex++)
			{
				// 		for each processSequence element, loop through the inputs
				for(int inputIndex = 0; inputIndex < this->processSequence[procIndex].processInputCount; inputIndex++)
				{
					//			for each input, get the process:port
					Connector procSeqInputConn;
					procSeqInputConn.object = this->processSequence[procIndex].processName;
					procSeqInputConn.port = this->processSequence[procIndex].inputBufferNames[inputIndex];
					// 				compare the procesSequence input process:port to the connectionsJson target process:port
					// 				if there is a match, set the processSequence input element index to the output process buffer index
					if(procSeqInputConn.object.compare(targetConn.object) == 0 &&
							procSeqInputConn.port.compare(targetConn.port) == 0)
					{
						this->processSequence[procIndex].inputBufferIndexes[inputIndex] = bufferIndex;
#if(dbg >= 2)
						if(debugOutput) cout << "Connected input : procBuffer[" << bufferIndex << "]: ";
						if(debugOutput) cout << conn.object << ":" << conn.port << ">";
						if(debugOutput) cout <<  procSeqInputConn.object << ":" << procSeqInputConn.port << endl;
#endif
					}
				}
			}
		}
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsJson: " << status << endl;
#endif
	return status;
}


#define dbg 0
int ComboDataInt::initializeProcessDataIntoProcessEventElement()
{
	unsigned int delaybCount = 0;
	unsigned int filter3bbCount = 0;
	unsigned int filter3bb2Count = 0;
	unsigned int lohifilterbCount = 0;
	unsigned int mixerbCount = 0;
	unsigned int volumebCount = 0;
	unsigned int waveshaperbCount = 0;
	unsigned int reverbbCount = 0;
	unsigned int samplerbCount = 0;
	unsigned int oscillatorbCount = 0;

	for(int processIndex = 0; processIndex < this->processCount; processIndex++)
	{
		this->processSequence[processIndex].processTypeIndex = 0;
	}

	for(int processIndex = 0; processIndex < this->processCount; processIndex++)
	{
		if(this->processesStruct[processIndex].procType.compare("delayb") == 0)
		{
			//this->processSequence[processIndex].procType = 0;
			this->processSequence[processIndex].processTypeIndex = delaybCount;
			delaybCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("filter3bb") == 0)
		{
			//this->processSequence[processIndex].procType = 1;
			this->processSequence[processIndex].processTypeIndex = filter3bbCount;
			filter3bbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("filter3bb2") == 0)
		{
			//this->processSequence[processIndex].procType = 2;
			this->processSequence[processIndex].processTypeIndex = filter3bb2Count;
			filter3bb2Count++;
		}
		else if(this->processesStruct[processIndex].procType.compare("lohifilterb") == 0)
		{
			//this->processSequence[processIndex].procType = 3;
			this->processSequence[processIndex].processTypeIndex = lohifilterbCount;
			lohifilterbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("mixerb") == 0)
		{
			//this->processSequence[processIndex].procType = 4;
			this->processSequence[processIndex].processTypeIndex = mixerbCount;
			mixerbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("volumeb") == 0)
		{
			//this->processSequence[processIndex].procType = 5;
			this->processSequence[processIndex].processTypeIndex = volumebCount;
			volumebCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("waveshaperb") == 0)
		{
			//this->processSequence[processIndex].procType = 6;
			this->processSequence[processIndex].processTypeIndex = waveshaperbCount;
			waveshaperbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("reverbb") == 0)
		{
			//this->processSequence[processIndex].procType = 7;
			this->processSequence[processIndex].processTypeIndex = reverbbCount;
			reverbbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("samplerb") == 0)
		{
			//this->processSequence[processIndex].procType = 8;
			this->processSequence[processIndex].processTypeIndex = samplerbCount;
			samplerbCount++;
		}
		else if(this->processesStruct[processIndex].procType.compare("oscillatorb") == 0)
		{
			//this->processSequence[processIndex].procType = 9;
			this->processSequence[processIndex].processTypeIndex = oscillatorbCount;
			oscillatorbCount++;
		}
	}

	//*********** initialize process data in ProcessEvent element **************************************
	for(int processIndex = 0; processIndex < this->processCount; processIndex++)
	{
		this->setProcData(&this->processSequence[processIndex], this->processesStruct[processIndex]);

		this->setProcParameters(&this->processSequence[processIndex], this->processesStruct[processIndex]);


		// Initialize input/output buffer indexes with addresses for dummy buffers

		//this->initProcInputBufferIndexes(&this->processSequence[processIndex]);
		//this->initProcOutputBufferIndexes(&this->processSequence[processIndex]);
	}

	//*********** initialize data in ProcessBuffer element **************************************

	this->initProcBufferArray(this->procBufferArray, this->connectionsStruct/*connectionsJson*/);
#if(dbg >= 2)
	if(debugOutput) cout << "OUTPUT BUFFERS: " << endl;
	for(int i = 0; i < this->bufferCount; i++)
	{
		if(debugOutput) cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
	}
	procBufferCount = connectionsStruct/*Json*/.size();
	// set data in procBufferArray elements, using connectionsJson data

	if(debugOutput) cout << endl;
#endif


}




#define dbg 0
int ComboDataInt::initializeControlDataIntoControlEventElement()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::initializeControlDataIntoControlEventElement" << endl;
#endif

	unsigned int normCount = 0;
	unsigned int envCount = 0;
	unsigned int lfoCount = 0;

	//loop through controlsStruct vector
	this->controlCount = this->controlsStruct.size();
#if(dbg >= 2)
	if(debugOutput) cout << "CONTROL EVENT:" << endl;
#endif
	for(int controlIndex = 0; controlIndex < this->controlCount; controlIndex++)
	{
		this->controlSequence[controlIndex].paramContConnectionCount = 0;
		this->controlSequence[controlIndex].paramContConnectionCountInv = 0;

		//for each controlsStruct, enter the control data into the ControlEvent element
		this->controlSequence[controlIndex].name = this->controlsStruct[controlIndex].name;
		if(this->controlsStruct[controlIndex].conType.compare("norm") == 0)
		{
			this->controlSequence[controlIndex].controlTypeIndex = normCount;
			this->controlSequence[controlIndex].conType = 0;
			normCount++;
		}
		else if(this->controlsStruct[controlIndex].conType.compare("env") == 0)
		{
			this->controlSequence[controlIndex].controlTypeIndex = envCount;
			this->controlSequence[controlIndex].conType = 1;
			envCount++;
		}
		else if(this->controlsStruct[controlIndex].conType.compare("lfo") == 0)
		{
			this->controlSequence[controlIndex].controlTypeIndex = lfoCount;
			this->controlSequence[controlIndex].conType = 2;
			lfoCount++;
		}

#if(dbg >= 2)
		if(debugOutput) cout << "name: " << this->controlSequence[controlIndex].name << endl;
		if(debugOutput) cout << "conType: " << this->controlSequence[controlIndex].conType << endl;
#endif
		// enter the parameter values
		for(unsigned int paramIndex = 0; paramIndex < this->controlsStruct[controlIndex].params.size(); paramIndex++)
		{
			this->controlSequence[controlIndex].parameter[paramIndex].value = this->controlsStruct[controlIndex].params[paramIndex].value;
			this->controlSequence[controlIndex].parameter[paramIndex].cvEnabled = this->controlsStruct[controlIndex].params[paramIndex].cvEnabled;

			#if(dbg >= 2)
			if(debugOutput) cout << "\tcontrolSequence[" << controlIndex << "].parameter[" << paramIndex << "].value: " << this->controlSequence[controlIndex].parameter[paramIndex].value << endl;
#endif
		}

		// control output: enter the process and parameter index, using controlsStruct absProcessParameterIndexes as indexing for sortedParameterArray
#if(dbg >= 2)
#endif
		for(unsigned int paramIndex = 0; paramIndex < this->controlsStruct[controlIndex].absProcessParameterIndexes.size(); paramIndex++)
		{
			// get absolute process parameter index
			int absIndex = this->controlsStruct[controlIndex].absProcessParameterIndexes[paramIndex];
#if(dbg >= 2)
			if(debugOutput) cout << "absIndex: " << absIndex << endl;
#endif
			// use absolute parameter index to enter absolute process index from sortedParameterArray into controlSequence connected process
			this->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex =
					this->sortedParameterArray[absIndex].absProcessIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processIndex: "
					<< this->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex << endl;
#endif
			// use absolute parameter index to enter process parameter index from sortedParameterArray into controlSequence connected process parameter
			this->controlSequence[controlIndex].paramContConnection[paramIndex].processParamIndex =
					this->sortedParameterArray[absIndex].processParamIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processParamIndex: "
					<< this->controlSequence[controlIndex].paramContConnection[paramIndex].processParamIndex << endl;
#endif
			this->controlSequence[controlIndex].paramContConnectionCount++;
		}
		// control outputInv: enter the process and parameter index, using controlsStruct absProcessParameterIndexes as indexing for sortedParameterArray
#if(dbg >= 2)
#endif
		for(unsigned int paramIndex = 0; paramIndex < this->controlsStruct[controlIndex].absProcessParameterIndexesInv.size(); paramIndex++)
		{
			// get absolute process parameter index
			int absIndex = this->controlsStruct[controlIndex].absProcessParameterIndexesInv[paramIndex];
#if(dbg >= 2)
			if(debugOutput) cout << "absIndex: " << absIndex << endl;
#endif
			// use absolute parameter index to enter absolute process index from sortedParameterArray into controlSequence connected process
			this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processIndex =
					this->sortedParameterArray[absIndex].absProcessIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "\tcontrolSequence[" << controlIndex << "].paramContConnectionInv[" << paramIndex << "].processIndex: "
					<< this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processIndex << endl;
#endif
			// use absolute parameter index to enter process parameter index from sortedParameterArray into controlSequence connected process parameter
			this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processParamIndex =
					this->sortedParameterArray[absIndex].processParamIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "\tcontrolSequence[" << controlIndex << "].paramContConnectionInv[" << paramIndex << "].processParamIndex: "
					<< this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processParamIndex << endl;
#endif
			this->controlSequence[controlIndex].paramContConnectionCountInv++;
		}
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::initializeControlDataIntoControlEventElement: " << status << endl;
#endif
	return status;
}







#define dbg 0
int ComboDataInt::loadComboStructFromName(char *comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::loadComboStructFromName" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "Getting combo data from file..." << endl;
#endif
	char pedalUiStr[10];
#if(dbg >= 1)
	if(debugOutput) cout << "combo title: " << comboName << endl;
#endif
	//effectCount = this->pedalUiJson["effects"].size();
#if(dbg >= 1)
	if(debugOutput) cout << "number of effects:" << effectCount << endl;
#endif


	/*unsigned int delaybCount = 0;
	unsigned int filter3bbCount = 0;
	unsigned int filter3bb2Count = 0;
	unsigned int lohifilterbCount = 0;
	unsigned int mixerbCount = 0;
	unsigned int volumebCount = 0;
	unsigned int waveshaperbCount = 0;
	unsigned int reverbbCount = 0;
	unsigned int samplerbCount = 0;
	unsigned int oscillatorbCount = 0;*/

	if(this->getCombo(comboName) >= 0)
	{
		if(this->getConnections2() >= 0)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "number of connections: " << this->connectionsStruct/*Json*/.size() << endl;
#endif
			if(this->getProcesses() >= 0)
			{
#if(dbg >= 2)
				if(debugOutput) cout << "number of processes: " << this->processesStruct.size() << endl;
#endif
				if(this->getControlConnections() >= 0)
				{
#if(dbg >= 2)
					if(debugOutput) cout << "number of controlConnections: " << this->controlConnectionsStruct.size() << endl;
#endif
					if(this->getControls() >= 0)
					{
#if(dbg >= 2)
						if(debugOutput) cout << "number of controls:" << this->controlsStruct.size() << endl;
#endif
						if(this->getPedalUi() >= 0)
						{
							if(debugOutput) cout << "loading combo struct: " << comboName << endl;
							//int processIndex = 0;
							int processInputCount = 0;
							int processOutputCount = 0;
							//int processSequenceIndex = 0;
							//int procBufferIndex = 0;
							int procBufferCount = 0;

							//int procType = 0;

							string processName;
							string connectionName[20];

							//initProcBuffers(this->procBufferArray);

							// setup dummy buffer for unconnected inputs
							//setProcBuffer(this->procBufferArray[58],0,1);

							//int parameters[10] = {0,0,0,0,0,0,0,0,0,0};
							//int processInputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
							//int processOutputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

							this->processCount = this->processesStruct.size();
						#if(dbg >= 2)
							if(debugOutput) cout << "this->processCount: " << this->processCount << endl;
						#endif
							this->controlCount = this->controlsStruct.size();


						#if(dbg >= 2)
							if(debugOutput) cout << endl;
						#endif
							this->initializeProcessDataIntoProcessEventElement();
							this->connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray();
							this->connectProcessInputsToProcessOutputBuffersUsingConnectionsStruct/*Json*/();
							this->initializeControlDataIntoControlEventElement();
#if(dbg >= 2)
							this->printSequencedProcessList();
							this->printBufferList();
							this->printSequencedControlList();
#endif
						}
						else
						{
							status = -1;
#if(dbg >= 1)
						if(debugOutput) cout << "getPedalUi failed." << endl;
#endif
						}
					}
					else
					{
						status = -1;
#if(dbg >= 1)
						if(debugOutput) cout << "getControls failed." << endl;
#endif
					}
				}
				else
				{
					status = -1;
#if(dbg >= 1)
					if(debugOutput) cout << "getControlConnections failed." << endl;
#endif
				}
			}
			else
			{
				status = -1;
#if(dbg >= 1)
				if(debugOutput) cout << "getProcesses failed." << endl;
#endif
			}
		}
		else
		{
			status = -1;
#if(dbg >= 1)
			if(debugOutput) cout << "getConnections failed." << endl;
#endif
		}
	}
	else
	{
		status = -1;
#if(dbg >= 1)
			if(debugOutput) cout << "file not found." << endl;
#endif
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::loadComboStructFromName: " << status << endl;
#endif

	return status;

}



#define dbg 0
int ComboDataInt::saveCombo(void/*Json::Value combo*/)
{
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::saveCombo" << endl;
#endif
	int status = 0;

    unsigned int writeSize = 0;
    //bool boolStatus = true;
    //bool done = false;

    int effectIndex = 0;
    int effectControlIndex = 0;
    int controlParamIndex = 0;
    int effectProcessIndex = 0;
    int processParamIndex = 0;
    //int paramArrayIndex = 0;

#if(dbg >= 1)
    this->listParameters();
#endif
    /***** Transfer parameter values from unsortedParameterArray to effectComboJson *******/

     for(std::vector<IndexedProcessParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
     {
     	effectIndex = this->unsortedParameterArray[parameterArrayIndex].effectIndex;
     	effectProcessIndex = this->unsortedParameterArray[parameterArrayIndex].effectProcessIndex;
     	processParamIndex = this->unsortedParameterArray[parameterArrayIndex].processParamIndex;
     	this->effectComboJson["effectArray"][effectIndex]["processArray"][effectProcessIndex]\
 			["paramArray"][processParamIndex]["value"] = this->unsortedParameterArray[parameterArrayIndex].paramValue;
     }

     /***** Transfer parameter values from controlParameterArray to effectComboJson *******/

      for(std::vector<IndexedControlParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->controlParameterArray.size(); parameterArrayIndex++)
      {
      	effectIndex = this->controlParameterArray[parameterArrayIndex].effectIndex;
      	effectControlIndex = this->controlParameterArray[parameterArrayIndex].effectControlIndex;
      	controlParamIndex = this->controlParameterArray[parameterArrayIndex].controlParamIndex;
      	this->effectComboJson["effectArray"][effectIndex]["controlArray"][effectControlIndex]\
  			["conParamArray"/*"controlParameterArray"*/][controlParamIndex]["value"] = this->controlParameterArray[parameterArrayIndex].controlParamValue;
      	if(debugOutput) cout << "effectIndex: " << effectIndex << "\teffectControlIndex: " << effectControlIndex;
      	if(debugOutput) cout << "\tcontrolParamIndex: " << controlParamIndex << "<- ";
      	this->printControlParameter(parameterArrayIndex);
      	if(debugOutput) cout << endl;
      }

#if(dbg >= 2)
     this->printControlParameterList();
#endif

#if(dbg >= 2)
      //if(debugOutput) cout << this->effectComboJson.toStyledString() << endl;
#endif

    /***** Store effectComboJson ********************************************/
    clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
	/* open combo file */
    sprintf(this->fileNamePathBuffer,"/home/Combos/%s.txt", fileNameBuffer);
	this->comboFD = open(this->fileNamePathBuffer,O_WRONLY|O_CREAT|O_TRUNC, 0666);
	/* read file into temp string */
	string tempJsonString = this->comboWriter.write(this->effectComboJson);
#if(dbg>=1)
	//if(debugOutput) cout << "tempJsonString: " << tempJsonString << endl;
#endif
	strncpy(this->jsonBuffer, tempJsonString.c_str(), tempJsonString.length());
	writeSize = write(this->comboFD, this->jsonBuffer, strlen(this->jsonBuffer));
#if(dbg>=1)
	/*cerr << "tempJsonString.length(): " << tempJsonString.length() << endl;
	cerr << "strlen(this->jsonBuffer): " << strlen(this->jsonBuffer) << endl;
	cerr << "writeSize: " << writeSize << endl;*/
	if(debugOutput) cout << this->comboWriter.write(this->effectComboJson);
#endif


	if(writeSize < strlen(this->jsonBuffer))
	{
		status = 1;
		printf("JSON write failed.\n");
	}

    close(this->comboFD);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::saveCombo" << endl;
#endif

	return status;
}




#define dbg 0
void ComboDataInt::listParameters(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::listParameters" << endl;
#endif
	if(debugOutput) cout << "UNSORTED PARAMETERS" << endl;


    for(unsigned int parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
    {
    	if(debugOutput) cout << "processName: " << this->unsortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tabsProcessIndex: " << this->unsortedParameterArray[parameterArrayIndex].absProcessIndex
    			<< "\t\tprocessParamIndex: " << this->unsortedParameterArray[parameterArrayIndex].processParamIndex
    			<< "\t\tabsParamIndex: " << this->unsortedParameterArray[parameterArrayIndex].absParamIndex
    			<< "\t\tparamName: " << this->unsortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->unsortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboDataInt::listParameters" << endl;
#endif

}



#define dbg 0
int ComboDataInt::getProcessParameterIndex(string processName, string parameterName)
{
	int paramIndex = -1;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ComboDataInt::getProcessParameterIndex" << endl;
	if(debugOutput) cout << "processName: " << processName << "\tparameterName: " << parameterName << endl;
#endif

	/*for(int parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
	{
		string compProcessString = this->unsortedParameterArray.at(parameterArrayIndex).processName;
		string compParamString = this->unsortedParameterArray.at(parameterArrayIndex).paramName;
		if(processName.compare(compProcessString) == 0 && parameterName.compare(compParamString) == 0)
		{
			paramIndex = parameterArrayIndex;
		}
	}*/
	for(unsigned int parameterArrayIndex = 0; parameterArrayIndex < this->sortedParameterArray.size(); parameterArrayIndex++)
	{
		string compProcessString = this->sortedParameterArray.at(parameterArrayIndex).processName;
		string compParamString = this->sortedParameterArray.at(parameterArrayIndex).paramName;
#if(dbg >= 2)
		{
			if(debugOutput) cout << "Comparing with: " << compProcessString << ":" << compParamString << endl;
		}
#endif
		if(processName.compare(compProcessString) == 0 && parameterName.compare(compParamString) == 0)
		{
			paramIndex = parameterArrayIndex;
			break;
		}
	}

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: ComboDataInt::getProcessParameterIndex: " << paramIndex << endl;
#endif
	return paramIndex;
}

#define dbg 0
int ComboDataInt::getControlParameterIndex(string controlName, string parameterName)
{
	int paramIndex = -1;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ComboDataInt::getControlParameterIndex" << endl;
	if(debugOutput) cout << "controlName: " << controlName << "\tparameterName: " << parameterName << endl;
#endif
	for(unsigned int parameterArrayIndex = 0; parameterArrayIndex < this->controlParameterArray.size(); parameterArrayIndex++)
	{
		string compControlString = this->controlParameterArray.at(parameterArrayIndex).controlName;
		string compParamString = this->controlParameterArray.at(parameterArrayIndex).controlParamName;
		if(controlName.compare(compControlString) == 0 && parameterName.compare(compParamString) == 0)
		{
			paramIndex = parameterArrayIndex;
		}
	}
#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: ComboDataInt::getControlParameterIndex: " << paramIndex << endl;
#endif

	return paramIndex;
}


#define dbg 0
int ComboDataInt::setControlSequenceParameter(string controlName, int parameterIndex, int valueIndex)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "ENTERING: ComboDataInt::setControlSequenceParameter" << endl;
	if(debugOutput) cout << "\tcontrolName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tvalueIndex: " << valueIndex << endl;
#endif

	int conSeqIndex = this->getControlSequenceIndex(controlName);
	if(conSeqIndex >= 0)
	{
		this->controlSequence[conSeqIndex].parameter[parameterIndex].value = valueIndex;
		status = 0;
	}
	else
	{
		if(debugOutput) cout << "invalid controlName." << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "EXITING: ComboDataInt::setControlSequenceParameter: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::updateProcess(int absParamIndex, int valueIndex)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::updateProcess" << endl;
	if(debugOutput) cout << "absParamIndex: " << absParamIndex << "valueIndex: " << valueIndex << endl;
#endif
	int status = 0;
#if(dbg >= 1)
	this->printProcessParameter(absParamIndex);
#endif
	this->sortedParameterArray[absParamIndex].paramValue = valueIndex;


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::updateProcess: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::updateControl(int absParamIndex, int valueIndex)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: ComboDataInt::updateControl" << endl;
	if(debugOutput) cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
#endif
	int status = 0;
	int effectIndex = 0;
	int effectParamArrayIndex = 0;
#if(dbg >= 2)
	this->printControlParameter(absParamIndex);
	if(debugOutput) cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
#endif
	this->controlParameterArray[absParamIndex].controlParamValue = valueIndex;

	//***************** Update controlSequence array element ******************
	if(this->setControlSequenceParameter(this->controlParameterArray[absParamIndex].controlName,
			this->controlParameterArray[absParamIndex].controlParamIndex,valueIndex) >= 0)
	{
		status = 0;
	}
	else
	{
		if(debugOutput) cout << "invalid conSeqIndex value." << endl;
		status = -1;
	}

	//*************** Update pedal UI data ***********************
	effectIndex = this->controlParameterArray[absParamIndex].effectIndex;
	effectParamArrayIndex = this->controlParameterArray[absParamIndex].effectParamIndex;
	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] = valueIndex;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: ComboDataInt::updateControl: " << status << endl;
#endif
	return status;
}

string ComboDataInt::getName()
{
	return this->effectComboJson["name"].asString();
}

int ComboDataInt::getInputProcBufferIndex(int LRindex)
{
	return this->inputProcBufferIndex[LRindex];
}
int ComboDataInt::getOutputProcBufferIndex(int LRindex)
{
	return this->outputProcBufferIndex[LRindex];
}
void ComboDataInt::setInputProcBufferIndex(int LRindex, int bufferIndex)
{
	this->inputProcBufferIndex[LRindex] = bufferIndex;
}
void ComboDataInt::setOutputProcBufferIndex(int LRindex, int bufferIndex)
{
	this->outputProcBufferIndex[LRindex] = bufferIndex;
}

#define dbg 0

#define dbg 2

#if(COMBO_DATA_MAP == 1)
	ComboStruct getComboStructFromComboName(string comboName)
	{
		ComboStruct combo;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboDataInt::getComboStructFromComboName" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	//if(comboDataMap[comboName].comboName.empty() == false)
		combo.controlVoltageEnabled = false;
		combo.processCount = comboDataMap[comboName].processCount;
		combo.controlCount = comboDataMap[comboName].controlCount;
		combo.bufferCount = comboDataMap[comboName].bufferCount;

		combo.name = comboDataMap[comboName].comboName;
		if(debugOutput) cout << combo.name << ":" << combo.processCount << ":" << combo.controlCount << ":" << combo.bufferCount << endl;

		for(int i = 0; i < combo.processCount; i++)
		{
			combo.processSequence[i].processName = comboDataMap[comboName].processSequence[i].processName;
			combo.processSequence[i].procType = comboDataMap[comboName].processSequence[i].procType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataMap[comboName].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataMap[comboName].processSequence[i].footswitchNumber;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.processSequence[" << i << "].procType: " << combo.processSequence[i].procType << endl;
			if(debugOutput) cout << "combo.processSequence[" << i << "].processTypeIndex: " << combo.processSequence[i].processTypeIndex << endl;
			if(debugOutput) cout << "combo.processSequence[" << i << "].footswitchNumber: " << combo.processSequence[i].footswitchNumber << endl;
#endif
			//***************** Get input count, allocate memory for input buffer indexes, and store values **************
			combo.processSequence[i].processInputCount = comboDataMap[comboName].processSequence[i].processInputCount;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.processSequence[" << i << "].processInputCount: " << combo.processSequence[i].processInputCount << endl;
#endif
			//combo.processSequence[i].inputBufferIndexes = (int *)calloc(combo.processSequence[i].processInputCount, sizeof(int));
			for(int j = 0; j < combo.processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataMap[comboName].processSequence[i].inputBufferIndexes[j];
#if(dbg >= 2)
				//combo.processSequence[i].inputBufferNames[j] = comboDataMap[comboName].processSequence[i].inputBufferNames[j];
				//if(debugOutput) cout << "combo.processSequence[" << i << "].inputBufferName[" << j << "]: " << combo.processSequence[i].inputBufferNames[j] << endl;
				if(debugOutput) cout << "combo.processSequence[" << i << "].inputBufferIndexes[" << j << "]: " << combo.processSequence[i].inputBufferIndexes[j] << endl;
#endif
			}

			//***************** Get output count, allocate memory for output buffer indexes, and store values  **************
			combo.processSequence[i].processOutputCount = comboDataMap[comboName].processSequence[i].processOutputCount;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.processSequence[" << i << "].processOutputCount: " << combo.processSequence[i].processOutputCount << endl;
#endif
			//combo.processSequence[i].outputBufferIndexes = (int *)calloc(combo.processSequence[i].processOutputCount, sizeof(int));
			for(int j = 0; j < combo.processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataMap[comboName].processSequence[i].outputBufferIndexes[j];
#if(dbg >= 2)
				//combo.processSequence[i].outputBufferNames[j] = comboDataMap[comboName].processSequence[i].outputBufferNames[j];
				//if(debugOutput) cout << "combo.processSequence[" << i << "].outputBufferName[" << j << "]: " << combo.processSequence[i].outputBufferNames[j] << endl;
				if(debugOutput) cout << "combo.processSequence[" << i << "].outputBufferIndexes[" << j << "]: " << combo.processSequence[i].outputBufferIndexes[j] << endl;
#endif
			}

			//***************** Get parameter count and store values **************
			combo.processSequence[i].parameterCount = comboDataMap[comboName].processSequence[i].parameterCount;
			for(int j = 0; j < combo.processSequence[i].parameterCount; j++)
			{
				combo.processSequence[i].parameters[j] = comboDataMap[comboName].processSequence[i].parameters[j]; // copying pointer to location in vector element
#if(dbg >= 2)
				if(debugOutput) cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}
			for(int j = combo.processSequence[i].parameterCount; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = 0;
#if(dbg >= 2)
				if(debugOutput) cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}

			//combo.processSequence[i].internalData[256] = comboDataMap[comboName].processSequence[i].internalData[256];
		}
		for(int i = 0; i < combo.controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataMap[comboName].controlSequence[i];
			combo.controlSequence[i].name = comboDataMap[comboName].controlSequence[i].name;
			combo.controlSequence[i].conType = comboDataMap[comboName].controlSequence[i].conType;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.controlSequence[" << i << "].conType: " << combo.controlSequence[i].conType << endl;
#endif
			combo.controlSequence[i].paramContConnectionCount = comboDataMap[comboName].controlSequence[i].paramContConnectionCount;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.controlSequence[" << i << "].paramContConnectionCount: " << combo.controlSequence[i].paramContConnectionCount << endl;
#endif
			for(int j = 0; j < combo.controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataMap[comboName].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataMap[comboName].controlSequence[i].paramContConnection[j].processParamIndex;
			}

			combo.controlSequence[i].paramContConnectionCountInv = comboDataMap[comboName].controlSequence[i].paramContConnectionCountInv;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.controlSequence[" << i << "].paramContConnectionCountInv: " << combo.controlSequence[i].paramContConnectionCountInv << endl;
#endif
			for(int j = 0; j < combo.controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataMap[comboName].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataMap[comboName].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}

			combo.controlSequence[i].controlTypeIndex = comboDataMap[comboName].controlSequence[i].controlTypeIndex;
#if(dbg >= 2)
			if(debugOutput) cout << "combo.controlSequence[" << i << "].controlTypeIndex: " << combo.controlSequence[i].controlTypeIndex << endl;
#endif


			for(int j = 0; j < 10; j++)
			{
				combo.controlSequence[i].parameter[j].value = comboDataMap[comboName].controlSequence[i].parameter[j].value; // copying pointer to location in vector element
				combo.controlSequence[i].parameter[j].cvEnabled = comboDataMap[comboName].controlSequence[i].parameter[j].cvEnabled; // copying pointer to location in vector element
				if(combo.controlSequence[i].parameter[j].cvEnabled == true)
				{
					combo.controlVoltageEnabled = true; // used to set analog input mode to DC.
				}
	#if(dbg >= 2)
				if(debugOutput) cout << "combo.controlSequence[" << i << "].parameter[" << j << "].value: " << combo.controlSequence[i].parameter[j].value << "combo.controlSequence[" << i << "].parameter[" << j << "].cvEnabled: " << combo.controlSequence[i].parameter[j].cvEnabled << endl;
	#endif
			}
#if(dbg >= 2)
			if(debugOutput) cout << "combo.controlVoltageEnabled: " << combo.controlVoltageEnabled << endl;
#endif
			/*for(int j = combo.controlSequence[i].parameterCount; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = 0;
	#if(dbg >= 2)
				if(debugOutput) cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
	#endif
			}*/
		}
		for(int i = 0; i < comboDataMap[comboName].bufferCount; i++)
		{
	//		//combo.procBufferArray[i] = comboDataMap[comboName].procBufferArray[i];
			combo.procBufferArray[i].processName = comboDataMap[comboName].procBufferArray[i].processName;
			combo.procBufferArray[i].portName = comboDataMap[comboName].procBufferArray[i].portName;
	//		combo.procBufferArray[i].bufferSum = comboDataMap[comboName].procBufferArray[i].bufferSum;
	//		//combo.procBufferArray[i].aveArray = comboDataMap[comboName].procBufferArray[i].aveArray;
	//		//combo.procBufferArray[i].offset = comboDataMap[comboName].procBufferArray[i].offset;
	//		combo.procBufferArray[i].aveArrayIndex = comboDataMap[comboName].procBufferArray[i].aveArrayIndex;
	//		//combo.procBufferArray[i].buffer = comboDataMap[comboName].procBufferArray[i].buffer;
	//		combo.procBufferArray[i].ready = comboDataMap[comboName].procBufferArray[i].ready;
	//		combo.procBufferArray[i].processed = comboDataMap[comboName].procBufferArray[i].processed;
		}

		for(int i = 0; i < 10; i++)
		{
			combo.footswitchStatus[i] = comboDataMap[comboName].footswitchStatus[i];
		}

		combo.inputProcBufferIndex[0] = comboDataMap[comboName].getInputProcBufferIndex(0);
#if(dbg >= 2)
			if(debugOutput) cout << "combo.inputProcBufferIndex[0]: " << combo.inputProcBufferIndex[0] << endl;
#endif
		combo.inputProcBufferIndex[1] = comboDataMap[comboName].getInputProcBufferIndex(1);
#if(dbg >= 2)
			if(debugOutput) cout << "combo.inputProcBufferIndex[1]: " << combo.inputProcBufferIndex[1] << endl;
#endif
		combo.outputProcBufferIndex[0] = comboDataMap[comboName].getOutputProcBufferIndex(0);
#if(dbg >= 2)
			if(debugOutput) cout << "combo.outputProcBufferIndex[0]: " << combo.outputProcBufferIndex[0] << endl;
#endif
		combo.outputProcBufferIndex[1] = comboDataMap[comboName].getOutputProcBufferIndex(1);
#if(dbg >= 2)
			if(debugOutput) cout << "combo.outputProcBufferIndex[1]: " << combo.outputProcBufferIndex[1] << endl;
#endif
		//Json::Value pedalUi;
#if(dbg >= 1)
	if(debugOutput) cout << "*****EXITING ComboDataInt::getComboStructFromComboName" << endl;
#endif

		return combo;
	}
#endif

