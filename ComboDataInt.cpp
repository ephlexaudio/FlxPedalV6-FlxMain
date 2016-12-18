/*
 * DatabaseInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

#include "ComboDataInt.h"
#include "utilityFunctions.h"
#include "ProcessingControl.h"
#include "DataFuncts.h"

extern int procCount;
extern int effectCount;
extern int comboIndex;
extern std::vector<string> comboNameList;
#define JSON_BUFFER_LENGTH 16000
extern int validateJsonBuffer(char *jsonBuffer);

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

	for(std::vector<Process>::size_type i = 0; i < this->processesStruct.size(); i++)
	{
		cout << this->processesStruct[i].name << endl;
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

#define dbg 2
int ComboDataInt::getTargetProcessIndex(string processName)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::getTargetProcessIndex" << endl;
#endif
	int targetProcessIndex = 0;
	// get index for target process
	cout << "process to search for index of: " << processName << endl;
	try
	{
		for(std::vector<Process>::size_type processIndex = 0; processIndex < this->unsequencedProcessListStruct.size(); processIndex++)
		{
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
#endif
	std::vector<string> inputs;
	int targetIndex = this->getTargetProcessIndex(processName);

	for(std::vector<Process>::size_type inputIndex = 0; inputIndex < this->unsequencedProcessListStruct[targetIndex].inputs.size(); inputIndex++)
	{
		inputs.push_back(this->unsequencedProcessListStruct[targetIndex].inputs[inputIndex]);
	}

#if(dbg >= 1)
	for(std::vector<string>::size_type i = 0; i < inputs.size(); i++)
	{
		cout << "inputs[" << i <<"]: " << inputs[i] << endl;
	}

	cout << "***** EXITING: ComboDataInt::getProcessInputs: " << inputs << endl;
#endif

	return inputs;
}

/*#define dbg 1
std::vector<string> ComboDataInt::getProcessOutputs(string processName)
{
	cout << "***** ENTERING: ComboDataInt::getProcessOutputs" << endl;
	std::vector<string> outputs;
	int targetIndex = this->getTargetProcessIndex(processName);

	for(int outputIndex = 0; outputIndex < this->unsequencedProcessListStruct[targetIndex]["outputArray"].size(); outputIndex++)
	{
		outputs.push_back(this->unsequencedProcessListStruct[targetIndex]["outputArray"][outputIndex].asString());
	}
#if(dbg >= 1)
	for(int i = 0; i < outputs.size(); i++)
	{
		cout << "outputs[" << i <<"]: " << outputs[i] << endl;
	}
#endif

	cout << "***** EXITING: ComboDataInt::getProcessOutputs" << endl;
	return outputs;
}*/


/*#define dbg 0
int ComboDataInt::getConnectionDestinationIndex(string destinationProcessName)
{
	cout << "***** ENTERING: ComboDataInt::getConnectionDestinationIndex" << endl;
	int targetConnectionIndex = 0;
	// get index for target process
	for(int connectionIndex = 0; connectionIndex < this->connectionsJson.size(); connectionIndex++)
	{
		if(destinationProcessName.compare(this->connectionsJson[connectionIndex]["dstProcess"].asString()) == 0)
		{
			targetConnectionIndex = connectionIndex;
			break;
		}
		if(connectionIndex == this->connectionsJson.size() - 1) //end of connection array reach, but no match found
		{
			targetConnectionIndex = -1;
		}

	}
	cout << "targetConnectionIndex: " << targetConnectionIndex << endl;
	cout << "***** EXITING: ComboDataInt::getConnectionDestinationIndex" << endl;
	return targetConnectionIndex;
}*/


