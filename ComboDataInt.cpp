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
	cout << "***** ENTERING: ComboDataInt::" << endl;
	cout << ": " <<  << endl;
#endif
#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::: " << status << endl;
#endif
***********/

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
}

ComboDataInt::~ComboDataInt()
{

}

void ComboDataInt::printUnsequencedProcessList()
{
	cout << "UNSEQUENCED PROCESSES" << endl;

	for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
	{
		cout << this->unsequencedProcessListStruct[i].name << endl;
	}
}

void ComboDataInt::printSequencedProcessList()
{
	cout << "SEQUENCED PROCESSES" << endl;

	/*for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
	{
		cout << this->processesStruct[i].name << endl;
	}*/
	for(int i = 0; i < 20; i++)
	{
		if(this->processSequence[i].processName.empty() == false)
		{
			cout << this->processSequence[i].processName << endl;
			for(int j = 0; j < this->processSequence[i].processInputCount; j++)
			{
				cout << "\t" << this->processSequence[i].inputBufferIndexes[j] << ":" << this->processSequence[i].inputBufferNames[j] << endl;
			}
			for(int j = 0; j < this->processSequence[i].parameterCount; j++)
			{
				cout << "\t" << this->processSequence[i].parameters[j] << ":" << this->processSequence[i].parameters[j] << endl;
			}
			for(int j = 0; j < this->processSequence[i].processOutputCount; j++)
			{
				cout << "\t" << this->processSequence[i].outputBufferIndexes[j] << ":" << this->processSequence[i].outputBufferNames[j] << endl;
			}
		}
		else break;
	}

}

void ComboDataInt::printUnsequencedConnectionList()
{
	cout << "UNSEQUENCED CONNECTIONS: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->unsequencedConnectionListJson.size(); i++)
	{
		cout << "unsequencedConnectionListJson[" << i << "]: " << this->unsequencedConnectionListJson[i]["srcProcess"] << ":";
		cout << this->unsequencedConnectionListJson[i]["srcPort"] << ">" << this->unsequencedConnectionListJson[i]["destProcess"] << ":";
		cout << this->unsequencedConnectionListJson[i]["destPort"] << endl;
	}

}

void ComboDataInt::printSequencedConnectionList()
{
	cout << "SEQUENCED CONNECTIONS: " << endl;
	for(std::vector<Json::Value>::size_type i = 0; i < this->connectionsJson.size(); i++)
	{
		cout << "connectionListJson[" << i << "]: " << this->connectionsJson[i]["srcProcess"] << ":";
		cout << this->connectionsJson[i]["srcPort"] << ">" << this->connectionsJson[i]["destProcess"] << ":";
		cout << this->connectionsJson[i]["destPort"] << endl;
	}
}


void ComboDataInt::printUnsortedParameters(void)
{
	cout << "UNSORTED PARAMETERS" << endl;

    for(std::vector<IndexedParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
    {
    	cout << "effectName: " << this->unsortedParameterArray[parameterArrayIndex].effectName
    			<< "\t\tprocessName: " << this->unsortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tparamName: " << this->unsortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->unsortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
}

void ComboDataInt::printSortedParameters(void)
{
	cout << "SORTED PARAMETERS" << endl;

    for(std::vector<IndexedParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->sortedParameterArray.size(); parameterArrayIndex++)
    {
    	cout << "effectName: " << this->sortedParameterArray[parameterArrayIndex].effectName
    			<< "\t\tabsProcessIndex: " << this->sortedParameterArray[parameterArrayIndex].absProcessIndex
    			<< "processName: " << this->sortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tprocessParamIndex: " << this->sortedParameterArray[parameterArrayIndex].processParamIndex
    			<< "paramName: " << this->sortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->sortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
}

void ComboDataInt::printDataReadyList(void)
{
	cout << "DATA READY LIST:" << endl;
	for(std::vector<Connector>::size_type i = 0; i < this->dataReadyList.size(); i++)
	{
		cout << "dataReadyList[" << i <<"]: " << this->dataReadyList[i].process << ":" << this->dataReadyList[i].port << endl;
	}
}

void ComboDataInt::printControlList(void)
{
	cout << "CONTROL LIST:" << endl;
	for(std::vector<Control>::size_type i = 0; i < this->controlsStruct.size(); i++)
	{
		cout << "control[" << i << "].name: " << this->controlsStruct[i].name << endl;
		cout << "control[" << i << "].parentEffect: " << this->controlsStruct[i].parentEffect << endl;
		cout << "control[" << i << "].type: " << this->controlsStruct[i].type << endl;
		for(std::vector<Control>::size_type j = 0; j < this->controlsStruct[i].params.size(); j++)
		{
			cout << "\tcontrol[" << i << "].params:[" << j << "].name: " << this->controlsStruct[i].params[j].name << endl;
			cout << "\tcontrol[" << i << "].params:[" << j << "].alias: " << this->controlsStruct[i].params[j].alias << endl;
			cout << "\tcontrol[" << i << "].params:[" << j << "].abbr: " << this->controlsStruct[i].params[j].abbr << endl;
			cout << "\tcontrol[" << i << "].params:[" << j << "].value: " << this->controlsStruct[i].params[j].value << endl;
		}
		for(std::vector<unsigned int>::size_type j = 0; j < this->controlsStruct[i].absProcessParameterIndexes.size(); j++)
		{
			cout << "\tcontrol[" << i << "].absProcessParameterIndexes:[" << j << "]: " << this->controlsStruct[i].absProcessParameterIndexes[j] << endl;
		}
		for(std::vector<unsigned int>::size_type j = 0; j < this->controlsStruct[i].absProcessParameterIndexesInv.size(); j++)
		{
			cout << "\tcontrol[" << i << "].absProcessParameterIndexesInv:[" << j << "]: " << this->controlsStruct[i].absProcessParameterIndexesInv[j] << endl;
		}

	cout << endl;
	}
}

void ComboDataInt::printSequencedControlList(void)
{
	cout << "SEQUENCED CONTROL LIST:" << endl;
	for(int i = 0; i < 20; i++)
	{
		if(this->controlSequence[i].name.empty() == false)
		{
			cout << "controlSequence[" << i << "].name: " << this->controlSequence[i].name << endl;
			cout << "controlSequence[" << i << "].type: " << this->controlSequence[i].type << endl;
			cout << "controlSequence[" << i << "].paramContConnectionCount: " << this->controlSequence[i].paramContConnectionCount << endl;

			for(int j = 0; j < this->controlSequence[i].paramContConnectionCount; j++)
			{
				cout << "\tcontrolSequence[" << i << "].paramContConnection:[" << j << "].processIndex: " << this->controlSequence[i].paramContConnection[j].processIndex << endl;
				cout << "\tcontrolSequence[" << i << "].paramContConnection:[" << j << "].processParamIndex: " << this->controlSequence[i].paramContConnection[j].processParamIndex << endl;
			}
			cout << "controlSequence[" << i << "].paramContConnectionCountInv: " << this->controlSequence[i].paramContConnectionCountInv << endl;

			for(int j = 0; j < this->controlSequence[i].paramContConnectionCountInv; j++)
			{
				cout << "\tcontrolSequence[" << i << "].paramContConnectionInv:[" << j << "].processIndex: " << this->controlSequence[i].paramContConnectionInv[j].processIndex << endl;
				cout << "\tcontrolSequence[" << i << "].paramContConnectionInv:[" << j << "].processParamIndex: " << this->controlSequence[i].paramContConnectionInv[j].processParamIndex << endl;
			}
		}
		else break;

		cout << endl;
	}
}


void ComboDataInt::printControlParameterList()
{
	cout << "CONTROL PARAMETER LIST:" << endl;

    for(std::vector<IndexedControlParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->controlParameterArray.size(); parameterArrayIndex++)
    {
    	cout << "controlName: " << this->controlParameterArray[parameterArrayIndex].controlName
    			<< "\t\tabsControlIndex: " << this->controlParameterArray[parameterArrayIndex].absControlIndex
    			<< "\t\tcontrolParamIndex: " << this->controlParameterArray[parameterArrayIndex].controlParamIndex
    			/*<< "\t\tabsProcessParamIndex: " << this->controlParameterArray[parameterArrayIndex].absProcessParamIndex*/
    			<< "\t\tcontrolParamName: " << this->controlParameterArray[parameterArrayIndex].controlParamName
    			<< "\t\tcontrolParamValue: " << this->controlParameterArray[parameterArrayIndex].controlParamValue << endl;
    }

}


void ComboDataInt::printProcessParameter(int processParameterIndex)
{
	cout << "PROCESS PARAMETER: ";

	cout << "processName: " << this->sortedParameterArray[processParameterIndex].processName;
	cout << "\tabsProcessIndex: " << this->sortedParameterArray[processParameterIndex].absProcessIndex
			<< "\tprocessParamIndex: " << this->sortedParameterArray[processParameterIndex].processParamIndex
			<< "\tabsParamIndex: " << this->sortedParameterArray[processParameterIndex].absParamIndex
			<< "\tparamName: " << this->sortedParameterArray[processParameterIndex].paramName
			<< "\tparamValue: " << this->sortedParameterArray[processParameterIndex].paramValue << endl;
}


void ComboDataInt::printControlParameter(int controlParameterIndex)
{
	cout << "CONTROL PARAMETER: " << controlParameterIndex << endl;
	cout << "CONTROL PARAMETER ARRAY SIZE: " << this->controlParameterArray.size() << endl;

	cout << "controlName: " << this->controlParameterArray[controlParameterIndex].controlName
			<< "\t\tabsControlIndex: " << this->controlParameterArray[controlParameterIndex].absControlIndex
			<< "\t\tcontrolParamIndex: " << this->controlParameterArray[controlParameterIndex].controlParamIndex
			/*<< "\t\tabsProcessParamIndex: " << this->controlParameterArray[controlParameterIndex].absProcessParamIndex*/
			<< "\t\tcontrolParamName: " << this->controlParameterArray[controlParameterIndex].controlParamName
			<< "\t\tcontrolParamValue: " << this->controlParameterArray[controlParameterIndex].controlParamValue << endl;
}


void ComboDataInt::printControlConnectionList(void)
{
	cout << "CONTROL CONNECTION LIST:" << endl;
	for(std::vector<ControlConnection>::size_type i = 0; i < this->controlConnectionsStruct.size(); i++)
	{
		cout << "controlConnection[" << i << "]: " << this->controlConnectionsStruct[i].src << ">" << this->controlConnectionsStruct[i].dest << endl;
	}
	cout << endl;
}

void ComboDataInt::printBufferList(void)
{
	cout << "PROCESS BUFFER LIST:" << endl;
	for(std::vector<ProcessBuffer>::size_type i = 0; i < 60; i++)
	{
		if(this->procBufferArray[i].processName.empty() == false)
		{
			cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
		}
		else break;
	}
	cout << endl;

}


#define dbg 0
int ComboDataInt::getTargetProcessIndex(string processName)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getTargetProcessIndex" << endl;
	cout << "processName: " << processName << endl;
#endif
	int targetProcessIndex = 0;
	// get index for target process
#if(dbg >=2)
	cout << "process to search for index of: " << processName << endl;
#endif
	try
	{
		for(std::vector<Process>::size_type processIndex = 0; processIndex < this->unsequencedProcessListStruct.size(); processIndex++)
		{
#if(dbg >=2)
			cout << "comparing: " << processName << " & " << this->unsequencedProcessListStruct[processIndex].name << endl;
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
		 cout << "exception in ? " << e.what() <<  endl;
	}
#if(dbg >= 2)
	if(targetProcessIndex >= 0)
		cout << "target process index: " << targetProcessIndex << ":" << this->unsequencedProcessListStruct[targetProcessIndex].name << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getTargetProcessIndex: " << targetProcessIndex << endl;
#endif
	return targetProcessIndex;
}


#define dbg 0
std::vector<string> ComboDataInt::getProcessInputs(string processName)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getProcessInputs" << endl;
	cout << "processName: " << processName << endl;
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
		cout << "inputs[" << i <<"]: " << inputs[i] << endl;
	}
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getProcessInputs: " << inputs.size() << endl;
#endif

	return inputs;
}




#define dbg 0
int ComboDataInt::fillUnsequencedProcessList()
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::fillUnsequencedProcessList" << endl;
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
		cout << "TRANSFERRING PROCESS DATA FROM JSON TO STRUCT..." << endl;
#endif
		for(int effectIndex = 0; (effectIndex < tempEffectCount); effectIndex++)
	    {
	    	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
	        int tempProcCount = tempProcs.size();
	    	for(int procIndex = 0; procIndex < tempProcCount; procIndex++)
	    	{

	    		struct Process tempProc;
	    		tempProc.name = tempProcs[procIndex]["name"].asString();
	    		tempProc.type = tempProcs[procIndex]["type"].asString();

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
					tempProc.inputs.push_back(tempProcs[procIndex]["inputArray"][inputIndex]["name"].asString());
				}

				for(std::vector<Json::Value>::size_type outputIndex = 0; outputIndex < tempProcs[procIndex]["outputArray"].size(); outputIndex++)
				{
					tempProc.outputs.push_back(tempProcs[procIndex]["outputArray"][outputIndex]["name"].asString());
				}

				for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < tempProcs[procIndex]["paramArray"].size(); paramIndex++)
				{
					ProcessParams tempProcParam;
					tempProcParam.name = tempProcs[procIndex]["paramArray"][paramIndex]["name"].asString();

					try
					{
						tempProcParam.type = tempProcs[procIndex]["paramArray"][paramIndex]["type"].asInt();
					}
					catch(std::exception &e)
					{
						tempProcParam.type =  atoi(tempProcs[procIndex]["paramArray"][paramIndex]["type"].asString().c_str());
					}

					try
					{
						tempProcParam.value = tempProcs[procIndex]["paramArray"][paramIndex]["value"].asInt();
					}
					catch(std::exception &e)
					{
						tempProcParam.value =  atoi(tempProcs[procIndex]["paramArray"][paramIndex]["value"].asString().c_str());
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
		cout << "name: " << this->unsequencedProcessListStruct[procIndex].name << endl;
		cout << "type: " << this->unsequencedProcessListStruct[procIndex].type << endl;
    	cout << "footswitchType: " << this->unsequencedProcessListStruct[procIndex].footswitchType << endl;
		cout << "footswitchNumber: " << this->unsequencedProcessListStruct[procIndex].footswitchNumber << endl;

		for(int inputIndex = 0; inputIndex < this->unsequencedProcessListStruct[procIndex].inputs.size(); inputIndex++)
		{
    		cout << "inputs[" << inputIndex << "]: " << this->unsequencedProcessListStruct[procIndex].inputs[inputIndex] << endl;
		}

		for(int outputIndex = 0; outputIndex < this->unsequencedProcessListStruct[procIndex].outputs.size(); outputIndex++)
		{
    		cout << "outputs[" << outputIndex << "]: " << this->unsequencedProcessListStruct[procIndex].outputs[outputIndex] << endl;
		}

		for(int paramIndex = 0; paramIndex < this->unsequencedProcessListStruct[procIndex].params.size(); paramIndex++)
		{
			cout << "params[" << paramIndex << "].name: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].name;
			cout << "params[" << paramIndex << "].type: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].type;
			cout << "params[" << paramIndex << "].value: " << this->unsequencedProcessListStruct[procIndex].params[paramIndex].value;
    		cout << endl;
		}
		cout << endl;
    }
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::fillUnsequencedProcessList: " << status << endl;
#endif
	return status;
}

#define dbg 0
bool ComboDataInt::areDataBuffersReadyForProcessInputs(string processName)
{
	// since data buffers are fed by process outputs, the dataReadyList contains the output process:port names

	// get process outputs that feed data buffers using process inputs and connection list.
	// Start at known process inputs and use connection list to work backwards toward outputs of previous processes.

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::areDataBuffersReadyForProcessInputs" << endl;
	cout << "processName: " << processName << endl;
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
	cout << "target process: " << processName << endl;
	this->printDataReadyList();
	this->printUnsequencedConnectionList();
#endif
	for(std::vector<string>::size_type inputIndex = 0; inputIndex < inputs.size(); inputIndex++)
	{
		for(std::vector<Connector>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListJson.size(); connIndex++)
		{
			string srcProcess = this->unsequencedConnectionListJson[connIndex]["srcProcess"].asString();
			string srcPort = this->unsequencedConnectionListJson[connIndex]["srcPort"].asString();
			string destProcess = this->unsequencedConnectionListJson[connIndex]["destProcess"].asString();
			string destPort = this->unsequencedConnectionListJson[connIndex]["destPort"].asString();

			// if connection dest process:port matches process input, put src process:port in procOutputs
			if(processName.compare(destProcess) == 0 && inputs[inputIndex].compare(destPort)==0)
			{
				Connector tempConn;
				tempConn.process = srcProcess;
				tempConn.port = srcPort;
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
				if(procOutputs[outputIndex].process.compare(this->dataReadyList[dataReadyIndex].process) == 0 &&
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
	cout << "dataReadyMatches: ";
	for(int i = 0; i < dataReadyMatches.size(); i++)
	{
		cout << dataReadyMatches[i] << ",";
	}
	cout << endl;
#endif
	for(std::vector<int>::size_type dataReadyIndex = 0; dataReadyIndex < dataReadyMatches.size(); dataReadyIndex++)
	{
		dataReadyMatchSum += dataReadyMatches[dataReadyIndex];
		//allProcessOutputsContainedInConnectionList &= dataReadyMatches[dataReadyIndex];
	}

	if(dataReadyMatchSum > 0) allProcessOutputsContainedInConnectionList = true;
	else allProcessOutputsContainedInConnectionList = false;

#if(dbg >= 1)
	//cout << "allProcessOutputsContainedInConnectionList: " << allProcessOutputsContainedInConnectionList << endl;
	cout << "***** EXITING: ComboDataInt::areDataBuffersReadyForProcessInputs:" << allProcessOutputsContainedInConnectionList << endl;
#endif

	return allProcessOutputsContainedInConnectionList;
}

#define dbg 0
bool ComboDataInt::isUnsequencedProcessListEmpty()
{
	bool isListEmpty;

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::isUnsequencedProcessListEmpty" << endl;
#endif

	if(this->unsequencedProcessListStruct.size() == 0) isListEmpty = true;
	else isListEmpty = false;
#if(dbg >= 2)
	cout << "unsequenced processes: " << endl;
	this->printUnsequencedProcessList();
#endif

#if(dbg >= 1)
	//cout << "isListEmpty: " << isListEmpty << endl;
	cout << "***** EXITING: ComboDataInt::isUnsequencedProcessListEmpty: " << isListEmpty << endl;
#endif
	return isListEmpty;
}

#define dbg 0
bool ComboDataInt::isOutputInDataReadyList(Connector output)
{
	bool inList = false;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::isOutputInDataReadyList" << endl;
	cout << "Connector process: " << output.process << "\t port: " << output.port << endl;
#endif

	for(unsigned int listedOutputIndex = 0; listedOutputIndex < this->dataReadyList.size(); listedOutputIndex++)
	{
		string listedOutputProcess = this->dataReadyList[listedOutputIndex].process;
		string listedOutputPort = this->dataReadyList[listedOutputIndex].port;
		if(output.process.compare(listedOutputProcess) == 0 && output.port.compare(listedOutputPort) == 0)
		{
			inList = true;
			break;
		}
	}

#if(dbg >= 1)
	//cout << "isListEmpty: " << isListEmpty << endl;
	cout << "***** EXITING: ComboDataInt::isOutputInDataReadyList: " << inList << endl;
#endif

	return inList;
}

#define dbg 0
string ComboDataInt::getFirstProcess()
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getFirstProcess" << endl;
#endif
	string srcProcess = "system";
	string srcPort = "capture_1";
	string firstProcess;
	//bool foundNextProcess = false;


	for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListJson.size(); connIndex++)
	{
#if(dbg >= 2)
		cout << "comparing: ";
		cout << srcProcess << " vs " << this->unsequencedConnectionListJson[connIndex]["srcProcess"];
		cout << "\t and \t";
		cout << srcPort << " vs " << this->unsequencedConnectionListJson[connIndex]["srcPort"] << endl;
#endif
		if(((srcProcess.compare(this->unsequencedConnectionListJson[connIndex]["srcProcess"].asString()) == 0) ||
				(srcProcess.compare(this->unsequencedConnectionListJson[connIndex]["srcEffect"].asString()) == 0)) &&
				srcPort.compare(this->unsequencedConnectionListJson[connIndex]["srcPort"].asString()) == 0)
		{
			/*Connector tempConn;
			tempConn.process = this->unsequencedConnectionListJson[connIndex]["srcProcess"].asString();
			tempConn.port = this->unsequencedConnectionListJson[connIndex]["srcPort"].asString();*/
			//if(foundNextProcess == false)
			{
				firstProcess = this->unsequencedConnectionListJson[connIndex]["destProcess"].asString();
				//this->dataReadyList.push_back(tempConn);
				break;//foundNextProcess = true;
			}
		}
	}

#if(dbg >= 2)
	for(std::vector<Connector>::size_type i = 0; i < this->dataReadyList.size(); i++)
	{
		cout << "dataReadyList[" << i <<"]: " << dataReadyList[i].process << ":" << dataReadyList[i].port << endl;
	}
	//cout << "firstProcess: " << firstProcess << endl;
#endif

#if(dbg >= 1)

	cout << "***** EXITING: ComboDataInt::getFirstProcess: " << firstProcess << endl;
#endif

	return firstProcess;
}

#define dbg 0
string ComboDataInt::getNextProcess()
{
	string nextProcess;
	bool foundNextProcess = false;

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getNextProcess" << endl;
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
		cout << "unsequencedProcessListStruct[" << i <<"]: " << unsequencedProcessListStruct[i].name << endl;
	}

#endif

#if(dbg >= 1)
	//cout << "nextProcess: " << nextProcess << endl;
	cout << "***** EXITING: ComboDataInt::getNextProcess: " << nextProcess << endl;
#endif

	return nextProcess;
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
		if(srcProcess.compare(this->connectionsJson[connIndex]["srcProcess"].asString()) == 0 &&
				srcPort.compare(this->connectionsJson[connIndex]["srcPort"].asString()) == 0)
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
	cout << "***** ENTERING: ComboDataInt::transferProcessToSequencedProcessList" << endl;
	cout << "processName: " << processName << endl;
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
		cout << "targetProcessIndex: " << targetProcessIndex << endl;
		cout << "unsequenced processes: " << endl;
		for(std::vector<Process>::size_type i = 0; i < this->unsequencedProcessListStruct.size(); i++)
		{
			cout << this->unsequencedProcessListStruct[i].name << endl;
		}

		cout << "sequenced processes: " << endl;
		for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
		{
			cout << this->processesStruct[i].name << endl;
		}
	#endif

	}
	catch(std::exception &e)
	{
		 cout << "exception in ? section " << e.what() <<  endl;
		 status = -1;
	}


#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::transferProcessToSequencedProcessList: " << status << endl;
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
			cout << "deleting index: " << connIndex << endl;
#endif
		}
#if(dbg >= 1)

		for(int i = 0; i < this->connectionListJson.size(); i++)
		{
			cout << "connectionListJson[" << i << "]: " << this->connectionListJson[i]["srcProcess"] << ":";
			cout << this->connectionListJson[i]["srcPort"] << ">" << this->connectionListJson[i]["destProcess"] << ":";
			cout << this->connectionListJson[i]["destPort"] << endl;
		}
#endif
	}
}*/

