/*
 * DatabaseInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */
#include "config.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"

#define JSON_BUFFER_LENGTH 32000

namespace std
{



	ComboDataInt::ComboDataInt()
	{
		this->comboFD = 0;
		this->processSignalBufferCount = 0;
		this->controlCount = 0;
		this->effectCount = 0;
		this->processCount = 0;
		this->inputSystemBufferIndex[0] = 0;
		this->inputSystemBufferIndex[1] = 1;
		this->outputSystemBufferIndex[0] = 0;
		this->outputSystemBufferIndex[1] = 1;
		this->paramControlBufferCount = 0;
	}

	ComboDataInt::~ComboDataInt()
	{

	}

	void ComboDataInt::printProcessList(bool sorted, vector<Process> processVector)
	{

		{
			if(sorted) cout << "SORTED PROCESSES" << endl;
			else cout << "UNSORTED PROCESSES" << endl;

			for(auto & process : processVector)
			{
				cout << process.processName << endl;
			}
		}
	}

	void ComboDataInt::printProcessStructVector(bool sorted, vector<Process> processVector)
	{

		{
			if(sorted) cout << "SORTED PROCESS STRUCTS" << endl;
			else cout << "UNSORTED PROCESS STRUCTS" << endl;

			for(auto & process : processVector)
			{
				cout << process.processName << endl;
				cout << "inputs" << endl;
				for(auto & input : process.inputVector)
				{
					cout << "\t" << input.objectName << ":" << input.connectedBufferIndex << endl;
				}
				cout << "outputs" << endl;
				for(auto & output : process.outputVector)
				{
					cout << "\t" << output.objectName << ":" << output.connectedBufferIndex << endl;
				}
				cout << "parameters" << endl;
				for(auto & parameter : process.paramVector)
				{
					cout << "\t" << parameter.param.objectName << ":" << parameter.param.connectedBufferIndex;
					cout << "\t" << parameter.valueIndex << endl;
				}
			}
		}
	}

	void ComboDataInt::printProcessSignalConnectionList(bool sorted, vector<ProcessSignalConnection> processSignalConnectionVector)
	{

		{
			if(sorted) cout << "SORTED PROCESS SIGNAL CONNECTIONS: " << endl;
			else cout << "UNSORTED PROCESS SIGNAL CONNECTIONS: " << endl;

			int i = 0;
			for(auto & procConnection : processSignalConnectionVector)
			{
				cout << "ProcessSignalConnectionStructVector[" << i << "]: " << procConnection.src.objectName << ":";
				cout << procConnection.src.portName << ">" << procConnection.dest.objectName << ":";
				cout << procConnection.dest.portName << endl;
				i++;
			}
		}
	}

	void ComboDataInt::printDataReadyList(void)
	{

		{
			cout << "DATA READY LIST:" << endl;

			for(auto & dataReady : this->dataReadyVector)
			{
				cout <<  dataReady.objectName << ":" << dataReady.portName << endl;
			}
		}

	}

	void ComboDataInt::printControlConnectionList(bool sorted, vector<ProcessParameterControlConnection> procParamContConnectVector)
	{

		{
			if(sorted) cout << "SORTED CONTROL CONNECTION LIST: " << endl;
			else cout << "UNSORTED CONTROL CONNECTION LIST: " << endl;
		}


		for(auto & controlConnection : procParamContConnectVector)
		{

			{
				cout << "controlConnection: " << controlConnection.src.objectName << ":";
				cout << controlConnection.src.portName << ">";
				cout << controlConnection.dest.objectName << ":";
				cout << controlConnection.dest.portName << endl;
			}
		}
		 cout << endl;
	}

	void ComboDataInt::printProcBufferList(void)
	{

		{
			cout << "PROCESS BUFFER LIST:" << endl;
			int i = 0;
			for(auto & procSigBuffer : this->processSignalBufferArray)
			{
				cout << "buffer[" << i << "]: " << endl;
				if(procSigBuffer.srcProcess.objectName.empty() == false)
				{
					cout << procSigBuffer.srcProcess.objectName << ":" << procSigBuffer.srcProcess.portName << endl;
					cout << "destProcessVector: " << endl;
					for(auto & destProcess : procSigBuffer.destProcessVector)
					{
						cout << "\t\t" << destProcess.objectName;
						cout << ":" << destProcess.portName << endl;
					}
				}
				else break;
				i++;
			}
			cout << endl;
		}
	}

#define dbg 0
	void ComboDataInt::printContBufferList(void)
	{

		{
			cout << "CONTROL BUFFER LIST:" << endl;
			int i = 0;
			for(auto & procParamContBuffer : this->processParamControlBufferArray)
			{
				cout << "buffer[" << i << "]: " << endl;
				cout << procParamContBuffer.srcControl.objectName << ":" << procParamContBuffer.srcControl.portName << endl;
				cout << "destProcessParamVector: " << endl;
				cout << "\t\t" << procParamContBuffer.destProcessParameter.objectName;
				cout << ":" << procParamContBuffer.destProcessParameter.portName;
				cout << ":" << procParamContBuffer.parameterValueIndex << endl;

				i++;
			}
			cout << endl;
		}


	}

#define dbg 0
	void ComboDataInt::debugPrintParamContBufferListWithConnections()
	{
#if(dbg >= 1)

		{
			cout << " ENTERING ComboDataInt::debugPrintParamContBufferListWithConnections" << endl;
			bool exit = false;
			int paramContBufferIndex = 0;
			for(auto & procParamContBuffer : this->processParamControlBufferArray)
			{
				Control control;
				Process process;
				int valueIndex = procParamContBuffer.parameterValueIndex;
				string controlConnector;
				string processParamConnector;
				Connector processParamConn = procParamContBuffer.destProcessParameter;

				controlConnector = procParamContBuffer.srcControl.objectName + ":" + procParamContBuffer.srcControl.portName;
				processParamConnector = processParamConn.objectName + ":" + processParamConn.portName;

				cout << "[" <<paramContBufferIndex << "] control: " << controlConnector << "\t\t\t\t process: " << processParamConnector << "\t\tvalueIndex: " << valueIndex << endl;

				paramContBufferIndex++;
			}
			cout << " EXITING ComboDataInt::debugPrintParamContBufferListWithConnections" << endl;
		}
#endif
	}

#define dbg 0
	void ComboDataInt::printIndexMappedProcessData()
	{
#if(dbg >= 1)

		{
			cout << "*****ENTERING ComboDataInt::printIndexMappedProcessData" << endl;
			cout << "PROCESS INDEX MAPPING" << endl;
			cout << "size: " << this->processIndexMap.size() << endl;
			for (auto &  processIndexing : this->processIndexMap)
			{

				cout << "PROCESS:" << processIndexing.second.processName << endl;
				cout << "name: " << processIndexing.second.processName << endl;
				cout << "processSequenceIndex: " << processIndexing.second.processSequenceIndex << endl;
				cout << "parentEffect: " << processIndexing.second.parentEffect << endl;


				map<string,ConnectorIndexing>::iterator inputIndexing = processIndexing.second.inputIndexMap.begin();
				cout << "inputIndexMap size: " << processIndexing.second.inputIndexMap.size() << endl;
				for(auto & inputIndexing : processIndexing.second.inputIndexMap)
				{

					cout << "\t\t input portName: " << inputIndexing.second.portName << endl;
					cout << "\t\t input portIndex: " << inputIndexing.second.portIndex << endl;
					cout << "\t\t input connectedBufferIndex: " << inputIndexing.second.connectedBufferIndex << endl;
				}

				map<string, ConnectorIndexing>::iterator outputIndexing = processIndexing.second.outputIndexMap.begin();
				cout << "outputIndexMap size: " << processIndexing.second.outputIndexMap.size() << endl;
				for(auto &  outputIndexing : processIndexing.second.outputIndexMap)
				{

					cout << "\t\t output portName: " << outputIndexing.second.portName << endl;
					cout << "\t\t output portIndex: " << outputIndexing.second.portIndex << endl;
					cout << "\t\t output connectedBufferIndex: " << outputIndexing.second.connectedBufferIndex << endl;
				}

				map<string,ProcessParameterIndexing>::iterator processParamIndexing = processIndexing.second.paramIndexMap.begin();
				cout << "paramIndexMap size: " << processIndexing.second.paramIndexMap.size() << endl;
				for(auto &  processParamIndexing : processIndexing.second.paramIndexMap)
				{
					cout << "\t\t parameter name: " << processParamIndexing.second.paramName << endl;
					cout << "\t\t parameter index: " << processParamIndexing.second.paramIndex << endl;
					cout << "\t\t parentProcess: " << processParamIndexing.second.parentProcess << endl;
					cout << "\t\t parameter paramControlBufferIndex: " << processParamIndexing.second.connectedBufferIndex << endl;
				}
				cout << "**********************************************" << endl;
			}
			cout << "***** EXITING ComboDataInt::printIndexMappedProcessData: "  << endl;
		}

#endif

	}

#define dbg 0
	void ComboDataInt::printIndexMappedControlData()
	{
#if(dbg >= 1)

		{
			cout << "*****ENTERING ComboDataInt::printIndexMappedControlData" << endl;
			cout << "CONTROL INDEX MAPPING" << endl;

			for (auto &  controlIndexing : this->controlIndexMap)
			{
				cout << "CONTROL:" << controlIndexing.second.controlName << endl;
				cout << "name: " << controlIndexing.second.controlName << endl;;
				cout << "index: " << controlIndexing.second.controlIndex << endl;
				cout << "parentEffect: " << controlIndexing.second.parentEffect << endl;
				cout << "conType: " << controlIndexing.second.controlTypeInt << endl;


				cout << "output.portName: " << controlIndexing.second.output.portName << endl;
				cout << "output.portIndex: " << controlIndexing.second.output.portIndex << endl;
				cout << "output.paramContBufferIndex: " << controlIndexing.second.output.connectedBufferIndex << endl;

				cout << "outputInv.portName: " << controlIndexing.second.outputInv.portName << endl;
				cout << "outputInv.portIndex: " << controlIndexing.second.outputInv.portIndex << endl;
				cout << "outputInv.paramContBufferIndex: " << controlIndexing.second.outputInv.connectedBufferIndex << endl;

				cout << "paramIndexMap size: " << controlIndexing.second.paramIndexMap.size() << endl;

				for(auto &  contParamIndexing : controlIndexing.second.paramIndexMap)
				{
					cout << "\t\t contParam.name: " << contParamIndexing.second.contParamName << endl;
					cout << "\t\t contParam.index: " << contParamIndexing.second.contParamIndex << endl;
					cout << "\t\t contParam.parentControl: " << contParamIndexing.second.parentControl << endl;
				}
			}

			cout << "***** EXITING ComboDataInt::printIndexMappedControlData: "  << endl;
		}


#endif
	}

#define dbg 0
	void ComboDataInt::printIndexMappedComboData(void)
	{
#if(dbg >= 1)

		{
			cout << "*****ENTERING ComboDataInt::printIndexMappedComboData" << endl;
			cout << "COMBO INDEX MAPPING" << endl;
			this->printIndexMappedProcessData();

			this->printIndexMappedControlData();
			cout << "***** EXITING ComboDataInt::printIndexMappedComboData: " << endl;
		}
#endif

	}

#define dbg 1
	void ComboDataInt::printPedalUIData()
	{
#if(dbg >= 1)

		{
			cout << "*****ENTERING ComboDataInt::printPedalUIData" << endl;


			cout << getCompactedJSONData(this->pedalUiJson) << endl;
			cout << "***** EXITING ComboDataInt::printPedalUIData " << endl;
		}
#endif

	}

	void ComboDataInt::printEffectComboJsonProcessParameters(Json::Value combo)
	{
		for(auto & effect : combo["effectArray"])
		{
			for(auto & process : effect["processArray"])
			{
				for(auto & parameter : process["paramArray"])
				{
					cout << effect["name"].asString() << ":" << process["name"].asString();
					cout << ":" << parameter["name"].asString() << "=" << parameter["value"].asInt();
					cout << "\t\t\t" << parameter["paramContType"].asString()<< endl;
				}

			}

		}

	}