#define dbg 1
int ComboDataInt::fillUnsequencedProcessList()
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::fillUnsequencedProcessList" << endl;
#endif
	int status = 0;
	/*for(int procIndex = 0; procIndex < this->unsequencedProcessListStruct.size(); procIndex++)
	{
		this->unsequencedProcessListStruct = this->unsequencedProcessListStruct;
	}*/

	try
	{
		this->unsequencedProcessListStruct.clear();// = new std::vector<Json::Value>;

		Json::Value tempEffects = this->effectComboJson["effectArray"];
		int tempEffectCount = tempEffects.size();
		cout << "TRANSFERRING PROCESS DATA FROM JSON TO STRUCT..." << endl;
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
/*#define dbg 0
bool ComboDataInt::checkConnectionListDestinationsAgainstUnsequencedProcessInputs(string processName)
{
	bool allProcessInputsContainedInConnectionList = true;
	bool inputMatches[10] = {true,true,true,true,true,true,true,true,true,true};
	int targetProcessIndex = 0;
	// get index for target process
	targetProcessIndex = this->getTargetProcessIndex(processName);

	int inputCount = this->unsequencedProcessListStruct[targetProcessIndex]["inputArrays"].size();
	for(int inputIndex = 0; inputIndex < inputCount; inputIndex++)
	{
		// loop through connection list and check target process inputs against dest process:port
		for(int connectionIndex = 0; connectionIndex < this->connectionsJson.size(); connectionIndex++)
		{
			string compDestProcess = this->connectionsJson[connectionIndex]["destProcess"].asString();
			string destPort = this->unsequencedProcessListStruct[targetProcessIndex]["inputArrays"][inputIndex]["name"].asString();
			string compDestPort = this->connectionsJson[connectionIndex]["destPort"].asString();

			if(processName.compare(compDestProcess) == 0 && destPort.compare(compDestPort) == 0)
			{
				break; // match found, so exit loop to avoid the next if statement
			}
			if(inputIndex == inputCount - 1) //end of connection array reach, but no match found
			{
				inputMatches[inputIndex] = false;
			}
		}
	}
	for(int inputMatchIndex = 0; inputMatchIndex < 10; inputMatchIndex++)
	{
		allProcessInputsContainedInConnectionList &= inputMatches[inputMatchIndex];
	}


	return allProcessInputsContainedInConnectionList;
}*/

#define dbg 2
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

bool ComboDataInt::isOutputInDataReadyList(Connector output)
{
	bool inList = false;

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

	return inList;
}

#define dbg 2
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
		cout << "comparing: ";
		cout << srcProcess << " vs " << this->unsequencedConnectionListJson[connIndex]["srcProcess"];
		cout << "\t and \t";
		cout << srcPort << " vs " << this->unsequencedConnectionListJson[connIndex]["srcPort"] << endl;

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

#define dbg 2
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
		cout << "procName:" << procName << endl;
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


#define dbg 2
int ComboDataInt::transferProcessToSequencedProcessList(string processName)
{
#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::transferProcessToSequencedProcessList" << endl;
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
		cout << "targetProcessIndex: " << targetProcessIndex << endl;

	#if(dbg >= 2)
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

/*#define dbg 1
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

#define dbg 2
int ComboDataInt::addOutputConnectionsToDataReadyList(string processName)
{

#if(dbg >= 1)
	cout << "***** ENTERING: ComboDataInt::addOutputConnectionsToDataReadyList" << endl;
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

Json::Value ComboDataInt::mergeConnections(Json::Value srcConn, Json::Value destConn)
{
	Json::Value mergedConnection;

	mergedConnection["srcEffect"] = srcConn["srcEffect"];
	mergedConnection["srcProcess"] = srcConn["srcProcess"];
	mergedConnection["srcPort"] = srcConn["srcPort"];

	mergedConnection["destEffect"] = destConn["destEffect"];
	mergedConnection["destProcess"] = destConn["destProcess"];
	mergedConnection["destPort"] = destConn["destPort"];


	return mergedConnection;
}

#define dbg 1
bool ComboDataInt::compareConnectionsSrc2Dest(Json::Value srcConn, Json::Value destConn)
{
	bool result = false;

#if(dbg >= 1)
	cout << "COMPARING: ";
	cout << "srcConn: " << srcConn["srcEffect"] << "/" << srcConn["srcProcess"] << ":" << srcConn["srcPort"] << ">"
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

	return result;
}


int ComboDataInt::transferConnection(Json::Value conn, vector<Json::Value> *srcConnArray, vector<Json::Value> *destConnArray)
{
	int status = 0;



	return status;
}


#define dbg 1
int ComboDataInt::getCombo(char *comboName)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::getCombo" << endl;
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
    			this->effectComboJson.clear();

    			boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
    	#if(dbg >= 1)
    			cout << "getting combo index" << endl;
    	#endif
    			string compCombo = this->effectComboJson["name"].asString();
    			for(std::vector<string>::size_type i = 0; i < comboNameList.size(); i++)
    			{
    				if(comboNameList.at(i).compare(compCombo)==0)
    				{
    					comboIndex = i;
    					break;
    				}
    			}
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



#if(dbg==1)
 /*   printf("combo:%s, process:%s, parent effect:%s\n", this->effectComboJson["name"].asString().c_str(),
			this->effectComboJson["processes"][0]["name"].asString().c_str(),
			this->effectComboJson["processes"][0]["parentEffect"]["name"].asString().c_str());*/
#endif
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::getCombo: " << status << endl;
#endif

    //return this->effectComboJson;
	return status;
}


#define dbg 2
int ComboDataInt::getPedalUi(void)
{
    int status = 0;
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
#if(dbg>=1)
    	printf("title: %s\n", this->pedalUiJson["title" ].asString().c_str());
#endif


    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamArrayIndex = 0;
			this->pedalUiJson["effects"][effectIndex]["abbr"] = tempEffects[effectIndex]["abbr"];
			this->pedalUiJson["effects"][effectIndex]["name"] = tempEffects[effectIndex]["name"];
#if(dbg>=1)
			printf("\teffect: %s\t%s\n", this->pedalUiJson["effects"][effectIndex]["abbr"].asString().c_str(),
					this->pedalUiJson["effects"][effectIndex]["name"].asString().c_str());
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
#if(dbg>=1)
           			printf("\t\tparam: %s:\t%s\t%s\t%s\t%s\n", this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"].asString().c_str(),
            					this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"].asString().c_str(),
            					this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"].asString().c_str(),
    							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"].asString().c_str(),
    							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["type"].asString().c_str());
#endif
        			}

       			effectParamArrayIndex++;
       			absParamArrayIndex++;
        		}
        	}
        }
        cout << "effectParamArrayIndex: " << effectParamArrayIndex << endl;
