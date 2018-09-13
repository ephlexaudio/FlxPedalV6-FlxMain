/*
 * main.cpp

 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#define dbgIO 0
#define withServer 1
#define testingServer 0
#define testingPedalUi 0
#define usbOnly 0
#define hostUiDbg 1
#define pedalUiDbg 0
#define processingDbg 0

#include "config.h"
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>


#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <jack/jack.h>
#include <jack/control.h>
#include <errno.h>

#include <time.h>
#include <dirent.h>
#include "utilityFunctions.h"

#include "ComboDataInt.h"
#include "PedalUiInt.h"
#include "HostUiInt.h"
#include "mainFunctions.h"
#include "FileSystemFuncts.h"
#include "GPIOClass.h"
#include "ProcessingControl.h"
#include "PedalUtilityData.h"


using namespace std;


#define FILE_LENGTH 1000

#define FILE_SIZE 32000

#define HOST_UI_ACTIVE_COUNT 1500

#define OFX_MAIN_READY 45





map<string, ComboDataInt> comboDataMap;

bool debugOutput = true;

int globalComboIndex = 0;
string globalComboName;

vector<string> componentVector;
vector<string> controlTypeVector;

vector<string> comboList;

HostUiInt hostUi;



static void signal_handler(int sig)
{

	if(debugOutput) cout << "signal received: " << sig <<", OfxMain exiting ..." << endl;

	hostUi.close();
	system("killall -9 jackd");
	signal(sig, SIG_DFL);
	kill(getpid(),sig);

}



bool justPoweredUp = true;
bool comboRunning = false;
bool hostGuiActive = false;
extern GPIOClass footswitch[2];
int footswitchStatusArray[10] = {0,0,0,0,0,0,0,0,0,0};
int wrapperParamData[10] = {1,0,0,0,0,0,0,0,0,0};
bool inputsSwitched;
ProcessingControl procCont;
extern int comboTime;

int inputCouplingMode;
ComboStruct combo;
PedalUtilityData pedalUtilityData;
int comboStructIndex = 0;
int currentComboStructIndex = 0;
int oldComboStructIndex = 0;



#define dbg 1
int main(int argc, char *argv[])
{
    if(debugOutput) cout << "************************************************************************************************" << endl;
    if(debugOutput) cout << "*****************************************   OFX START   ****************************************" << endl;
    if(debugOutput) cout << "************************************************************************************************" << endl;

	int exit = 0;
	uint8_t status = 0;
	errno = 0;
	string pinString;
	bool newDataFromServer = false;
	char simpleResponseStr[20];
	string comboName;
	string pedalUiRequest;
	string pedalUiRequestCommand;
	string pedalUiRequestData;
	bool pedalUiRequestIsValid = false;
	string hostUiRequest;
	string hostUiRequestCommand;
	string hostUiRequestData;
	hostUiRequest.resize(FILE_SIZE);
	hostUiRequestData.resize(FILE_SIZE);
	bool hostUiRequestIsValid = false;

	string footswitch1Status;
	string footswitch2Status;
	char ofxMainStatus[15];

	Json::Value ofxParamJsonData;
	Json::Reader ofxParamJsonReader;
	Json::Reader hostUiJsonReader;
	Json::Value hostUiJsonData;
	std::string serverRequest;
	std::string serverRequestCommand;
	std::string serverRequestDataString;
	Json::Value serverRequestDataJson;
	Json::Reader serverRequestReader;
	bool loadSuccess = false;

	/*****************START: Start up and init of pedal *******************/
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGPIPE, signal_handler);
	signal(SIGALRM, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGSTKFLT, signal_handler);
	signal(SIGSTOP, signal_handler);
	signal(SIGXCPU, signal_handler);
	signal(SIGXFSZ, signal_handler);
	signal(SIGVTALRM, signal_handler);
	signal(SIGPROF, signal_handler);
	loadComponentSymbols();
	loadControlTypeSymbols();

	errno = 0;

	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			debugOutput = true;
			if(debugOutput) cout << "debug mode enabled." << endl;
		}
	}


	for(int i = 0; i < argc; i++)
	{
		printf("argument[%d]: %s\n", i, argv[i]);
	}

	pedalUtilityData.readUtilityDataFromFile();


	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			debugOutput = true;
			if(debugOutput) cout << "debug mode enabled." << endl;
		}
	}



	/*****************END: Start up and init of pedal *******************/


	/*****************START: Start up server and connect ***************/

	loadComboStructMapAndList();

	startJack();
	procCont.disableAudioOutput();
	procCont.start();

	clearBuffer(ofxMainStatus,15);
	strcpy(ofxMainStatus, "starting up");

	procCont.enableEffects();

	FILE *pedalUiProc;
	if(pedalUtilityData.getUsbEnableMode())
	{
		pedalUiProc = popen("/home/FlxPedalUi -d | tee /uiLog.txt &", "w");
		if(debugOutput) cout << "launching FlxPedalUi with USB enabled." << endl;
	}
	else
	{
		pedalUiProc = popen("/home/FlxPedalUi -u -d | tee /uiLog.txt &", "w");
		if(debugOutput) cout << "launching FlxPedalUi with USB disabled." << endl;
	}

	PedalUiInt pedalUi;



	while(exit == 0)
	{


		hostUi.clearAllInternalCharArrays();
		if(hostGuiActive == true)
		{
	        if(hostUi.checkForNewHostData() == 1)
	        {
	        	hostUiRequest = hostUi.getUserRequest();
	#if(hostUiDbg >= 1)
				if(debugOutput) cout << "HOST UI REQUEST: " << hostUiRequest << endl;
				if(debugOutput) cout << "hostUiRequest size: " << hostUiRequest.size() << endl;
	#endif
				hostUiRequestIsValid = true;
				hostUiRequest.erase(remove(hostUiRequest.begin(), hostUiRequest.end(), '\n'), hostUiRequest.end());
				hostUiRequest.erase(remove(hostUiRequest.begin(), hostUiRequest.end(), '\r'), hostUiRequest.end());
				for(unsigned int charIndex = 0; charIndex < hostUiRequest.size(); charIndex++)
				{
					if((hostUiRequest[charIndex] < ' ' || '~' < hostUiRequest[charIndex]))
					{
						hostUiRequestIsValid = false;
						break;
					}
				}

				if(hostUiRequestIsValid)
				{
					uint8_t colonPosition = hostUiRequest.find(":");
					if(0 < colonPosition && colonPosition < hostUiRequest.length())
					{
						hostUiRequestCommand = hostUiRequest.substr(0,colonPosition);
						hostUiRequestData = hostUiRequest.substr(colonPosition+1);
					}
					else hostUiRequestCommand = hostUiRequest;

					if(hostUiRequestCommand.compare("getControlTypes") == 0)
					{
						hostUi.sendControlTypeData();
					}
					else if(hostUiRequestCommand.compare("getComponents") == 0)
					{
						hostUi.sendComponentData();
					}
					else if(hostUiRequestCommand.compare("listCombos") == 0)
					{
						string comboNameArray;
						comboList = getComboMapList();
						for(unsigned int i = 0; i < comboList.size(); i++)
						{
							comboNameArray.append(comboList.at(i).c_str());
							if(i < comboList.size()-1) comboNameArray.append(",");
						}
						hostUi.sendComboList(comboNameArray);

	#if(hostUiDbg >= 2)
						if(debugOutput) cout << comboNameArray << endl;
						if(debugOutput) cout << "sent listCombos" << endl;
	#endif
					}
					else if(hostUiRequestCommand.compare("getCombo") == 0)
					{
						if((hostUiRequestData.size() > 1) && (comboDataMap.find(hostUiRequestData) != comboDataMap.end()))
						{
							globalComboName = hostUiRequestData;
							procCont.disableAudioOutput();
							if(comboDataMap.find(globalComboName) != comboDataMap.end())
							{
								if(debugOutput) cout << "hostUiRequestData: " << hostUiRequestData << endl;
								cout << "sending combo data to host PC."  << endl;
								hostUi.sendComboToHost(hostUiRequestData);
#if(dbg >= 1)
								cout << "combo data sent to host PC."  << endl;
#endif

								procCont.disableEffects();

								if(procCont.load(globalComboName) >= 0) loadSuccess = true;
								else loadSuccess = false;

								procCont.enableEffects();

								if(loadSuccess == true)
								{
									strcpy(ofxMainStatus,"combo running");
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}
							}
							else
							{
								if(debugOutput) cout << "could not get combo" << endl;
								strcpy(ofxMainStatus,"load failed");
								status = -1;
							}
							procCont.enableAudioOutput();
						}
						else
						{
							if(debugOutput) cout << "no combo name given." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("changeValue") == 0)
					{
						if(hostUiRequestData.empty() == false)
						{
							string procContString, paramString, valueString, procParamString;
							int absParamIndex;
							int valueIndex;

							if(hostUiJsonReader.parse(hostUiRequestData.c_str(),hostUiJsonData) == true)
							{
								// get control parameter index and value
								if(hostUiJsonData["control"].isNull() == false)
								{
									procContString = hostUiJsonData["control"].asString();
									paramString = hostUiJsonData["parameter"].asString();
									valueString = hostUiJsonData["value"].asString();
#if(hostUiDbg >= 1)
									if(debugOutput) cout << "control: " << procContString;
									if(debugOutput) cout << "\tparameter: " << paramString;
									if(debugOutput) cout << "\tvalue: " << valueString << endl;
#endif
									absParamIndex = comboDataMap[globalComboName].getControlParameterIndex(procContString, paramString);
									valueIndex = atoi(valueString.c_str());

									if((0 <= absParamIndex && absParamIndex < comboDataMap[globalComboName].getControlParameterArraySize()) && (0 <= valueIndex && valueIndex <= 255))
									{

										comboDataMap[globalComboName].updateControl(absParamIndex, valueIndex);
										// enter into controlsStruct element
										procCont.updateControlParameter(absParamIndex, valueIndex);
										if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
			#if(hostUiDbg >= 1)
										if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
			#endif
									}
									else
									{
#if(hostUiDbg >= 1)
										if(debugOutput) cout << "absParamIndex and/or valueIndex are out of range." << endl;
#endif
									}


								}
								else if(hostUiJsonData["process"].isNull() == false)
								{
									procContString = hostUiJsonData["process"].asString();
									paramString = hostUiJsonData["parameter"].asString();
									valueString = hostUiJsonData["value"].asString();

									if(debugOutput) cout << "process: " << procContString;
									if(debugOutput) cout << "\tparameter: " << paramString;
									if(debugOutput) cout << "\tvalue: " << valueString << endl;

									absParamIndex = comboDataMap[globalComboName].getProcessParameterIndex(procContString, paramString);
									valueIndex = atoi(valueString.c_str());
									if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
									{
										comboDataMap[globalComboName].updateProcess(absParamIndex, valueIndex);
										//// enter into controlsStruct element

										procCont.updateProcessParameter(absParamIndex, valueIndex);
										if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
			#if(hostUiDbg >= 1)
										if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
			#endif
									}
									else
									{
#if(hostUiDbg >= 1)
										if(debugOutput) cout << "absParamIndex and/or valueIndex are out of range." << endl;
#endif
									}
								}
								else
								{
#if(hostUiDbg >= 1)
									if(debugOutput) cout << "invalid value change request." << endl;
#endif
								}

							}
							strcpy(simpleResponseStr,"ChangedValue");
							hostUi.sendSimpleResponse(simpleResponseStr);
						}
						else
						{
							if(debugOutput) cout << "no change data given." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("saveCombo") == 0)
					{
						if(hostUiRequestData.empty() == false)
						{
							bool validComboIndexName;

							procCont.disableAudioOutput();
							procCont.disableEffects();

							string tempComboName = hostUi.getComboFromHost(hostUiRequestData); // new combo data is saved here.

							vector<string> newComboList = getComboListFromFileSystem();

							if(comboDataMap.find(tempComboName) == comboDataMap.end()) // combo was added
							{


								globalComboName = tempComboName;
								if(debugOutput) cout << "new combo added" << endl;
								if(debugOutput) cout << "new combo data to be saved: " << endl;
								if(debugOutput) cout << globalComboName << endl;


								comboList = newComboList; // update combo list
								addComboStructToMapAndList(tempComboName);

								globalComboName = tempComboName;
								validComboIndexName = true;
							}
							else // existing combo: update by erasing map element and adding newer struct to map.
							{

								globalComboName = tempComboName;
								if(comboDataMap.find(tempComboName) != comboDataMap.end())
								{
									comboDataMap.erase(globalComboName);
									addComboStructToMapAndList(globalComboName);
									validComboIndexName = true;
								}
								else
								{
									if(debugOutput) cout << "invalid combo name: " << globalComboName << endl;
									validComboIndexName = false;
								}
								usleep(500000);
							}

							if(validComboIndexName == true)
							{
								string comboNameArray;
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
								hostUi.sendComboList(comboNameArray);
								/****************** Load saved combo *****************/

								if(procCont.load(globalComboName) >= 0)
								{
									if(debugOutput) cout << "combo running: " << globalComboName << ":" << comboDataMap[globalComboName].getName() << endl;
									strcpy(ofxMainStatus,"combo running");
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}
								procCont.enableEffects();
								procCont.enableAudioOutput();
							}

						}
						else
						{
							if(debugOutput) cout << "no data given to save." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("deleteCombo") == 0)
					{
						if(hostUiRequestData.empty() == false)
						{
	 						if(deleteComboFromFileSystem(hostUiRequestData) == 0)
							{
								comboList = getComboListFromFileSystem();

								procCont.disableEffects();
								deleteComboStructFromMapAndList(hostUiRequestData);

								comboList = getComboMapList();
								if(debugOutput) cout << "combo map list size: " << comboList.size() << endl;

								string comboNameArray;

								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
								hostUi.sendComboList(comboNameArray);
								strcpy(ofxMainStatus,"combo deleted");
								procCont.enableEffects();
							}
							else
							{
								if(debugOutput) cout << "failed to delete combo." << endl;
							}
						}
						else
						{
							if(debugOutput) cout << "no combo name given to delete." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("getCurrentValues") == 0)
					{
						if(newDataFromServer)
						{
							newDataFromServer = false;
						}
	#if(pedalUiDbg >= 1)
						if(debugOutput) cout << "sent current data to HOST " << endl;
	#endif
					}
					else if(hostUiRequestCommand.compare("getCurrentStatus") == 0)
					{
						if(newDataFromServer)
						{
							newDataFromServer = false;
						}
						hostUi.sendCurrentStatus(ofxMainStatus);
						pedalUi.sendCurrentStatus(ofxMainStatus); // keep pedal UI receiving status
	#if(pedalUiDbg >= 1)
						if(debugOutput) cout << "sent current status to HOST: " << ofxMainStatus << endl;
	#endif
					}
					else
					{
						if(debugOutput) cout << "sent nothing to host" << endl;
					}
				}
				else
				{
					if(debugOutput) cout << "hostUiRequest is invalid or corrupted." << endl;
				}
				hostUiRequest.clear();
				hostUiRequestCommand.clear();
				hostUiRequestData.clear();
	        }

	        usleep(100000);
		}

		//************* Process requests from PCB UI Interface  *************
		// this needs to be running even when USB is connected to be able to
		// detect, via FlxPedalUi process, when the USB is disconnected

		{
        	{
				if(pedalUi.checkForNewPedalData() == 1)
				{
					pedalUiRequest = pedalUi.getUserRequest();

#if(pedalUiDbg >= 1)
					if(debugOutput) cout << "PEDAL UI REQUEST: " << pedalUiRequest << endl;
					if(debugOutput) cout << "pedalUiRequest size: " << pedalUiRequest.size() << endl;
#endif

					// validate request (make sure it isn't corrupted)
					pedalUiRequestIsValid = true;
					for(string::size_type charIndex = 0; charIndex < pedalUiRequest.size(); charIndex++)
					{
						if(pedalUiRequest[charIndex] < ' ' || '~' < pedalUiRequest[charIndex])
						{
							pedalUiRequestIsValid = false;
							break;
						}
					}

					if(pedalUiRequestIsValid)
					{
						uint8_t colonPosition = pedalUiRequest.find(":");
						if(0 < colonPosition && colonPosition < pedalUiRequest.length())
						{
							pedalUiRequestCommand = pedalUiRequest.substr(0,colonPosition);
							pedalUiRequestData = pedalUiRequest.substr(colonPosition+1);
						}
						else pedalUiRequestCommand = pedalUiRequest;

						if(pedalUiRequestCommand.compare("listCombos") == 0)
						{

							string comboNameArray;
							comboList = getComboMapList();
							for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
							{
								comboNameArray.append(comboList.at(i).c_str());
								if(i < comboList.size()-1) comboNameArray.append(",");
							}
							pedalUi.sendComboList(comboNameArray);

#if(pedalUiDbg >= 1)
							if(debugOutput) cout << comboNameArray << endl;
							if(debugOutput) cout << "sent listCombos" << endl;
#endif
						}
						else if(pedalUiRequestCommand.compare("getCombo") == 0)
						{
							if((pedalUiRequestData.size() > 1) && (comboDataMap.find(pedalUiRequestData) != comboDataMap.end()))
							{
								globalComboName = pedalUiRequestData;
								procCont.disableAudioOutput();
								if(comboDataMap.find(globalComboName) != comboDataMap.end())
								{
									procCont.disableEffects();

									globalComboName = pedalUiRequestData;

									if(debugOutput) cout << globalComboName << endl;

									usleep(1000);
									if(procCont.load(globalComboName) >= 0) loadSuccess = true;
									else loadSuccess = false;
									usleep(1000);
									procCont.enableEffects();
									if(loadSuccess == true)
									{
										comboDataMap[globalComboName].getPedalUi();
										pedalUi.sendComboUiData(comboDataMap[pedalUiRequestData].getPedalUiJson());
										if(debugOutput) cout << "sent pedalUiJson" << endl;

										strcpy(ofxMainStatus,"combo running");
									}
									else
									{
										strcpy(ofxMainStatus,"load failed");
										status = -1;
									}
									procCont.enableAudioOutput();

								}
								else
								{
									if(debugOutput) cout << "globalComboIndex out of range." << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}

							}
							else
							{
								if(debugOutput) cout << "invalid pedalUiRequestData: " << pedalUiRequestData << endl;
								status = -1;
							}
						}
						else if(pedalUiRequestCommand.compare("changeValue") == 0)
						{
							uint8_t equals = 0;

							string effectString, paramString, valueString, procParamString;
							int absParamIndex;
							int valueIndex;
							int size = pedalUiRequestData.size();
							equals = pedalUiRequestData.find("=");
							if(equals < size ) // error check to make sure "colon" and "equals" aren't
								// screwed up.
							{
								// get control parameter index and value
								paramString = pedalUiRequestData.substr(0,equals);
								valueString = pedalUiRequestData.substr(equals+1);
								absParamIndex = atoi(paramString.c_str());
								valueIndex = atoi(valueString.c_str());
								// find corresponding controller and enter index and value into Control struct
								// (control will write value to process parameter during process run)

								if((0 <= absParamIndex && absParamIndex < comboDataMap[globalComboName].getControlParameterArraySize()) && (0 <= valueIndex && valueIndex < 100))
								{
									comboDataMap[globalComboName].updateControl(absParamIndex, valueIndex);
									// enter into controlsStruct element
									procCont.updateControlParameter(absParamIndex, valueIndex);
#if(pedalUiDbg >= 1)
									if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
									if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
#endif
								}
								else
								{
									if(debugOutput) cout << "absParamIndex and/or valueIndex are out of range." << endl;
									if(debugOutput) cout << "absParamIndex: " << absParamIndex << "\tvalueIndex: " << valueIndex << endl;
								}
							}
						}
						else if(pedalUiRequestCommand.compare("saveCombo") == 0)
						{
							{
								bool validComboIndexName;

								procCont.disableEffects();
								{
									comboDataMap[globalComboName].saveCombo();
									usleep(200000);
									validComboIndexName = true;
								}

								strcpy(ofxMainStatus,"combo saved");
								procCont.enableEffects();
							}


						}
						else if(pedalUiRequestCommand.compare("getCurrentValues") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
						}
						else if(pedalUiRequestCommand.compare("getUtilities") == 0)
						{
							pedalUtilityData.readUtilityDataFromFile();
							pedalUi.sendFlxUtilUiData(pedalUtilityData.getUtilityDataForPedalUi());
						}
						else if(pedalUiRequestCommand.compare("changeUtilValue") == 0)
						{
							string utilityParameterName;
							string utilityParameterValue;
							int equalIndex;

							if((equalIndex = pedalUiRequestData.find("=")) > 0)
							{
								utilityParameterName = pedalUiRequestData.substr(0,equalIndex);
								utilityParameterValue = pedalUiRequestData.substr(equalIndex+1);

								pedalUtilityData.updateUtilityValue(utilityParameterName, utilityParameterValue);
							}
							else
							{
								cout << "invalid changeUtilValue request." << endl;
							}
						}
						else if(pedalUiRequestCommand.compare("saveUtilities") == 0)
						{
							pedalUtilityData.writeUtilityDataToFile();
						}
						else if(pedalUiRequestCommand.compare("getCurrentStatus") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							if(pedalUiRequestData.compare("host") == 0)
							{
								if(hostUi.isConnected() == 0)
								{
									hostUi.connect();
								}
								hostGuiActive = true;
							}
							else
							{
								if(hostUi.isConnected() == 1)
								{
									hostUi.disconnect();
								}

								hostGuiActive = false;
							}
#if(processingDbg >= 1)
							if(debugOutput) cout << "combo processing time: " << comboTime << endl;
#endif
							pedalUi.sendCurrentStatus(ofxMainStatus);
						}
						else if(pedalUiRequestCommand.compare("powerOff") == 0)
						{
							system("poweroff");
							exit = 1;  // not sure what real effect this will have,
									   // since it comes after poweroff
						}

						else
						{
							if(debugOutput) cout << "sent nothing to pedal" << endl;
						}
					}
					else
					{
						if(debugOutput) cout << "pedalUiRequest is invalid or corrupted." << endl;
					}

					pedalUiRequest.clear();
					pedalUiRequestCommand.clear();
					pedalUiRequestData.clear();
				}
			}

		}

    procCont.readFootswitches();

	}
	procCont.stop();

	stopCombo();


	return 0;
}