#define dbg 0
int ComboDataInt::addOutputConnectionsToDataReadyList(string processName)
{

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::addOutputConnectionsToDataReadyList" << endl;
	cout << "processName: " << processName << endl;
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
				tempConn.process = processName;
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
		 cout << "exception in getTargetProcessIndex(processName) section " << e.what() <<  endl;
		 status = -1;
	}

#if(dbg >= 2)
	this->printDataReadyList();
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::addOutputConnectionsToDataReadyList: " << status << endl;
#endif
	return status;
}


/*int ComboDataInt::getParameterArray()
{
    int status = 0;
    bool processMatchFoundInParameterArray = true;
    //this->parameterArray.clear();
    this->unsortedParameterArray.clear();


	for(int paramIndex = 0; paramIndex < this->tempParameterArray.size(); paramIndex++)
	{
   		struct IndexedParameter tempParam;
		tempParam.effectName = this->tempParameterArray[paramIndex].effectName;
		tempParam.effectIndex = this->tempParameterArray[paramIndex].effectIndex;
		tempParam.processName = this->tempParameterArray[paramIndex].processName;
		tempParam.effectProcessIndex = this->tempParameterArray[paramIndex].effectProcessIndex;
		tempParam.absProcessIndex = this->tempParameterArray[paramIndex].absProcessIndex;
		tempParam.paramName = this->tempParameterArray[paramIndex].paramName;
		tempParam.processParamIndex = this->tempParameterArray[paramIndex].processParamIndex;
		tempParam.effectParamIndex = this->tempParameterArray[paramIndex].effectParamIndex;
		tempParam.absParamIndex = this->tempParameterArray[paramIndex].absParamIndex;
		tempParam.paramValue = this->tempParameterArray[paramIndex].paramValue;
		this->unsortedParameterArray.push_back(tempParam);
	}


    //this->printUnsortedParameters();
    //this->printSortedParameters();

	return status;
}*/

#define dbg 0
Json::Value ComboDataInt::mergeConnections(Json::Value srcConn, Json::Value destConn)
{
	Json::Value mergedConnection;
#if(dbg >= 1)
	cout << "ComboDataInt::mergeConnections" << endl;
	cout << "srcConn: " << srcConn["srcEffect"] << "/" << srcConn["srcProcess"] << ":" << srcConn["srcPort"] << ">"
			<< srcConn["destEffect"] << "/" << srcConn["destProcess"] << ":" << srcConn["destPort"] << " & ";
	cout << "destConn: " << destConn["srcEffect"] << "/" << destConn["srcProcess"] << ":" << destConn["srcPort"] << ">"
			<< destConn["destEffect"] << "/" << destConn["destProcess"] << ":" << destConn["destPort"] << endl;
#endif

	mergedConnection["srcEffect"] = srcConn["srcEffect"];
	mergedConnection["srcProcess"] = srcConn["srcProcess"];
	mergedConnection["srcPort"] = srcConn["srcPort"];

	mergedConnection["destEffect"] = destConn["destEffect"];
	mergedConnection["destProcess"] = destConn["destProcess"];
	mergedConnection["destPort"] = destConn["destPort"];

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::mergeConnections: ";// << status << endl;
	cout << "mergeConnection: " << mergedConnection["srcEffect"] << "/" << mergedConnection["srcProcess"] << ":" << mergedConnection["srcPort"] << ">"
			<< mergedConnection["destEffect"] << "/" << mergedConnection["destProcess"] << ":" << mergedConnection["destPort"] << endl;
#endif
	return mergedConnection;
}

#define dbg 0
bool ComboDataInt::compareConnectionsSrc2Dest(Json::Value srcConn, Json::Value destConn)
{
	bool result = false;

#if(dbg >= 1)
	cout << "ComboDataInt::compareConnectionsSrc2Dest" << endl;
	cout << "\tsrcConn: " << srcConn["srcEffect"] << "/" << srcConn["srcProcess"] << ":" << srcConn["srcPort"] << ">"
			<< srcConn["destEffect"] << "/" << srcConn["destProcess"] << ":" << srcConn["destPort"] << " & ";
	cout << "destConn: " << destConn["srcEffect"] << "/" << destConn["srcProcess"] << ":" << destConn["srcPort"] << ">"
			<< destConn["destEffect"] << "/" << destConn["destProcess"] << ":" << destConn["destPort"] << endl;
#endif

	bool processMatch = destConn["srcProcess"].asString().compare(srcConn["destProcess"].asString())==0
			&& destConn["srcEffect"].empty() && srcConn["destEffect"].empty();
	bool effectMatch = destConn["srcEffect"].asString().compare(srcConn["destEffect"].asString())==0
			&& destConn["srcProcess"].empty() && srcConn["destProcess"].empty();
	bool portMatch = destConn["srcPort"].asString().compare(srcConn["destPort"].asString())==0;

	cout << "effectMatch: " << effectMatch << "\tprocessMatch: " << processMatch << "\tportMatch: " << portMatch << endl;
	result = ((processMatch || effectMatch) && portMatch);

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::compareConnectionsSrc2Dest: " << result << endl;
#endif

	return result;
}


int ComboDataInt::transferConnection(Json::Value conn, vector<Json::Value> *srcConnArray, vector<Json::Value> *destConnArray)
{
	int status = 0;



	return status;
}

#define dbg 0
int ComboDataInt::getProcessSequenceIndex(string processName)
{
	int index = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getProcessSequenceIndex" << endl;
	cout << "/tprocessName: " << processName <<  endl;
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
	cout << "***** EXITING ComboDataInt::getProcessSequenceIndex: " << index << endl;
#endif
	return index;
}

#define dbg 0
int ComboDataInt::getControlSequenceIndex(string controlName)
{
	int index = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getControlSequenceIndex" << endl;
	cout << "/tcontrolName: " << controlName << endl;
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
	cout << "***** EXITING ComboDataInt::getControlSequenceIndex: " << index << endl;
#endif
	return index;

}


//***************************** PUBLIC *************************************


#define dbg 1
int ComboDataInt::getCombo(char *comboName)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getCombo" << endl;
	cout << "comboName: " << comboName << endl;
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
    cout << "reading JSON file into jsonString." << endl;
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
    		cout << "parsing jsonString in effectComboJson" << endl;

#endif
    		int result = validateJsonBuffer(this->jsonBuffer);
    		if(result == 0) // file needed to cleaned, so replacing file
    		{
    			cout << "file needed cleaning, so replacing with cleaned file" << endl;
    			cout << "new file: " << this->jsonBuffer << endl;
    			close(this->comboFD);
    			/*char command[50];
    			sprintf(command,"rm %s", fileNamePathBuffer);
    			cout << "command: " << command << endl;
    			system(command);*/
    			this->comboFD = open(fileNamePathBuffer, O_WRONLY|O_CREAT|O_TRUNC, 0666);

    			if(write(this->comboFD,this->jsonBuffer,strlen(this->jsonBuffer)) == -1)
    			{
    				clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
    				cout << "error writing jsonBufferString back to combo file." << endl;
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
    			cout << "getting combo index" << endl;
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
    				cout << "JSON parse failed." << endl;
    			}
    		}
    		else
    		{
				status = -1;
				cout << "JSON parse failed." << endl;
    		}
    	}
    	else
    	{
    		cout << "failed to read file: " << fileNamePathBuffer << endl;
    		status = -1;
    	}
    }
    else
    {
    	cout << "failed to open file: " << fileNamePathBuffer << endl;
    	status = -1;
    }

	if(this->comboFD >= 0)
	{
		close(this->comboFD);
	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::getCombo: " << status << endl;
#endif

    //return this->effectComboJson;
	return status;
}



#define dbg 1
int ComboDataInt::getCombo(string comboJson)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getCombo" << endl;
	cout << "comboJson: " << comboJson << endl;
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
    cout << "reading JSON file into jsonString." << endl;
#endif
    //char jsonString[JSON_BUFFER_LENGTH];
    clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
	/* read file into temp string */
    if(comboJson.empty() == false)
    {
    	strncpy(this->jsonBuffer, comboJson.c_str(), JSON_BUFFER_LENGTH);
#if(dbg >= 1)
    		cout << "parsing jsonString in effectComboJson" << endl;

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
			cout << "getting combo index" << endl;
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
				cout << "JSON parse failed." << endl;
			}
		}
		else
		{
			status = -1;
			cout << "JSON parse failed." << endl;
		}
    }
    else
    {
    	cout << "string is empty." << endl;
    	status = -1;
    }

	if(this->comboFD >= 0)
	{
		close(this->comboFD);
	}


#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::getCombo: " << status << endl;
#endif

    //return this->effectComboJson;
	return status;
}

#define dbg 1
int ComboDataInt::getPedalUi(void)
{
    int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getPedalUi" << endl;
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
    	cout << "title: " << this->pedalUiJson["title" ].asString() << endl;
#endif


    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamArrayIndex = 0;
			this->pedalUiJson["effects"][effectIndex]["abbr"] = tempEffects[effectIndex]["abbr"];
			this->pedalUiJson["effects"][effectIndex]["name"] = tempEffects[effectIndex]["name"];
#if(dbg>=2)
			cout << "\teffect: " << this->pedalUiJson["effects"][effectIndex]["abbr"].asString();
			cout << "\t" << this->pedalUiJson["effects"][effectIndex]["name"].asString() << endl;
#endif

        	effectString = tempEffects[effectIndex]["name"].asString();
        	//Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            //procCount = tempProcs.size();
        	Json::Value tempControls = tempEffects[effectIndex]["controlArray"];
        	controlCount = tempControls.size();
        	for(int controlIndex = 0; controlIndex < controlCount; controlIndex++)
        	{
            	controlString = tempControls[controlIndex]["name"].asString();
            	Json::Value params = tempControls[controlIndex]["controlParameterArray"];
        		for(paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
        			paramString = params[paramIndex]["name"].asString();
        			/*if(params[paramIndex]["alias"].asString().size() > 1
        					&& params[paramIndex]["alias"].asString().compare("none") != 0)*/
        			{
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"] =
        						params[paramIndex]["alias"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"] =
        						params[paramIndex]["abbr"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] =
        						params[paramIndex]["value"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["type"] = 0;//params[paramIndex]["type"];
        				this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"] = absParamArrayIndex;
#if(dbg>=2)
						cout << "\t\tparam: " << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"].asString();
						cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"].asString();
						cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"].asString();
						cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"].asString();
						cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["type"].asString() << endl;
#endif
        			}

       			effectParamArrayIndex++;
       			absParamArrayIndex++;
        		}
        	}
        }
        cout << "effectParamArrayIndex: " << effectParamArrayIndex << endl;
#if(dbg>=2)
    	cout << "param count: " << this->pedalUiJson["effects"][0]["params"].size() << endl;
#endif
    	status = 0;//return this->pedalUiJson;
    }
    else
    {
    	status = -1;//return NULL;
    }

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::getPedalUi: " << status << endl;
#endif

    return status;
}