#if(dbg>=1)
    	printf("param count: %d\n", this->pedalUiJson["effects"][0]["params"].size());
#endif
    	status = 0;//return this->pedalUiJson;
    }
    else status = 1;//return NULL;


    return status;
}

#define dbg 1
int ComboDataInt::getConnections(void)
{
    int status = 0;
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

#if(dbg==1)
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

#if(dbg==1)
        	cout << "proc2procConnections: " << proc2procConnections[i]["srcProcess"] << ":" <<
        			proc2procConnections[i]["srcPort"] << ">" <<
					proc2procConnections[i]["destProcess"] << ":" <<
					proc2procConnections[i]["destPort"] << endl;
#endif
        }


        int srcIndex = 0;
        int destIndex = 0;
#if(dbg==1)
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

#if(dbg==1)
        		cout << "proc2fxConnections[" << proc2fxIndex << "] contains a src process " << endl;
#endif
        		tempProc2fxConnection = proc2fxConnections[proc2fxIndex];
        		proc2fxConnections.erase(proc2fxConnections.begin() + proc2fxIndex);
        		proc2fxIndex--;
#if(dbg >= 1)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
#if(dbg >= 1)
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
#if(dbg >= 1)
							cout << "merged tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
							//				if resulting tempProc2fxConnection connects original tempProc2fxConnection to another process or system port, move it to mergedConnections
							if((tempProc2fxConnection["srcEffect"].empty() || tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(tempProc2fxConnection["destEffect"].empty() || tempProc2fxConnection["destEffect"].compare("system") == 0))
							{
								mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg >= 1)
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
#if(dbg >= 1)
        		cout << "proc2fxConnections[" << proc2fxIndex << "] contains a dest process " << endl;
#endif
        		tempProc2fxConnection = proc2fxConnections[proc2fxIndex];
        		proc2fxConnections.erase(proc2fxConnections.begin() + proc2fxIndex);
        		proc2fxIndex--;
#if(dbg >= 1)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif

#if(dbg >= 1)
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

	#if(dbg >= 1)
							cout << "merged tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
	#endif
							//				if resulting tempProc2fxConnection connects original tempProc2fxConnection to another process or system port, move it to mergedConnections
							if((tempProc2fxConnection["srcEffect"].empty() || tempProc2fxConnection["srcEffect"].compare("system") == 0) &&
									(tempProc2fxConnection["destEffect"].empty() || tempProc2fxConnection["destEffect"].compare("system") == 0))
							{
								mergedConnections.push_back(tempProc2fxConnection);
								exit = true;
#if(dbg >= 1)
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
#if(dbg >= 1)
        		cout << "tempProc2fxConnection: " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
        				<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << endl;
#endif
#if(dbg >= 1)
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
#if(dbg >= 1)
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
#if(dbg >= 1)
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
#if(dbg >= 1)
							cout << "moving " << tempProc2fxConnection["srcEffect"] << "/" << tempProc2fxConnection["srcProcess"] << ":" << tempProc2fxConnection["srcPort"] << ">"
									<< tempProc2fxConnection["destEffect"] << "/" << tempProc2fxConnection["destProcess"] << ":" << tempProc2fxConnection["destPort"] << " to mergedConnections. "<< endl;
#endif
								break;
							}
						}
					}
                }
        }

        /*while(fxConnections.size() != prevSize)
        {
			if(prevSize == fxConnections.size())
			{
				break;
			}
			prevSize = fxConnections.size();

#if(dbg==1)
        	cout<<"fxConnections size: "<< fxConnections.size()<<endl;
#endif
        	refresh = false;
            for(int i = 0; i < fxConnections.size(); i++) // source index
            {
            	if(refresh == true || exit == true) break;
        		for(int j = 0; j < fxConnections.size(); j++) // destination index
            	{
//        			bool processMatch = fxConnections[j]["srcProcess"].asString().compare(fxConnections[i]["destProcess"].asString())==0
//        					&& fxConnections[j]["srcEffect"].empty() && fxConnections[i]["destEffect"].empty();
//        			bool effectMatch = fxConnections[j]["srcEffect"].asString().compare(fxConnections[i]["destEffect"].asString())==0
//        					&& fxConnections[j]["srcProcess"].empty() && fxConnections[i]["destProcess"].empty();
//        			bool portMatch = fxConnections[j]["srcPort"].asString().compare(fxConnections[i]["destPort"].asString())==0;
//        			// find connections to be merged by matching dest connection of source "wire" with src connection of destination "wire"
//        			if((processMatch || effectMatch) && portMatch)
        			if(this->compareConnectionsSrc2Dest(fxConnections[i], fxConnections[j]))
            		{
#if(dbg==1)
                		cout << "***** MATCH ******"<<endl;
#endif
                    	Json::Value mergedConn = this->mergeConnections(fxConnections[i], fxConnections[j]);;
//                    	mergedConn["srcEffect"] = fxConnections[i]["srcEffect"];
//                    	mergedConn["srcProcess"] = fxConnections[i]["srcProcess"];
//
//                    	mergedConn["srcPort"] = fxConnections[i]["srcPort"];
//
//                    	mergedConn["destEffect"] = fxConnections[j]["destEffect"];
//                    	mergedConn["destProcess"] = fxConnections[j]["destProcess"];
//
//                    	mergedConn["destPort"] = fxConnections[j]["destPort"];

#if(dbg==1)
                		cout << "MERGING: "<< fxConnections[i]["srcProcess"]<<"/"<<fxConnections[i]["srcEffect"]<<":"<<fxConnections[i]["srcPort"]
                				<<">"<<fxConnections[i]["destProcess"]<<"/"<<fxConnections[i]["destEffect"]<<":"<<fxConnections[i]["destPort"]

                				<<"\t"<<fxConnections[j]["srcProcess"]<<"/"<<fxConnections[j]["srcEffect"]<<":"<<fxConnections[j]["srcPort"]
        						<<">"<<fxConnections[j]["destProcess"]<<"/"<<fxConnections[j]["destEffect"]<<":"<<fxConnections[j]["destPort"]<<endl;

                		cout << mergedConn["srcEffect"] << "/" << mergedConn["srcProcess"] <<":"<<mergedConn["srcPort"]<<">"<< mergedConn["destEffect"] << "/" << mergedConn["destProcess"] <<":"<<mergedConn["destPort"]<<endl;
#endif

                		fxConnections.erase (fxConnections.begin()+i); // delete first half of merged connection
                		if(j>i) j--;

                		fxConnections.erase (fxConnections.begin()+j); // delete second half of merged connection
                		fxConnections.push_back(mergedConn);
                		refresh = true;
            		}
            	}
            }
        }*/

       /* for(int i = 0; i < fxConnections.size(); i++)
        {
        	Json::Value mergedConn;
        	if(fxConnections[i]["srcEffect"].empty() == false)
        		mergedConn["srcProcess"] = fxConnections[i]["srcEffect"];
        	else
        		mergedConn["srcProcess"] = fxConnections[i]["srcProcess"];

        	mergedConn["srcPort"] = fxConnections[i]["srcPort"];

        	if(fxConnections[i]["destEffect"].empty() == false)
        		mergedConn["destProcess"] = fxConnections[i]["destEffect"];
        	else
        		mergedConn["destProcess"] = fxConnections[i]["destProcess"];

        	mergedConn["destPort"] = fxConnections[i]["destPort"];

#if(dbg==1)
        	cout << mergedConn["srcProcess"]<<":"<<mergedConn["srcPort"]<<">"
        			<<mergedConn["destProcess"]<<":"<<mergedConn["destPort"]<<endl;
#endif
        	this->unsequencedConnectionListJson.push_back(mergedConn);
        }*/



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