	void ComboDataInt::printEffectComboJsonControlParameters(Json::Value combo)
	{
		for(auto & effect : combo["effectArray"])
		{
			for(auto & control : effect["controlArray"])
			{
				for(auto & parameter : control["conParamArray"])
				{
					cout << effect["name"].asString() << ":" << control["name"].asString();
					cout << ":" << parameter["alias"].asString() << "=" << parameter["value"].asInt() << endl;
				}
			}
		}
	}


	/**********************************************************************************************************************/
	/**********************************************************************************************************************/
	/**********************************************************************************************************************/
	/**********************************************************************************************************************/




	/*********************** Load Unsorted Struct Vectors ****************************************************/

#define dbg 0
	vector<Process>  ComboDataInt::loadProcessStructVectorFromJsonProcessArray(Json::Value processArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadProcessStructVectorFromJsonProcessArray: " << endl;
#endif
#if(dbg >= 3)
		cout << getCompactedJSONData(processArray) << endl;
#endif
		int status = 0;
		string causeOfException;
		vector<Process> processVector;

		try
		{
			//int processCount = processArray.size();
			for(auto & process : processArray)
			{
				Process tempProcess;

				tempProcess.processName = process["name"].asString();
#if(dbg >= 2)
				cout << "process name: " << tempProcess.processName << endl;
#endif
				tempProcess.processTypeString = process["procType"].asString();

				if(tempProcess.processTypeString.compare("delayb") == 0)
					tempProcess.processTypeInt = 0;
				else if(tempProcess.processTypeString.compare("filter3bb") == 0)
					tempProcess.processTypeInt = 1;
				else if(tempProcess.processTypeString.compare("filter3bb2") == 0)
					tempProcess.processTypeInt = 2;
				else if(tempProcess.processTypeString.compare("lohifilterb") == 0)
					tempProcess.processTypeInt = 3;
				else if(tempProcess.processTypeString.compare("mixerb") == 0)
					tempProcess.processTypeInt = 4;
				else if(tempProcess.processTypeString.compare("volumeb") == 0)
					tempProcess.processTypeInt = 5;
				else if(tempProcess.processTypeString.compare("waveshaperb") == 0)
					tempProcess.processTypeInt = 6;


				tempProcess.parentEffect = process["parentEffect"].asString();

				try
				{
					tempProcess.footswitchNumber = process["footswitchNumber"].asUInt();
				}
				catch(std::exception &e)
				{
					tempProcess.footswitchNumber =  static_cast<int>(strtol(process["footswitchNumber"].asString().c_str(),NULL,10));
				}


				tempProcess.footswitchType = process["footswitchType"].asString();


				Json::Value inputArray = process["inputArray"];
				int inputArraySize = inputArray.size();
				tempProcess.inputCount = inputArraySize;
				vector<Connector> tempInputArray(inputArraySize); // need temporary array to use indexing.
				for(auto & input : inputArray)
				{
					causeOfException = "input index";
					int inputArrayIndex = input["index"].asUInt();
					tempInputArray[inputArrayIndex].objectName = tempProcess.processName;
					tempInputArray[inputArrayIndex].portName = input["name"].asString();
#if(dbg >= 2)
					cout << "input name: " << tempInputArray[inputArrayIndex].portName << endl;
#endif
					tempInputArray[inputArrayIndex].portIndex = inputArrayIndex;
					tempInputArray[inputArrayIndex].connectedBufferIndex = 58; // dummy input buffer
				}
				for(auto & input : tempInputArray)
				{
					tempProcess.inputVector.push_back(input);
				}

				Json::Value outputArray = process["outputArray"];
				int outputArraySize = outputArray.size();
				tempProcess.outputCount = outputArraySize;
				vector<Connector> tempOutputArray(outputArraySize);
				for(auto & output : outputArray)
				{
					causeOfException = "output index";
					int outputArrayIndex = output["index"].asUInt();
					tempOutputArray[outputArrayIndex].objectName = tempProcess.processName;
					tempOutputArray[outputArrayIndex].portName =  output["name"].asString();
#if(dbg >= 2)
					cout << "output name: " << tempOutputArray[outputArrayIndex].portName << endl;
#endif
					tempOutputArray[outputArrayIndex].portIndex = outputArrayIndex;
					tempOutputArray[outputArrayIndex].connectedBufferIndex = 59; // dummy output buffer
				}
				for(auto & output : tempOutputArray)
				{
					tempProcess.outputVector.push_back(output);
				}

				Json::Value paramArray = process["paramArray"];
				int paramArraySize = paramArray.size();
				tempProcess.paramCount = paramArraySize;
				vector<ProcessParameter> tempParamArray(paramArraySize);
				for(auto & parameter : paramArray)
				{
					causeOfException = "parameter index";
					int paramArrayIndex = parameter["index"].asUInt();
					tempParamArray[paramArrayIndex].param.portName = parameter["name"].asString();
					tempParamArray[paramArrayIndex].param.objectName = tempProcess.processName;

					tempParamArray[paramArrayIndex].param.portIndex = paramArrayIndex;
					tempParamArray[paramArrayIndex].param.connectedBufferIndex = 59; //"dummy" buffer index for

					tempParamArray[paramArrayIndex].paramControlType = parameter["paramContType"].asString();

					try
					{
						tempParamArray[paramArrayIndex].paramType = parameter["type"].asInt();
					}
					catch(std::exception &e)
					{
						tempParamArray[paramArrayIndex].paramType =  atoi(parameter["type"].asString().c_str());
					}

					try
					{
						tempParamArray[paramArrayIndex].valueIndex = parameter["value"].asInt();
					}
					catch(std::exception &e)
					{
						tempParamArray[paramArrayIndex].valueIndex =  atoi(parameter["value"].asString().c_str());
					}
#if(dbg >= 2)
					cout << "parameter name: " << tempParamArray[paramArrayIndex].param.portName << ":";
					cout << tempParamArray[paramArrayIndex].valueIndex << endl;
#endif
				}
				for(auto & parameter : tempParamArray)
				{
					tempProcess.paramVector.push_back(parameter);

				}

				processVector.push_back(tempProcess);
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadProcessStructVectorFromJsonProcessArray: " << causeOfException << ":" << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadProcessStructVectorFromJsonProcessArray: " << status << endl;
#endif

		return processVector;
	}
#define dbg 0
	vector<ProcessSignalConnection>  ComboDataInt::loadProcessConnectionStructVectorFromJsonConnectionArray(Json::Value connectionArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadProcessConnectionStructVectorFromJsonConnectionArray" << endl;
#endif
		 /*Description	Resource	Path	Location	Type
		 variable ‘status’ set but not used [-Wunused-but-set-variable]	ComboDataInt.cpp	/FlxMain	line 556	C/C++ Problem
*/

		int status = 0;


#if(dbg >= 3)
		getCompactedJSONData(connectionArray);
#endif
		vector<ProcessSignalConnection> processConnVector;

		try
		{
			for(auto & connection : connectionArray)
			{
				ProcessSignalConnection tempProcConnection;

				tempProcConnection.src.objectName = connection["src"]["object"].asString();
				tempProcConnection.src.portName = connection["src"]["port"].asString();
				tempProcConnection.src.portIndex = 0;
				tempProcConnection.src.connectedBufferIndex = 59;
				tempProcConnection.dest.objectName = connection["dest"]["object"].asString();
				tempProcConnection.dest.portName = connection["dest"]["port"].asString();
				tempProcConnection.dest.portIndex = 0;
				tempProcConnection.dest.connectedBufferIndex = 59;
				processConnVector.push_back(tempProcConnection);
			}

		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadProcessConnectionStructVectorFromJsonConnectionArray: " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadProcessConnectionStructVectorFromJsonConnectionArray: " << status << endl;
#endif

		return processConnVector;
	}

#define dbg 0
	vector<Control>  ComboDataInt::loadControlStructVectorFromJsonControlArray(Json::Value controlArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadControlStructVectorFromJsonControlArray" << endl;
#endif

#if(dbg >= 3)
		getCompactedJSONData(controlArray);
#endif
		int status = 0;
		vector<Control> controlVector;

		try
		{

			vector<Control> tempContVector(controlArray.size());	// need temporary array to use indexing.

			for(auto & control : controlArray)
			{


				int controlIndex = control["index"].asUInt();
				tempContVector[controlIndex].name = control["name"].asString();

				tempContVector[controlIndex].index = controlIndex;
				tempContVector[controlIndex].parentEffect = control["parentEffect"].asString();
				tempContVector[controlIndex].conType = control["conType"].asString();

				Json::Value output = control["outputArray"][0];
				tempContVector[controlIndex].output.portName = output["name"].asString();
				tempContVector[controlIndex].output.portIndex = output["index"].asUInt();
				tempContVector[controlIndex].output.connectedBufferIndex = 59;

				Json::Value outputInv = control["outputArray"][1];
				tempContVector[controlIndex].outputInv.portName = outputInv["name"].asString();
				tempContVector[controlIndex].outputInv.portIndex = outputInv["index"].asUInt();
				tempContVector[controlIndex].outputInv.connectedBufferIndex = 59;


				vector<ControlParameter> paramVector;
				Json::Value contParamArray = control["conParamArray"];
				int contParamCount = contParamArray.size();
				vector<ControlParameter> tempContParamVector(contParamCount);

				for(auto & controlParam : contParamArray)
				{
					int contParamIndex = controlParam["index"].asUInt();

					tempContParamVector[contParamIndex].index = contParamIndex;
					tempContParamVector[contParamIndex].name = controlParam["alias"].asString();
					tempContParamVector[contParamIndex].alias = controlParam["alias"].asString();
					tempContParamVector[contParamIndex].abbr = controlParam["abbr"].asString();
					tempContParamVector[contParamIndex].paramType = controlParam["type"].asUInt();
					tempContParamVector[contParamIndex].controlledParamType = controlParam["controlledParamType"].asUInt();
					tempContParamVector[contParamIndex].inheritControlledParamType = controlParam["inheritControlledParamType"].asBool();
					tempContParamVector[contParamIndex].cvEnabled = controlParam["controlVoltageEnabled"].asBool();
					tempContParamVector[contParamIndex].valueIndex = controlParam["value"].asInt();

				}
				for(auto & controlParam : tempContParamVector)
				{
					tempContVector[controlIndex].paramVector.push_back(controlParam);
				}

			}
			for(auto & control : tempContVector)
			{
#if(dbg >= 2)

#endif
				controlVector.push_back(control);
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadControlStructVectorFromJsonControlArray: " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadControlStructVectorFromJsonControlArray: " << status << endl;
#endif

		return controlVector;
	}

#define dbg 0
	vector<ProcessParameterControlConnection>  ComboDataInt::loadControlConnectionStructVectorFromJsonControlConnectionArray(Json::Value controlConnectionArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadControlConnectionStructVectorFromJsonControlConnectionArray" << endl;
#endif
		int status = 0;
#if(dbg >= 3)
		getCompactedJSONData(controlConnectionArray);
#endif
		vector<ProcessParameterControlConnection> controlConnVector;

		try
		{
			//int controlConnectionCount = controlConnectionArray.size();
			for(auto & controlConnection : controlConnectionArray)
			{
				ProcessParameterControlConnection tempContConnection;

				tempContConnection.src.objectName = controlConnection["src"]["object"].asString();
				tempContConnection.src.portName = controlConnection["src"]["port"].asString();
				tempContConnection.src.portIndex = 0;
				tempContConnection.src.connectedBufferIndex = 59;
				tempContConnection.dest.objectName = controlConnection["dest"]["object"].asString();
				tempContConnection.dest.portName = controlConnection["dest"]["port"].asString();
				tempContConnection.dest.portIndex = 0;
				tempContConnection.dest.connectedBufferIndex = 59;


				controlConnVector.push_back(tempContConnection);
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadControlConnectionStructVectorFromJsonControlConnectionArray: " << e.what() <<  endl;
			status = -1;

		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadControlConnectionStructVectorFromJsonControlConnectionArray: " << status << endl;
#endif

		return controlConnVector;
	}

	/****************** Loading struct vectors from effectComboJson arrays *************************************/

#define dbg 1
	int ComboDataInt::loadEffectComboJsonFromFile(string comboName)
	{
#if(dbg >= 1)
		cout << "*****ENTERING ComboDataInt::loadEffectComboJsonFromFile" << endl;
		cout << "comboName: " << comboName << endl;
#endif
		int status = 0;
		bool boolStatus = true;
		string effectString;
		string currentEffectString;
		string procString;
		string paramString;
		this->comboName = comboName;
#if(dbg >= 2)
		cout << "reading JSON file into jsonString." << endl;
#endif

		try
		{
			clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
			clearBuffer(this->fileNameBuffer,20);
			// open combo file
			strncpy(this->fileNameBuffer, comboName.c_str(), 19);
			sprintf(this->fileNamePathBuffer,"/home/Combos/%s.txt", this->fileNameBuffer);
			this->comboFD = open(fileNamePathBuffer,O_RDONLY);
			// read file into temp string
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
#if(dbg >= 3)
						cout << "file needed cleaning, so replacing with cleaned file" << endl;
						cout << "new file: " << this->jsonBuffer << endl;
#endif
						close(this->comboFD);

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
#if(dbg >= 2)
						cout << "getting combo index" << endl;
#endif

#if(dbg >= 2)
						getCompactedJSONData(this->effectComboJson);
#endif

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
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::loadEffectComboJsonFromFile: " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		cout << "***** EXITING ComboDataInt::loadEffectComboJsonFromFile: " << status << endl;
#endif


		return status;
	}


	int ComboDataInt::loadEffectComboJson(string comboData)
	{
#if(dbg >= 1)
		cout << "*****ENTERING ComboDataInt::loadEffectComboJson" << endl;

#endif
		int status = 0;
		bool boolStatus = true;
		string effectString;
		string currentEffectString;
		string procString;
		string paramString;

#if(dbg >= 2)
		cout << "reading JSON file into jsonString." << endl;
#endif

		try
		{
			strncpy(this->jsonBuffer,comboData.c_str(),JSON_BUFFER_LENGTH);
#if(dbg >= 1)
			cout << "parsing jsonString in effectComboJson" << endl;

#endif
			int result = validateJsonBuffer(this->jsonBuffer);
			if(result == 1)
			{
				if(this->effectComboJson.empty() == false)
				{
					this->effectComboJson.clear();
				}

				boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
#if(dbg >= 2)
				cout << "getting combo index" << endl;
#endif
				this->comboName = this->effectComboJson["name"].asString();
				cout << "comboName: " << this->comboName << endl;
#if(dbg >= 2)
				getCompactedJSONData(this->effectComboJson);
#endif

				if(boolStatus == false)
				{
					status = -1;
					cout << "JSON parse failed." << endl;
				}

			}
			else if(result == 0) // file needed to cleaned, so replacing file
			{
#if(dbg >= 3)
				cout << "file needed cleaning, so replacing with cleaned file" << endl;
				cout << "new file: " << this->jsonBuffer << endl;
#endif
				close(this->comboFD);

				this->comboFD = open(fileNamePathBuffer, O_WRONLY|O_CREAT|O_TRUNC, 0666);

				if(write(this->comboFD,this->jsonBuffer,strlen(this->jsonBuffer)) == -1)
				{
					clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
					cout << "error writing jsonBufferString back to combo file." << endl;
				}
			}
			else
			{
					status = -1;
					cout << "JSON parse failed." << endl;

			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::loadEffectComboJson: " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		cout << "***** EXITING ComboDataInt::loadEffectComboJson: " << status << endl;
#endif


		return status;

	}


#define dbg 1
	int  ComboDataInt::loadComboJsonFileStructFromEffectComboJson(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadComboJsonFileStructFromEffectComboJson" << endl;
#endif
		int status = 0;

		try
		{
			this->comboFileStruct.name = this->effectComboJson["name"].asString();
			if(this->comboFileStruct.name.empty() == false)
			{
				this->comboName = this->comboFileStruct.name;
			}

			cout << "comboName: " << this->comboName << endl;
			vector<Control> tempContStructVector;
			vector<ProcessParameterControlConnection> tempContConnStructVector;
			Json::Value effectArray = this->effectComboJson["effectArray"];
			for(auto & effect : this->effectComboJson["effectArray"]) // effects having indexing, but effect order has never been an issue
			{

				int effectIndex = effect["index"].asInt();
				this->comboFileStruct.effectArray[effectIndex].name = effect["name"].asString();
				this->comboFileStruct.effectArray[effectIndex].abbr = effect["abbr"].asString();
				this->comboFileStruct.effectArray[effectIndex].index = effectIndex;

				Json::Value processArray = effect["processArray"];
				this->comboFileStruct.effectArray[effectIndex].processVector =
								this->loadProcessStructVectorFromJsonProcessArray(processArray);

				Json::Value connectionArray = effect["connectionArray"];
				this->comboFileStruct.effectArray[effectIndex].processConnectionVector =
								this->loadProcessConnectionStructVectorFromJsonConnectionArray(connectionArray);

				Json::Value controlArray = effect["controlArray"];
				this->comboFileStruct.effectArray[effectIndex].processParamControlVector =
								this->loadControlStructVectorFromJsonControlArray(controlArray);


				Json::Value controlConnectionArray = effect["controlConnectionArray"];
				this->comboFileStruct.effectArray[effectIndex].processParamControlConnectionVector =
								this->loadControlConnectionStructVectorFromJsonControlConnectionArray(controlConnectionArray);
			}



			Json::Value effectConnectionArray = this->effectComboJson["effectConnectionArray"];

			for(auto & effectConnect : effectConnectionArray)
			{
				EffectConnection effectConnection;
				effectConnection.src.objectName = effectConnect["src"]["object"].asString();
				effectConnection.src.portName = effectConnect["src"]["port"].asString();

				effectConnection.dest.objectName = effectConnect["dest"]["object"].asString();
				effectConnection.dest.portName = effectConnect["dest"]["port"].asString();


				this->comboFileStruct.effectConnectionVector.push_back({effectConnection.src, effectConnection.dest});
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadComboJsonFileStructFromEffectComboJson: " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadComboJsonFileStructFromEffectComboJson: " << status << endl;
#endif


		return status;
	}

#define dbg 0
	int ComboDataInt::loadUnsortedProcessStructVectorFromComboJsonFileStruct(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadUnsortedProcessStructVectorFromComboJsonFileStruct" << endl;
#endif
		int status = 0;
		this->unsortedProcessStructVector.clear();

		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				//int processCount = effect.processVector.size();

				for(auto & process : effect.processVector)
				{
					this->unsortedProcessStructVector.push_back(process);
				}
			}
#if(dbg >= 2)
			this->printProcessStructVector(false,this->unsortedProcessStructVector);
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadUnsortedProcessStructVectorFromComboJsonFileStruct: " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadUnsortedProcessStructVectorFromComboJsonFileStruct: " << status << endl;
#endif
		return status;
	}


	int ComboDataInt::loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct" << endl;
#endif
		int status = 0;
		this->unmergedProcessSubconnectionStructVector.clear();

		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				vector<ProcessSignalConnection> procConnVector =
								effect.processConnectionVector;
				//int procConnCount = procConnVector.size();
				for(auto & processSubConnection : procConnVector)
				{
					this->unmergedProcessSubconnectionStructVector.push_back(processSubConnection);
				}

				vector<EffectConnection> effectConnVector = this->comboFileStruct.effectConnectionVector;

				for(auto & effectConnection : effectConnVector)
				{
					this->unmergedProcessSubconnectionStructVector.push_back({effectConnection.src, effectConnection.dest});
				}
			}

		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct: " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct: " << status << endl;
#endif
		return status;
	}

	/************************* Internal data processing *********************************/


#define dbg 0
	int ComboDataInt::mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector(void)
	{

#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector" << endl;
#endif
		int status = 0;

		struct _procInput {
			string procName;
			string procInput;
		};


		if(this->effectComboJson.isNull() == false)
		{
			try
			{
				this->unsortedProcessConnectionStructVector.clear();

				std::vector<ProcessSignalConnection> procConnectionsStruct;
				std::vector<Connector> procInputs;

				std::vector<ProcessSignalConnection> mergedConnections;
				ProcessSignalConnection tempProcConnection;  // used for merging subconnections which are then transferred
				// to mergedConnections.
				mergedConnections.clear();
				procInputs.clear();

				Effect tempEffects[2] = this->comboFileStruct.effectArray;
				this->effectCount = 2;

				// ASSUMPTION: EACH PROCESS INPUT WILL BE CONNECTED TO ONLY ONE OUTPUT.


				// ************ Gather inputs for all processes ********************
				Connector tempProcInput;
				tempProcInput.objectName = string("system");
				tempProcInput.portName = string("playback_1");
				procInputs.push_back(tempProcInput);
				tempProcInput.objectName = string("system");
				tempProcInput.portName = string("playback_2");
				procInputs.push_back(tempProcInput);


				for(auto & effect : this->comboFileStruct.effectArray)
				{
					vector<Process> tempProcVector = effect.processVector;
					//int procCount = tempProcVector.size();
					for(auto & process : effect.processVector)
					{
						vector<Connector> tempInputVector = process.inputVector;
						for(auto & processInput : process.inputVector)
						{
							tempProcInput.objectName = process.processName;
							tempProcInput.portName = processInput.portName;
							procInputs.push_back(tempProcInput);
						}
					}
				}


				for(auto & input : procInputs)
				{
#if(dbg >=3)
					 cout << "procInput[" << procInputIndex << "]: " << procInputs[procInputIndex].objectName << ":" << procInputs[procInputIndex].portName << endl;
					 cout << "PROCESS CONNECTIONS:" << endl;
#endif
					Json::Value inputConn;
					// For each input, find the connection that directly feeds into it.

					for(auto & unmergedConnection : this->unmergedProcessSubconnectionStructVector)
					{
						if(this->compareConnectors(false, unmergedConnection.dest, input) == true)
						{
							tempProcConnection = unmergedConnection;
							bool exit = false;

#if(dbg >= 3)
							 cout << "BASE CONNECTION FOUND....." << endl;
							 cout << tempProcConnection.src.objectName << ":";
							 cout << tempProcConnection.src.portName << ">";
							 cout << tempProcConnection.dest.objectName << ":";
							 cout << tempProcConnection.dest.portName << endl;
							 cout << "************************************" << endl;
#endif
							for(int loopNum = 0; loopNum < 5 && exit == false; loopNum++) // loop multiple times to make sure
								// all subconnections were merged
							{
								size_t connIndex = 0;
								for(auto & unmergedConnection : this->unmergedProcessSubconnectionStructVector)
								{
#if(dbg >= 3)
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].src.objectName << ":";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].src.portName << ">";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].dest.objectName << ":";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].dest.portName << endl;
#endif

									if(this->compareConnectors(false, unmergedConnection.dest, tempProcConnection.src) == true)
									{
										tempProcConnection = this->mergeConnections(unmergedConnection,tempProcConnection);
										exit = (tempProcConnection.src.objectName.find("(") == string::npos) || (tempProcConnection.src.objectName.compare("system") == 0); // object is process output or "system"
										if(exit)
										{
											this->unsortedProcessConnectionStructVector.push_back(tempProcConnection);
										}
#if(dbg >= 3)
										 cout << "MATCH....." << endl;
										 cout << tempProcConnection.src.objectName << ":";
										 cout << tempProcConnection.src.portName << ">";
										 cout << tempProcConnection.dest.objectName << ":";
										 cout << tempProcConnection.dest.portName << endl;
#endif

										break;
									}
									else if((tempProcConnection.src.objectName.find("(") == string::npos) || (tempProcConnection.src.objectName.compare("system") == 0))
										// original connection can't be merged further
									{
										this->unsortedProcessConnectionStructVector.push_back(tempProcConnection);
										exit = true;
										break;
									}
									else if(connIndex == this->unmergedProcessSubconnectionStructVector.size()-1) // no matches found, so no connections exist
									{
										exit = true;
										break;
									}
									connIndex++;
								}
							}
							break;
						}
					}
				}

#if(dbg >= 2)
				this->printProcessSignalConnectionList(false,this->unsortedProcessConnectionStructVector);
#endif

			}
			catch (exception &e)
			{
				 cout << "exception in ComboDataInt::mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector: " << e.what() <<  endl;
				status = -1;
			}
		}
		else status = -1;

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector: " << status << endl;
#endif
		return status;
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
			size_t processIndex = 0;
			for(auto & unsortedProcess : this->unsortedProcessStructVector)
			{
#if(dbg >=2)
				 cout << "comparing: " << processName << " & " << this->unsortedProcessStructVector[processIndex].paramName << endl;
#endif

				if(processName.compare(unsortedProcess.processName) == 0)
				{
					targetProcessIndex = processIndex;
					break;
				}
				if(processIndex == this->unsortedProcessStructVector.size() - 1) //end of connection array reach, but no match found
				{
					targetProcessIndex = -1;
				}
				processIndex++;
			}
		}
		catch(std::exception &e)
		{
			 cout << "exception in ComboDataInt::getTargetProcessIndex: " << e.what() <<  endl;
		}
#if(dbg >= 2)
		if(targetProcessIndex >= 0)
			 cout << "target process index: " << targetProcessIndex << ":" << this->unsortedProcessStructVector[targetProcessIndex].paramName << endl;
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
		int  targetIndex = this->getTargetProcessIndex(processName);

		for(auto & input : this->unsortedProcessStructVector[targetIndex].inputVector)
		{
			inputs.push_back(input.portName);
		}

#if(dbg >= 2)
		for(auto & effectConnection : effectConnVector)
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
	std::vector<string> ComboDataInt::getFirstProcesses()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::getFirstProcesses" << endl;
#endif
		Connector start;
		start.objectName = "system";
		start.portName = "capture_1";

		string firstProcess;
		std::vector<string> firstProcesses;

		try
		{
			for(auto & unsortedProcConn : this->unsortedProcessConnectionStructVector)
			{
	#if(dbg >= 2)
				 cout << "comparing: ";
				 cout << start.objectName << " vs " << unsortedProcConn.src.objectName;
				 cout << "\t and \t";
				 cout << start.portName << " vs " << unsortedProcConn.src.portName << endl;
	#endif
				if((start.objectName.compare(unsortedProcConn.src.objectName) == 0)  &&
								(start.portName.compare(unsortedProcConn.src.portName) == 0))
				{
					firstProcess = unsortedProcConn.dest.objectName;
					if(std::find(firstProcesses.begin(),firstProcesses.end(),firstProcess)==firstProcesses.end()) // procName not in nextProcesses
					{
						firstProcesses.push_back(firstProcess);
					}

				}
			}

	#if(dbg >= 2)
			for(std::vector<Connector>::size_type i = 0; i < this->dataReadyVector.size(); i++)
			{
				 cout << "dataReadyVector[" << i <<"]: " << dataReadyVector[i].objectName << ":" << dataReadyVector[i].portName << endl;
			}
	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::getFirstProcesses: " << e.what() << endl;
		}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::getFirstProcesses: ";
		for(std::vector<string>::size_type i = 0; i < firstProcesses.size(); i++)
		{
			 cout << firstProcesses[i] << ", ";
		}
		 cout << endl;

#endif

		return firstProcesses;
	}

#define dbg 0
	std::vector<string> ComboDataInt::getNextProcesses()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::getNextProcesses" << endl;
#endif
		std::vector<string> nextProcesses;
		bool foundNextProcesses = false;

		try
		{
			for(int inputSatisfactionLevel = 2; (inputSatisfactionLevel > 0) && (foundNextProcesses == false); inputSatisfactionLevel--)
			{
				for(auto & unsortedProcess : this->unsortedProcessStructVector)
				{
					string procName = unsortedProcess.processName;
					if(this->areAllProcessInputsSatisfied(procName) == inputSatisfactionLevel) // all inputs satisfied.  Check for this first.
					{
						if(std::find(nextProcesses.begin(),nextProcesses.end(),procName)==nextProcesses.end()) // procName not in nextProcesses
						{
							nextProcesses.push_back(procName);
							foundNextProcesses = true;
						}
					}
				}
			}

	#if(dbg >= 2)
			int i = 0;
			for(auto & unsortedProcess this->unsortedProcessStructVector)
			{
				 cout << "unsortedProcessStructVector[" << i <<"]: " << unsortedProcess.name << endl;
				i++;
			}

	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::getNextProcesses: " << e.what() << endl;
		}


#if(dbg >= 1)

		{
			cout << "***** EXITING: ComboDataInt::getNextProcesses: ";
			for(auto & nextProcess : nextProcesses)
			{
				cout << nextProcess << ", ";
			}
			cout << endl;
		}

#endif

		return nextProcesses;
	}



#define dbg 0
	int ComboDataInt::areAllProcessInputsSatisfied(string processName)
	{
		/*	status = 2: all inputs satisfied
		 * status = 1: some inputs satisfied
		 * status = 0: no inputs satisfied
		 */
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::areAllProcessInputsSatisfied" << endl;
		 cout << "processName: " << processName << endl;
#endif

		int status = 0;
		try
		{
			std::vector<string> inputs = getProcessInputs(processName);
			std::vector<ProcessSignalConnection> connections;
			int inputPortCount = inputs.size();
			int inputPortSatisfiedCount = 0;
#if(dbg >= 2)
		this->printUnsortedConnectionList();
#endif
		// Get all connections connecting to the inputs of process "processName"
		for(auto & input : inputs)
		{
#if(dbg >= 2)
			 cout << "input port: " << processName << ":" << inputs[inputPortIndex] << endl;
#endif

			for(auto & unsortedProcConnect : this->unsortedProcessConnectionStructVector)
			{

				if((unsortedProcConnect.dest.objectName.compare(processName) == 0) &&
								(unsortedProcConnect.dest.portName.compare(input) == 0))
				{
#if(dbg >= 2)
					Connection conn = this->unsortedProcessConnectionStructVector[connIndex];


					{
						cout << "unsorted connection: " << conn.src.objectName << ":" << conn.src.portName;
						cout << ">" << conn.dest.objectName << ":" << conn.dest.portName << endl;
					}

#endif
					connections.push_back(unsortedProcConnect);
				}
			}
		}

		//int connectionsCount = connections.size();

		for(auto & connection : connections)
		{
			for(auto & dataReadyConn : this->dataReadyVector)
			{
#if(dbg >= 2)
				 cout << "comparing: " << connections[connectionsIndex].src.objectName << ":" << connections[connectionsIndex].src.portName << " to " << this->dataReadyVector[dataReadyListIndex].objectName << ":" << this->dataReadyVector[dataReadyListIndex].portName << endl;
#endif

				if(this->compareConnectors(false, dataReadyConn, connection.src) == true)
				{
#if(dbg >= 2)
					 cout << "inputPortSatisfied: " << connections[connectionsIndex].src.objectName << ":" << connections[connectionsIndex].src.portName << endl;
#endif
					inputPortSatisfiedCount++;
				}
			}
		}

		if(inputPortSatisfiedCount == inputPortCount) status = 2;
		else if(inputPortSatisfiedCount > 0) status = 1;

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::areAllProcessInputsSatisfied: " << e.what() << endl;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::areAllProcessInputsSatisfied:" << status << endl;
#endif

		return status;
	}

#define dbg 0
	int ComboDataInt::addOutputConnectionsToDataReadyVector(vector<string> processNames)
	{

#if(dbg >= 1)

		{
			cout << "***** ENTERING: ComboDataInt::addOutputConnectionsToDataReadyVector" << endl;
			for(auto & name : processNames)
			{
				cout << name << ", ";
			}
			cout << endl;
		}
#endif
		int status = 0;

		try
		{
			for(auto & processName : processNames)
			{
				int procIndex = this->getTargetProcessIndex(processName);

				if(procIndex >= 0)
				{
					for(auto & output : this->unsortedProcessStructVector[procIndex].outputVector)
					{
						Connector tempConn;
						tempConn.objectName = processName;
						tempConn.portName = output.portName;
						if(this->isOutputInDataReadyVector(tempConn) == false)
							this->dataReadyVector.push_back(tempConn);
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
			 cout << "exception in getTargetProcessIndex(processName) section " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 2)
		this->printDataReadyList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::addOutputConnectionsToDataReadyVector: " << status << endl;
#endif
		return status;
	}



	int ComboDataInt::transferProcessStructsToSortedProcessStructVector(vector<string> processNames)
	{
#if(dbg >= 1)

		{

		}
		cout << "***** ENTERING: ComboDataInt::transferProcessStructsToSortedProcessStructVector" << endl;
		for(auto & name : processNames)
		{
			cout << name << ", ";
		}
#endif
		int status = 0;

		try
		{
			for(auto & processName : processNames)
			{
				int  targetProcessIndex = 0;
				// get index for target process
				targetProcessIndex = this->getTargetProcessIndex(processName);
				if(targetProcessIndex >= 0)
				{
					this->sortedProcessStructVector.push_back(this->unsortedProcessStructVector[targetProcessIndex]);
					this->unsortedProcessStructVector.erase(this->unsortedProcessStructVector.begin() + targetProcessIndex);
				}
				else
				{
					status = -1;
					break;
				}
#if(dbg >= 2)

				{
					cout << "targetProcessIndex: " << targetProcessIndex << endl;
					cout << "unsorted processes: " << endl;
					for(auto & unsortedProcess : unsortedProcessStructVector)
					{
						cout << unsortedProcess.name << endl;
					}

					cout << "sorted processes: " << endl;
					for(auto & sortedProcess : sortedProcessStructVector)
					{
						cout << sortedProcess.name << endl;
					}

				}

#endif
			}
		}
		catch(std::exception &e)
		{
			 cout << "exception in transferProcessStructsToSortedProcessStructVector " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::transferProcessStructsToSortedProcessStructVector: " << status << endl;
#endif
		return status;
	}


#define dbg 0
	bool ComboDataInt::isUnsortedProcessStructVectorEmpty()
	{
		bool isListEmpty;

#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::isUnsequencedProcessStructVectorEmpty" << endl;
#endif

		if(this->unsortedProcessStructVector.size() == 0) isListEmpty = true;
		else isListEmpty = false;
#if(dbg >= 2)
		 cout << "unsequenced processes: " << endl;
		this->printUnsequencedProcessList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::isUnsequencedProcessStructVectorEmpty: " << isListEmpty << endl;
#endif
		return isListEmpty;
	}

#define dbg 0
	bool ComboDataInt::isOutputInDataReadyVector(Connector output)
	{
		bool inList = false;
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::isOutputInDataReadyVector" << endl;
		 cout << "Connector process: " << output.objectName << "\t port: " << output.portName << endl;
#endif
			try
			{
				for(auto & dataReadyConn : this->dataReadyVector)
				{
					string listedOutputProcess = dataReadyConn.objectName;
					string listedOutputPort = dataReadyConn.portName;
					if(output.objectName.compare(listedOutputProcess) == 0 && output.portName.compare(listedOutputPort) == 0)
					{
						inList = true;
						break;
					}
				}

			}
			catch(exception &e)
			{
				cout << "exception in ComboDataInt::isOutputInDataReadyVector: " << e.what() << endl;
			}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::isOutputInDataReadyVector: " << inList << endl;
#endif

		return inList;
	}


#define dbg 0
	ProcessSignalConnection ComboDataInt::mergeConnections(ProcessSignalConnection srcConn, ProcessSignalConnection destConn)
	{
		ProcessSignalConnection mergedConnection;
#if(dbg >= 1)

		{
			cout << "ComboDataInt::mergeConnections" << endl;
			cout << "srcConn: " << srcConn.src.objectName << ":" << srcConn.src.portName << ">"
							<< srcConn.dest.objectName << ":" << srcConn.dest.portName << " & ";
			cout << "destConn: " << destConn.src.objectName << ":" << destConn.src.portName << ">"
							<< destConn.dest.objectName << ":" << destConn.dest.portName << endl;
		}
#endif

		mergedConnection.src.objectName = srcConn.src.objectName;
		mergedConnection.src.portName = srcConn.src.portName;

		mergedConnection.dest.objectName = destConn.dest.objectName;
		mergedConnection.dest.portName = destConn.dest.portName;

#if(dbg >= 1)

		{
			cout << "***** EXITING: ComboDataInt::mergeConnections: ";
			cout << "mergeConnection: " << mergedConnection.src.objectName << ":" << mergedConnection.src.portName << ">"
							<< mergedConnection.dest.objectName << ":" << mergedConnection.dest.portName << endl;
		}
#endif
		return mergedConnection;
	}



	/****************** Sort unsorted struct-vectors into sorted struct-vectors***********************************/

#define dbg 0
	int ComboDataInt::sortUnsortedProcessStructsIntoSortedProcessStructVector()
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::sortUnsortedProcessStructsIntoSortedProcessStructVector" << endl;
#endif
		int status = 0;
		int  breakLoopCount = 0;
		bool sequencingStart = true;
		string process;
		vector<string> processes;
		string effectString;
		string currentEffectString;

		string procString;
		string paramString;

		try
		{
#if(dbg>=2)
			 cout << "loadUnsortedProcessStructVectorFromComboJsonFileStruct" << endl;
#endif
			this->dataReadyVector.clear();

			Connector tempConn;
			tempConn.objectName = string("system");
			tempConn.portName = string("capture_1");
			this->dataReadyVector.push_back(tempConn);
			tempConn.objectName = string("system");
			tempConn.portName = string("capture_2");
			this->dataReadyVector.push_back(tempConn);
			this->loadUnsortedProcessStructVectorFromComboJsonFileStruct();
		}
		catch(std::exception &e)
		{
			 cout << "exception in fillUnsequencedProcessVector section: " << e.what() <<  endl;
			status = -1;
		}


		this->sortedProcessStructVector.clear();
		try
		{
			breakLoopCount = 0;
			while(this->isUnsortedProcessStructVectorEmpty() == false)
			{
				if(sequencingStart)
				{
					processes = this->getFirstProcesses();
					if(processes.empty() == true)
					{
						status = -1;
						 cout << "first process string vector is blank." << endl;
						break;
					}
					else
					{
						sequencingStart = false;
					}
				}
				else
				{
					processes = this->getNextProcesses();
					if(processes.empty() == true)
					{
						status = -1;
						 cout << "next process string vector is empty." << endl;
						break;
					}
				}
				if(this->addOutputConnectionsToDataReadyVector(processes) != 0)
				{
					status = -1;
					 cout << "addOutputConnectionsToDataReadyVector failed." << endl;
					break;
				}

				if(this->transferProcessStructsToSortedProcessStructVector(processes) != 0)
				{
					status = -1;
					break;
				}

				if(breakLoopCount++ >= 100)
				{
					status = -1;
					 cout << "Couldn't sort processes." << endl;
					break;
				}
			}

			if(status == 0)
			{
				this->processCount = this->sortedProcessStructVector.size();
			}

#if(dbg >= 2)
			this->printSortedProcessList();

#endif
		}
		catch(std::exception &e)
		{
			 cout << "exception in sortUnsortedProcessStructsIntoSortedProcessStructVector: " << e.what() <<  endl;
			return -1;
		}

		// Create IndexedProcessParameter vector here
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::sortUnsortedProcessStructsIntoSortedProcessStructVector: " << status << endl;
#endif

		return status;
	}


#define dbg 0
	int ComboDataInt::sortUnsortedConnectionStructsIntoSortedConnectionStructVector()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::sortUnsortedConnectionStructsIntoSortedConnectionStructVector" << endl;
#endif

		try
		{
			while(this->unsortedProcessConnectionStructVector.empty() == false)
			{
				int dataReadyIndex = 0;
				for(auto & dataReadyConn : this->dataReadyVector)
				{
					int connIndex = 0;
					for(auto connection = this->unsortedProcessConnectionStructVector.begin();
									connection <  this->unsortedProcessConnectionStructVector.end(); connection++)
					{
						if(this->compareConnectors(false, dataReadyConn,connection->src) == true)
						{
							this->sortedProcessConnectionStructVector.push_back(*connection);
							this->unsortedProcessConnectionStructVector.erase(connection);
						}
						connIndex++;
					}
				}
				dataReadyIndex++;
			}
#if(dbg >= 2)
	this->printSortedConnectionList();
#endif

		}
		catch(std::exception &e)
		{
			 cout << "exception in sortUnsortedConnectionStructsIntoSortedConnectionStructVector: " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::sortUnsortedConnectionStructsIntoSortedConnectionStructVector: " << status << endl;
#endif
		return status;
	}

#define dbg 0
	int ComboDataInt::loadSortedControlStructVectorFromComboJsonFileStruct(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadSortedControlStructVectorFromComboJsonFileStruct" << endl;
#endif
		int absControlIndex = 0;
		int effectIndex = 0;
		this->sortedControlStructVector.clear();
		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				string effectName = effect.name;

				for(auto & processParamControl : effect.processParamControlVector)
				{
					processParamControl.index = absControlIndex;
					processParamControl.parentEffectIndex = effectIndex;
					this->sortedControlStructVector.push_back(processParamControl);
					absControlIndex++;
				}
				effectIndex++;
			}
#if(dbg >= 2)
			this->printSortedControlList();
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in loadSortedControlStructVectorFromComboJsonFileStruct: " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadSortedControlStructVectorFromComboJsonFileStruct: " << status << endl;
#endif
		return status = 0;
	}


#define dbg 0
	int ComboDataInt::loadSortedControlConnectionStructVectorFromComboJsonFileStruct(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::loadSortedControlConnectionStructVectorFromComboJsonFileStruct" << endl;
#endif
		string process;
		string effectString;
		string currentEffectString;

		string procString;
		string paramString;


		this->sortedControlConnectionStructVector.clear();
		try
		{
#if(dbg >= 1)
			 cout << "getting effects" << endl;
#endif
			for(auto & effect : this->comboFileStruct.effectArray)
			{
				for(auto & processParamControlConnection : effect.processParamControlConnectionVector)
				{
					this->sortedControlConnectionStructVector.push_back(processParamControlConnection);
				}
			}
		}
		catch(exception &e)
		{
			 cout << "exception in loadSortedControlConnectionStructVectorFromComboJsonFileStruct: " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 2)
		this->printControlConnectionList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::loadSortedControlConnectionStructVectorFromComboJsonFileStruct: " << status << endl;
#endif
		return status;
	}


	/******************** Set interconnections ***********************************/

#define dbg 0
	int ComboDataInt::setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::setProcBufferArrayOutputAndInputConnectionsUsingSortedConnectionStructVector" << endl;
#endif
		int status = 0;
		try
		{
			this->processSignalBufferCount = this->dataReadyVector.size();
	#if(dbg >= 3)
			this->printDataReadyList();
			this->printSortedConnectionList();
	#endif
			int procSigBufferIndex = 0;
			for(auto & dataReady : this->dataReadyVector)
			{
				// set buffer source process to object in dataReadyVector. This could be a process output or
				// a system output (i.e. capture_1 or capture_2)
				this->processSignalBufferArray[procSigBufferIndex].srcProcess = dataReady;
				procSigBufferIndex++;
			}

			for(auto & procSignalBuffer : this->processSignalBufferArray)
			{

				// for getting inputs, loop through sortedProcessConnectionStructVector inside  processSignalBufferArray loop.
				// Compare processSignalBufferArray.srcProcess connection with src connection in sortedProcessConnectionStructVector, and if a
				// match if found push sortedProcessConnectionStructVector dest connection into processSignalBufferArray
				// destProcessVector.


				for(auto & processConnection : this->sortedProcessConnectionStructVector)
				{
					if(compareConnectors(false, procSignalBuffer.srcProcess,processConnection.src) == true)
					{
	#if(dbg >= 4)
						cout << "match found: " <<  this->processSignalBufferArray[procSignalBufferIndex].srcProcess.objectName;
						cout << ":" << this->processSignalBufferArray[procSignalBufferIndex].srcProcess.portName << endl;
						cout << "pushing " << this->sortedProcessConnectionStructVector[connIndex].dest.objectName;
						cout << ":" << this->sortedProcessConnectionStructVector[connIndex].dest.portName;
						cout << " into procBufferArray[bufferIndex].destProcessVector" << endl;
	#endif
	#if(dbg >= 2)
						cout << "pushing " << processConnection.dest.objectName;
						cout << "into  procSignalBuffer.destProcessVector" << procSignalBuffer.srcProcess.objectName  << endl;
	#endif
						procSignalBuffer.destProcessVector.push_back(processConnection.dest);

					}

				}

			}
	#if(dbg >= 2)
			this->printProcBufferList();
	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector: " << e.what() << endl;
		}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::setProcBufferArrayOutputAndInputConnectionsUsingSortedConnectionStructVector: " << status << endl;
#endif

		return status;
	}

#define dbg 0
	int ComboDataInt::setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray" << endl;
#endif
		int status = 0;
		// loop through sortedProcessStructVector
		try
		{
			Connector systemPlayback[2];
			systemPlayback[0].objectName = "system";
			systemPlayback[1].objectName = "system";
			systemPlayback[0].portName = "playback_1";
			systemPlayback[1].portName = "playback_2";
			Connector systemCapture[2];
			systemCapture[0].objectName = "system";
			systemCapture[1].objectName = "system";
			systemCapture[0].portName = "capture_1";
			systemCapture[1].portName = "capture_2";

			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				//loop through procBufferArray
				int signalProcessBufferIndex = 0;
				for(auto & signalProcessBuffer : processSignalBufferArray)
				{

					//loop through procBuffer:destProcessVector inside sortedProcessStruct:inputVector loop
					//and, if a match is found between sortedProcess name and procBuffer:destProcess:object
					//, and sortedProcess:input:portName and procBuffer:destProcess:port, then
					//set the sortedProcess:input:connectedBufferIndex to procBufferIndex
					// and exit procBuffer:destProcessVector loop.

					for(auto & procInput : sortedProcess.inputVector)
					{
						for(auto & destProcess : signalProcessBuffer.destProcessVector)
						{
							if(this->compareConnectors(false, destProcess,procInput) == true)
							{
								procInput.connectedBufferIndex = signalProcessBufferIndex;
								break;
							}
						}
					}

					//loop through sortedProcessStructVector outputVector and, if a match is found between
					//sortedProcess name and procBuffer:srcProcess:object, and
					//sortedProcess:output:portName and procBuffer:srcProcess:port, then
					//set the sortedProcess:output:connectedBufferIndex to procBufferIndex.

					for(vector<Connector>::size_type procOutputIndex = 0; procOutputIndex < sortedProcess.outputVector.size(); procOutputIndex++)
					{
						Connector outConn;
						outConn.objectName = sortedProcess.processName;
						outConn.portName = sortedProcess.outputVector[procOutputIndex].portName;

						if(this->compareConnectors(false, outConn,signalProcessBuffer.srcProcess) == true)
						{
							sortedProcess.outputVector[procOutputIndex].connectedBufferIndex =
											signalProcessBufferIndex;
							break;
						}
						if(this->compareConnectors(false, systemCapture[0],signalProcessBuffer.srcProcess) == true)
						{
							this->inputSystemBufferIndex[0] = signalProcessBufferIndex;
	#if(dbg >= 2)
							cout << "system:capture_1 connected to buffer: " << this->inputSystemBufferIndex[0] << endl;
	#endif
						}
						else if(this->compareConnectors(false, systemCapture[1],signalProcessBuffer.srcProcess) == true)
						{
							this->inputSystemBufferIndex[1] = signalProcessBufferIndex;
	#if(dbg >= 2)
							cout << "system:capture_2 connected to buffer: " << this->inputSystemBufferIndex[1] << endl;
	#endif
						}
					}
					signalProcessBufferIndex++;
				}
			}

			// ****************  Connect system playback inputs ***********************************************
			int processSignalBufferIndex = 0;
			for(auto & processSignalBuffer : this->processSignalBufferArray)
			{
				for(auto & destProcess : processSignalBuffer.destProcessVector)
				{
					if(this->compareConnectors(false, systemPlayback[0],destProcess) == true)
					{

						this->outputSystemBufferIndex[0] = processSignalBufferIndex;
	#if(dbg >= 2)
						cout << "system:playback_1 connected to buffer: " << this->outputSystemBufferIndex[0] << endl;
	#endif

					}
					else if(this->compareConnectors(false, systemPlayback[1],destProcess) == true)
					{

						this->outputSystemBufferIndex[1] = processSignalBufferIndex;
	#if(dbg >= 2)
						cout << "system:playback_2 connected to buffer: " << this->outputSystemBufferIndex[1] << endl;
	#endif

					}
				}
				processSignalBufferIndex++;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray: " << e.what() << endl;
		}

#if(dbg >= 2)
		this->printProcessStructVector(true, this->sortedProcessStructVector);
#endif
#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray: " << status << endl;
#endif
		return status;
	}

#define dbg 0
	int ComboDataInt::setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray" << endl;
#endif
		int status = 0;
		try
		{
			// initialize all buffers to "empty state"
			for(auto & procParamContBuffer : this->processParamControlBufferArray)
			{
				procParamContBuffer.destProcessParameter.objectName = "empty";
				procParamContBuffer.destProcessParameter.portName = "empty";
				procParamContBuffer.destProcessParameter.connectedBufferIndex = 59;
				procParamContBuffer.parameterValueIndex = 0;
				procParamContBuffer.srcControl.objectName = "empty";
				procParamContBuffer.srcControl.portName = "empty";
				procParamContBuffer.srcControl.connectedBufferIndex = 59;
			}

			// First set dest side Connectors to include all parameters for all process
			// and set sortedProcessStruct:param:paramContBufferIndex  to paramContBufferIndex

			int processParamControlBufferIndex = 0;

			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				for(auto & parameter : sortedProcess.paramVector)
				{
					this->processParamControlBufferArray[processParamControlBufferIndex].destProcessParameter.objectName =
									sortedProcess.processName;
					this->processParamControlBufferArray[processParamControlBufferIndex].destProcessParameter.portName =
									parameter.param.portName;
					this->processParamControlBufferArray[processParamControlBufferIndex].destProcessParameter.connectedBufferIndex =
									processParamControlBufferIndex;
					parameter.param.connectedBufferIndex =
									processParamControlBufferIndex;
					this->processParamControlBufferArray[processParamControlBufferIndex].parameterValueIndex =
									parameter.valueIndex;

					this->processIndexMap[sortedProcess.processName].paramIndexMap[parameter.param.portName].connectedBufferIndex =
									processParamControlBufferIndex;
					processParamControlBufferIndex++;
				}
			}

			this->paramControlBufferCount = processParamControlBufferIndex;


			processParamControlBufferIndex = 0; // reset buffer index

			// do control connection loop inside a parameter controller buffer loop
			for(auto & processParamControlBuffer : this->processParamControlBufferArray)
			{
				for(auto & sortedControlConnection : this->sortedControlConnectionStructVector)
				{	// if the control connection dest connector matches the destProcessParameter connector,
					// loop through the processParamControlStructVector to match the connection src.
					// When a match is found,set the processParamControlBuffer.srcControl.connectedBufferIndex
					// to the processParamControlBufferIndex and push the outputDest into
					// sortedControlStruct.targetProcessParamVector

					if(this->compareConnectors(false, processParamControlBuffer.destProcessParameter
											   ,sortedControlConnection.dest) == true)
					{
						processParamControlBuffer.srcControl = sortedControlConnection.src;
						int controlIndex = 0;
						for(auto & sortedControlStruct : this->sortedControlStructVector)
						{
							Connector controlOut, controlOutInv;
							controlOut.objectName = sortedControlStruct.name;
							controlOut.portName = sortedControlStruct.output.portName;
							controlOutInv.objectName = sortedControlStruct.name;
							controlOutInv.portName = sortedControlStruct.outputInv.portName;
							if(this->compareConnectors(false, controlOut,processParamControlBuffer.srcControl) == true)
							{
								Connector outputDest = processParamControlBuffer.destProcessParameter;
								outputDest.connectedBufferIndex = processParamControlBufferIndex;
								sortedControlStruct.targetProcessParamVector.push_back(outputDest);
								processParamControlBuffer.srcControl.connectedBufferIndex = processParamControlBufferIndex;
	#if(dbg >= 2)
								cout << "this->sortedControlStructVector[" << controlIndex <<"].targetProcessParamVector.connectedBufferIndex: " << outputDest.objectName << ":" << outputDest.portName << "=" << outputDest.connectedBufferIndex << endl;
	#endif
							}
							if(this->compareConnectors(false, controlOutInv,processParamControlBuffer.srcControl) == true)
							{
								Connector outputInvDest = processParamControlBuffer.destProcessParameter;
								outputInvDest.connectedBufferIndex = processParamControlBufferIndex;
								sortedControlStruct.targetProcessParamInvVector.push_back(outputInvDest);
								processParamControlBuffer.srcControl.connectedBufferIndex = processParamControlBufferIndex;
	#if(dbg >= 2)
								cout << "sortedControlStructVector[" << controlIndex <<"].targetProcessParamVector.connectedBufferIndex: " << outputInvDest.objectName << ":" << outputInvDest.portName << "=" << outputInvDest.connectedBufferIndex << endl;
	#endif
							}
							controlIndex++;
						}
					}
				}
				processParamControlBufferIndex++;
			}


	#if(dbg >= 2)
			this->debugPrintParamContBufferListWithConnections();
			this->printContBufferList();
			this->printControlStructVector(true,this->sortedControlStructVector);
	#endif
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray: " << e.what() << endl;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray: " << status << endl;
#endif

		return status;
	}


	/******************* Set other data ****************************************/

	int ComboDataInt::setProcessAndControlTypeIntsInSortedStructVectors()
	{
		int status = 0;
		try
		{
			int processIndex = 0;
			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				this->sortedProcessStructVector[processIndex].processSequenceIndex = processIndex;
				this->processIndexMap[sortedProcess.processName].processSequenceIndex = processIndex;
				processIndex++;
			}


			//loop through sortedControlStructVector vector
			for(auto & sortedControlStruct : this->sortedControlStructVector)
			{
				//for each sortedControlStructVector, enter the control data into the ControlEvent element
				if(sortedControlStruct.conType.compare("norm") == 0)
				{
					sortedControlStruct.conTypeInt = 0;
				}
				else if(sortedControlStruct.conType.compare("env") == 0)
				{
					sortedControlStruct.conTypeInt = 1;
				}
				else if(sortedControlStruct.conType.compare("lfo") == 0)
				{
					sortedControlStruct.conTypeInt = 2;
				}

			}
		}
		catch(exception &e)
		{
			cout << "exception in : " << e.what() << endl;
			status = -1;
		}


		return status;
	}






#define dbg 0
	int  ComboDataInt::loadProcessIndexMapFromSortedProcessVector()
	{

#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadProcessIndexMapFromSortedProcessVector: " << endl;
#endif
		int status = 0;
		string causeOfException;

		try
		{


			for(auto & process : this->sortedProcessStructVector)
			{
				ProcessIndexing procIndexing;

				procIndexing.processName = process.processName;
				procIndexing.processSequenceIndex = process.processSequenceIndex;
				procIndexing.processTypeInt = process.processTypeInt;
				procIndexing.parentEffect = process.parentEffect;


				for(auto & parameter : process.paramVector)
				{
					causeOfException = "parameter index";
					ProcessParameterIndexing procParamIndexing;

					procParamIndexing.paramName = parameter.param.portName;
					procParamIndexing.paramIndex = parameter.param.portIndex;
					procParamIndexing.parentProcess = process.processName;
					procParamIndexing.connectedBufferIndex = parameter.param.connectedBufferIndex;
					procIndexing.paramIndexMap.insert(pair<string,ProcessParameterIndexing>(procParamIndexing.paramName,procParamIndexing));
				}
				this->processIndexMap.insert(pair<string,ProcessIndexing>(procIndexing.processName,
																		  procIndexing));

			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadProcessIndexMapFromSortedProcessVector: " << causeOfException << ":" << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadProcessIndexMapFromSortedProcessVector: " << status << endl;
#endif

		 return status;
	}


#define dbg 0
	int  ComboDataInt::loadControlIndexMapFromSortedControlVector()
	{
		map<string, ControlIndexing> controlIndexMap;
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadControlIndexMapFromSortedControlVector" << endl;
#endif


		try
		{
			for(auto & control : this->sortedControlStructVector)
			{
				ControlIndexing contIndexing;

				contIndexing.controlName = control.name;
				contIndexing.controlIndex = control.index;
				contIndexing.parentEffect = control.parentEffect;
				contIndexing.controlTypeInt = control.conTypeInt;


				map<string,ProcessControlParameterIndexing> paramIndexMap;

				for(auto & controlParameter : control.paramVector)
				{
					ProcessControlParameterIndexing contParamIndexing;
					contParamIndexing.contParamName = controlParameter.alias;
					contParamIndexing.contParamIndex = controlParameter.index;
					contParamIndexing.parentControl = contIndexing.controlName;
					contIndexing.paramIndexMap.insert(pair<string,ProcessControlParameterIndexing>
					(contParamIndexing.contParamName,contParamIndexing));
				}
				this->controlIndexMap.insert(pair<string,ControlIndexing>(contIndexing.controlName,contIndexing));
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadControlIndexMapFromSortedControlVector: " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadControlIndexMapFromSortedControlVector: " << status << endl;
#endif
		 return status;
	}



	int ComboDataInt::loadIndexMappedComboDataFromSortedVectors(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadIndexMappedComboDataFromEffectComboJson" << endl;
#endif
		int status = 0;


		try
		{
			this->loadProcessIndexMapFromSortedProcessVector();
			this->loadControlIndexMapFromSortedControlVector();
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::loadIndexMappedComboDataFromEffectComboJson: " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadIndexMappedComboDataFromEffectComboJson: " << status << endl;
#endif


		return status;
	}




#define dbg 3
	int ComboDataInt::getPedalUi(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::getPedalUi" << endl;
#endif

		string effectString;
		string currentEffectString;
		string controlString;

		vector<Control>::size_type controlCount = 0;
		string paramString;
		vector<ControlParameter>::size_type  paramIndex = 0;
		int effectParamArrayIndex = 0;

		int absParamArrayIndex = 0;

		try
		{
			if(this->effectComboJson.isNull() == false)
			{
				this->pedalUiJson = Json::Value(); // clear any previous data

				this->pedalUiJson["title" ] = this->comboFileStruct.name;
	#if(dbg>=2)
				 cout << "title: " << this->pedalUiJson["title"].asString() << endl;
	#endif

				int effectIndex = 0;
				for(auto & effect : this->comboFileStruct.effectArray)
				{
					effectParamArrayIndex = 0;
					this->pedalUiJson["effects"][effectIndex]["abbr"] = effect.abbr;
					this->pedalUiJson["effects"][effectIndex]["name"] = effect.name;
	#if(dbg>=2)
					 cout << "\teffect: " << this->pedalUiJson["effects"][effectIndex]["abbr"].asString();
					 cout << "\t" << this->pedalUiJson["effects"][effectIndex]["name"].asString() << endl;
	#endif

	#if(dbg>=2)
					 cout << "\tcontrolCount: " << controlCount << endl;
	#endif

					for(auto & control : effect.processParamControlVector)
					{
						vector<ControlParameter> contParamVector =
										control.paramVector;
						for(auto & parameter : control.paramVector)
						{
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"] =
											parameter.alias;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"] =
											parameter.abbr;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] =
											parameter.valueIndex;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["parentControl"] =
											control.name;
							if(contParamVector[paramIndex].inheritControlledParamType == true)
							{
	#if(dbg>=3)
								 cout << contParamVector[paramIndex].alias << "inheriting controlled parameter: " << contParamVector[paramIndex].paramType << " becoming " << contParamVector[paramIndex].controlledParamType << endl;
	#endif
								this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"] =
												parameter.controlledParamType;
							}
							else
							{
								this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"] =
												parameter.paramType;
							}

							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"] = absParamArrayIndex;
	#if(dbg>=3)

							{
								cout << "\tparam: " << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["index"].asString();
								cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"].asString();
								cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"].asString();
								cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"].asInt();
								cout << "\t" <<	this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"].asInt() << endl;
							}
	#endif
							effectParamArrayIndex++;
							absParamArrayIndex++;
						}
					}
					effectIndex++;
				}
	#if(dbg>=2)
				 cout << "param count: " << this->pedalUiJson["effects"][0]["params"].size() << endl;
	#endif
				status = 0;

			}
			else
			{
				status = -1;
				cout << "effectComboJson is NULL" << endl;
			}

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::getPedalUi: " << e.what() << endl;
		}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::getPedalUi: " << status << endl;
#endif
#if(dbg >= 2)
		 getCompactedJSONData(this->pedalUiJson);
#endif
		return status;
	}






	/************************************************************************************************************************/
	/************************************************************************************************************************/
	/************************************************************************************************************************/
	/************************************************************************************************************************/
	/************************************************************************************************************************/



#define dbg 0
	bool ComboDataInt::compareConnectors(bool display, Connector conn1, Connector conn2)
	{
		bool result = (conn1.objectName.compare(conn2.objectName) == 0 && conn1.portName.compare(conn2.portName) == 0);
		if(display)
		{
			cout << "comparing: " << conn1.objectName << ":" << conn1.portName << " with ";
			cout << conn2.objectName << ":" << conn2.portName << "\t\tresult: " << result << endl;
		}
		return result;
	}



#define dbg 1
	int ComboDataInt::updateControlParameterValuesInComboFileStruct(ComboStruct combo)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::updateControlParameterValuesInComboFileStruct" << endl;
#endif

		try
		{
			string name = combo.name;

#if(dbg >= 2)
			for(auto & effect : this->comboFileStruct.effectArray)
			{
				cout << effect.name << endl;
				for(auto & control : effect.processParamControlVector)
				{
					cout << "\tcontrol: " << control.name << endl;
					for(auto & seqControl : combo.controlSequenceData)
					{
						cout << "\tseqControl: " << seqControl.controlName << endl;
						if(control.name.compare(seqControl.controlName) == 0)
						{
							// Match at the parameter level
							cout << "CONTROL MATCH" << endl;
							for(auto & parameter : control.paramVector)
							{
								cout << "\tparameter: " << parameter.name << endl;
								for(auto & seqParam : seqControl.parameter)
								{
									cout << "\tseqParam: " << seqParam.parameterName << endl;
									if(parameter.alias.compare(seqParam.parameterName) == 0)
									{
										cout << "PARAMETER MATCH" << endl;
										cout << effect.name << ":" << control.name << ":" << parameter.alias << "=" << parameter.valueIndex << endl;
									}
								}
							}
						}
					}
				}
			}
			cout << endl;
#endif
			for(auto & effect : this->comboFileStruct.effectArray)
			{
				cout << effect.name << endl;
				// Match at the control level
				for(auto & control : effect.processParamControlVector)
				{
					for(auto & seqControl : combo.controlSequenceData)
					{
						if(control.name.compare(seqControl.controlName) == 0)
						{
							// Match at the parameter level
							for(auto & parameter : control.paramVector)
							{
								for(auto & seqParam : seqControl.parameter)
								{
									if(parameter.alias.compare(seqParam.parameterName) == 0)
									{
										parameter.valueIndex = seqParam.value;
										cout << effect.name << ":" << control.name << ":" << parameter.alias << "=" << parameter.valueIndex << endl;
									}
								}
							}
						}
					}
				}
			}
#if(dbg >= 2)
			for(auto & effect : this->comboFileStruct.effectArray)
			{
				cout << effect.name << endl;
				for(auto & control : effect.processParamControlVector)
				{
					cout << "\t" << control.name << endl;
					for(auto & seqControl : combo.controlSequenceData)
					{
						cout << "\t" << seqControl.controlName << endl;
						if(control.name.compare(seqControl.controlName) == 0)
						{
							// Match at the parameter level
							for(auto & parameter : control.paramVector)
							{
								cout << "\t" << parameter.name << endl;
								for(auto & seqParam : seqControl.parameter)
								{
									cout << "\t" << seqParam.parameterName << endl;
									if(parameter.alias.compare(seqParam.parameterName) == 0)
									{
										cout << effect.name << ":" << control.name << ":" << parameter.alias << "=" << parameter.valueIndex << endl;
									}
								}
							}
						}
					}
				}
			}
			cout << endl;
#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::updateControlParameterValuesInComboFileStruct:" << e.what() << endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::updateControlParameterValuesInComboFileStruct: " << status << endl;
#endif



		return status;
	}

#define dbg 1
	int  ComboDataInt::loadComboFileStructDataBackIntoEffectComboJson()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadComboFileStructDataBackIntoEffectComboJson" << endl;
#endif
#if(dbg >= 2)
		printEffectComboJsonControlParameters(this->effectComboJson);
#endif

		try
		{
			// EFFECT
			for(auto & effectJson : this->effectComboJson["effectArray"])
			{
				for(auto & effect : this->comboFileStruct.effectArray)
				{
					if(effectJson["name"].asString().compare(effect.name) == 0)
					{
						// CONTROL
						for(auto & controlJson : effectJson["controlArray"])
						{
							for(auto & control : effect.processParamControlVector)
							{
								if(controlJson["name"].asString().compare(control.name) == 0)
								{
									// PARAMETER
									for(auto & parameterJson : controlJson["conParamArray"])
									{
										for(auto & parameter : control.paramVector)
										{
											if(parameterJson["alias"].asString().compare(parameter.alias) == 0)
											{
#if(dbg >= 2)
												cout << "MATCH:" << effect.name << ":" << control.name << ":" << parameter.alias << endl;
#endif
												parameterJson["value"] = parameter.valueIndex;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			cout << endl;

		}
		catch(exception &e)
		{
			cout << "exception in loadComboFileStructDataBackIntoEffectComboJson: " << e.what() << endl;
			status = -1;
		}



#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadComboFileStructDataBackIntoEffectComboJson: " << status << endl;
#endif
#if(dbg >= 2)
		printEffectComboJsonControlParameters(this->effectComboJson);
#endif

		return status;
	}



	//***************************** PUBLIC *************************************

	void ComboDataInt::setProcessUtilityData(ProcessUtility procUtil)
	{
		this->processUtil.bufferSize = procUtil.bufferSize;
		this->processUtil.inputCouplingMode = procUtil.inputCouplingMode;
		this->processUtil.antiAliasingNumber = procUtil.antiAliasingNumber;
		this->processUtil.waveshaperMode = procUtil.waveshaperMode;
	}


	Json::Value ComboDataInt::getPedalUiJson(void)
	{
		return this->pedalUiJson;
	}

	string ComboDataInt::getName()
	{
		return this->effectComboJson["name"].asString();
	}
#define dbg 1
	ComboStruct ComboDataInt::getComboStruct()
	{
		ComboStruct tempCombo;
		ComboStruct nullCombo;
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::getComboStruct" << endl;
		 cout << "comboName: " << this->comboName << endl;
#endif

		try
		{
			tempCombo.controlVoltageEnabled = false;
			tempCombo.processCount = this->processCount;
			tempCombo.controlCount = this->controlCount;
			tempCombo.processSignalBufferCount = this->processSignalBufferCount;
			tempCombo.paramControlBufferCount = this->paramControlBufferCount;

			//int controlParamBufferIndex = 0;
			tempCombo.name = this->comboName;
			 cout << tempCombo.name << ":" << tempCombo.processCount << ":" << tempCombo.controlCount << ":" << tempCombo.processSignalBufferCount << endl;

			// set Processes
			int i = 0;
			int j = 0;
			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				tempCombo.processSequenceData[i].processName = sortedProcess.processName;
				tempCombo.processSequenceData[i].processTypeInt = sortedProcess.processTypeInt;

				tempCombo.processSequenceData[i].footswitchNumber = sortedProcess.footswitchNumber-1;
				tempCombo.processSequenceData[i].processSequenceIndex = sortedProcess.processSequenceIndex;
#if(dbg >= 2)
				 cout << "tempCombo.processSequenceData[" << i << "].processName: " << tempCombo.processSequenceData[i].processName << endl;
				 cout << "tempCombo.processSequenceData[" << i << "].procType: " << tempCombo.processSequenceData[i].processTypeInt << endl;
				 cout << "tempCombo.processSequenceData[" << i << "].footswitchNumber: " << tempCombo.processSequenceData[i].footswitchNumber << endl;
#endif
				//***************** Get input count and store values **************
				tempCombo.processSequenceData[i].processInputCount = sortedProcess.inputCount;
#if(dbg >= 2)
				 cout << "tempCombo.processSequenceData[" << i << "].processInputCount: " << tempCombo.processSequenceData[i].processInputCount << endl;
#endif
				j = 0;
				for(auto & input : sortedProcess.inputVector)
				{
					tempCombo.processSequenceData[i].inputConnectedBufferIndexArray[j] = input.connectedBufferIndex;

#if(dbg >= 2)
					 cout << "tempCombo.processSequenceData[" << i << "].inputConnectedBufferIndexArray[" << j << "]: " << tempCombo.processSequenceData[i].inputConnectedBufferIndexArray[j] << endl;
#endif
					j++;
				}

				//***************** Get output count and store values  **************
				tempCombo.processSequenceData[i].processOutputCount = sortedProcess.outputCount;
#if(dbg >= 2)
				 cout << "tempCombo.processSequenceData[" << i << "].processOutputCount: " << tempCombo.processSequenceData[i].processOutputCount << endl;
#endif
				j = 0;
				for(auto & output : sortedProcess.outputVector)
				{

					tempCombo.processSequenceData[i].outputConnectedBufferIndexArray[j] = output.connectedBufferIndex;
#if(dbg >= 2)
					 cout << "tempCombo.processSequenceData[" << i << "].outputConnectedBufferIndexArray[" << j << "]: " << tempCombo.processSequenceData[i].outputConnectedBufferIndexArray[j] << endl;
#endif
					j++;
				}

				//***************** Get parameter count and store values **************
				tempCombo.processSequenceData[i].parameterCount = sortedProcess.paramCount;
				j = 0;
				for(auto & parameter : sortedProcess.paramVector)
				{
					tempCombo.processSequenceData[i].parameterArray[j].internalIndexValue = parameter.valueIndex;
					tempCombo.processSequenceData[i].parameterArray[j].paramContBufferIndex = parameter.param.connectedBufferIndex;
					tempCombo.processSequenceData[i].parameterArray[j].parameterName = parameter.param.portName;
					if(parameter.paramControlType.compare("none") != 0) //parameter is controlled
					{
						tempCombo.processSequenceData[i].parameterArray[j].controlConnected = true;
					}
					else tempCombo.processSequenceData[i].parameterArray[j].controlConnected = false;
#if(dbg >= 2)
					 cout << "tempCombo.processSequenceData[" << i << "].parameterArray[" << j << "]->";
					 cout << " internalIndexValue: "<< tempCombo.processSequenceData[i].parameterArray[j].internalIndexValue;
					 cout << "\tparamContBufferIndex: " << tempCombo.processSequenceData[i].parameterArray[j].paramContBufferIndex << endl;
#endif
					j++;
				}

				for(; j < 10; j++)
				{
					tempCombo.processSequenceData[i].parameterArray[j].internalIndexValue = 0;
					tempCombo.processSequenceData[i].parameterArray[j].controlConnected = false;
					j++;
				}
				tempCombo.processSequenceData[i].bufferSize = 256;//this->procUtil.bufferSize;
				tempCombo.processSequenceData[i].inputCouplingMode = 1;//this->procUtil.inputCouplingMode;
				tempCombo.processSequenceData[i].antiAliasingNumber = 1;//this->procUtil.antiAliasingNumber;
				tempCombo.processSequenceData[i].waveshaperMode = 0;//this->procUtil.waveshaperMode;


				i++;
			}

			// set Controls
			i = 0;
			for(auto & control : this->sortedControlStructVector)
			{
				tempCombo.controlSequenceData[i].controlName = control.name;
				tempCombo.controlSequenceData[i].conType = control.conTypeInt;
#if(dbg >= 2)
				 cout << "tempCombo.controlSequenceData[" << i << "].name: " << tempCombo.controlSequenceData[i].controlName << endl;
				 cout << "tempCombo.controlSequenceData[" << i << "].conType: " << tempCombo.controlSequenceData[i].conType << endl;
#endif
				{
					j = 0;
					for(auto & outputTarget : control.targetProcessParamVector)
					{
						tempCombo.controlSequenceData[i].outputToParamControlBufferIndex[j] =  outputTarget.connectedBufferIndex;
#if(dbg >= 2)
						 cout << "tempCombo.controlSequenceData[" << i << "].outputToParamControlBufferIndex: " << tempCombo.controlSequenceData[i].outputToParamControlBufferIndex[j] << endl;
#endif
						j++;
					}
					tempCombo.controlSequenceData[i].outputConnectionCount = j;

					j = 0;
					for(auto & outputInvTarget : control.targetProcessParamInvVector)
					{
						tempCombo.controlSequenceData[i].outputInvToParamControlBufferIndex[j] =  outputInvTarget.connectedBufferIndex;
#if(dbg >= 2)
						 cout << "tempCombo.controlSequenceData[" << i << "].outputInvToParamControlBufferIndex: " << tempCombo.controlSequenceData[i].outputInvToParamControlBufferIndex[j] << endl;
#endif
						j++;
					}
					tempCombo.controlSequenceData[i].outputInvConnectionCount = j;
#if(dbg >= 2)
					 cout << "tempCombo.controlSequenceData[" << i << "].outputToParamControlBufferIndex: " << tempCombo.controlSequenceData[i].outputToParamControlBufferIndex << endl;
					 cout << "tempCombo.controlSequenceData[" << i << "].outputInvToParamControlBufferIndex: " << tempCombo.controlSequenceData[i].outputInvToParamControlBufferIndex << endl;
#endif
				}



				tempCombo.controlSequenceData[i].parameterCount = control.paramVector.size();
				int j = 0;
				for(auto & parameter : control.paramVector)
				{
					tempCombo.controlSequenceData[i].parameter[j].parameterName = parameter.alias;
					tempCombo.controlSequenceData[i].parameter[j].value = parameter.valueIndex;
					tempCombo.controlSequenceData[i].parameter[j].cvEnabled = parameter.cvEnabled;
					if(tempCombo.controlSequenceData[i].parameter[j].cvEnabled == true)
					{
						tempCombo.controlVoltageEnabled = true; // used to set analog input mode to DC.
					}
#if(dbg >= 2)

					{
						cout << "tempCombo.controlSequenceData[" << i << "].parameter[" << j << "].value: ";
						cout << tempCombo.controlSequenceData[i].parameter[j].value;
						cout << "\t\ttempCombo.controlSequenceData[" << i << "].parameter[" << j << "].cvEnabled: ";
						cout << tempCombo.controlSequenceData[i].parameter[j].cvEnabled << endl;
					}
#endif
					j++;
				}
				for(; j < 10; j++)
				{
					tempCombo.controlSequenceData[i].parameter[j].value = 0;
					tempCombo.controlSequenceData[i].parameter[j].cvEnabled = false;
				}
#if(dbg >= 2)
				 cout << "tempCombo.controlVoltageEnabled: " << tempCombo.controlVoltageEnabled << endl;
#endif
				i++;
			}

			// set process signal buffering
			i = 0;
			for(auto & procSigBuffer : this->processSignalBufferArray)
			{
				tempCombo.processSignalBufferArray[i].srcProcess = procSigBuffer.srcProcess;
				i++;
			}

			// set parameterControl buffering
			 cout << "set parameterControl buffering: " << this->paramControlBufferCount << endl;
			i = 0;
			for(auto & processParamControlBuffer : this->processParamControlBufferArray)
			{
				{
					tempCombo.processParamControlBufferArray[i].destProcessParameter.objectName =
									processParamControlBuffer.destProcessParameter.objectName;
					tempCombo.processParamControlBufferArray[i].destProcessParameter.portName =
									processParamControlBuffer.destProcessParameter.portName;
					tempCombo.processParamControlBufferArray[i].destProcessParameter.connectedBufferIndex =
									processParamControlBuffer.destProcessParameter.connectedBufferIndex;
					tempCombo.processParamControlBufferArray[i].srcControl.objectName =
									processParamControlBuffer.srcControl.objectName;
					tempCombo.processParamControlBufferArray[i].srcControl.portName =
									processParamControlBuffer.srcControl.portName;
					tempCombo.processParamControlBufferArray[i].srcControl.connectedBufferIndex =
									processParamControlBuffer.srcControl.connectedBufferIndex;
					tempCombo.processParamControlBufferArray[i].parameterValueIndex =
									processParamControlBuffer.parameterValueIndex;


				}
#if(dbg >= 2)
				if(tempCombo.processParamControlBufferArray[i].destProcessParameter.objectName.compare("empty") != 0)
				{
					cout << "destProcessParameter.objectName: " <<
									tempCombo.processParamControlBufferArray[i].destProcessParameter.objectName << endl;
					cout << "destProcessParameter.portName: " <<
									tempCombo.processParamControlBufferArray[i].destProcessParameter.portName << endl;
					cout << "destProcessParameter.connectedBufferIndex: " <<
									tempCombo.processParamControlBufferArray[i].destProcessParameter.connectedBufferIndex << endl;
					cout << "srcControl.objectName: " << tempCombo.processParamControlBufferArray[i].srcControl.objectName << endl;
					cout << "srcControl.portName: " << tempCombo.processParamControlBufferArray[i].srcControl.portName << endl;
					cout << "srcControl.connectedBufferIndex: " <<
									tempCombo.processParamControlBufferArray[i].srcControl.connectedBufferIndex << endl;
					cout << "parameterValueIndex: " << tempCombo.processParamControlBufferArray[i].parameterValueIndex << endl;
				}
#endif
				i++;
			}




			 cout << "set footswitches" << endl;
			for(int i = 0; i < 10; i++)
			{
				tempCombo.footswitchStatus[i] = this->footswitchStatus[i];
			}

			//****************** Copy indexing maps ***************************

			tempCombo.processIndexMap.insert(this->processIndexMap.begin(),
											 this->processIndexMap.end());

			tempCombo.controlIndexMap.insert(this->controlIndexMap.begin(),
											 this->controlIndexMap.end());





			tempCombo.inputSystemBufferIndex[0] = this->inputSystemBufferIndex[0];
#if(dbg >= 2)
			 cout << "tempCombo.inputSystemBufferIndex[0]: " << tempCombo.inputSystemBufferIndex[0] << endl;
#endif
			tempCombo.inputSystemBufferIndex[1] = this->inputSystemBufferIndex[1];
#if(dbg >= 2)
			 cout << "tempCombo.inputSystemBufferIndex[1]: " << tempCombo.inputSystemBufferIndex[1] << endl;
#endif
			tempCombo.outputSystemBufferIndex[0] = this->outputSystemBufferIndex[0];
#if(dbg >= 2)
			 cout << "tempCombo.outputSystemBufferIndex[0]: " << tempCombo.outputSystemBufferIndex[0] << endl;
#endif
			tempCombo.outputSystemBufferIndex[1] = this->outputSystemBufferIndex[1];
#if(dbg >= 2)
			 cout << "tempCombo.outputSystemBufferIndex[1]: " << tempCombo.outputSystemBufferIndex[1] << endl;
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::getComboStruct: " << e.what() << endl;
			status = -1;

		}
		cout << "this->comboFileStruct.name: " << this->comboFileStruct.name << endl;
#if(dbg >= 3)
		this->debugPrintParamContBufferListWithConnections();
		this->printIndexMappedComboData();
#endif
#if(dbg >= 1)
		 cout << "*****EXITING ComboDataInt::getComboStruct" << endl;
#endif

		if(status == 0) return tempCombo;
		else return nullCombo;
	}

	Json::Value ComboDataInt::getEffectComboJson()
	{
		return this->effectComboJson;
	}





#define dbg 1
	int ComboDataInt::loadIndexMappedComboData2(Json::Value comboJson)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::loadIndexMappedComboData2" << endl;
#endif

#if(dbg >= 2)
		 cout << "comboName: " << comboJson["name"].asString() << endl;

#endif
			try
			{
				this->effectComboJson = comboJson;
				if(this->loadComboJsonFileStructFromEffectComboJson() >= 0)
				{
					if(this->loadUnsortedProcessStructVectorFromComboJsonFileStruct() >= 0)
					{
						if((this->loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct() >= 0) &&
										(this->mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector() >= 0))
						{
							if(this->sortUnsortedProcessStructsIntoSortedProcessStructVector() >= 0)
							{
								if(this->sortUnsortedConnectionStructsIntoSortedConnectionStructVector() >= 0)
								{
									if(this->loadSortedControlStructVectorFromComboJsonFileStruct() >= 0)
									{
										if(this->loadSortedControlConnectionStructVectorFromComboJsonFileStruct() >= 0)
										{
											this->loadProcessIndexMapFromSortedProcessVector();
											this->loadControlIndexMapFromSortedControlVector();

											if(this->getPedalUi() >= 0)
											{
		#if(dbg >= 2)
												 cout << "loading combo struct: " << comboName << endl;
		#endif

												string processName;
												string connectionName[20];

		#if(dbg >= 2)
												 cout << "this->processCount: " << this->processCount << endl;
		#endif
												this->controlCount = this->sortedControlStructVector.size();


		#if(dbg >= 2)
												 cout << endl;
		#endif
												this->setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector();
												this->setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray();
												this->setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray();
												this->setProcessAndControlTypeIntsInSortedStructVectors();
		#if(dbg >= 2)
												this->printIndexMappedProcessData();
												this->printIndexMappedControlData();
		#endif
											}
											else
											{
												status = -1;
		#if(dbg >= 2)
												 cout << "getPedalUi failed." << endl;
		#endif
											}
										}
										else
										{
											status = -1;
		#if(dbg >= 2)
											 cout << "loadSortedControlConnectionStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
										}
									}
									else
									{
										status = -1;
		#if(dbg >= 2)
										 cout << "loadSortedControlStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
									}
								}
								else
								{
									status = -1;
		#if(dbg >= 2)
									 cout << "sortUnsortedConnectionStructsIntoSortedConnectionStructVector failed." << endl;
		#endif
								}
							}
							else
							{
								status = -1;
		#if(dbg >= 2)
								 cout << "sortUnsortedProcessStructsIntoSortedProcessStructVector failed." << endl;
		#endif
							}
						}
						else
						{
							status = -1;

							{
		#if(dbg >= 2)
								 cout << "loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct or" << endl;
								 cout << "mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector or" << endl;
		#endif
							}
						}
					}
					else
					{
						status = -1;
		#if(dbg >= 2)
						 cout << "loadUnsortedProcessStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
					}
				}
				else
				{
					status = -1;
		#if(dbg >= 2)

					 cout << "loadEffectComboJsonFromFile or loadComboStructFromName failed." << endl;
		#endif
				}
			}
			catch(exception &e)
			{
				cout << "exception in ComboDataInt::loadIndexMappedComboData: " << e.what() << endl;
			}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::loadIndexMappedComboData2: " << status << endl;
#endif

		return status;

	}

	void ComboDataInt::transferComboStructBackToEffectComboJson(ComboStruct combo)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::transferComboStructBackToEffectComboJson" << endl;
#endif
		int status = 0;

		//int writeSize = 0;
		try
		{
			this->updateControlParameterValuesInComboFileStruct(combo);
			this->getPedalUi();
			this->loadComboFileStructDataBackIntoEffectComboJson();

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::transferComboStructBackToEffectComboJson: " << e.what() << endl;
			status = -1;
		}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::transferComboStructBackToEffectComboJson: " << status << endl;
#endif

	}









}