#define dbg 1
int ComboDataInt::getConnections(void)
{
    int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getPedalUi" << endl;
#endif

    //int connIndex = 0;
    this->connectionsJson.clear();
    int loopCount = 0;
    if(this->effectComboJson.isNull() == false)
    {
    	this->unsequencedConnectionListJson.clear();// = new std::vector<Json::Value>;
    	std::vector<Json::Value> proc2procConnections;
    	std::vector<Json::Value> proc2fxConnections;
    	//std::vector<Json::Value> fx2fxConnections;
    	//std::vector<Json::Value> fxConnections;
    	std::vector<Json::Value> mergedConnections;
    	mergedConnections.clear();
		/*************  Read the effect process intra-connections into proc2proc and proc2fx vectors  *************/
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	Json::Value tempProcConns = tempEffects[effectIndex]["connectionArray"];
        	int procConnCount = tempProcConns.size();
        	for(int procConnIndex = 0; procConnIndex < procConnCount; procConnIndex++)
        	{
        		bool srcIsEffectInput = tempProcConns[procConnIndex]["srcProcess"].empty();
        		bool destIsEffectOutput = tempProcConns[procConnIndex]["destProcess"].empty();

				//if(srcIsEffectInput || destIsEffectOutput)
        		{
        			proc2fxConnections.push_back(tempProcConns[procConnIndex]);
					//fxConnections.push_back(tempProcConns[procConnIndex]);
        		}
        		/*else
        		{
        			proc2procConnections.push_back(tempProcConns[procConnIndex]);
        		}*/
        	}
        }

        //**********  Read the effect connections into fx2fx vector  *************
        Json::Value tempEffectConns = this->effectComboJson["effectConnectionArray"];
        int effectConnCount = tempEffectConns.size();
        for(int effectConnIndex = 0; effectConnIndex < effectConnCount; effectConnIndex++)
        {
        	proc2fxConnections.push_back(tempEffectConns[effectConnIndex]);
        }

#if(dbg>=2)
        cout << "proc2procConnections: " << proc2procConnections.size() << endl;
#endif
        for(std::vector<Json::Value>::size_type i = 0; i < proc2procConnections.size(); i++)
        {
        	Json::Value tempConn;
        	tempConn["srcProcess"] = proc2procConnections[i]["srcProcess"];
        	tempConn["srcPort"] = proc2procConnections[i]["srcPort"];
        	tempConn["destProcess"] = proc2procConnections[i]["destProcess"];
        	tempConn["destPort"] = proc2procConnections[i]["destPort"];
        	this->unsequencedConnectionListJson.push_back(tempConn);

#if(dbg>=2)
        	cout << "proc2procConnections: " << proc2procConnections[i]["srcProcess"] << ":" <<
        			proc2procConnections[i]["srcPort"] << ">" <<
					proc2procConnections[i]["destProcess"] << ":" <<
					proc2procConnections[i]["destPort"] << endl;
#endif
        }


        int srcIndex = 0;
        int destIndex = 0;
#if(dbg>=2)
        cout << "FXCONNECTIONS: " << endl;
        /*for(int i = 0; i < fxConnections.size(); i++)
        {
        	cout << fxConnections[i]["srcEffect"]<<"/"<<fxConnections[i]["srcProcess"]<<":"<<fxConnections[i]["srcPort"]<<">"<<fxConnections[i]["destEffect"]<<"/"<<fxConnections[i]["destProcess"]<<":"<<fxConnections[i]["destPort"]<<endl;
        }*/
        cout << "PROC2FX CONNECTIONS: " << endl;
        for(std::vector<Json::Value>::size_type i = 0; i < proc2fxConnections.size(); i++)
        {
        	cout << proc2fxConnections[i]["srcEffect"]<<"/"<<proc2fxConnections[i]["srcProcess"]<<":"<<proc2fxConnections[i]["srcPort"]<<">"<<proc2fxConnections[i]["destEffect"]<<"/"<<proc2fxConnections[i]["destProcess"]<<":"<<proc2fxConnections[i]["destPort"]<<endl;
        }
        cout << "******************** MATCHING *********************" << endl;
#endif
        bool refresh = false;
        bool exit = false;
        int prevSize=0;

        Json::Value tempProc2fxConnection;
        bool matchFound = false;

        //loop through proc2fxConnections
        cout << " pre-reduction proc2fxConnections size: " << proc2fxConnections.size() << endl;
        for(std::vector<Json::Value>::size_type proc2fxIndex = 0; proc2fxIndex < proc2fxConnections.size(); proc2fxIndex++)
        {
            cout << "reduction proc2fxConnections size: " << proc2fxConnections.size() << endl;
            bool exit = false;
		//	for each proc2fxConnection that contains a src process, move it from proc2fxConnection to tempProc2fxConnection
        	if(proc2fxConnections[proc2fxIndex]["srcProcess"].empty() == false)
        	{

#if(dbg>=2)
        		cout << "proc2fxConnections[" << proc2fxIndex << "] contains a src process " << endl;
#endif
        		tempProc2fxConnection = proc2fxConnections[proc2fxIndex];
        		proc2fxConnections.erase(proc2fxConnections.begin() + proc2fxIndex);
        		proc2fxIndex--;
#if(dbg>=2)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
#if(dbg>=2)
        		cout << "PROC2FXCONNECTIONS:"  << endl;
                for(std::vector<Json::Value>::size_type i = 0; i < proc2fxConnections.size(); i++)
                {
                	cout << proc2fxConnections[i]["srcEffect"]<<"/"<<proc2fxConnections[i]["srcProcess"]<<":"<<proc2fxConnections[i]["srcPort"]<<">"<<proc2fxConnections[i]["destEffect"]<<"/"<<proc2fxConnections[i]["destProcess"]<<":"<<proc2fxConnections[i]["destPort"]<<endl;
                }
#endif
                loopCount = 0;
                while(exit == false && loopCount++ < 100)
                {
					//	do another loop through proc2fxConnection inside first loop
					for(std::vector<Json::Value>::size_type innerProc2fxIndex = 0; innerProc2fxIndex < proc2fxConnections.size(); innerProc2fxIndex++)
					{
						//		for each proc2fxConnection, compare tempProc2fxConnection dest to src in rest of proc2fxConnections
						if(this->compareConnectionsSrc2Dest(tempProc2fxConnection, proc2fxConnections[innerProc2fxIndex]))
						{
						//			if a match is found, replace tempProc2fxConnection dest with matched proc2fxConnections dest
							tempProc2fxConnection = this->mergeConnections(tempProc2fxConnection, proc2fxConnections[innerProc2fxIndex]);
							//if(proc2fxConnections[innerProc2fxIndex]["destProcess"].empty() == false)
							{
								proc2fxConnections.erase(proc2fxConnections.begin() + innerProc2fxIndex);
							}
#if(dbg>=2)
							cout << "merged tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
							//				if resulting tempProc2fxConnection connects original tempProc2fxConnection to another process or system port, move it to mergedConnections
							if((tempProc2fxConnection["srcEffect"].empty() || tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(tempProc2fxConnection["destEffect"].empty() || tempProc2fxConnection["destEffect"].compare("system") == 0))
							{
								mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg>=2)
							cout << "moving " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << " to mergedConnections. "<< endl;
#endif
								break;
							}
						}
					}
                }
                if(loopCount >= 1000)
                {
            		cout << "ComboDataInt::getConnections loopCount overflow." << endl;
                }
        	}
        //	for each proc2fxConnection that contains a dest process, move it from proc2fxConnection to tempProc2fxConnection
        	else if(proc2fxConnections[proc2fxIndex]["destProcess"].empty() == false)
        	{
#if(dbg>=2)
        		cout << "proc2fxConnections[" << proc2fxIndex << "] contains a dest process " << endl;
#endif
        		tempProc2fxConnection = proc2fxConnections[proc2fxIndex];
        		proc2fxConnections.erase(proc2fxConnections.begin() + proc2fxIndex);
        		proc2fxIndex--;
#if(dbg>=2)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif

#if(dbg>=2)
        		cout << "PROC2FXCONNECTIONS:"  << endl;
				for(std::vector<Json::Value>::size_type i = 0; i < proc2fxConnections.size(); i++)
				{
					cout << proc2fxConnections[i]["srcEffect"]<<"/"<<proc2fxConnections[i]["srcProcess"]<<":"<<proc2fxConnections[i]["srcPort"]<<">"<<proc2fxConnections[i]["destEffect"]<<"/"<<proc2fxConnections[i]["destProcess"]<<":"<<proc2fxConnections[i]["destPort"]<<endl;
				}
#endif
                loopCount = 0;
                while(exit == false && loopCount++ < 100)
                {
					//	do another loop through proc2fxConnection inside first loop
					for(std::vector<Json::Value>::size_type innerProc2fxIndex = 0; innerProc2fxIndex < proc2fxConnections.size(); innerProc2fxIndex++)
					{
						//		for each proc2fxConnection, compare tempProc2fxConnection src to dest in rest of proc2fxConnections
						if(this->compareConnectionsSrc2Dest(proc2fxConnections[innerProc2fxIndex], tempProc2fxConnection))
						{
							//			if a match is found, replace tempProc2fxConnection src with matched proc2fxConnections src
							tempProc2fxConnection = this->mergeConnections(proc2fxConnections[innerProc2fxIndex], tempProc2fxConnection);
							//if(proc2fxConnections[innerProc2fxIndex]["srcProcess"].empty() == false)
							{
								proc2fxConnections.erase(proc2fxConnections.begin() + innerProc2fxIndex);
							}

	#if(dbg>=2)
							cout << "merged tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
	#endif
							//				if resulting tempProc2fxConnection connects original tempProc2fxConnection to another process or system port, move it to mergedConnections
							if((tempProc2fxConnection["srcEffect"].empty() || tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(tempProc2fxConnection["destEffect"].empty() || tempProc2fxConnection["destEffect"].compare("system") == 0))
							{
								mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg>=2)
							cout << "moving " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << " to mergedConnections. "<< endl;
#endif
								break;
							}
						}
					}
                }
                if(loopCount >= 1000)
                {
            		cout << "ComboDataInt::getConnections loopCount overflow." << endl;
                }
        	}
        }
        cout << " post-reduction proc2fxConnections size: " << proc2fxConnections.size() << endl;

        for(std::vector<Json::Value>::size_type proc2fxIndex = 0; proc2fxIndex < proc2fxConnections.size(); proc2fxIndex++)
        {
            bool exit = false;
		//	for each proc2fxConnection that contains a src process, move it from proc2fxConnection to tempProc2fxConnection
        		tempProc2fxConnection = proc2fxConnections[proc2fxIndex];
        		proc2fxConnections.erase(proc2fxConnections.begin() + proc2fxIndex);
        		proc2fxIndex--;
#if(dbg>=2)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
#if(dbg>=2)
        		cout << "PROC2FXCONNECTIONS:"  << endl;
                for(std::vector<Json::Value>::size_type i = 0; i < proc2fxConnections.size(); i++)
                {
                	cout << proc2fxConnections[i]["srcEffect"]<<"/"<<proc2fxConnections[i]["srcProcess"]<<":"<<proc2fxConnections[i]["srcPort"]<<">"<<proc2fxConnections[i]["destEffect"]<<"/"<<proc2fxConnections[i]["destProcess"]<<":"<<proc2fxConnections[i]["destPort"]<<endl;
                }
#endif
                loopCount = 0;
                while(exit == false && loopCount++ < 100)
                {
					//	do another loop through proc2fxConnection inside first loop
					for(std::vector<Json::Value>::size_type innerProc2fxIndex = 0; innerProc2fxIndex < proc2fxConnections.size(); innerProc2fxIndex++)
					{
						//		for each proc2fxConnection, compare tempProc2fxConnection dest to src in rest of proc2fxConnections
						if(this->compareConnectionsSrc2Dest(tempProc2fxConnection, proc2fxConnections[innerProc2fxIndex]))
						{
						//			if a match is found, replace tempProc2fxConnection dest with matched proc2fxConnections dest
							tempProc2fxConnection = this->mergeConnections(tempProc2fxConnection, proc2fxConnections[innerProc2fxIndex]);
							if(((tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(proc2fxConnections[innerProc2fxIndex]["destEffect"].compare("system") == 0))
								|| ((proc2fxConnections[innerProc2fxIndex]["srcEffect"].compare("system") != 0) &&
									(proc2fxConnections[innerProc2fxIndex]["destEffect"].compare("system") != 0))
								|| ((tempProc2fxConnection["destEffect"].compare("system") == 0) &&
									(proc2fxConnections[innerProc2fxIndex]["destEffect"].compare("system") == 0)))
							{
								cout << "erasing " << proc2fxConnections[innerProc2fxIndex]["srcEffect"];
								cout << "/" << proc2fxConnections[innerProc2fxIndex]["srcProcess"];
								cout << ":" << proc2fxConnections[innerProc2fxIndex]["srcPort"];
								cout << ">" << proc2fxConnections[innerProc2fxIndex]["destEffect"];
								cout << "/" << proc2fxConnections[innerProc2fxIndex]["destProcess"];
								cout << ":" << proc2fxConnections[innerProc2fxIndex]["destPort"] << endl;

								proc2fxConnections.erase(proc2fxConnections.begin() + innerProc2fxIndex);
							}
#if(dbg>=2)
							cout << "merged tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
							//				if resulting tempProc2fxConnection connects original tempProc2fxConnection to another process or system port, move it to mergedConnections
							if(((tempProc2fxConnection["srcEffect"].compare("system") == 0)
									&& (tempProc2fxConnection["destEffect"].compare("system") != 0)) ||
									((tempProc2fxConnection["srcEffect"].compare("system") != 0)
									&& (tempProc2fxConnection["destEffect"].compare("system") == 0))
									)
							{
								proc2fxConnections.push_back(tempProc2fxConnection);
								//mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg>=2)
							cout << "copying " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << " back to proc2fxConnections. "<< endl;
#endif
								break;
							}
							else if((tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(tempProc2fxConnection["destEffect"].compare("system") == 0))
							{

								mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg>=2)
							cout << "moving " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << " to mergedConnections. "<< endl;
#endif
								break;
							}
						}
					}
                }
        }

        for(std::vector<Json::Value>::size_type i = 0; i < mergedConnections.size(); i++)
        {
        	Json::Value mergedConn;
        	if(mergedConnections[i]["srcEffect"].empty() == false)
        		mergedConn["srcProcess"] = mergedConnections[i]["srcEffect"];
        	else
        		mergedConn["srcProcess"] = mergedConnections[i]["srcProcess"];

        	mergedConn["srcPort"] = mergedConnections[i]["srcPort"];

        	if(mergedConnections[i]["destEffect"].empty() == false)
        		mergedConn["destProcess"] = mergedConnections[i]["destEffect"];
        	else
        		mergedConn["destProcess"] = mergedConnections[i]["destProcess"];

        	mergedConn["destPort"] = mergedConnections[i]["destPort"];

#if(dbg>=2)
        	cout << mergedConn["srcProcess"]<<":"<<mergedConn["srcPort"]<<">"
        			<<mergedConn["destProcess"]<<":"<<mergedConn["destPort"]<<endl;
#endif
        	this->unsequencedConnectionListJson.push_back(mergedConn);
        }
#if(dbg>=2)
        cout << "FINAL UNSORTED CONNECTIONS: " << endl;
        this->printUnsequencedConnectionList();
#endif
    }
    else status = 1;//return NULL;
#if(dbg >= 1)
	cout << "*****EXITING ComboDataInt::getPedalUi: " << status << endl;
#endif

    return status;

}