#if(dbg==1)
        	cout << mergedConn["srcProcess"]<<":"<<mergedConn["srcPort"]<<">"
        			<<mergedConn["destProcess"]<<":"<<mergedConn["destPort"]<<endl;
#endif
        	this->unsequencedConnectionListJson.push_back(mergedConn);
        }
#if(dbg==1)
        cout << "FINAL UNSORTED CONNECTIONS: " << endl;
        this->printUnsequencedConnectionList();
#endif
    }
    else status = 1;//return NULL;

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
        	cout << "procInput[" << procInputIndex << "]: " << procInputs[procInputIndex].procName << ":" << procInputs[procInputIndex].procInput << endl;
        	Json::Value inputConn;
        	// For each input, find the connection that directly feeds into it.
        	cout << "PROCESS CONNECTIONS:" << endl;

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

#if(dbg >= 1)
        for(std::vector<Json::Value>::size_type mergedConnIndex = 0; mergedConnIndex < mergedConnections.size(); mergedConnIndex++)
        {
        	cout << "mergedConnections[" << mergedConnIndex << "]: ";
			cout << mergedConnections[mergedConnIndex]["srcEffect"] << "/";
			cout << mergedConnections[mergedConnIndex]["srcProcess"] << ":";
			cout << mergedConnections[mergedConnIndex]["srcPort"] << ">";
			cout << mergedConnections[mergedConnIndex]["destEffect"] << "/";
			cout << mergedConnections[mergedConnIndex]["destProcess"] << ":";
			cout << mergedConnections[mergedConnIndex]["destPort"] << endl;
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
#endif

    }
    else status = -1;//return NULL;


