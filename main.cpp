/*
 * main.cpp

 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#define hostUiDbg 2
#define pedalUiDbg 3
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
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>



#include "utilityFunctions.h"
#include "ComboDataInt.h"
#include "PedalUiInt.h"
#include "HostUiInt.h"
#include "ComboMap.h"
#include "FileSystemInt.h"
#include "GPIOClass.h"
#include "ProcessingControl.h"
#include "PedalUtilityData.h"


using namespace std;


bool debugOutput = true; // set by running FlxMain with option "-d"


static void signal_handler(int sig)
{

	if(debugOutput) cout << "signal received: " << sig <<", OfxMain exiting ..." << endl;


	system("killall -9 jackd");
	signal(sig, SIG_DFL);
	kill(getpid(),sig);

}




vector<string> componentVector;  //need to turn Processes and Controls into classes before getting rid of these
vector<string> controlTypeVector;




#define dbg 0
int main(int argc, char *argv[])
{
	if(debugOutput) cout << "************************************************************************************************" << endl;
	if(debugOutput) cout << "*****************************************   OFX START   ****************************************" << endl;
	if(debugOutput) cout << "************************************************************************************************" << endl;

	vector<string> comboList;
	PedalUtilityData pedalUtilityData;
	HostUiInt hostUi;
	ProcessingControl procCont;

	ComboStruct combo;
	ComboMap comboMap;
	FileSystemInt fsInt;

	int hostGuiActive = 0;

	int exit = 0;
	errno = 0;
	string pinString;
	bool usbConnected = false;
	bool newDataFromServer = false;
	string comboName;
	string pedalUiRequest;
	string pedalUiRequestCommand;
	string pedalUiRequestData;
	//bool pedalUiRequestIsValid = false;
	string hostUiRequest;
	string hostUiRequestCommand;
	string hostUiRequestData;
	//bool statusChange = false;
	hostUiRequest.resize(FILE_SIZE);
	hostUiRequestData.resize(FILE_SIZE);
	//bool hostUiRequestIsValid = false;
	string ofxMainStatus;
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
	loadControlSymbols();

	errno = 0;

	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			debugOutput = true;
			if(debugOutput) cout << "debug mode enabled." << endl;
		}
	}

	pedalUtilityData.readUtilityDataFromFile();
	/*****************END: Start up and init of pedal *******************/


	comboMap.loadComboMapAndList();
	comboMap.setProcessUtilityData(pedalUtilityData.getProcessUtility());
	procCont.setProcessingUtility(pedalUtilityData.getProcessingUtility());
	procCont.setJackUtility(pedalUtilityData.getJackUtility());
	if(procCont.startJack() == -1)
	{
		cout << "startJack failed"  << endl;
		signal_handler(SIGQUIT);
	}
	procCont.enableAudioOutput();
	procCont.startComboProcessing();
	procCont.disableEffects();
	ofxMainStatus.clear();



	/***************** Start up FlxPedalUi process and connect ***************/



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

	pedalUi.openIPCFiles();
	pedalUi.sendUsbPortOpen(false);
	pedalUi.sendHostGuiActive(false);


	//*************************** ENTER MAIN PROCESSING LOOP *****************************************

	while(exit == 0)
	{


			if(hostUi.isUsbCableConnected() == true && hostUi.isUsbConfigured() == true && usbConnected == false)
			{
				if(hostUi.openPort() >= 0)
				{
					pedalUi.sendUsbPortOpen(true);
					pedalUi.sendHostGuiActive(true);
					usbConnected = true;
					hostGuiActive = 1;
				}
				else
				{
					pedalUi.sendUsbPortOpen(false);
					pedalUi.sendHostGuiActive(false);
					hostGuiActive = 0;
				}

			}
			else if(hostUi.isUsbCableConnected() == false && usbConnected == true)
			{
				if(hostUi.closePort() >= 0)
				{
					usbConnected = false;
					pedalUi.sendUsbPortOpen(false);
					pedalUi.sendHostGuiActive(false);
					hostGuiActive = 0;
				}
				else
				{
					pedalUi.sendUsbPortOpen(true);
					pedalUi.sendHostGuiActive(true);
					hostGuiActive = 1;
				}
			}


		if(hostGuiActive == 1)
		{
			//***************************** Process requests from Host PC *********************************************
			if((hostUiRequest = hostUi.getUserRequest()).size() > 5)  // data is validated/filtered in UsbInt
			{

#if(hostUiDbg >= 1)
				if(debugOutput) cout << "HOST UI REQUEST: " << hostUiRequest.substr(0,100) << endl;
				if(debugOutput) cout << "hostUiRequest size: " << hostUiRequest.size() << endl;
#endif

				hostUiRequest = removeReturnRelatedCharacters(hostUiRequest);
				{
					int colonPosition = hostUiRequest.find(":");
					if(0 < colonPosition && colonPosition < hostUiRequest.length())
					{
						hostUiRequestCommand = hostUiRequest.substr(0,colonPosition);
						hostUiRequestData = hostUiRequest.substr(colonPosition+1);
#if(hostUiDbg >= 2)
						cout << "hostUiRequestCommand: " << hostUiRequestCommand << endl;
						cout << "hostUiRequestData: " << hostUiRequestData.substr(0,100) << endl;
#endif
					}
					else hostUiRequestCommand = hostUiRequest;

					if(hostUiRequestCommand.compare("getControlTypes") == 0)
					{
						hostUi.sendControlTypeData(controlTypeVector);
					}
					else if(hostUiRequestCommand.compare("getComponents") == 0)
					{
						hostUi.sendComponentData(componentVector);
					}
					else if(hostUiRequestCommand.compare("listCombos") == 0)
					{
						comboList = comboMap.getComboNameList();
						hostUi.sendComboList(getStringListString(comboList));
#if(hostUiDbg >= 2)
						if(debugOutput) cout << "sent listCombos" << endl;
#endif
					}
					else if(hostUiRequestCommand.compare("getCombo") == 0)
					{
						if((hostUiRequestData.size() > 1))
						{
							comboName = hostUiRequestData;
							procCont.disableAudioOutput();
							if(comboMap.isInComboMap(comboName))
							{
								procCont.disableEffects();
								if(debugOutput) cout << "hostUiRequestData: " << hostUiRequestData << endl;
								cout << "sending combo data to host PC."  << endl;
								hostUi.sendComboToHost(hostUiRequestData);
#if(dbg >= 1)
								cout << "combo data sent to host PC."  << endl;
#endif
								ComboStruct comboStruct = comboMap.getComboObject(comboName).getComboStruct();
								if(procCont.loadComboStruct(comboStruct) == 0) loadSuccess = true;
								else loadSuccess = false;
								if(loadSuccess == true)
								{
									ofxMainStatus = "combo running";
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									ofxMainStatus = "load failed";
								}
								usleep(10000);
								procCont.enableEffects();
							}
							else
							{
								if(debugOutput) cout << "could not get combo" << endl;
								ofxMainStatus = "load failed";
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
							int delimiterPosition[2] = {0,0}; // 1: colon, 2: equals
							string procContString;
							string paramString;
							string valueString;
							string procParamString;
							int valueIndex;

							try
							{
								int size = hostUiRequestData.size();
								if(size > 0)
								{
									delimiterPosition[0] = hostUiRequestData.find(":",0);
									procContString = hostUiRequestData.substr(0,delimiterPosition[0]);
									delimiterPosition[1] = hostUiRequestData.find("=",delimiterPosition[0]);
									paramString = hostUiRequestData.substr(delimiterPosition[0]+1,delimiterPosition[1]-delimiterPosition[0]-1);

									valueString = hostUiRequestData.substr(delimiterPosition[1]+1);
									valueIndex = std::strtol((char *)valueString.c_str(), NULL, 10);
									if(procContString.find("control") >= 0)
									{
										procCont.updateControlParameter(procContString, paramString, valueIndex);
									}
									else
									{
										procCont.updateProcessParameter(procContString, paramString, valueIndex);
									}
								}
							}
							catch(exception &e)
							{
								cout << "error parsing changeValue request: " << e.what() << endl;
							}

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
							bool validComboName;

							procCont.disableAudioOutput();
							procCont.disableEffects();
							usleep(10000);
							// hostUiRequestData is the first block of data of the JSON combo file.
							string tempComboName = hostUi.getComboFromHost(hostUiRequestData); // new combo data is saved here.


							if(tempComboName.empty() == false)
							{
								vector<string> newComboList = fsInt.getComboListFromFileSystem();
								if(comboMap.isInComboMap(tempComboName) == false) // combo was added
								{
									comboName = tempComboName;
									if(debugOutput) cout << "new combo added" << endl;
									if(debugOutput) cout << "new combo data to be saved: " << endl;
									if(debugOutput) cout << comboName << endl;


									comboList = newComboList; // update combo list
									comboMap.addNewComboObjectToMapAndList(tempComboName);

									comboName = tempComboName;
									validComboName = true;
								}
								else // existing combo: update by erasing map element and adding newer struct to map.
								{

									comboName = tempComboName;
									if(comboMap.isInComboMap(tempComboName))
									{
										comboMap.eraseFromMap(comboName);
										comboMap.addNewComboObjectToMapAndList(comboName);
										validComboName = true;
									}
									else
									{
										if(debugOutput) cout << "invalid combo name: " << comboName << endl;
										validComboName = false;
									}
									usleep(500000);
								}

							}
							else
							{
								cout << "combo name was empty" << endl;
								validComboName = false;
							}

							if(validComboName == true)
							{

								hostUi.sendComboList(getStringListString(comboList));
								//****************** Load saved combo *****************

								ComboStruct comboStruct = comboMap.getComboObject(comboName).getComboStruct();
								if(procCont.loadComboStruct(comboStruct) == 0)
								{
									if(debugOutput) cout << "combo running: " << comboName << endl;
									ofxMainStatus = "combo running";
								}
								else
								{
									if(debugOutput) cout << "could not load combo" << endl;
									ofxMainStatus = "load failed";
								}
							}
							else
							{
								cout << "invalid combo name. Combo was not saved." << endl;
							}
							usleep(10000);
							procCont.enableEffects();
							procCont.enableAudioOutput();
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
							if(fsInt.deleteComboFromFileSystem(hostUiRequestData) == 0)
							{
								comboList = fsInt.getComboListFromFileSystem();

								procCont.disableEffects();
								comboMap.deleteComboObjectFromMapAndList(hostUiRequestData); // update comboMap

								if(debugOutput) cout << "combo map list size: " << comboList.size() << endl;

								string comboNameArray;

								hostUi.sendComboList(getStringListString(comboList));
								ofxMainStatus = "combo deleted";
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
					else
					{
						if(debugOutput) cout << "sent nothing to host" << endl;
					}
				}
				hostUiRequest.clear();
				hostUiRequestCommand.clear();
				hostUiRequestData.clear();
			}

			usleep(100000);

		}
		else
		{

		}


		//************* Process requests from PCB UI Interface  *************
		// this needs to be running even when USB is connected to be able to
		// detect, via FlxPedalUi process, when the USB is disconnected

		if((pedalUiRequest = pedalUi.getUserRequest()).size() > 5)
		{

#if(pedalUiDbg >= 3)
			if(debugOutput) cout << "PEDAL UI REQUEST: " << pedalUiRequest << endl;
			if(debugOutput) cout << "pedalUiRequest size: " << pedalUiRequest.size() << endl;
#endif

			// validate request (make sure it isn't corrupted)

			if(validateString(pedalUiRequest) == true)
			{
				int colonPosition = pedalUiRequest.find(":");
				if(0 < colonPosition && colonPosition < pedalUiRequest.length())
				{
					pedalUiRequestCommand = pedalUiRequest.substr(0,colonPosition);
					pedalUiRequestData = pedalUiRequest.substr(colonPosition+1);
				}
				else pedalUiRequestCommand = pedalUiRequest;
#if(pedalUiDbg >= 2)
				cout << "pedalUiRequestCommand: " << pedalUiRequestCommand;
				cout << "\tpedalUiRequestData: " << pedalUiRequestData << endl;
#endif
				if(pedalUiRequestCommand.compare("listCombos") == 0)
				{
					comboList = comboMap.getComboNameList();
					string comboListString = getStringListString(comboList);
					pedalUi.sendComboList(comboListString);

#if(pedalUiDbg >= 2)
					if(debugOutput) cout << comboListString << endl;
					if(debugOutput) cout << "sent listCombos" << endl;
#endif
				}
				else if(pedalUiRequestCommand.compare("getCombo") == 0)
				{
					if(pedalUiRequestData.size() > 1)
					{
						comboName = pedalUiRequestData;
						procCont.disableAudioOutput();
						if(comboMap.isInComboMap(comboName))
						{
							procCont.disableEffects();

							comboName = pedalUiRequestData;

							if(debugOutput) cout << comboName << endl;

							usleep(1000);

							ComboStruct comboStruct = comboMap.getComboObject(comboName).getComboStruct();
							if(procCont.loadComboStruct(comboStruct) == 0) loadSuccess = true;
							else loadSuccess = false;
							usleep(1000);

							if(loadSuccess == true)
							{
								pedalUi.sendComboPedalUiData(comboMap.getComboObject(comboName).getPedalUiJson());
#if(pedalUiDbg >= 1)
								if(debugOutput) cout << "sent pedalUiJson for: " << comboName << endl;
#endif
								ofxMainStatus = "combo running";
							}
							else
							{
								ofxMainStatus = "load failed";
							}

							procCont.enableEffects();


						}
						else
						{
							if(debugOutput) cout << "comboName not found." << endl;
							ofxMainStatus = "load failed";
						}
						procCont.enableAudioOutput();
					}
					else
					{
						if(debugOutput) cout << "invalid pedalUiRequestData: " << pedalUiRequestData << endl;
					}
				}
				else if(pedalUiRequestCommand.compare("changeValue") == 0)
				{
					int delimiterPosition[2] = {0,0}; // 1: colon, 2: equals
					string controlString;
					string paramString;
					string valueString;
					int valueIndex;
					try
					{
						int size = pedalUiRequestData.size();
						if(size > 0)
						{
							delimiterPosition[0] = pedalUiRequestData.find(":",0);
							controlString = pedalUiRequestData.substr(0,delimiterPosition[0]);
							delimiterPosition[1] = pedalUiRequestData.find("=",delimiterPosition[0]);
							paramString = pedalUiRequestData.substr(delimiterPosition[0]+1,delimiterPosition[1]-delimiterPosition[0]-1);

							valueString = pedalUiRequestData.substr(delimiterPosition[1]+1);
							valueIndex = std::strtol((char *)valueString.c_str(), NULL, 10);
							procCont.updateControlParameter(controlString, paramString, valueIndex);
						}
					}
					catch(exception &e)
					{
						cout << "error parsing changeValue request: " << e.what() << endl;
					}
				}
				else if(pedalUiRequestCommand.compare("saveCombo") == 0)
				{
					{
						ComboStruct comboData;
						procCont.disableEffects();
						{
							comboData = procCont.getComboStruct();
							comboMap.saveCombo(comboName,comboData);
							usleep(200000);
						}

						ofxMainStatus = "combo saved";
						procCont.enableEffects();
					}
				}
				else if(pedalUiRequestCommand.compare("getCurrentValues") == 0)
				{
				}
				else if(pedalUiRequestCommand.compare("getUtilities") == 0)
				{
					pedalUtilityData.readUtilityDataFromFile();
					pedalUi.sendFlxUtilPedalUiData(pedalUtilityData.getUtilityDataForPedalUi());
				}
				else if(pedalUiRequestCommand.compare("changeUtilValue") == 0)
				{
					string utilityParameterName;
					string utilityParameterValue;
					int  equalIndex;

					if((equalIndex = pedalUiRequestData.find("=")) > 0)
					{
						utilityParameterName = pedalUiRequestData.substr(0,equalIndex);
						utilityParameterValue = pedalUiRequestData.substr(equalIndex+1);

						if(pedalUtilityData.updateUtilityValue(utilityParameterName, utilityParameterValue) == -1)
						{
							cout << "invalid utility value." << endl;
						}
						else
						{
							try
							{
								pedalUtilityData.processPedalUtilityChange( utilityParameterName,
																			utilityParameterValue,
														   pedalUtilityData,  hostUi, procCont);

							}
							catch(exception &e)
							{
								cout << "exception process updating utility value: " << e.what() << endl;
							}
						}
					}
					else
					{
						cout << "invalid changeUtilValue request." << endl;
					}
					/***************************************************/
				}
				else if(pedalUiRequestCommand.compare("saveUtilities") == 0)
				{
					pedalUtilityData.writeUtilityDataToFile();
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


		procCont.readFootswitches();
	}
	pedalUi.closeIPCFiles();
	procCont.stopComboProcessing();
	hostUi.closePort();
	procCont.stopJack();


	return 0;
}