#define dbg 1
int ComboDataInt::getConnections2(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getConnections2" << endl;
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
    	this->unsequencedConnectionListJson.clear();// = new std::vector<Json::Value>;
    	std::vector<Json::Value> procConnections;
    	std::vector<_procInput> procInputs;
    	std::vector<std::vector<int>> connectionStrings;
    	std::vector<Json::Value> mergedConnections;
    	Json::Value tempProcConnection;
    	mergedConnections.clear();
    	procInputs.clear();
    	procConnections.clear();
		/*************  Read the effect process intra-connections into proc vector  *************/
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	Json::Value tempProcConns = tempEffects[effectIndex]["connectionArray"];
        	int procConnCount = tempProcConns.size();
        	for(int procConnIndex = 0; procConnIndex < procConnCount; procConnIndex++)
        	{
				procConnections.push_back(tempProcConns[procConnIndex]);
        	}
        }

        Json::Value tempEffectConns = this->effectComboJson["effectConnectionArray"];
        int effectConnCount = tempEffectConns.size();
        for(int effectConnIndex = 0; effectConnIndex < effectConnCount; effectConnIndex++)
        {
        	procConnections.push_back(tempEffectConns[effectConnIndex]);
        }

        //*********** Put straight proc2proc connections in mergedConnections ***************
        /*for(int procConnIndex = 0; procConnIndex < procConnections.size(); procConnIndex++)
        {
        	if(procConnections[procConnIndex]["srcProcess"].isNull() == false &&
        			procConnections[procConnIndex]["destProcess"].isNull() == false)
        	{
        		mergedConnections.push_back(procConnections[procConnIndex])
        	}
        }*/
        // ************ Gather inputs for all processes ********************
        _procInput tempProcInput;
        tempProcInput.procName = string("system");
        tempProcInput.procInput = string("playback_1");
        procInputs.push_back(tempProcInput);
        tempProcInput.procName = string("system");
        tempProcInput.procInput = string("playback_2");
        procInputs.push_back(tempProcInput);

    	for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
        	int procCount = tempProcs.size();
        	for(int procIndex = 0; procIndex < procCount; procIndex++)
        	{
        		Json::Value tempInputs = tempProcs[procIndex]["inputArray"];
        		for(int procInputIndex = 0; procInputIndex < tempInputs.size(); procInputIndex++)
        		{
        			tempProcInput.procName = tempProcs[procIndex]["name"].asString();
        			tempProcInput.procInput = tempInputs[procInputIndex]["name"].asString();
        			procInputs.push_back(tempProcInput);
        		}
        	}
        }

    	// ASSUMPTION: EACH PROCESS INPUT WILL BE CONNECTED TO ONLY ONE OUTPUT.
        /* For each process/system input, create connectionString integer vector,
         * with each integer in the vector being the index for a connection
         * in procConnections.  A connectionString is done when the last integer
         * points to the process/system output to which the original process input
         * was connected.
         */

        for(std::vector<_procInput>::size_type procInputIndex = 0; procInputIndex < procInputs.size(); procInputIndex++)
        {

#if(dbg >=2)
        	cout << "procInput[" << procInputIndex << "]: " << procInputs[procInputIndex].procName << ":" << procInputs[procInputIndex].procInput << endl;
        	cout << "PROCESS CONNECTIONS:" << endl;
#endif
        	Json::Value inputConn;
        	// For each input, find the connection that directly feeds into it.

    		for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < procConnections.size(); connIndex++)
    		{
				if(((procConnections[connIndex]["destProcess"].compare(procInputs[procInputIndex].procName) == 0) ||
						(procConnections[connIndex]["destEffect"].compare(procInputs[procInputIndex].procName) == 0)) &&
    					(procConnections[connIndex]["destPort"].compare(procInputs[procInputIndex].procInput) == 0))
    			{
    				tempProcConnection = procConnections[connIndex];
    				bool exit = false;

#if(dbg >= 2)
    				cout << "BASE CONNECTION FOUND....." << endl;
    				cout << tempProcConnection["srcEffect"] << "/";
    				cout << tempProcConnection["srcProcess"]<< ":";
    				cout << tempProcConnection["srcPort"] << ">";
    				cout << tempProcConnection["destEffect"] << "/";
    				cout << tempProcConnection["destProcess"]<< ":";
    				cout << tempProcConnection["destPort"] << endl;
    				cout << "************************************" << endl;
#endif



    	    		for(int loopNum = 0; loopNum < 5 && exit == false; loopNum++)
    	        	{
#if(dbg >= 2)
        				cout << "BASE CONNECTION: ";
        				cout << tempProcConnection["srcEffect"] << "/";
        				cout << tempProcConnection["srcProcess"]<< ":";
        				cout << tempProcConnection["srcPort"] << ">";
        				cout << tempProcConnection["destEffect"] << "/";
        				cout << tempProcConnection["destProcess"]<< ":";
        				cout << tempProcConnection["destPort"] << endl;
#endif

    	        		for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < procConnections.size(); connIndex++)
    	        		{
#if(dbg >= 2)
	        				cout << procConnections[connIndex]["srcEffect"] << "/";
	        				cout << procConnections[connIndex]["srcProcess"]<< ":";
	        				cout << procConnections[connIndex]["srcPort"] << ">";
	        				cout << procConnections[connIndex]["destEffect"] << "/";
	        				cout << procConnections[connIndex]["destProcess"]<< ":";
	        				cout << procConnections[connIndex]["destPort"] << endl;
#endif


    	        			if(((procConnections[connIndex]["destEffect"].compare(tempProcConnection["srcEffect"]) == 0) && (procConnections[connIndex]["destProcess"].compare(tempProcConnection["srcProcess"]) == 0)) &&
    	        					(procConnections[connIndex]["destPort"].compare(tempProcConnection["srcPort"]) == 0)
    	        			)
    	        			{
    	        				tempProcConnection = this->mergeConnections(procConnections[connIndex],tempProcConnection);
    	        				exit = (tempProcConnection["srcProcess"].isNull() == false) || (tempProcConnection["srcEffect"].compare("system") == 0);
    	        				if(exit)
    	        				{
    	        					mergedConnections.push_back(tempProcConnection);
    	        				}
#if(dbg >= 2)
    	        				cout << "MATCH....." << endl;
    	        				cout << tempProcConnection["srcEffect"] << "/";
    	        				cout << tempProcConnection["srcProcess"]<< ":";
    	        				cout << tempProcConnection["srcPort"] << ">";
    	        				cout << tempProcConnection["destEffect"] << "/";
    	        				cout << tempProcConnection["destProcess"]<< ":";
    	        				cout << tempProcConnection["destPort"] << endl;
#endif

    	        				break;
    	        			}
    	        			else if((tempProcConnection["srcProcess"].isNull() == false) || (tempProcConnection["srcEffect"].compare("system") == 0))
    	        			// original connection can't be merged further
	        				{
	        					mergedConnections.push_back(tempProcConnection);
	        					exit = true;
	        					break;
	        				}
    	        			else if(connIndex == procConnections.size()-1) // no matches found, so no connections exist
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

        for(std::vector<Json::Value>::size_type mergedConnIndex = 0; mergedConnIndex < mergedConnections.size(); mergedConnIndex++)
        {
#if(dbg >= 2)
        	cout << "mergedConnections[" << mergedConnIndex << "]: ";
			cout << mergedConnections[mergedConnIndex]["srcEffect"] << "/";
			cout << mergedConnections[mergedConnIndex]["srcProcess"] << ":";
			cout << mergedConnections[mergedConnIndex]["srcPort"] << ">";
			cout << mergedConnections[mergedConnIndex]["destEffect"] << "/";
			cout << mergedConnections[mergedConnIndex]["destProcess"] << ":";
			cout << mergedConnections[mergedConnIndex]["destPort"] << endl;
#endif
			Json::Value tempConn;
			if(mergedConnections[mergedConnIndex]["srcEffect"].compare("system") == 0)
			{
				tempConn["srcProcess"] = mergedConnections[mergedConnIndex]["srcEffect"];
			}
			else
			{
				tempConn["srcProcess"] = mergedConnections[mergedConnIndex]["srcProcess"];
			}

			tempConn["srcPort"] = mergedConnections[mergedConnIndex]["srcPort"];

			if(mergedConnections[mergedConnIndex]["destEffect"].compare("system") == 0)
			{
				tempConn["destProcess"] = mergedConnections[mergedConnIndex]["destEffect"];
			}
			else
			{
				tempConn["destProcess"] = mergedConnections[mergedConnIndex]["destProcess"];
			}


			tempConn["destPort"] = mergedConnections[mergedConnIndex]["destPort"];

			this->unsequencedConnectionListJson.push_back(tempConn);
        }

    }
    else status = -1;//return NULL;


#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getConnections2: " << status << endl;
#endif
	return status;
}

#define dbg 1
int ComboDataInt::getProcesses(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getProcesses" << endl;
#endif

	int status = 0;
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
        cout << "getting effects" << endl;
    #endif
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamIndex = 0;
        	effectString = tempEffects[effectIndex]["name"].asString();
        	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            int tempProcCount = tempProcs.size();
            procCount += tempProcCount;
    #if(dbg >= 2)
            cout << "getting processes" << endl;
    #endif

        	for(int procIndex = 0; procIndex < tempProcCount; procIndex++)
        	{
            	procString = tempProcs[procIndex]["name"].asString();
            	Json::Value params = tempProcs[procIndex]["paramArray"];
    #if(dbg >= 2)
                cout << "getting parameters" << endl;
    #endif
        		for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
        			paramString = params[paramIndex]["name"].asString();
        			IndexedParameter tempParam;
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
        			cout << "[" << effectIndex << "]:" << effectString << "[" << procIndex << "]:" << procString << "[" << paramIndex << "]:" << paramString << ":" << paramArrayIndex << endl;
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
    			cout << "fillUnsequencedProcessList" << endl;
#endif
    			this->dataReadyList.clear();

    			Connector tempConn;
    			tempConn.process = string("system");
    			tempConn.port = string("capture_1");
    			this->dataReadyList.push_back(tempConn);
    			tempConn.process = string("system");
    			tempConn.port = string("capture_2");
    			this->dataReadyList.push_back(tempConn);
    			if(this->fillUnsequencedProcessList() != 0)
    			{
    				status = -1;
    			}
            }
        	catch(std::exception &e)
        	{
        		 cout << "exception in fillUnsequencedProcessList section: " << e.what() <<  endl;
        		 status = -1;
        	}
        }

#if(dbg>=2)
        this->printUnsequencedConnectionList();
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
						process = this->getFirstProcess();
						if(process.empty() == true)
						{
							status = -1;
							cout << "first process string is blank." << endl;
							break;
						}
						else
						{
							sequencingStart = false;
						}

					}
					else
					{
						process = this->getNextProcess();
						if(process.empty() == true)
						{
							status = -1;
							cout << "next process string is blank." << endl;
							break;
						}
					}
					//if(process.length() > 1)
					{
						if(this->addOutputConnectionsToDataReadyList(process) != 0)
						{
							status = -1;
							//cout << "addOutputConnectionsToDataReadyList failed."
							break;
						}
						if(this->transferProcessToSequencedProcessList(process) != 0)
						{
							status = -1;
							break;
						}
					}
					if(breakLoopCount++ >= 100)
					{
						status = -1;
						cout << "Couldn't sort processes." << endl;
						break;
					}
				}
			}
			catch(std::exception &e)
			{
				 cout << "exception in isUnsequencedProcessListEmpty while loop: " << e.what() <<  endl;
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
                		for(std::vector<Json::Value>::size_type connIndex = 0; connIndex < this->unsequencedConnectionListJson.size(); connIndex++)
                		{
                			Json::Value connection = this->unsequencedConnectionListJson[connIndex];

                			if(dataReadyItem.process.compare(connection["srcProcess"].asString()) == 0)
                			{
                				this->connectionsJson.push_back(connection);
                				this->unsequencedConnectionListJson.erase(this->unsequencedConnectionListJson.begin() + connIndex);
                			}

                		}
                	}
                }
            }
        	catch(std::exception &e)
        	{
        		cout << "exception in this->dataReadyList.size for loop: " << e.what() <<  endl;
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
        cout << "Create sorted parameter list." << endl;
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
    				for(std::vector<IndexedParameter>::size_type parameterIndex = 0; parameterIndex < this->unsortedParameterArray.size(); parameterIndex++)
    				{
    					// for each parameter, compare process name with parameter.processName
    					if(procName.compare(this->unsortedParameterArray[parameterIndex].processName) == 0)
    					{
    						// if a match is found, push parameter into sortedParameterArray
    						IndexedParameter tempParam;
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
        		cout << "exception in this->processesStruct.size for loop: " << e.what() <<  endl;
        		status = -1;
        	}
        }

#if(dbg >= 2)
        if(status == 0)
        {
            try
            {
                //cout << "SEQUENCED PROCESSES: " << endl;
        		this->printSequencedProcessList();
        		this->printSortedParameters();
                cout << "SEQUENCED CONNECTIONS: " << endl;
                this->printSequencedConnectionList();
            }
        	catch(std::exception &e)
        	{
        		cout << "exception in dbg >= 2  section: " << e.what() <<  endl;
        		status = -1;
        	}
        }
#endif
#if(dbg >= 2)
    cout << "getProcesses end: procCount = " << procCount << endl;
#endif
    }
    else status = -1;
	//this->getParameterArray();

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getProcesses: " << status << endl;
#endif

    return status;
}

#define dbg 2
int ComboDataInt::getControls(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getControls" << endl;
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
        cout << "getting effects" << endl;
    #endif
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamIndex = 0;
        	effectString = tempEffects[effectIndex]["name"].asString();

        	Json::Value tempControls = tempEffects[effectIndex]["controlArray"];
            int tempControlCount = tempControls.size();

        	for(int controlIndex = 0; controlIndex < tempControlCount; controlIndex++)
        	{
        		controlString = tempControls[controlIndex]["name"].asString();
            	Json::Value params = tempControls[controlIndex]["controlParameterArray"];
    #if(dbg >= 2)
                cout << "getting parameters" << endl;
    #endif
        		for(std::vector<Json::Value>::size_type paramIndex = 0; paramIndex < params.size(); paramIndex++)
        		{
        			paramString = params[paramIndex]["name"].asString();
        			IndexedControlParameter tempParam;

        			tempParam.effectName = effectString;
        			tempParam.effectIndex = effectIndex;
        			tempParam.controlName = controlString;
        			tempParam.effectControlIndex = controlIndex;
        			tempParam.absControlIndex = absControlIndex;
        			tempParam.controlParamName = paramString;
        			tempParam.controlParamIndex = paramIndex;
        			tempParam.effectParamIndex = effectParamIndex;
        			//tempParam.absProcessParamIndex = absParamIndex;
        			try
        			{
        				tempParam.controlParamValue = params[paramIndex]["value"].asInt();
        			}
        			catch(std::exception &e)
        			{
        				tempParam.controlParamValue = atoi(params[paramIndex]["value"].asString().c_str());
        			}
       #if(dbg >= 2)
        			cout << "[" << effectIndex << "]:" << effectString << "[" << controlIndex << "]:" << controlString << "[" << paramIndex << "]:" << paramString << ":" << paramArrayIndex << endl;
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
        		Control tempControl;

        		int paramArraySize = tempControls[controlIndex]["controlParameterArray"].size();
        		tempControl.name = tempControls[controlIndex]["name"].asString();
        		tempControl.parentEffect = effectString;
        		tempControl.type = tempControls[controlIndex]["type"].asString();

        		for(int paramArrayIndex = 0; paramArrayIndex < paramArraySize; paramArrayIndex++)
        		{
        			ControlParameter tempParam;
           			tempParam.name = tempControls[controlIndex]["controlParameterArray"][paramArrayIndex]["name"].asString();
           			tempParam.alias = tempControls[controlIndex]["controlParameterArray"][paramArrayIndex]["alias"].asString();
           			tempParam.abbr = tempControls[controlIndex]["controlParameterArray"][paramArrayIndex]["abbr"].asString();
        			try
        			{
        				tempParam.value = tempControls[controlIndex]["controlParameterArray"][paramArrayIndex]["value"].asInt();
					}
						catch(std::exception &e)
						{
							tempParam.value = atoi(tempControls[controlIndex]["controlParameterArray"][paramArrayIndex]["value"].asString().c_str());
					}
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
				cout << "controlConnectionsStruct: " << this->controlConnectionsStruct[connIndex].src << endl;
#endif
				int delimiter = this->controlConnectionsStruct[connIndex].src.find(":");
				if(delimiter != string::npos)
				{
					string controlName = this->controlConnectionsStruct[connIndex].src.substr(0,delimiter);
					string controlOutput = this->controlConnectionsStruct[connIndex].src.substr(delimiter+1);
#if(dbg >= 2)
					cout << "controlsStruct[" << controlIndex << "].name" << this->controlsStruct[controlIndex].name << endl;
					cout << "controlName:  " << controlName << endl;
					cout << "controlOutput:  " << controlOutput << endl;
#endif
					if(controlName.compare(this->controlsStruct[controlIndex].name) == 0)
					{
					//if a match is found, get the absolute parameter index of the dest process:parameter
						uint8_t colonPosition = this->controlConnectionsStruct[connIndex].dest.find(":");
						string process;
						string parameter;

						if(0 < colonPosition && colonPosition < this->controlConnectionsStruct[connIndex].dest.length())
						{
							process = this->controlConnectionsStruct[connIndex].dest.substr(0,colonPosition);
							parameter = this->controlConnectionsStruct[connIndex].dest.substr(colonPosition+1);
						}
						else
							cout << "invalid syntax in control connection: " << this->controlConnectionsStruct[connIndex].dest << endl;

						int paramIndex = this->getProcessParameterIndex(process, parameter);
						//if valid, enter the parameter index into the target parameter index array in the controlsStruct
						if(0 <= paramIndex && paramIndex < 100)
						{
							if(controlOutput.compare("output") == 0)
							{
	#if(dbg >= 2)
								cout << "pushing paramIndex " << paramIndex << " for " << process << ":" << parameter << " into absProcessParameterIndexes" << endl;
	#endif
								this->controlsStruct[controlIndex].absProcessParameterIndexes.push_back(paramIndex);
							}
							else if(controlOutput.compare("outputInv") == 0)
							{
	#if(dbg >= 2)
								cout << "pushing paramIndex " << paramIndex << " for " << process << ":" << parameter << " into absProcessParameterIndexesInv" << endl;
	#endif
								this->controlsStruct[controlIndex].absProcessParameterIndexesInv.push_back(paramIndex);
							}
							else
								cout << "invalid control output name." << endl;

						}
						else
							cout << "invalid control output name." << endl;
					}
				}
				else
				{
					cout << "couldn't find ':' delimiter in src." << endl;
					status = -1;
				}
			}
		}

#if(dbg >= 2)
		this->printControlList();
#endif
    }

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getControls: " << status << endl;
#endif

	return status;
}


#define dbg 1
int ComboDataInt::getControlConnections(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getControlConnections" << endl;
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
    cout << "getting effects" << endl;
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
    		cout << tempControlConnections[controlConnectionIndex]["src"]["name"].asString() << endl;
    		tempContConn.src = tempControlConnections[controlConnectionIndex]["src"]["name"].asString();
    		cout << tempControlConnections[controlConnectionIndex]["dest"]["name"].asString() << endl;
    		tempContConn.dest = tempControlConnections[controlConnectionIndex]["dest"]["name"].asString();
    		this->controlConnectionsStruct.push_back(tempContConn);
    	}
    }

#if(dbg >= 1)
	this->printControlConnectionList();
#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getControlConnections: " << status << endl;
#endif
	return status;
}