#if(dbg >= 1)
	cout << "***** EXITING: ComboDataInt::getConnections2: " << status << endl;
#endif
	return status;
}

#define dbg 2
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


    /****************** Create unsortedParameterArray *******************************/
    if(this->effectComboJson.isNull() == false)
    {
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
    #if(dbg >= 1)
        cout << "getting effects" << endl;
    #endif
        for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
        {
        	effectParamIndex = 0;
        	effectString = tempEffects[effectIndex]["name"].asString();
        	Json::Value tempProcs = tempEffects[effectIndex]["processArray"];
            int tempProcCount = tempProcs.size();
            procCount += tempProcCount;
    #if(dbg >= 1)
            cout << "getting processes" << endl;
    #endif

        	for(int procIndex = 0; procIndex < tempProcCount; procIndex++)
        	{
            	procString = tempProcs[procIndex]["name"].asString();
            	Json::Value params = tempProcs[procIndex]["paramArray"];
    #if(dbg >= 1)
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
       #if(dbg==1)
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
		/*for(int effectIndex = 0; (effectIndex < effectCount); effectIndex++)
		{
			effectParamIndex = 0;
			effectString = tempEffects[effectIndex]["name"].asString();

			Json::Value tempProcesses = tempEffects[effectIndex]["processArray"];
			int tempProcessCount = tempProcesses.size();

        	for(int processIndex = 0; processIndex < tempProcessCount; processIndex++)
        	{
        		Process tempProcess;

        		int paramArraySize = tempProcesses[processIndex]["paramArray"].size();
        		tempProcess.name = tempProcesses[processIndex]["name"].asString();
        		//tempProcess.parentEffect = effectString;
        		tempProcess.type = tempProcesses[processIndex]["type"].asString();

        		for(int paramArrayIndex = 0; paramArrayIndex < paramArraySize; paramArrayIndex++)
        		{
        			ProcessParams tempParam;
           			tempParam.name = tempProcesses[processIndex]["paramArray"][paramArrayIndex]["name"].asString();
        			try
        			{
        				tempParam.value = tempProcesses[processIndex]["paramArray"][paramArrayIndex]["value"].asInt();
       			}
        			catch(std::exception &e)
        			{
        				tempParam.value = atoi(tempProcesses[processIndex]["paramArray"][paramArrayIndex]["value"].asString().c_str());
       			}
        			tempProcess.params.push_back(tempParam);
        		}
        		this->processesStruct.push_back(tempProcess);
        	}
        }*/


        /************** Sort processes ********************************/
        bool sequencingStart = true;
        if(status == 0)
        {
            try
            {
    			cout << "fillUnsequencedProcessList" << endl;
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
							break;
						}
					}
					//if(process.length() > 1)
					{
						if(this->addOutputConnectionsToDataReadyList(process) != 0)
						{
							status = -1;
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
#if(dbg >= 1)
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

#define dbg 1
int ComboDataInt::getControls(void)
{
	int status = 0;

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

	this->printControlList();
	this->printControlParameterList();
	this->printUnsortedParameters();

    if(this->effectComboJson.isNull() == false)
    {
    	Json::Value tempEffects = this->effectComboJson["effectArray"];
        effectCount = tempEffects.size();
    #if(dbg >= 1)
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
    #if(dbg >= 1)
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
       #if(dbg==1)
        			cout << "[" << effectIndex << "]:" << effectString << "[" << controlIndex << "]:" << controlString << "[" << paramIndex << "]:" << paramString << ":" << paramArrayIndex << endl;
        #endif
        			effectParamIndex++;
        			absParamIndex++;
        			this->controlParameterArray.push_back(tempParam);
        		}
        		absControlIndex++;
        	}
        }
    	this->printControlParameterList();
    	this->printUnsortedParameters();

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


	this->printControlList();
	this->printControlParameterList();


	int controlCount = this->controlsStruct.size();
	int connCount = this->controlConnectionsStruct.size();
	// *******************  enter the absolute parameter indexes of parameters controlled by a parameter controller

	//loop through controlsStruct vector
	controlCount = this->controlsStruct.size();
	for(int controlIndex = 0; controlIndex < controlCount; controlIndex++)
	{
		//for each controlsStruct, get the name
		this->controlsStruct[controlIndex].absProcessParameterIndexes.clear();
		//loop through controlConnectionsStruct vector
		for(int connIndex = 0; connIndex < connCount; connIndex++)
		{
			//for each controlConnectionsStruct, compare the src name to the controlsStruct name
			if(this->controlConnectionsStruct[connIndex].src.compare(this->controlsStruct[controlIndex].name) == 0)
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
				//enter the parameter index into the target parameter index array in the controlsStruct
				cout << "pushing paramIndex " << paramIndex << " for " << process << ":" << parameter << " into absProcessParameterIndexes" << endl;
				this->controlsStruct[controlIndex].absProcessParameterIndexes.push_back(paramIndex);
			}
		}
	}

	this->printControlList();
    }

	return status;
}



int ComboDataInt::getControlConnections(void)
{
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
    		tempContConn.src = tempControlConnections[controlConnectionIndex]["src"]["name"].asString();
    		tempContConn.dest = tempControlConnections[controlConnectionIndex]["dest"]["name"].asString();
    		this->controlConnectionsStruct.push_back(tempContConn);
    	}
    }

	this->printControlConnectionList();

	return status;
}