#define dbg 0
int ComboDataInt::initProcBufferArray(struct ProcessBuffer *bufferArray, vector<Json::Value> connectionsJson)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::initProcBufferArray" << endl;
#endif
	int procBufferIndex = 0;
	int procBufferCount = 0;
	bool bufferAlreadyInList = false;
	vector<Connector> bufferNameArray;
	vector<Connector>::iterator it;
	Connector tempConn;

	tempConn.process = string("system");
	tempConn.port = string("capture_1");
	bufferNameArray.push_back(tempConn);
	tempConn.process = string("system");
	tempConn.port = string("capture_2");
	bufferNameArray.push_back(tempConn);


	// loop through connectionsJson and create list of process output buffers
	for(std::vector<Json::Value>::size_type connectionsJsonIndex = 0; connectionsJsonIndex < connectionsJson.size(); connectionsJsonIndex++)
	{

		string compConnectionProc = connectionsJson[connectionsJsonIndex]["srcProcess"].asString();
		string compConnectionPort = connectionsJson[connectionsJsonIndex]["srcPort"].asString();

#if(dbg >= 2)
		cout << "[" << connectionsJsonIndex << "]:" << compConnectionProc << ":" << compConnectionPort << endl;
		cout << endl;
		cout << "OUTPUT BUFFERS: " << endl;
		cout << "bufferNameArray size: " << bufferNameArray.size() << endl;
		for(std::vector<Connector>::size_type bufferNameArrayIndex = 0; bufferNameArrayIndex < bufferNameArray.size(); bufferNameArrayIndex++)
		{
			cout << "procBuffer[" << bufferNameArrayIndex << "]: " << bufferNameArray[bufferNameArrayIndex].process;
			cout << ":" << bufferNameArray[bufferNameArrayIndex].port << endl;
		}
		cout << endl;
#endif

		// for each connection, loop through current list of process buffers

		tempConn.process = compConnectionProc;
		tempConn.port = compConnectionPort;



		bufferAlreadyInList = false;
		for(std::vector<Connector>::size_type bufferNameArrayIndex = 0; bufferNameArrayIndex < bufferNameArray.size(); bufferNameArrayIndex++)
		{
			//tempConn.process = bufferNameArray[bufferNameArrayIndex].process;
			//tempConn.port = bufferNameArray[bufferNameArrayIndex].port;

			// compare connection output (source) process:port to listed buffer process:port
#if(dbg >=2)
			cout << "comparing: " << bufferNameArray[bufferNameArrayIndex].process << ":" << bufferNameArray[bufferNameArrayIndex].port;
			cout << " & " << compConnectionProc << ":" << compConnectionPort << endl;
#endif

			if(compConnectionProc.compare(bufferNameArray[bufferNameArrayIndex].process) == 0 &&
					compConnectionPort.compare(bufferNameArray[bufferNameArrayIndex].port) == 0)
			{
#if(dbg >= 2)
				cout << "MATCH: " << compConnectionProc << ":" << compConnectionPort << " already in list." << endl;
#endif
				bufferAlreadyInList = true;
				break;  // connection output already listed in buffer list
			}
		}
		if(bufferAlreadyInList == false) // connection output was not in list, so add to list
		{
			bufferNameArray.push_back(tempConn);
#if(dbg >= 2)
			cout << "NO MATCH: adding " << tempConn.process << ":" << tempConn.port << " to list." << endl;
#endif
		}
	}

#if(dbg >= 2)
	cout << endl;
	cout << "OUTPUT BUFFERS: " << endl;
	cout << "bufferNameArray size: " << bufferNameArray.size() << endl;
#endif
	this->bufferCount = bufferNameArray.size();
	for(int bufferNameArrayIndex = 0; bufferNameArrayIndex < this->bufferCount; bufferNameArrayIndex++)
	{
#if(dbg >= 2)
		cout << "procBuffer[" << bufferNameArrayIndex << "]: " << bufferNameArray[bufferNameArrayIndex].process;
		cout << ":" << bufferNameArray[bufferNameArrayIndex].port << endl;
#endif
		bufferArray[bufferNameArrayIndex].processName = bufferNameArray[bufferNameArrayIndex].process;
		bufferArray[bufferNameArrayIndex].portName = bufferNameArray[bufferNameArrayIndex].port;
	}
#if(dbg >= 2)
	cout << endl;
#endif
#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::initProcBufferArray: " << status << endl;
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
	cout << "***** ENTERING: ComboDataInt::setProcBuffer" << endl;
#endif
	int status = 0;

	procBufferArray.processed = processed;
	procBufferArray.ready = ready;

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::setProcBuffer: " << status << endl;
#endif

	return status;
}*/

/*int ComboDataInt::initProcBuffers(struct ProcessBuffer *procBufferArray)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::initProcBuffers" << endl;
#endif
	int status = 0;

	for(int i = 0; i < 60; i++)
	{
		resetProcBuffer(procBufferArray[i]);
	}
#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::initProcBuffers: " << status << endl;
#endif

	return status;
}*/

#define dbg 0
int ComboDataInt::setProcData(struct ProcessEvent *procEvent, Process processStruct)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::setProcData" << endl;
#endif
	int status = 0;
	string processName;
	volatile int processType = 0;
	//volatile int processIndex = 0;
	volatile int footswitchNumber = 0;
	volatile int processInputCount = 0;
	volatile int processOutputCount = 0;


	if(processStruct.type.compare("delayb") == 0) processType = 0;
	else if(processStruct.type.compare("filter3bb") == 0) processType = 1;
	else if(processStruct.type.compare("filter3bb2") == 0) processType = 2;
	else if(processStruct.type.compare("lohifilterb") == 0) processType = 3;
	else if(processStruct.type.compare("mixerb") == 0) processType = 4;
	else if(processStruct.type.compare("volumeb") == 0) processType = 5;
	else if(processStruct.type.compare("waveshaperb") == 0) processType = 6;
	else if(processStruct.type.compare("reverbb") == 0) processType = 7;
	else if(processStruct.type.compare("samplerb") == 0) processType = 8;
	else if(processStruct.type.compare("oscillatorb") == 0) processType = 9;
	//else if(processStruct.type.compare("blankb") == 0) processType = ?;

	processName = processStruct.name;
	footswitchNumber = processStruct.footswitchNumber;
	processInputCount = processStruct.inputs.size();
	processOutputCount = processStruct.outputs.size();

#if(dbg >= 2)
	std::cout << "ProcessingControl process name: " << processName << std::endl;
	//std::cout << "processType: " << processType << std::endl;
	std::cout << "footswitch number: " << footswitchNumber << std::endl;
	std::cout << "process type index: " << atoi(processName.substr(processName.find("_")+1).c_str()) << std::endl;
	std::cout << "processInputCount: " << processInputCount << std::endl;
	std::cout << "processOutputCount: " << processOutputCount << std::endl;
#endif


	procEvent->processName = processName;
	procEvent->processType = processType;
	procEvent->footswitchNumber = footswitchNumber-1;
	procEvent->processInputCount = processInputCount;

#if(dbg >= 2)
	cout << "procEvent->inputBufferNames: ";
#endif
	for(int processInputIndex = 0; processInputIndex < procEvent->processInputCount; processInputIndex++)
	{
		procEvent->inputBufferNames.push_back(processStruct.inputs[processInputIndex].c_str());
#if(dbg >= 2)
		cout << procEvent->inputBufferNames[processInputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	cout << endl;
	cout << "procEvent->outputBufferNames: ";
#endif

	procEvent->processOutputCount = processOutputCount;
	for(int processOutputIndex = 0; processOutputIndex < procEvent->processOutputCount; processOutputIndex++)
	{
		procEvent->outputBufferNames.push_back(processStruct.outputs[processOutputIndex].c_str());
#if(dbg >= 2)
		cout << procEvent->outputBufferNames[processOutputIndex] << ",";
#endif
	}
#if(dbg >= 2)
	cout << endl;
	cout <<  endl;

#endif

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::setProcData: " << status << endl;
#endif

	return status;
}

#define dbg 0
int ComboDataInt::setProcParameters(struct ProcessEvent *procEvent, Process processStruct)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::setProcParameters" << endl;
#endif

	volatile int parameterCount = processStruct.params.size();
	procEvent->parameterCount = parameterCount;

	for(int paramArrayIndex = 0; paramArrayIndex < procEvent->parameterCount; paramArrayIndex++)
	{
		procEvent->parameters[paramArrayIndex] = processStruct.params[paramArrayIndex].value;

#if(dbg >= 1)
		std::cout << "parameter[" << paramArrayIndex << "]: " << procEvent.parameters[paramArrayIndex] << std::endl;
#endif
	}

	for(int paramArrayIndex = procEvent->parameterCount; paramArrayIndex < 10; paramArrayIndex++)
	{
		procEvent->parameters[paramArrayIndex] = 0;
	}

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::setProcParameters: " << status << endl;
#endif

	return status;
}


int ComboDataInt::initProcInputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::initProcInputBufferIndexes" << endl;
#endif

	for(int procInputIndex = 0; procInputIndex < procEvent->processInputCount; procInputIndex++)
	{
		procEvent->inputBufferIndexes[procInputIndex] = 58;
	}

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::initProcInputBufferIndexes: " << status << endl;
#endif
	return status;
}

int ComboDataInt::initProcOutputBufferIndexes(struct ProcessEvent *procEvent)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::initProcOutputBufferIndexes" << endl;
#endif

	for(int procOutputIndex = 0; procOutputIndex < procEvent->processOutputCount; procOutputIndex++)
	{
		procEvent->outputBufferIndexes[procOutputIndex] = 59;
	}

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::initProcOutputBufferIndexes: " << status << endl;
#endif

	return status;
}

int ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray()
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray" << endl;
#endif

	// loop through connectionsJson
	//for(int bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
	for(unsigned int connIndex = 0; connIndex < this->connectionsJson.size(); connIndex++)
	{
		// 		for each procBufferArray element, get the buffer index and output (source) process:port

		Connector conn, targetConn;

		conn.process = this->connectionsJson[connIndex]["srcProcess"].asString();
		conn.port = this->connectionsJson[connIndex]["srcPort"].asString();
		targetConn.process = this->connectionsJson[connIndex]["destProcess"].asString();
		targetConn.port = this->connectionsJson[connIndex]["destPort"].asString();

#if(dbg >= 2)
		cout << "************************************************************" << endl;
		cout << "conn: " << conn.process << ":" << conn.port << endl;
		cout << "targetConn: " << targetConn.process << ":" << targetConn.port << endl;
#endif
		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
		{
			if(this->procBufferArray[bufferIndex].processName.compare(conn.process) == 0 &&
					this->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{
#if(dbg >= 2)
				cout << "found buffer index for " << conn.process << ": " << bufferIndex << endl;
#endif
				break; // found input process:port index
			}
		}

		//*********** connect relevant system input to output buffer  ********************************

		this->inputProcBufferIndex[0] = 0;
		this->inputProcBufferIndex[1] = 1;
		/*if(conn.process.compare("system") == 0 && conn.port.compare("capture_1") == 0)
		{
			this->inputProcBufferIndex[0] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected to input1: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}
		else if(conn.process.compare("system") == 0 && conn.port.compare("capture_2") == 0)
		{
			this->inputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected to input2: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
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
					procSeqOutputConn.process = this->processSequence[procIndex].processName;
					procSeqOutputConn.port = this->processSequence[procIndex].outputBufferNames[outputIndex];
					// 				compare the procesSequence output process:port to the connectionsJson process:port
					// 				if there is a match, set the processSequence output element index to the output process buffer index

					if(procSeqOutputConn.process.compare(conn.process) == 0 &&
							procSeqOutputConn.port.compare(conn.port) == 0)
					{
						this->processSequence[procIndex].outputBufferIndexes[outputIndex] = bufferIndex;
#if(dbg >= 2)
						cout << "Connected output : procBuffer[" << bufferIndex << "]: ";
						cout <<  procSeqOutputConn.process << ":" << procSeqOutputConn.port << ">";
						cout << targetConn.process << ":" << targetConn.port  << endl;
#endif
					}
				}
			}
		}
	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray: " << status << endl;
#endif
	return status;
}

int ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsJson()
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsJson" << endl;
#endif

	// initialize outputProcBufferIndexes
	this->outputProcBufferIndex[0] = 59;
	this->outputProcBufferIndex[1] = 59;

	// loop through connectionsJson
	for(unsigned int connIndex = 0; connIndex < this->connectionsJson.size(); connIndex++)
	{
		// 		for each connectionsJson element, get the buffer index, output (source) process:port and the
		// 		target (dest) process:port
		Connector conn, targetConn;
		conn.process = this->connectionsJson[connIndex]["srcProcess"].asString();
		conn.port = this->connectionsJson[connIndex]["srcPort"].asString();
		targetConn.process = this->connectionsJson[connIndex]["destProcess"].asString();
		targetConn.port = this->connectionsJson[connIndex]["destPort"].asString();

		int bufferIndex = 0;
		for(bufferIndex = 0; bufferIndex < this->bufferCount; bufferIndex++)
		{
			if(this->procBufferArray[bufferIndex].processName.compare(conn.process) == 0 &&
					this->procBufferArray[bufferIndex].portName.compare(conn.port) == 0)
			{
				break; // found output process:port index
			}
		}

		//*********** connect relevant output buffer to system output ********************************
		if(targetConn.process.compare("system") == 0 && targetConn.port.compare("playback_1") == 0)
		{
			this->outputProcBufferIndex[0] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected output1: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
#endif
		}
		else if(targetConn.process.compare("system") == 0 && targetConn.port.compare("playback_2") == 0)
		{
			this->outputProcBufferIndex[1] = bufferIndex;
#if(dbg >= 2)
			cout << "Connected output2: procBuffer[" << bufferIndex << "]: ";
			cout << conn.process << ":" << conn.port << ">";
			cout <<  targetConn.process << ":" << targetConn.port << endl;
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
					procSeqInputConn.process = this->processSequence[procIndex].processName;
					procSeqInputConn.port = this->processSequence[procIndex].inputBufferNames[inputIndex];
					// 				compare the procesSequence input process:port to the connectionsJson target process:port
					// 				if there is a match, set the processSequence input element index to the output process buffer index
					if(procSeqInputConn.process.compare(targetConn.process) == 0 &&
							procSeqInputConn.port.compare(targetConn.port) == 0)
					{
						this->processSequence[procIndex].inputBufferIndexes[inputIndex] = bufferIndex;
#if(dbg >= 2)
						cout << "Connected input : procBuffer[" << bufferIndex << "]: ";
						cout << conn.process << ":" << conn.port << ">";
						cout <<  procSeqInputConn.process << ":" << procSeqInputConn.port << endl;
#endif
					}
				}
			}
		}
	}
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::connectProcessInputsToProcessOutputBuffersUsingConnectionsJson: " << status << endl;
#endif
	return status;
}

#define dbg 2
int ComboDataInt::initializeControlDataIntoControlEventElement()
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::initializeControlDataIntoControlEventElement" << endl;
#endif
	//loop through controlsStruct vector
	this->controlCount = this->controlsStruct.size();
#if(dbg >= 2)
	cout << "CONTROL EVENT:" << endl;
#endif
	for(int controlIndex = 0; controlIndex < this->controlCount; controlIndex++)
	{
		this->controlSequence[controlIndex].paramContConnectionCount = 0;
		this->controlSequence[controlIndex].paramContConnectionCountInv = 0;
		//for each controlsStruct, enter the control data into the ControlEvent element
		this->controlSequence[controlIndex].name = this->controlsStruct[controlIndex].name;
		if(this->controlsStruct[controlIndex].type.compare("Norm") == 0)
			this->controlSequence[controlIndex].type = 0;
		else if(this->controlsStruct[controlIndex].type.compare("Env") == 0)
			this->controlSequence[controlIndex].type = 1;
		else if(this->controlsStruct[controlIndex].type.compare("LFO") == 0)
			this->controlSequence[controlIndex].type = 2;

#if(dbg >= 2)
		cout << "name: " << this->controlSequence[controlIndex].name << endl;
		cout << "type: " << this->controlSequence[controlIndex].type << endl;
#endif
		// enter the parameter values
		for(unsigned int paramIndex = 0; paramIndex < this->controlsStruct[controlIndex].params.size(); paramIndex++)
		{
			this->controlSequence[controlIndex].parameter[paramIndex] = this->controlsStruct[controlIndex].params[paramIndex].value;
#if(dbg >= 2)
			cout << "\tcontrolSequence[" << controlIndex << "].parameter[" << paramIndex << "]: " << this->controlSequence[controlIndex].parameter[paramIndex] << endl;
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
			cout << "absIndex: " << absIndex << endl;
#endif
			// use absolute parameter index to enter absolute process index from sortedParameterArray into controlSequence connected process
			this->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex =
					this->sortedParameterArray[absIndex].absProcessIndex;
#if(dbg >= 2)
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processIndex: "
					<< this->controlSequence[controlIndex].paramContConnection[paramIndex].processIndex << endl;
#endif
			// use absolute parameter index to enter process parameter index from sortedParameterArray into controlSequence connected process parameter
			this->controlSequence[controlIndex].paramContConnection[paramIndex].processParamIndex =
					this->sortedParameterArray[absIndex].processParamIndex;
#if(dbg >= 2)
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnection[" << paramIndex << "].processParamIndex: "
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
			cout << "absIndex: " << absIndex << endl;
#endif
			// use absolute parameter index to enter absolute process index from sortedParameterArray into controlSequence connected process
			this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processIndex =
					this->sortedParameterArray[absIndex].absProcessIndex;
#if(dbg >= 2)
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnectionInv[" << paramIndex << "].processIndex: "
					<< this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processIndex << endl;
#endif
			// use absolute parameter index to enter process parameter index from sortedParameterArray into controlSequence connected process parameter
			this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processParamIndex =
					this->sortedParameterArray[absIndex].processParamIndex;
#if(dbg >= 2)
			cout << "\tcontrolSequence[" << controlIndex << "].paramContConnectionInv[" << paramIndex << "].processParamIndex: "
					<< this->controlSequence[controlIndex].paramContConnectionInv[paramIndex].processParamIndex << endl;
#endif
			this->controlSequence[controlIndex].paramContConnectionCountInv++;
		}
	}


#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::initializeControlDataIntoControlEventElement: " << status << endl;
#endif
	return status;
}

#define dbg 1
int ComboDataInt::loadComboStructFromName(char *comboName)
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::loadComboStructFromName" << endl;
	cout << "comboName: " << comboName << endl;
#endif

#if(dbg >= 1)
	cout << "Getting combo data from file..." << endl;
#endif
	char pedalUiStr[10];
#if(dbg >= 1)
	cout << "combo title: " << comboName << endl;
#endif
	//effectCount = this->pedalUiJson["effects"].size();
#if(dbg >= 1)
	cout << "number of effects:" << effectCount << endl;
#endif


	if(this->getCombo(comboName) >= 0)
	{
		if(this->getConnections2() >= 0)
		{
#if(dbg >= 2)
			cout << "number of connections: " << this->connectionsJson.size() << endl;
#endif
			if(this->getProcesses() >= 0)
			{
#if(dbg >= 2)
				cout << "number of processes: " << this->processesStruct.size() << endl;
#endif
				if(this->getControlConnections() >= 0)
				{
#if(dbg >= 2)
					cout << "number of controlConnections: " << this->controlConnectionsStruct.size() << endl;
#endif
					if(this->getControls() >= 0)
					{
#if(dbg >= 2)
						cout << "number of controls:" << this->controlsStruct.size() << endl;
#endif
						if(this->getPedalUi() >= 0)
						{
							cout << "loading combo." << endl;
							//volatile int processIndex = 0;
							volatile int processInputCount = 0;
							volatile int processOutputCount = 0;
							//volatile int processSequenceIndex = 0;
							//volatile int procBufferIndex = 0;
							volatile int procBufferCount = 0;

							//volatile int processType = 0;

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
							cout << "this->processCount: " << this->processCount << endl;
						#endif
							this->controlCount = this->controlsStruct.size();

							//*********** initialize process data in ProcessEvent element **************************************
							for(int processIndex = 0; processIndex < this->processCount; processIndex++)
							{
								this->setProcData(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								this->setProcParameters(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								//this->processSequence[processIndex].inputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processInputCount, sizeof(int));
								//this->processSequence[processIndex].outputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processOutputCount, sizeof(int));

								// Initialize input/output buffer indexes with addresses for dummy buffers

								this->initProcInputBufferIndexes(&this->processSequence[processIndex]);
								this->initProcOutputBufferIndexes(&this->processSequence[processIndex]);
							}

							//*********** initialize data in ProcessBuffer element **************************************

							this->initProcBufferArray(this->procBufferArray, connectionsJson);
						#if(dbg >= 2)
							cout << "OUTPUT BUFFERS: " << endl;
							for(int i = 0; i < this->bufferCount; i++)
							{
								cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
							}
							procBufferCount = connectionsJson.size();
							// set data in procBufferArray elements, using connectionsJson data

							cout << endl;
						#endif


						#if(dbg >= 2)
							cout << endl;
						#endif

							this->connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray();
							this->connectProcessInputsToProcessOutputBuffersUsingConnectionsJson();
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
						cout << "getPedalUi failed." << endl;
#endif
						}
					}
					else
					{
						status = -1;
#if(dbg >= 1)
						cout << "getControls failed." << endl;
#endif
					}
				}
				else
				{
					status = -1;
#if(dbg >= 1)
					cout << "getControlConnections failed." << endl;
#endif
				}
			}
			else
			{
				status = -1;
#if(dbg >= 1)
				cout << "getProcesses failed." << endl;
#endif
			}
		}
		else
		{
			status = -1;
#if(dbg >= 1)
			cout << "getConnections failed." << endl;
#endif
		}
	}
	else
	{
		status = -1;
#if(dbg >= 1)
			cout << "file not found." << endl;
#endif
	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::loadComboStructFromName: " << status << endl;
#endif

	return status;

}


int ComboDataInt::loadComboStructFromName2(char *comboName)
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::loadComboStructFromName2" << endl;
	cout << "comboName: " << comboName << endl;
#endif

#if(dbg >= 1)
	cout << "Getting combo data from file..." << endl;
#endif
	char pedalUiStr[10];
#if(dbg >= 1)
	cout << "combo title: " << comboName << endl;
#endif
	//effectCount = this->pedalUiJson["effects"].size();
#if(dbg >= 1)
	cout << "number of effects:" << effectCount << endl;
#endif


/*	if(this->getCombo(comboName) >= 0)
	{
		if(this->getConnections2() >= 0)
		{
#if(dbg >= 2)
			cout << "number of connections: " << this->connectionsJson.size() << endl;
#endif
			if(this->getProcesses() >= 0)
			{
#if(dbg >= 2)
				cout << "number of processes: " << this->processesStruct.size() << endl;
#endif
				if(this->getControlConnections() >= 0)
				{
#if(dbg >= 2)
					cout << "number of controlConnections: " << this->controlConnectionsStruct.size() << endl;
#endif
					if(this->getControls() >= 0)
					{
#if(dbg >= 2)
						cout << "number of controls:" << this->controlsStruct.size() << endl;
#endif
						if(this->getPedalUi() >= 0)
						{*/
							cout << "loading combo." << endl;
							//volatile int processIndex = 0;
							volatile int processInputCount = 0;
							volatile int processOutputCount = 0;
							//volatile int processSequenceIndex = 0;
							//volatile int procBufferIndex = 0;
							volatile int procBufferCount = 0;

							//volatile int processType = 0;

							string processName;
							string connectionName[20];

							/*initProcBuffers(this->procBufferArray);

							// setup dummy buffer for unconnected inputs
							setProcBuffer(this->procBufferArray[58],0,1);*/

							//int parameters[10] = {0,0,0,0,0,0,0,0,0,0};
							//int processInputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
							//int processOutputIndexes[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

							this->processCount = this->processesStruct.size();
						#if(dbg >= 2)
							cout << "this->processCount: " << this->processCount << endl;
						#endif
							this->controlCount = this->controlsStruct.size();

							//*********** initialize process data in ProcessEvent element **************************************
							for(int processIndex = 0; processIndex < this->processCount; processIndex++)
							{
								this->setProcData(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								this->setProcParameters(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								//this->processSequence[processIndex].inputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processInputCount, sizeof(int));
								//this->processSequence[processIndex].outputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processOutputCount, sizeof(int));

								// Initialize input/output buffer indexes with addresses for dummy buffers

								this->initProcInputBufferIndexes(&this->processSequence[processIndex]);
								this->initProcOutputBufferIndexes(&this->processSequence[processIndex]);
							}

							//*********** initialize data in ProcessBuffer element **************************************

							//this->initProcBufferArray(this->procBufferArray, connectionsJson);
						#if(dbg >= 2)
							cout << "OUTPUT BUFFERS: " << endl;
							for(int i = 0; i < this->bufferCount; i++)
							{
								cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
							}
							procBufferCount = connectionsJson.size();
							// set data in procBufferArray elements, using connectionsJson data

							cout << endl;
						#endif


						#if(dbg >= 2)
							cout << endl;
						#endif

							/*this->connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray();
							this->connectProcessInputsToProcessOutputBuffersUsingConnectionsJson();
							this->initializeControlDataIntoControlEventElement();*/
#if(dbg >= 2)
							this->printSequencedProcessList();
							this->printBufferList();
							this->printSequencedControlList();
#endif
						/*}
						else
						{
							status = -1;
#if(dbg >= 1)
						cout << "getPedalUi failed." << endl;
#endif
						}
					}
					else
					{
						status = -1;
#if(dbg >= 1)
						cout << "getControls failed." << endl;
#endif
					}
				}
				else
				{
					status = -1;
#if(dbg >= 1)
					cout << "getControlConnections failed." << endl;
#endif
				}
			}
			else
			{
				status = -1;
#if(dbg >= 1)
				cout << "getProcesses failed." << endl;
#endif
			}
		}
		else
		{
			status = -1;
#if(dbg >= 1)
			cout << "getConnections failed." << endl;
#endif
		}
	}
	else
	{
		status = -1;
#if(dbg >= 1)
			cout << "file not found." << endl;
#endif
	}*/

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::loadComboStructFromName2: " << status << endl;
#endif

	return status;

}




#define dbg 1
int ComboDataInt::loadComboStructFromJsonString(string comboJson)
{
	int status = 0;

#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::loadComboStructFromJsonString" << endl;
	cout << "comboJson: " << comboJson << endl;
#endif

	if(this->getCombo(comboJson) >= 0)
	{
		if(this->getConnections2() >= 0)
		{
#if(dbg >= 1)
			cout << "number of connections: " << this->connectionsJson.size() << endl;
#endif
			if(this->getProcesses() >= 0)
			{
#if(dbg >= 1)
				cout << "number of processes: " << this->processesStruct.size() << endl;
#endif
				if(this->getControlConnections() >= 0)
				{
#if(dbg >= 1)
					cout << "number of controlConnections: " << this->controlConnectionsStruct.size() << endl;
#endif
					if(this->getControls() >= 0)
					{
#if(dbg >= 1)
						cout << "number of controls:" << this->controlsStruct.size() << endl;
#endif
						if(this->getPedalUi() >= 0)
						{
							cout << "loading combo." << endl;
							//volatile int processIndex = 0;
							volatile int processInputCount = 0;
							volatile int processOutputCount = 0;
							//volatile int processSequenceIndex = 0;
							//volatile int procBufferIndex = 0;
							volatile int procBufferCount = 0;

							//volatile int processType = 0;

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
							cout << "this->processCount: " << this->processCount << endl;
						#endif
							this->controlCount = this->controlsStruct.size();

							//*********** initialize process data in ProcessEvent element **************************************
							for(int processIndex = 0; processIndex < this->processCount; processIndex++)
							{
								this->setProcData(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								this->setProcParameters(&this->processSequence[processIndex], this->processesStruct[processIndex]);

								//this->processSequence[processIndex].inputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processInputCount, sizeof(int));
								//this->processSequence[processIndex].outputBufferIndexes = (int *)calloc(this->processSequence[processIndex].processOutputCount, sizeof(int));

								// Initialize input/output buffer indexes with addresses for dummy buffers

								this->initProcInputBufferIndexes(&this->processSequence[processIndex]);
								this->initProcOutputBufferIndexes(&this->processSequence[processIndex]);
							}

							//*********** initialize data in ProcessBuffer element **************************************

							this->initProcBufferArray(this->procBufferArray, connectionsJson);
						#if(dbg >= 2)
							cout << "OUTPUT BUFFERS: " << endl;
							for(int i = 0; i < this->bufferCount; i++)
							{
								cout << this->procBufferArray[i].processName << ":" << this->procBufferArray[i].portName << endl;
							}
							procBufferCount = connectionsJson.size();
							// set data in procBufferArray elements, using connectionsJson data

							cout << endl;
						#endif


						#if(dbg >= 2)
							cout << endl;
						#endif

							this->connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray();
							this->connectProcessInputsToProcessOutputBuffersUsingConnectionsJson();
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
						cout << "getPedalUi failed." << endl;
#endif
						}
					}
					else
					{
						status = -1;
#if(dbg >= 1)
						cout << "getControls failed." << endl;
#endif
					}
				}
				else
				{
					status = -1;
#if(dbg >= 1)
					cout << "getControlConnections failed." << endl;
#endif
				}
			}
			else
			{
				status = -1;
#if(dbg >= 1)
				cout << "getProcesses failed." << endl;
#endif
			}
		}
		else
		{
			status = -1;
#if(dbg >= 1)
			cout << "getConnections failed." << endl;
#endif
		}
	}
	else
	{
		status = -1;
#if(dbg >= 1)
			cout << "file not found." << endl;
#endif
	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::loadComboStructFromJsonString: " << status << endl;
#endif

	return status;

}

#if(COMBO_DATA_VECTOR == 1)
	ComboStruct getComboStructFromName(char *comboName)
	{
		ComboStruct combo;
		string comboNameString = string(comboName);
		int index = getComboIndex(comboName);

		combo.processCount = comboDataVector[index].processCount;
		combo.controlCount = comboDataVector[index].controlCount;
		combo.bufferCount = comboDataVector[index].bufferCount;

		//*************** GET INDEX ****************************
		/*for(vector<ComboDataInt>::size_type comboDataIndex = 0; comboDataIndex < comboDataVector.size(); comboDataIndex++)
		{
			if(comboDataVector[comboDataIndex].comboName.compare(comboNameString))
			{
				index = comboDataIndex;
				break;
			}
		}*/

		combo.name = comboDataVector[index].comboName;
		for(int i = 0; i < comboDataVector[index].processCount; i++)
		{
			//combo.processSequence[i].processName = comboDataVector[index].processSequence[i].processName;
			combo.processSequence[i].processType = comboDataVector[index].processSequence[i].processType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataVector[index].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataVector[index].processSequence[i].footswitchNumber;

			combo.processSequence[i].parameters = comboDataVector[index].processSequence[i].parameters; // copying pointer to location in vector element
			combo.processSequence[i].internalData[256] = comboDataVector[index].processSequence[i].internalData[256];

			combo.processSequence[i].processInputCount = comboDataVector[index].processSequence[i].processInputCount;
			for(int j = 0; j < comboDataVector[index].processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataVector[index].processSequence[i].inputBufferIndexes[j];
				//combo.processSequence[i].inputBufferNames[j] = comboDataVector[index].processSequence[i].inputBufferNames[j];
			}

			combo.processSequence[i].processOutputCount = comboDataVector[index].processSequence[i].processOutputCount;
			for(int j = 0; j < comboDataVector[index].processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataVector[index].processSequence[i].outputBufferIndexes[j];
				//combo.processSequence[i].outputBufferNames[j] = comboDataVector[index].processSequence[i].outputBufferNames[j];
			}

		}
		for(int i = 0; i < comboDataVector[index].controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataVector[index].controlSequence[i];
			combo.controlSequence[i].type = comboDataVector[index].controlSequence[i].type;
			combo.controlSequence[i].parameter = comboDataVector[index].controlSequence[i].parameter;  // copying pointer to location in vector element
			combo.controlSequence[i].paramContConnectionCount = comboDataVector[index].controlSequence[i].paramContConnectionCount;
			for(int j = 0; j < comboDataVector[index].controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataVector[index].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataVector[index].controlSequence[i].paramContConnection[j].processParamIndex;
			}

			combo.controlSequence[i].paramContConnectionCountInv = comboDataVector[index].controlSequence[i].paramContConnectionCountInv;
			for(int j = 0; j < comboDataVector[index].controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataVector[index].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataVector[index].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}
			//combo.controlSequence[i].*controlContext = comboDataVector[index].controlSequence[i].*controlContext;

		}
	//	for(int i = 0; i < comboDataVector[index].bufferCount; i++)
	//	{
	//		//combo.procBufferArray[i] = comboDataVector[index].procBufferArray[i];
	//		//combo.procBufferArray[i].processName = comboDataVector[index].procBufferArray[i].processName;
	//		//combo.procBufferArray[i].portName = comboDataVector[index].procBufferArray[i].portName;
	//		combo.procBufferArray[i].bufferSum = comboDataVector[index].procBufferArray[i].bufferSum;
	//		//combo.procBufferArray[i].aveArray = comboDataVector[index].procBufferArray[i].aveArray;
	//		//combo.procBufferArray[i].offset = comboDataVector[index].procBufferArray[i].offset;
	//		combo.procBufferArray[i].aveArrayIndex = comboDataVector[index].procBufferArray[i].aveArrayIndex;
	//		//combo.procBufferArray[i].buffer = comboDataVector[index].procBufferArray[i].buffer;
	//		combo.procBufferArray[i].ready = comboDataVector[index].procBufferArray[i].ready;
	//		combo.procBufferArray[i].processed = comboDataVector[index].procBufferArray[i].processed;
	//	}

		for(int i = 0; i < 10; i++)
		{
			combo.footswitchStatus[i] = comboDataVector[index].footswitchStatus[i];
		}

		combo.inputProcBufferIndex[0] = comboDataVector[index].inputProcBufferIndex[0];
		combo.inputProcBufferIndex[1] = comboDataVector[index].inputProcBufferIndex[1];
		combo.outputProcBufferIndex[0] = comboDataVector[index].outputProcBufferIndex[0];
		combo.outputProcBufferIndex[1] = comboDataVector[index].outputProcBufferIndex[1];
		//Json::Value pedalUi;


		return combo;
	}
#elif(COMBO_DATA_ARRAY == 1)
	ComboStruct getComboStructFromName(char *comboName)
	{
		ComboStruct combo;
		string comboNameString = string(comboName);

		int index = getComboIndex(comboName);

		combo.processCount = comboDataArray[index].processCount;
		combo.controlCount = comboDataArray[index].controlCount;
		combo.bufferCount = comboDataArray[index].bufferCount;

		//*************** GET INDEX ****************************
		/*for(vector<ComboDataInt>::size_type comboDataIndex = 0; comboDataIndex < comboList.size(); comboDataIndex++)
		{
			if(comboDataArray[comboDataIndex].comboName.compare(comboNameString))
			{
				index = comboDataIndex;
				break;
			}
		}*/

		combo.name = comboDataArray[index].comboName;
		for(int i = 0; i < comboDataArray[index].processCount; i++)
		{
			//combo.processSequence[i].processName = comboDataArray[index].processSequence[i].processName;
			combo.processSequence[i].processType = comboDataArray[index].processSequence[i].processType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataArray[index].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataArray[index].processSequence[i].footswitchNumber;

			for(int j = 0; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = comboDataArray[index].processSequence[i].parameters[j]; // copying pointer to location in vector element
			}
			//combo.processSequence[i].internalData[256] = comboDataArray[index].processSequence[i].internalData[256];

			combo.processSequence[i].processInputCount = comboDataArray[index].processSequence[i].processInputCount;
			for(int j = 0; j < comboDataArray[index].processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataArray[index].processSequence[i].inputBufferIndexes[j];
				//combo.processSequence[i].inputBufferNames[j] = comboDataArray[index].processSequence[i].inputBufferNames[j];
			}

			combo.processSequence[i].processOutputCount = comboDataArray[index].processSequence[i].processOutputCount;
			for(int j = 0; j < comboDataArray[index].processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataArray[index].processSequence[i].outputBufferIndexes[j];
				//combo.processSequence[i].outputBufferNames[j] = comboDataArray[index].processSequence[i].outputBufferNames[j];
			}

		}
		for(int i = 0; i < comboDataArray[index].controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataArray[index].controlSequence[i];
			combo.controlSequence[i].type = comboDataArray[index].controlSequence[i].type;
			for(int j = 0; j < 10; j++)
			{
				combo.controlSequence[i].parameter[j] = comboDataArray[index].controlSequence[i].parameter[j];  // copying pointer to location in vector element
			}
			combo.controlSequence[i].paramContConnectionCount = comboDataArray[index].controlSequence[i].paramContConnectionCount;
			for(int j = 0; j < comboDataArray[index].controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataArray[index].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataArray[index].controlSequence[i].paramContConnection[j].processParamIndex;
			}
			combo.controlSequence[i].paramContConnectionCountInv = comboDataArray[index].controlSequence[i].paramContConnectionCountInv;
			for(int j = 0; j < comboDataArray[index].controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataArray[index].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataArray[index].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}
			//combo.controlSequence[i].*controlContext = comboDataArray[index].controlSequence[i].*controlContext;

		}
	//	for(int i = 0; i < comboDataArray[index].bufferCount; i++)
	//	{
	//		//combo.procBufferArray[i] = comboDataArray[index].procBufferArray[i];
	//		//combo.procBufferArray[i].processName = comboDataArray[index].procBufferArray[i].processName;
	//		//combo.procBufferArray[i].portName = comboDataArray[index].procBufferArray[i].portName;
	//		combo.procBufferArray[i].bufferSum = comboDataArray[index].procBufferArray[i].bufferSum;
	//		//combo.procBufferArray[i].aveArray = comboDataArray[index].procBufferArray[i].aveArray;
	//		//combo.procBufferArray[i].offset = comboDataArray[index].procBufferArray[i].offset;
	//		combo.procBufferArray[i].aveArrayIndex = comboDataArray[index].procBufferArray[i].aveArrayIndex;
	//		//combo.procBufferArray[i].buffer = comboDataArray[index].procBufferArray[i].buffer;
	//		combo.procBufferArray[i].ready = comboDataArray[index].procBufferArray[i].ready;
	//		combo.procBufferArray[i].processed = comboDataArray[index].procBufferArray[i].processed;
	//	}

		for(int i = 0; i < 10; i++)
		{
			combo.footswitchStatus[i] = comboDataArray[index].footswitchStatus[i];
		}

		combo.inputProcBufferIndex[0] = comboDataArray[index].inputProcBufferIndex[0];
		combo.inputProcBufferIndex[1] = comboDataArray[index].inputProcBufferIndex[1];
		combo.outputProcBufferIndex[0] = comboDataArray[index].outputProcBufferIndex[0];
		combo.outputProcBufferIndex[1] = comboDataArray[index].outputProcBufferIndex[1];
		//Json::Value pedalUi;


		return combo;
	}
#elif(COMBO_DATA_MAP == 1)
	ComboStruct getComboStructFromName(char *comboName)
	{
		ComboStruct combo;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getComboStructFromName" << endl;
	cout << "comboName: " << comboName << endl;
#endif
		string comboNameString = string(comboName);

		//int index = getComboIndex(comboName);

		combo.processCount = comboDataMap[comboNameString].processCount;
		combo.controlCount = comboDataMap[comboNameString].controlCount;
		combo.bufferCount = comboDataMap[comboNameString].bufferCount;

		combo.name = comboDataMap[comboNameString].comboName;
		for(int i = 0; i < comboDataMap[comboNameString].processCount; i++)
		{
			combo.processSequence[i].processName = comboDataMap[comboNameString].processSequence[i].processName;
			combo.processSequence[i].processType = comboDataMap[comboNameString].processSequence[i].processType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataMap[comboNameString].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataMap[comboNameString].processSequence[i].footswitchNumber;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processType: " << combo.processSequence[i].processType << endl;
			cout << "combo.processSequence[" << i << "].processTypeIndex: " << combo.processSequence[i].processTypeIndex << endl;
			cout << "combo.processSequence[" << i << "].footswitchNumber: " << combo.processSequence[i].footswitchNumber << endl;
#endif

			//combo.processSequence[i].internalData[256] = comboDataMap[comboNameString].processSequence[i].internalData[256];

			combo.processSequence[i].processInputCount = comboDataMap[comboNameString].processSequence[i].processInputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processInputCount: " << combo.processSequence[i].processInputCount << endl;
#endif
			for(int j = 0; j < combo.processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataMap[comboNameString].processSequence[i].inputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].inputBufferIndexes[" << j << "]: " << combo.processSequence[i].inputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].inputBufferNames[j] = comboDataMap[comboNameString].processSequence[i].inputBufferNames[j];
			}

			combo.processSequence[i].processOutputCount = comboDataMap[comboNameString].processSequence[i].processOutputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processOutputCount: " << combo.processSequence[i].processOutputCount << endl;
#endif
			for(int j = 0; j < combo.processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataMap[comboNameString].processSequence[i].outputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].outputBufferIndexes[" << j << "]: " << combo.processSequence[i].outputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].outputBufferNames[j] = comboDataMap[comboNameString].processSequence[i].outputBufferNames[j];
			}

			combo.processSequence[i].parameterCount = comboDataMap[comboName].processSequence[i].parameterCount;
			for(int j = 0; j < combo.processSequence[i].parameterCount; j++)
			{
				combo.processSequence[i].parameters[j] = comboDataMap[comboName].processSequence[i].parameters[j]; // copying pointer to location in vector element
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}
			for(int j = combo.processSequence[i].parameterCount; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = 0;
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}
		}
		for(int i = 0; i < combo.controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataMap[comboNameString].controlSequence[i];
			combo.controlSequence[i].name = comboDataMap[comboNameString].controlSequence[i].name;
			combo.controlSequence[i].type = comboDataMap[comboNameString].controlSequence[i].type;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].type: " << combo.controlSequence[i].type << endl;
#endif
			for(int j = 0; j < 10; j++)
			{
				combo.controlSequence[i].parameter[j] = comboDataMap[comboNameString].controlSequence[i].parameter[j];  // copying pointer to location in vector element
			}
			combo.controlSequence[i].paramContConnectionCount = comboDataMap[comboNameString].controlSequence[i].paramContConnectionCount;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCount: " << combo.controlSequence[i].paramContConnectionCount << endl;
#endif
			for(int j = 0; j < comboDataMap[comboNameString].controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataMap[comboNameString].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataMap[comboNameString].controlSequence[i].paramContConnection[j].processParamIndex;
			}

			combo.controlSequence[i].paramContConnectionCountInv = comboDataMap[comboNameString].controlSequence[i].paramContConnectionCountInv;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCountInv: " << combo.controlSequence[i].paramContConnectionCountInv << endl;
#endif
			for(int j = 0; j < comboDataMap[comboNameString].controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataMap[comboNameString].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataMap[comboNameString].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}
			//combo.controlSequence[i].*controlContext = comboDataMap[comboNameString].controlSequence[i].*controlContext;

		}
	//	for(int i = 0; i < comboDataMap[comboNameString].bufferCount; i++)
	//	{
	//		//combo.procBufferArray[i] = comboDataMap[comboNameString].procBufferArray[i];
	//		//combo.procBufferArray[i].processName = comboDataMap[comboNameString].procBufferArray[i].processName;
	//		//combo.procBufferArray[i].portName = comboDataMap[comboNameString].procBufferArray[i].portName;
	//		combo.procBufferArray[i].bufferSum = comboDataMap[comboNameString].procBufferArray[i].bufferSum;
	//		//combo.procBufferArray[i].aveArray = comboDataMap[comboNameString].procBufferArray[i].aveArray;
	//		//combo.procBufferArray[i].offset = comboDataMap[comboNameString].procBufferArray[i].offset;
	//		combo.procBufferArray[i].aveArrayIndex = comboDataMap[comboNameString].procBufferArray[i].aveArrayIndex;
	//		//combo.procBufferArray[i].buffer = comboDataMap[comboNameString].procBufferArray[i].buffer;
	//		combo.procBufferArray[i].ready = comboDataMap[comboNameString].procBufferArray[i].ready;
	//		combo.procBufferArray[i].processed = comboDataMap[comboNameString].procBufferArray[i].processed;
	//	}

		for(int i = 0; i < 10; i++)
		{
			combo.footswitchStatus[i] = comboDataMap[comboNameString].footswitchStatus[i];
		}

		combo.inputProcBufferIndex[0] = comboDataMap[comboNameString].inputProcBufferIndex[0];
		combo.inputProcBufferIndex[1] = comboDataMap[comboNameString].inputProcBufferIndex[1];
		combo.outputProcBufferIndex[0] = comboDataMap[comboNameString].outputProcBufferIndex[0];
		combo.outputProcBufferIndex[1] = comboDataMap[comboNameString].outputProcBufferIndex[1];
		//Json::Value pedalUi;

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getComboStructFromName" << endl;
#endif

		return combo;
	}

#endif

#define dbg 1
#if(COMBO_DATA_VECTOR == 1)
#elif(COMBO_DATA_ARRAY == 1)
	ComboStruct getComboStructFromComboIndex(int index)
	{
		ComboStruct combo;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getComboStructFromComboIndex" << endl;
#endif

		combo.processCount = comboDataArray[index].processCount;
		combo.controlCount = comboDataArray[index].controlCount;
		combo.bufferCount = comboDataArray[index].bufferCount;

		combo.name = comboDataArray[index].comboName;
		for(int i = 0; i < combo.processCount; i++)
		{
			combo.processSequence[i].processName = comboDataArray[index].processSequence[i].processName;
			combo.processSequence[i].processType = comboDataArray[index].processSequence[i].processType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataArray[index].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataArray[index].processSequence[i].footswitchNumber;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processType: " << combo.processSequence[i].processType << endl;
			cout << "combo.processSequence[" << i << "].processTypeIndex: " << combo.processSequence[i].processTypeIndex << endl;
			cout << "combo.processSequence[" << i << "].footswitchNumber: " << combo.processSequence[i].footswitchNumber << endl;
#endif

			for(int j = 0; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = comboDataArray[index].processSequence[i].parameters[j]; // copying pointer to location in vector element
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}
			//combo.processSequence[i].internalData[256] = comboDataArray[index].processSequence[i].internalData[256];

			combo.processSequence[i].processInputCount = comboDataArray[index].processSequence[i].processInputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processInputCount: " << combo.processSequence[i].processInputCount << endl;
#endif
			combo.processSequence[i].inputBufferIndexes = (int *)calloc(combo.processSequence[i].processInputCount, sizeof(int));
			for(int j = 0; j < comboDataArray[index].processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataArray[index].processSequence[i].inputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].inputBufferIndexes[" << j << "]: " << combo.processSequence[i].inputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].inputBufferNames[j] = comboDataArray[index].processSequence[i].inputBufferNames[j];
			}

			combo.processSequence[i].processOutputCount = comboDataArray[index].processSequence[i].processOutputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processOutputCount: " << combo.processSequence[i].processOutputCount << endl;
#endif
			combo.processSequence[i].outputBufferIndexes = (int *)calloc(combo.processSequence[i].processOutputCount, sizeof(int));
			for(int j = 0; j < comboDataArray[index].processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataArray[index].processSequence[i].outputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].outputBufferIndexes[" << j << "]: " << combo.processSequence[i].outputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].outputBufferNames[j] = comboDataArray[index].processSequence[i].outputBufferNames[j];
			}

		}
		for(int i = 0; i < combo.controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataArray[index].controlSequence[i];
			combo.controlSequence[i].name = comboDataArray[index].controlSequence[i].name;
			combo.controlSequence[i].type = comboDataArray[index].controlSequence[i].type;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].type: " << combo.controlSequence[i].type << endl;
#endif
			for(int j = 0; j < 10; j++)
			{
				combo.controlSequence[i].parameter[j] = comboDataArray[index].controlSequence[i].parameter[j];  // copying pointer to location in vector element
			}
			combo.controlSequence[i].paramContConnectionCount = comboDataArray[index].controlSequence[i].paramContConnectionCount;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCount: " << combo.controlSequence[i].paramContConnectionCount << endl;
#endif
			for(int j = 0; j < comboDataArray[index].controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataArray[index].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataArray[index].controlSequence[i].paramContConnection[j].processParamIndex;
			}

			combo.controlSequence[i].paramContConnectionCountInv = comboDataArray[index].controlSequence[i].paramContConnectionCountInv;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCount: " << combo.controlSequence[i].paramContConnectionCountInv << endl;
#endif
			for(int j = 0; j < comboDataArray[index].controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataArray[index].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataArray[index].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}
			//combo.controlSequence[i].*controlContext = comboDataArray[index].controlSequence[i].*controlContext;

		}
	//	for(int i = 0; i < comboDataArray[index].bufferCount; i++)
	//	{
	//		//combo.procBufferArray[i] = comboDataArray[index].procBufferArray[i];
	//		//combo.procBufferArray[i].processName = comboDataArray[index].procBufferArray[i].processName;
	//		//combo.procBufferArray[i].portName = comboDataArray[index].procBufferArray[i].portName;
	//		combo.procBufferArray[i].bufferSum = comboDataArray[index].procBufferArray[i].bufferSum;
	//		//combo.procBufferArray[i].aveArray = comboDataArray[index].procBufferArray[i].aveArray;
	//		//combo.procBufferArray[i].offset = comboDataArray[index].procBufferArray[i].offset;
	//		combo.procBufferArray[i].aveArrayIndex = comboDataArray[index].procBufferArray[i].aveArrayIndex;
	//		//combo.procBufferArray[i].buffer = comboDataArray[index].procBufferArray[i].buffer;
	//		combo.procBufferArray[i].ready = comboDataArray[index].procBufferArray[i].ready;
	//		combo.procBufferArray[i].processed = comboDataArray[index].procBufferArray[i].processed;
	//	}

		for(int i = 0; i < 10; i++)
		{
			combo.footswitchStatus[i] = comboDataArray[index].footswitchStatus[i];
		}

		combo.inputProcBufferIndex[0] = comboDataArray[index].inputProcBufferIndex[0];
		combo.inputProcBufferIndex[1] = comboDataArray[index].inputProcBufferIndex[1];
		combo.outputProcBufferIndex[0] = comboDataArray[index].outputProcBufferIndex[0];
		combo.outputProcBufferIndex[1] = comboDataArray[index].outputProcBufferIndex[1];
		//Json::Value pedalUi;