#define dbg 0
void ComboDataInt::getProcParameters(int procIndex, int params[10])
{

	int paramsIndex = 0;

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


#define dbg 1
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
#if(dbg==1)
	cout << "tempJsonString: " << tempJsonString << endl;
#endif
	strncpy(this->jsonBuffer, tempJsonString.c_str(), tempJsonString.length());
	writeSize = write(this->comboFD, this->jsonBuffer, strlen(this->jsonBuffer));
#if(dbg==1)
	cerr << "tempJsonString.length(): " << tempJsonString.length() << endl;
	cerr << "strlen(this->jsonBuffer): " << strlen(this->jsonBuffer) << endl;
	cerr << "writeSize: " << writeSize << endl;
#endif
	cout << this->comboWriter.write(this->effectComboJson);


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

	 return status;
}



#define dbg 1
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
	cout << "***** EXITING ComboDataInt::getCombo" << endl;
#endif

}

int ComboDataInt::getProcessIndex(int parameterIndex)
{
	int processIndex = 0;

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

	return processIndex;
}


int ComboDataInt::getControlIndex(string targetProcessName, string targetParameterName)
{
	unsigned int controlIndex = -1;

	//loop through controlConnectionsStruct vector
	for(unsigned int connIndex = 0; connIndex < this->controlConnectionsStruct.size(); connIndex++)
	{
		//	for each controlConnectionsStruct, compare dest process:parameter with targetProcessName and targetParameterName
		string compTarget = targetProcessName + ":" + targetParameterName;
		cout << "comparing: " << this->controlConnectionsStruct[connIndex].dest << " & " << compTarget << endl;
 		if(this->controlConnectionsStruct[connIndex].dest.compare(compTarget) == 0)
		{
 		//	if there is a match, loop through controlsStruct vector
			cout << "MATCH" << endl;
			for(controlIndex = 0; controlIndex < this->controlsStruct.size(); controlIndex++)
			{
			//	for each controlsStruct, compare the control name with the src name contained in controlConnectionsStruct
				if(this->controlConnectionsStruct[connIndex].src.compare(this->controlsStruct[controlIndex].name) == 0)
				{
					//	if there is a match, return the controlsStruct index
					return controlIndex;
				}
			}
		}
	}

	return controlIndex;
}