#if(dbg >= 1)
	cout << "*****EXITING ComboDataInt::getComboStructFromComboIndex" << endl;
#endif

		return combo;
	}
#elif(COMBO_DATA_MAP == 1)
	ComboStruct getComboStructFromComboName(string comboName)
	{
		ComboStruct combo;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getComboStructFromComboName" << endl;
	cout << "comboName: " << comboName << endl;
#endif

	//if(comboDataMap[comboName].comboName.empty() == false)

		combo.processCount = comboDataMap[comboName].processCount;
		combo.controlCount = comboDataMap[comboName].controlCount;
		combo.bufferCount = comboDataMap[comboName].bufferCount;

		combo.name = comboDataMap[comboName].comboName;
		cout << combo.name << ":" << combo.processCount << ":" << combo.controlCount << ":" << combo.bufferCount << endl;

		for(int i = 0; i < combo.processCount; i++)
		{
			combo.processSequence[i].processName = comboDataMap[comboName].processSequence[i].processName;
			combo.processSequence[i].processType = comboDataMap[comboName].processSequence[i].processType;  //used to identify process type, not position in processing sequence
			combo.processSequence[i].processTypeIndex = comboDataMap[comboName].processSequence[i].processTypeIndex;
			combo.processSequence[i].footswitchNumber = comboDataMap[comboName].processSequence[i].footswitchNumber;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processType: " << combo.processSequence[i].processType << endl;
			cout << "combo.processSequence[" << i << "].processTypeIndex: " << combo.processSequence[i].processTypeIndex << endl;
			cout << "combo.processSequence[" << i << "].footswitchNumber: " << combo.processSequence[i].footswitchNumber << endl;
#endif
			//***************** Get input count, allocate memory for input buffer indexes, and store values **************
			combo.processSequence[i].processInputCount = comboDataMap[comboName].processSequence[i].processInputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processInputCount: " << combo.processSequence[i].processInputCount << endl;
#endif
			//combo.processSequence[i].inputBufferIndexes = (int *)calloc(combo.processSequence[i].processInputCount, sizeof(int));
			for(int j = 0; j < combo.processSequence[i].processInputCount; j++)
			{
				combo.processSequence[i].inputBufferIndexes[j] = comboDataMap[comboName].processSequence[i].inputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].inputBufferIndexes[" << j << "]: " << combo.processSequence[i].inputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].inputBufferNames[j] = comboDataMap[comboName].processSequence[i].inputBufferNames[j];
			}

			//***************** Get output count, allocate memory for output buffer indexes, and store values  **************
			combo.processSequence[i].processOutputCount = comboDataMap[comboName].processSequence[i].processOutputCount;
#if(dbg >= 2)
			cout << "combo.processSequence[" << i << "].processOutputCount: " << combo.processSequence[i].processOutputCount << endl;
#endif
			//combo.processSequence[i].outputBufferIndexes = (int *)calloc(combo.processSequence[i].processOutputCount, sizeof(int));
			for(int j = 0; j < combo.processSequence[i].processOutputCount; j++)
			{
				combo.processSequence[i].outputBufferIndexes[j] = comboDataMap[comboName].processSequence[i].outputBufferIndexes[j];
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].outputBufferIndexes[" << j << "]: " << combo.processSequence[i].outputBufferIndexes[j] << endl;
#endif
				//combo.processSequence[i].outputBufferNames[j] = comboDataMap[comboName].processSequence[i].outputBufferNames[j];
			}

			//***************** Get parameter count and store values **************
			combo.processSequence[i].parameterCount = comboDataMap[comboName].processSequence[i].parameterCount;
			for(int j = 0; j < combo.processSequence[i].parameterCount; j++)
			{
				combo.processSequence[i].parameters[j] = comboDataMap[comboName].processSequence[i].parameters[j]; // copying pointer to location in vector element
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}
			for(int j = combo.processSequence[i].parameterCount; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = 0;
#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
#endif
			}

			//combo.processSequence[i].internalData[256] = comboDataMap[comboName].processSequence[i].internalData[256];
		}
		for(int i = 0; i < combo.controlCount; i++)
		{
			//combo.controlSequence[i] = comboDataMap[comboName].controlSequence[i];
			combo.controlSequence[i].name = comboDataMap[comboName].controlSequence[i].name;
			combo.controlSequence[i].type = comboDataMap[comboName].controlSequence[i].type;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].type: " << combo.controlSequence[i].type << endl;
#endif
			combo.controlSequence[i].paramContConnectionCount = comboDataMap[comboName].controlSequence[i].paramContConnectionCount;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCount: " << combo.controlSequence[i].paramContConnectionCount << endl;
#endif
			for(int j = 0; j < combo.controlSequence[i].paramContConnectionCount; j++)
			{
				combo.controlSequence[i].paramContConnection[j].processIndex = comboDataMap[comboName].controlSequence[i].paramContConnection[j].processIndex;
				combo.controlSequence[i].paramContConnection[j].processParamIndex = comboDataMap[comboName].controlSequence[i].paramContConnection[j].processParamIndex;
			}

			combo.controlSequence[i].paramContConnectionCountInv = comboDataMap[comboName].controlSequence[i].paramContConnectionCountInv;
#if(dbg >= 2)
			cout << "combo.controlSequence[" << i << "].paramContConnectionCountInv: " << combo.controlSequence[i].paramContConnectionCountInv << endl;
#endif
			for(int j = 0; j < combo.controlSequence[i].paramContConnectionCountInv; j++)
			{
				combo.controlSequence[i].paramContConnectionInv[j].processIndex = comboDataMap[comboName].controlSequence[i].paramContConnectionInv[j].processIndex;
				combo.controlSequence[i].paramContConnectionInv[j].processParamIndex = comboDataMap[comboName].controlSequence[i].paramContConnectionInv[j].processParamIndex;
			}

			for(int j = 0; j < 10; j++)
			{
				combo.controlSequence[i].parameter[j] = comboDataMap[comboName].controlSequence[i].parameter[j]; // copying pointer to location in vector element
	#if(dbg >= 2)
				cout << "combo.controlSequence[" << i << "].parameter[" << j << "]: " << combo.controlSequence[i].parameter[j] << endl;
	#endif
			}
			/*for(int j = combo.controlSequence[i].parameterCount; j < 10; j++)
			{
				combo.processSequence[i].parameters[j] = 0;
	#if(dbg >= 2)
				cout << "combo.processSequence[" << i << "].parameters[" << j << "]: " << combo.processSequence[i].parameters[j] << endl;
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

		combo.inputProcBufferIndex[0] = comboDataMap[comboName].inputProcBufferIndex[0];
#if(dbg >= 2)
			cout << "combo.inputProcBufferIndex[0]: " << combo.inputProcBufferIndex[0] << endl;
#endif
		combo.inputProcBufferIndex[1] = comboDataMap[comboName].inputProcBufferIndex[1];
#if(dbg >= 2)
			cout << "combo.inputProcBufferIndex[1]: " << combo.inputProcBufferIndex[1] << endl;
#endif
		combo.outputProcBufferIndex[0] = comboDataMap[comboName].outputProcBufferIndex[0];
#if(dbg >= 2)
			cout << "combo.outputProcBufferIndex[0]: " << combo.outputProcBufferIndex[0] << endl;
#endif
		combo.outputProcBufferIndex[1] = comboDataMap[comboName].outputProcBufferIndex[1];
#if(dbg >= 2)
			cout << "combo.outputProcBufferIndex[1]: " << combo.outputProcBufferIndex[1] << endl;
#endif
		//Json::Value pedalUi;
#if(dbg >= 1)
	cout << "*****EXITING ComboDataInt::getComboStructFromComboName" << endl;
#endif

		return combo;
	}
#endif




ComboStruct getComboStructFromJsonString(string comboJson)
{
	ComboStruct combo;



	return combo;
}

int ComboDataInt::saveComboStruct(ComboStruct comboStructData)
{

	return 0;
}

#define dbg 0
void ComboDataInt::getProcParameters(int procIndex, int params[10])
{
	int paramsIndex = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getProcParameters" << endl;
	cout << "procIndex: " << procIndex << endl;
#endif
#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getProcParameters: " << status << endl;
#endif

	for(int i = 0; i < 10; i++) params[i] = 0;

	for(std::vector<IndexedParameter>::size_type i = 0; i < this->unsortedParameterArray.size(); i++)
	{
		if(this->unsortedParameterArray.at(i).absProcessIndex == procIndex)
		{
			IndexedParameter temp = this->unsortedParameterArray.at(i);

			params[paramsIndex++] = temp.paramValue;
		}
	}

#if(dbg >= 1)
	cout << "[";
	for(int i = 0; i < 10; i++)
	{
		cout << params[i] << ",";
	}
	cout << "]" << endl;
#endif

}


#define dbg 0
int ComboDataInt::saveCombo(void/*Json::Value combo*/)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::saveCombo" << endl;
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

     for(std::vector<IndexedParameter>::size_type parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
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
  			["controlParameterArray"][controlParamIndex]["value"] = this->controlParameterArray[parameterArrayIndex].controlParamValue;
      }


    /***** Store effectComboJson ********************************************/
    clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
	/* open combo file */
    sprintf(this->fileNamePathBuffer,"/home/Combos/%s.txt", fileNameBuffer);
	this->comboFD = open(this->fileNamePathBuffer,O_WRONLY|O_CREAT|O_TRUNC, 0666);
	/* read file into temp string */
	string tempJsonString = this->comboWriter.write(this->effectComboJson);
#if(dbg>=1)
	//cout << "tempJsonString: " << tempJsonString << endl;
#endif
	strncpy(this->jsonBuffer, tempJsonString.c_str(), tempJsonString.length());
	writeSize = write(this->comboFD, this->jsonBuffer, strlen(this->jsonBuffer));
#if(dbg>=1)
	/*cerr << "tempJsonString.length(): " << tempJsonString.length() << endl;
	cerr << "strlen(this->jsonBuffer): " << strlen(this->jsonBuffer) << endl;
	cerr << "writeSize: " << writeSize << endl;*/
	cout << this->comboWriter.write(this->effectComboJson);
#endif


	if(writeSize < strlen(this->jsonBuffer))
	{
		status = 1;
		printf("JSON write failed.\n");
	}

    close(this->comboFD);
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::saveCombo" << endl;
#endif

	return status;
}

#define dbg 0
int ComboDataInt::updateJson(int paramIndex, int value)
{
	 int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::updateJson" << endl;
	cout << "paramIndex: " << paramIndex << "\tvalue: " << value << endl;
#endif

	 if(value != this->unsortedParameterArray[paramIndex].paramValue)
	 {

		 this->unsortedParameterArray[paramIndex].paramValue = value;
#if(dbg >= 1)
		 	cout << "UPDATE: [" << paramIndex << "] effectName: " << this->unsortedParameterArray[paramIndex].effectName << '\t'
		 			<< "processName: " << this->unsortedParameterArray[paramIndex].processName << '\t'
		 			<< "paramName: " << this->unsortedParameterArray[paramIndex].paramName << '\t'
		 			<< "paramValue: " << this->unsortedParameterArray[paramIndex].paramValue << endl;
#endif
	 }

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::updateJson: " << status << endl;
#endif
	 return status;
}



#define dbg 0
void ComboDataInt::listParameters(void)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::listParameters" << endl;
#endif
	cout << "UNSORTED PARAMETERS" << endl;


    for(unsigned int parameterArrayIndex = 0; parameterArrayIndex < this->unsortedParameterArray.size(); parameterArrayIndex++)
    {
    	cout << "processName: " << this->unsortedParameterArray[parameterArrayIndex].processName
    			<< "\t\tabsProcessIndex: " << this->unsortedParameterArray[parameterArrayIndex].absProcessIndex
    			<< "\t\tprocessParamIndex: " << this->unsortedParameterArray[parameterArrayIndex].processParamIndex
    			<< "\t\tabsParamIndex: " << this->unsortedParameterArray[parameterArrayIndex].absParamIndex
    			<< "\t\tparamName: " << this->unsortedParameterArray[parameterArrayIndex].paramName
    			<< "\t\tparamValue: " << this->unsortedParameterArray[parameterArrayIndex].paramValue << endl;
    }
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::listParameters" << endl;
#endif

}

#define dbg 0
int ComboDataInt::getProcessIndex(int parameterIndex)
{
	int processIndex = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getProcessIndex" << endl;
	cout << "parameterIndex: " << parameterIndex << endl;
#endif

	for(unsigned int i = 0; i < this->unsortedParameterArray.size(); i++)
	{
		if(this->unsortedParameterArray[i].absParamIndex == parameterIndex)
		{
			processIndex = this->unsortedParameterArray[i].absProcessIndex;
			break;
		}
	}
#if(dbg >= 1)
	std::cout << "parameterIndex: " << parameterIndex << "\t\tprocess index: " << processIndex << std::endl;
#endif
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::getProcessIndex" << endl;
#endif

	return processIndex;
}

#define dbg 1
int ComboDataInt::getControlIndex(string targetProcessName, string targetParameterName)
{
	unsigned int controlIndex = -1;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getControlIndex" << endl;
	cout << "targetProcessName: " << targetProcessName << "\ttargetParameterName: " << targetParameterName << endl;
#endif

	//loop through controlConnectionsStruct vector
	for(unsigned int connIndex = 0; connIndex < this->controlConnectionsStruct.size(); connIndex++)
	{
		//	for each controlConnectionsStruct, compare dest process:parameter with targetProcessName and targetParameterName
		string compTarget = targetProcessName + ":" + targetParameterName;
#if(dbg >= 2)
		cout << "comparing: " << this->controlConnectionsStruct[connIndex].dest << " & " << compTarget << endl;
#endif
		if(this->controlConnectionsStruct[connIndex].dest.compare(compTarget) == 0)
		{
 		//	if there is a match, loop through controlsStruct vector
#if(dbg >= 2)
			cout << "MATCH" << endl;
#endif
			for(controlIndex = 0; controlIndex < this->controlsStruct.size(); controlIndex++)
			{
			//	for each controlsStruct, compare the control name with the src name contained in controlConnectionsStruct
				int delimiter = this->controlConnectionsStruct[connIndex].src.find(":");
				string controlName = this->controlConnectionsStruct[connIndex].src.substr(0,delimiter);
				string controlOutput = this->controlConnectionsStruct[connIndex].src.substr(delimiter+1);
				if(controlName.compare(this->controlsStruct[controlIndex].name) == 0)
				{
					//	if there is a match, return the controlsStruct index
					return controlIndex;
				}
			}
		}
	}

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getControlIndex" << endl;
#endif

	return controlIndex;
}

#define dbg 2
int ComboDataInt::getProcessParameterIndex(string processName, string parameterName)
{
	int paramIndex = -1;
#if(dbg >= 1)
	cout << "ENTERING: ComboDataInt::getProcessParameterIndex" << endl;
	cout << "processName: " << processName << "\tparameterName: " << parameterName << endl;
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
			cout << "Comparing with: " << compProcessString << ":" << compParamString << endl;
		}
#endif
		if(processName.compare(compProcessString) == 0 && parameterName.compare(compParamString) == 0)
		{
			paramIndex = parameterArrayIndex;
			break;
		}
	}

#if(dbg >= 1)
	cout << "EXITING: ComboDataInt::getProcessParameterIndex: " << paramIndex << endl;
#endif
	return paramIndex;
}

#define dbg 1
int ComboDataInt::getControlParameterIndex(string controlName, string parameterName)
{
	int paramIndex = -1;
#if(dbg >= 1)
	cout << "ENTERING: ComboDataInt::getControlParameterIndex" << endl;
	cout << "controlName: " << controlName << "\tparameterName: " << parameterName << endl;
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
	cout << "EXITING: ComboDataInt::getControlParameterIndex: " << paramIndex << endl;
#endif

	return paramIndex;
}

#define dbg 0
int ComboDataInt::setProcessSequenceParameter(string processName, int parameterIndex, int valueIndex)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ComboDataInt::setProcessSequenceParameter" << endl;
	cout << "\tprocessName: " << processName <<  "\tparameterIndex: " << parameterIndex <<  "\tvalueIndex: " << valueIndex << endl;
#endif

	int procSeqIndex = this->getProcessSequenceIndex(processName);
	this->processSequence[procSeqIndex].parameters[parameterIndex] = valueIndex;

#if(dbg >= 1)
	cout << "EXITING: ComboDataInt::setProcessSequenceParameter: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::setControlSequenceParameter(string controlName, int parameterIndex, int valueIndex)
{
	int status = 0;
#if(dbg >= 1)
	cout << "ENTERING: ComboDataInt::setControlSequenceParameter" << endl;
	cout << "\tcontrolName: " << controlName << "\tparameterIndex: " << parameterIndex << "\tvalueIndex: " << valueIndex << endl;
#endif

	int conSeqIndex = this->getControlSequenceIndex(controlName);
	if(conSeqIndex >= 0)
	{
		this->controlSequence[conSeqIndex].parameter[parameterIndex] = valueIndex;
		status = 0;
	}
	else
	{
		cout << "invalid controlName." << endl;
		status = -1;
	}

#if(dbg >= 1)
	cout << "EXITING: ComboDataInt::setControlSequenceParameter: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboDataInt::updateProcess(int absParamIndex, int valueIndex)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::updateProcess" << endl;
	cout << "absParamIndex: " << absParamIndex << "valueIndex: " << valueIndex << endl;
#endif
	int status = 0;
#if(dbg >= 1)
	this->printProcessParameter(absParamIndex);
#endif
	this->sortedParameterArray[absParamIndex].paramValue = valueIndex;


#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::updateProcess: " << status << endl;
#endif
	return status;
}

#define dbg 1
int ComboDataInt::updateControl(int absParamIndex, int valueIndex)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::updateControl" << endl;
	cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
#endif
	int status = 0;
	int effectIndex = 0;
	int effectParamArrayIndex = 0;
#if(dbg >= 2)
	this->printControlParameter(absParamIndex);
	cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
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
		cout << "invalid conSeqIndex value." << endl;
		status = -1;
	}

	//*************** Update pedal UI data ***********************
	effectIndex = this->controlParameterArray[absParamIndex].effectIndex;
	effectParamArrayIndex = this->controlParameterArray[absParamIndex].effectParamIndex;
	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] = valueIndex;

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::updateControl: " << status << endl;
#endif
	return status;
}

string ComboDataInt::getName()
{
	return this->effectComboJson["name"].asString();
}


string getComboStringFromFile(string comboName)
{
	string comboString;
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getComboStringFromFile" << endl;
	cout << "comboName: " << comboName << endl;
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
	int comboFD;
	//FILE * comboFD;
	Json::Reader comboReader;
	Json::FastWriter comboWriter;

	char jsonBuffer[JSON_BUFFER_LENGTH];
	char fileNameBuffer[20];
	char fileNamePathBuffer[40];
    //this->tempParameterArray.clear();

#if(dbg >= 1)
    cout << "reading JSON file into jsonString." << endl;
#endif
    //char jsonString[JSON_BUFFER_LENGTH];
    clearBuffer(jsonBuffer,JSON_BUFFER_LENGTH);
    clearBuffer(fileNameBuffer,20);
	/* open combo file */
    strncpy(fileNameBuffer, comboName.c_str(), 19);
    sprintf(fileNamePathBuffer,"/home/Combos/%s.txt", fileNameBuffer);
    comboFD = open(fileNamePathBuffer,O_RDONLY);
	/* read file into temp string */
    if(comboFD >= 0)
    {
    	if(read(comboFD, jsonBuffer, JSON_BUFFER_LENGTH) >= 0)
    	{
    		comboString = string(jsonBuffer);
    	}
    	else
    	{
    		cout << "no data in jsonBuffer" << endl;
    	}
    }
    else
    {
    	cout << "file not found: " << fileNameBuffer << endl;
    }

#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getComboStringFromFile: " << comboString << endl;
#endif

    return comboString;
}