#define dbg 0
int ComboDataInt::getProcessParameterIndex(string processName, string parameterName)
{
	int paramIndex = -1;
#if(dbg == 1)
	cout << "ENTERING: ComboDataInt::getProcessParameterIndex" << endl;
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
		if(processName.compare(compProcessString) == 0 && parameterName.compare(compParamString) == 0)
		{
			paramIndex = parameterArrayIndex;
		}
	}

#if(dbg == 1)
	cout << "EXIT: ComboDataInt::getProcessParameterIndex" << endl;
#endif
	return paramIndex;
}

#define dbg 0
int ComboDataInt::getControlParameterIndex(string controlName, string parameterName)
{
	int paramIndex = -1;
#if(dbg == 1)
	cout << "ENTERING: ComboDataInt::getControlParameterIndex" << endl;
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
#if(dbg == 1)
	cout << "EXIT: ComboDataInt::getControlParameterIndex" << endl;
#endif

	return paramIndex;
}


int ComboDataInt::updateProcess(int absParamIndex, int valueIndex)
{
	cout << "ENTERING: ComboDataInt::updateProcess" << endl;
	int status = 0;
	this->printProcessParameter(absParamIndex);
	this->sortedParameterArray[absParamIndex].paramValue = valueIndex;

	cout << "EXIT: ComboDataInt::updateProcess" << endl;
	return status;
}


int ComboDataInt::updateControl(int absParamIndex, int valueIndex)
{
	cout << "ENTERING: ComboDataInt::updateControl" << endl;
	int status = 0;
	this->printControlParameter(absParamIndex);
	cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
	this->controlParameterArray[absParamIndex].controlParamValue = valueIndex;

	cout << "EXIT: ComboDataInt::updateControl" << endl;
	return status;
}
