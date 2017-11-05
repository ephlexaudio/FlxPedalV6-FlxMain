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
#define pedalUiDbg 1
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
//#include <hid.h>
#include <time.h>
#include "utilityFunctions.h"
//#include "BaseUiInt2.h"
#include "ComboDataInt.h"
#include "PedalUiInt.h"
#include "HostUiInt.h"
#include "mainFunctions.h"
#include "FileSystemFuncts.h"
#include "GPIOClass.h"
#include "ProcessingControl.h"
/*extern "C" {
#include "CFuncts.h"
};*/


using namespace std;
/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1*/

#define FILE_LENGTH 1000
#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define MCU_SHARED_MEMORY_SECTION_SIZE 4096

#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define CM0_SHARED_MEMORY_SECTION_SIZE 4096

//#define HOST_SHARED_MEMORY_SECTION_ADDRESS 8192
//#define HOST_SHARED_MEMORY_SECTION_SIZE 4096

//#define HOST_SHARED_MEMORY_FILE_ADDRESS 32768
#define FILE_SIZE 32000

#define HOST_UI_ACTIVE_COUNT 1500

#define OFX_MAIN_READY 45

/*
#if(COMBO_DATA_VECTOR == 1)
#elif(COMBO_DATA_ARRAY == 1)
#elif(COMBO_DATA_MAP == 1)
#endif
*/

struct linuxProcess {
	int pid;
	int pgid;
	char command[20];
};

struct _segFaultInfo {
	string function;
	int line;
} segFaultInfo;

#if(COMBO_DATA_VECTOR == 1)
	vector<ComboDataInt> comboDataVector;
#elif(COMBO_DATA_ARRAY == 1)
	ComboDataInt comboDataArray[15];
#elif(COMBO_DATA_MAP == 1)
	map<string, ComboDataInt> comboDataMap;
#endif

int comboDataCount;  // this includes erased ComboDataInts
ComboDataInt currentCombo;
bool debugOutput = false;
int globalComboIndex = 0;
string globalComboName;
std::vector<linuxProcess> linuxProcessList;
vector<string> componentVector;
vector<string> controlTypeVector;
std::vector<string> componentDataList;
std::vector<string> componentNameList;
#if(COMBO_DATA_VECTOR == 1)
	std::vector<string> comboList;//comboVectorList;
#elif(COMBO_DATA_ARRAY == 1)
	std::vector<string> comboList;
#elif(COMBO_DATA_MAP == 1)
	std::vector<string> comboList;
#endif




static void signal_handler(int sig)
{
	//system("echo \"0\" > /sys/class/gpio/gpio13/value");
	if(debugOutput) cout << "signal received: " << sig <<", OfxMain exiting ..." << endl;
	//procCont.stop();
	system("killall -9 jackd");
	signal(sig, SIG_DFL);
	kill(getpid(),sig);

	//exit(0);
}



//ComboDataInt comboData;
//
int ofxPid;
int childPgid = 0;
int procCount = 0;
int effectCount = 0;
//int comboIndex = 0;
unsigned int bufferSize = 0;
bool justPoweredUp = true;
bool comboRunning = false;
bool hostGuiActive = false;
extern GPIOClass footswitch[2];
int footswitchStatusArray[10] = {0,0,0,0,0,0,0,0,0,0};
int wrapperParamData[10] = {1,0,0,0,0,0,0,0,0,0};
bool inputsSwitched;
//int opLogFD = open("/home/opLog.txt",O_WRONLY);
struct _jackParams jackParams;
struct _processingParams processingParams;
extern ProcessingControl procCont;
extern int comboTime;

int inputCouplingMode;
int waveshaperMode;
int antiAliasingNumber;

ComboStruct combo;
int comboStructIndex = 0;
int currentComboStructIndex = 0;
int oldComboStructIndex = 0;

/*extern int Processing2_start();
extern void Processing2_stop();*/

//extern "C" int jack_main();

#define dbg 1
int main(int argc, char *argv[])
{
    if(debugOutput) cout << "************************************************************************************************" << endl;
    if(debugOutput) cout << "*****************************************   OFX START   ****************************************" << endl;
    if(debugOutput) cout << "************************************************************************************************" << endl;

	int exit = 0;
	int count = 0;
	bool enteringLoop = false;//true;
	uint8_t status = 0;
	char procOutput[15];
	char sharedMemoryBuffer[1000];
	char sharedMemoryRequestBuffer[50];
	char debugString[500];
	char initComboName[15];
	char pressedKey = ' ';
	errno = 0;
	int serverStatus = 1;
	long serverCheckCount = 0;
	uint16_t cycleCount = 0;
	uint8_t gpioStatus = 0;
	string pinString;
	bool newDataFromServer = false;
	//long hostUiActiveCount = 0;
	char gpioStr[5];
	char simpleResponseStr[20];
	string comboName;


	string pedalUiRequest;
	string pedalUiRequestCommand;
	string pedalUiRequestData;
	bool pedalUiRequestIsValid = false;

	HostUiInt hostUi;

	string hostUiRequest;
	string hostUiRequestCommand;
	string hostUiRequestData;
	hostUiRequest.resize(FILE_SIZE);
	hostUiRequestData.resize(FILE_SIZE);
	bool hostUiRequestIsValid = false;

	string footswitch1Status;
	string footswitch2Status;
	char ofxMainStatus[15];

	char ofxParamString[500];
	Json::Value ofxParamJsonData;
	Json::Reader ofxParamJsonReader;
	Json::Reader hostUiJsonReader;
	Json::Value hostUiJsonData;
	float dataBuffer[256];
	int getProcessDataPassCount = 0;
	std::string serverRequest;
	std::string serverRequestCommand;
	std::string serverRequestDataString;
	bool serverRequestIsValid = false;
	Json::Value serverRequestDataJson;
	Json::Reader serverRequestReader;


	/*****************START: Start up and init of pedal *******************/
	int connCount = 0;

	ofxPid = getpid();
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
	//signal(SIGPIPE, SIG_IGN);
	loadComponentSymbols();
	loadControlTypeSymbols();
	//printf("OfxMain PID: %d.\n", ofxPid);

	//usbMain();
	/*UsbServerInt usb;

	usb.startServer();

	usb.stopServer();*/

	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			debugOutput = true;
			if(debugOutput) cout << "debug mode enabled." << endl;
		}
		else
		{
			//strcpy(initComboName, argv[1]);
		}
	}

	/******* Get JACK initialization data ***************/
	cout << "getting ofxParams.txt." << endl;
	int ofxParamsFD = open("/home/ofxParams.txt",O_RDONLY);
	/* read file into temp string */
	read(ofxParamsFD, ofxParamString, 500);

	ofxParamJsonData.clear();

	if(ofxParamJsonReader.parse(ofxParamString, ofxParamJsonData) == false)
	{
		if(debugOutput) cout << "failed to read JACK initialization data file.\n" << endl;
		/************ Should use default initialization data here ****************/
		return -1;
	}
	antiAliasingNumber = atoi(ofxParamJsonData["antiAliasingNumber"].asString().c_str());
	if(antiAliasingNumber > 1)
	{
		if(debugOutput) cout << "anti-aliasing: on." << endl;
	}
	else
	{
		if(debugOutput) cout << "anti-aliasing: off." << endl;
	}

	/*if(ofxParamJsonData["antiAliasingNumber"].asString().compare("on") == 0)
	{
		antiAliasing = true;
		if(debugOutput) cout << "anti-aliasing: on." << endl;
	}
	else
	{
		antiAliasing = false;
		if(debugOutput) cout << "anti-aliasing: off." << endl;
	}*/

	if(ofxParamJsonData["inputCoupling"].asString().compare("offset") == 0)
	{
		inputCouplingMode = 0;
		if(debugOutput) cout << "input coupling mode: averaging offset." << endl;
	}
	else if(ofxParamJsonData["inputCoupling"].asString().compare("filter") == 0)
	{
		inputCouplingMode = 1;
		if(debugOutput) cout << "input coupling mode: highpass filter." << endl;
	}
	else
	{
		inputCouplingMode = 2;
		if(debugOutput) cout << "input coupling mode: none." << endl;
	}


	if(ofxParamJsonData["waveshaperMode"].asString().compare("circuitModel") == 0)
	{
		waveshaperMode = 0;
		if(debugOutput) cout << "waveshaper mode: circuit model." << endl;
	}
	else
	{
		waveshaperMode = 1;
		if(debugOutput) cout << "waveshaper mode: slope/intercept." << endl;
	}

	jackParams.period = atoi(ofxParamJsonData["jack"]["period"].asString().c_str());
	bufferSize = jackParams.period;
	jackParams.buffer = atoi(ofxParamJsonData["jack"]["buffer"].asString().c_str());
	if(debugOutput) cout << "JACK period: " << jackParams.period << "\tJACK buffer: " << jackParams.buffer << endl;

	processingParams.noiseGate.highThres = atof(ofxParamJsonData["noiseGate"]["highThres"].asString().c_str());
	processingParams.noiseGate.lowThres = atof(ofxParamJsonData["noiseGate"]["lowThres"].asString().c_str());
	processingParams.noiseGate.gain = atof(ofxParamJsonData["noiseGate"]["gain"].asString().c_str());
	if(debugOutput) cout << "Noise gate low threshold: " << processingParams.noiseGate.lowThres << "\tNoise gate high threshold: " << processingParams.noiseGate.highThres << endl;
	processingParams.trigger.highThres = atof(ofxParamJsonData["trigger"]["highThres"].asString().c_str());
	processingParams.trigger.lowThres = atof(ofxParamJsonData["trigger"]["lowThres"].asString().c_str());
	if(debugOutput) cout << "Trigger low threshold: " << processingParams.trigger.lowThres << "\tTrigger high threshold: " << processingParams.trigger.highThres << endl;
	for(int i = 0; i < argc; i++)
	{
		printf("argument[%d]: %s\n", i, argv[i]);
	}

	if(argc > 1)
	{
		if(strcmp(argv[1],"-d") == 0)
		{
			debugOutput = true;
			if(debugOutput) cout << "debug mode enabled." << endl;
		}
		else
		{
			//strcpy(initComboName, argv[1]);
		}
	}
	/*else
	{
		strcpy(initComboName, "eq");
	}*/



	/*****************END: Start up and init of pedal *******************/

	//initializePedal(initComboName);
	/*****************START: Start up server and connect ***************/

#if(COMBO_DATA_VECTOR == 1)
	loadComboStructVector();
	comboList = getComboVectorList();
#elif(COMBO_DATA_ARRAY == 1)
	comboList = getComboList();
	loadComboStructArray(comboList);
#elif(COMBO_DATA_MAP == 1)
	//comboList = getComboMapList();
	loadComboStructMapAndList();
#endif

	/*for(vector<string>::size_type comboListIndex = 0; comboListIndex < comboList.size(); comboListIndex++)
	{
		if(comboDataArray[comboListIndex].loadComboStruct((char *)comboList[comboListIndex].c_str()) < 0)
		{
			if(debugOutput) cout << "failed to open file: " << (char *)comboList[comboListIndex].c_str();
		}
	}*/

	char choice = 0;


	startJack();
	procCont.start();


	char serverRequestData[20];

	std::vector<string> pedalUiTestData;
	pedalUiTestData.push_back("listCombos");
	pedalUiTestData.push_back("getCombo:0eq");

	pinString = "1";

	clearBuffer(ofxMainStatus,15);
	strcpy(ofxMainStatus, "starting up");
	//gpioStatus = ofxMainRdy.setval_gpio(1);
	int smSectionIndex = 0;
	procCont.enableEffects();


	FILE *pedalUiProc;
	if(ofxParamJsonData["usbEnable"].asBool())
	{
		pedalUiProc = popen("/home/FlxPedalUi", "w");
		if(debugOutput) cout << "launching FlxPedalUi with USB enabled." << endl;
	}
	else
	{
		pedalUiProc = popen("/home/FlxPedalUi -u", "w");
		if(debugOutput) cout << "launching FlxPedalUi with USB disabled." << endl;
	}


	PedalUiInt pedalUi;


	int speedTest = 0;
	while(exit == 0)
	{
		//************* Process requests from Host UI Interface  *************

		if(hostGuiActive == true)
		{
	        if(hostUi.checkForNewHostData() == 1)
	        {
	        	//enteringLoop = false;
	        	hostUiRequest = hostUi.getUserRequest();
	        	//if(debugOutput) cout << "usb data received: " << hostUiRequestCharArray << endl;
	        	//if(debugOutput) cout << "usb data size: " << strlen(hostUiRequestCharArray) << endl;
	#if(hostUiDbg >= 1)
				if(debugOutput) cout << "HOST UI REQUEST: " << hostUiRequest/*.substr(0,50)*/ << endl;
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
					//if(debugOutput) cout << "hostUiRequest: " << hostUiRequest << endl;
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
#if(COMBO_DATA_VECTOR == 1)
						comboList = getComboVectorList();
						for(unsigned int i = 0; i < comboList.size(); i++)
						{
							comboNameArray.append(comboList.at(i).c_str());
							if(i < comboList.size()-1) comboNameArray.append(",");
						}
#elif(COMBO_DATA_ARRAY == 1)
						comboList = getComboList();
						for(unsigned int i = 0; i < comboList.size(); i++)
						{
							comboNameArray.append(comboList.at(i).c_str());
							if(i < comboList.size()-1) comboNameArray.append(",");
						}
#elif(COMBO_DATA_MAP == 1)
						comboList = getComboMapList();
						for(unsigned int i = 0; i < comboList.size(); i++)
						{
							comboNameArray.append(comboList.at(i).c_str());
							if(i < comboList.size()-1) comboNameArray.append(",");
						}
#endif
						hostUi.sendComboList(comboNameArray);
						//strcpy(hostUiResponseCharArray, comboNameArray.c_str());

	#if(hostUiDbg == 1)
						if(debugOutput) cout << comboNameArray << endl;
						if(debugOutput) cout << "sent listCombos" << endl;
	#endif
					}
					else if(hostUiRequestCommand.compare("getCombo") == 0)
					{
						//bypassAll();
#if(COMBO_DATA_VECTOR == 1)
						if((hostUiRequestData.size() > 1) && (getComboVectorIndex(hostUiRequestData) >= 0))
#elif(COMBO_DATA_ARRAY == 1)
						if((hostUiRequestData.size() > 1) && (getComboIndex(hostUiRequestData) >= 0))
#elif(COMBO_DATA_MAP == 1)
						if((hostUiRequestData.size() > 1) && (comboDataMap.find(hostUiRequestData) != comboDataMap.end()))
#endif
						{
							globalComboName = hostUiRequestData;
							procCont.disableEffects();
#if(COMBO_DATA_VECTOR == 1)
							int tempComboIndex = getComboVectorIndex(hostUiRequestData);
							if(tempComboIndex >= 0)
#elif(COMBO_DATA_ARRAY == 1)
							int tempComboIndex = getComboIndex(hostUiRequestData);
							if(tempComboIndex >= 0)
#endif
#if(COMBO_DATA_MAP == 1)
							if(comboDataMap.find(globalComboName) != comboDataMap.end())
#endif
							{
								//procCont.disableEffects();
								//comboDataMap[globalComboName].loadComboStructFromName((char *)globalComboName.c_str());

#if(COMBO_DATA_VECTOR == 1 || COMBO_DATA_ARRAY == 1)
								globalComboIndex = tempComboIndex;
								if(debugOutput) cout << globalComboName << ":" << globalComboIndex << endl;
								if(procCont.load(globalComboIndex) >= 0)
#elif(COMBO_DATA_MAP == 1)
								if(debugOutput) cout << hostUiRequestData << endl;
								hostUi.sendComboToHost(hostUiRequestData);


								if(procCont.load(globalComboName) >= 0)
#endif
								{
//									hostUi.sendComboToHost(hostUiRequestData);
									strcpy(ofxMainStatus,"combo running");
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}
								//procCont.enableEffects();
							}
							else
							{
								if(debugOutput) cout << "could not get combo" << endl;
								strcpy(ofxMainStatus,"load failed");
								status = -1;
							}
							procCont.enableEffects();

						}
						else
						{
							if(debugOutput) cout << "no combo name given." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("changeValue") == 0)
					{
						//uint8_t colon = 0;
						//uint8_t equals = 0;
						if(hostUiRequestData.empty() == false)
						{
							string procContString, paramString, valueString, procParamString;
							volatile int absParamIndex;
							volatile int valueIndex;
							//int size = hostUiRequestData.size();

							//colon = requestData.find(":");
							if(hostUiJsonReader.parse(hostUiRequestData.c_str(),hostUiJsonData) == true)
							{
								//effectString = requestData.substr(0,colon);

								// get control parameter index and value
								if(hostUiJsonData["control"].isNull() == false)
								{
									procContString = hostUiJsonData["control"].asString();
									paramString = hostUiJsonData["parameter"].asString();
									valueString = hostUiJsonData["value"].asString();

									if(debugOutput) cout << "control: " << procContString;
									if(debugOutput) cout << "\tparameter: " << paramString;
									if(debugOutput) cout << "\tvalue: " << valueString << endl;
#if(COMBO_DATA_VECTOR == 1)
									absParamIndex = comboDataVector[globalComboIndex].getControlParameterIndex(procContString, paramString);
#elif(COMBO_DATA_ARRAY == 1)
									absParamIndex = comboDataArray[globalComboIndex].getControlParameterIndex(procContString, paramString);
#elif(COMBO_DATA_MAP == 1)
									absParamIndex = comboDataMap[globalComboName].getControlParameterIndex(procContString, paramString);
#endif
									valueIndex = atoi(valueString.c_str());

#if(COMBO_DATA_VECTOR == 1)
									if((0 <= absParamIndex && absParamIndex < comboDataVector[globalComboIndex].controlParameterArray.size()) && (0 <= valueIndex && valueIndex <= 255))
#elif(COMBO_DATA_ARRAY == 1)
									if((0 <= absParamIndex && absParamIndex < comboDataArray[globalComboIndex].controlParameterArray.size()) && (0 <= valueIndex && valueIndex <= 255))
#elif(COMBO_DATA_MAP == 1)
									if((0 <= absParamIndex && absParamIndex < comboDataMap[globalComboName].controlParameterArray.size()) && (0 <= valueIndex && valueIndex <= 255))
#endif
									{

#if(COMBO_DATA_VECTOR == 1)
										comboDataVector[globalComboIndex].updateControl(absParamIndex, valueIndex);
#elif(COMBO_DATA_ARRAY == 1)
										comboDataArray[globalComboIndex].updateControl(absParamIndex, valueIndex);
#elif(COMBO_DATA_MAP == 1)
										comboDataMap[globalComboName].updateControl(absParamIndex, valueIndex);
#endif
										//// enter into controlsStruct element

										procCont.updateControlParameter(absParamIndex, valueIndex);
										//comboData.updateProcess(absParamIndex, valueIndex);
										//procCont.updateProcessParameter(absParamIndex, valueIndex);
										if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
										//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
			#if(hostUiDbg == 1)
										if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
			#endif
										//updateCombo(paramIndex,valueIndex);
									}
									else
									{
										if(debugOutput) cout << "absParamIndex and/or valueIndex are out of range." << endl;
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

#if(COMBO_DATA_VECTOR == 1)
									absParamIndex = comboDataVector[globalComboIndex].getProcessParameterIndex(procContString, paramString);
#elif(COMBO_DATA_ARRAY == 1)
									absParamIndex = comboDataArray[globalComboIndex].getProcessParameterIndex(procContString, paramString);
#elif(COMBO_DATA_MAP == 1)
									absParamIndex = comboDataMap[globalComboName].getProcessParameterIndex(procContString, paramString);
#endif
									valueIndex = atoi(valueString.c_str());
									if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
									{
#if(COMBO_DATA_VECTOR == 1)
										comboDataVector[globalComboIndex].updateProcess(absParamIndex, valueIndex);
#elif(COMBO_DATA_ARRAY == 1)
										comboDataArray[globalComboIndex].updateProcess(absParamIndex, valueIndex);
#elif(COMBO_DATA_MAP == 1)
										comboDataMap[globalComboName].updateProcess(absParamIndex, valueIndex);
#endif
										//// enter into controlsStruct element

#if(COMBO_DATA_VECTOR == 1)
#elif(COMBO_DATA_ARRAY == 1)
#elif(COMBO_DATA_MAP == 1)
#endif
										procCont.updateProcessParameter(absParamIndex, valueIndex);
										//comboData.updateProcess(absParamIndex, valueIndex);
										//procCont.updateProcessParameter(absParamIndex, valueIndex);
										if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
										//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
			#if(hostUiDbg == 1)
										if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
			#endif
										//updateCombo(paramIndex,valueIndex);
									}
									else
									{
										if(debugOutput) cout << "absParamIndex and/or valueIndex are out of range." << endl;
									}
								}
								else
								{
									if(debugOutput) cout << "invalid value change request." << endl;
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
						//comboData.saveCombo();//saveCombo(requestData);
						if(hostUiRequestData.empty() == false)
						{
							bool validComboIndexName;

							string tempComboName = hostUi.getComboFromHost(hostUiRequestData); // new combo data is saved here.
							procCont.disableEffects();

							vector<string> newComboList = getComboListFromFileSystem();

#if(COMBO_DATA_VECTOR == 1)
							if(newComboList.size() != comboList.size()) // combo was added
#elif(COMBO_DATA_ARRAY == 1)
							if(newComboList.size() != comboList.size()) // combo was added
#elif(COMBO_DATA_MAP == 1)
							if(comboDataMap.find(tempComboName) == comboDataMap.end())//if(newComboList.size() != comboList.size()) // combo was added
#endif
							{
#if(COMBO_DATA_VECTOR == 1)
								//if(newComboList.size() > comboList.size())
								{
									if(debugOutput) cout << "new combo added" << endl;
									if(debugOutput) cout << "new combo data to be saved: " << endl;
									if(debugOutput) cout << globalComboName << endl;
								}

#elif(COMBO_DATA_ARRAY == 1)
								//if(newComboList.size() > comboList.size())
								{
									if(debugOutput) cout << "new combo added" << endl;
									if(debugOutput) cout << "new combo data to be saved: " << endl;
									if(debugOutput) cout << globalComboName << endl;
								}

#elif(COMBO_DATA_MAP == 1)
								//if(comboDataMap.find(tempComboName) != comboDataMap.end())
								{
									globalComboName = tempComboName;
									if(debugOutput) cout << "new combo added" << endl;
									if(debugOutput) cout << "new combo data to be saved: " << endl;
									if(debugOutput) cout << globalComboName << endl;
								}
#endif

#if(COMBO_DATA_VECTOR == 1)
								//comboVectorList = comboFileList; // update combo list
								comboDataVector.clear();
								loadComboStructVector(); // this could be done more efficiently, but will do for now
								comboList = getComboVectorList();
								int tempComboIndex = getComboVectorIndex(tempComboName);
#elif(COMBO_DATA_ARRAY == 1)
								comboList = newComboList; // update combo list
								loadComboStructArray(comboList); // this could be done more efficiently, but will do for now
								int tempComboIndex = getComboIndex(tempComboName);
#elif(COMBO_DATA_MAP == 1)
								comboList = newComboList; // update combo list
								//loadComboStructMapAndList(); // this could be done more efficiently, but will do for now
								addComboStructToMapAndList(tempComboName);
#endif

#if(COMBO_DATA_VECTOR == 1 || COMBO_DATA_ARRAY == 1)
								if(tempComboIndex >= 0)
								{
									globalComboIndex = tempComboIndex;
									if(debugOutput) cout << "new combo index: " << tempComboIndex << endl;
									validComboIndexName = true;
								}
								else
								{
									if(debugOutput) cout << "invalid combo index: " << tempComboIndex << endl;
									validComboIndexName = false;
								}

#elif(COMBO_DATA_MAP == 1)
								globalComboName = tempComboName;
								validComboIndexName = true;
#endif
							}
							else
							{

								globalComboName = tempComboName;
#if(COMBO_DATA_VECTOR == 1)
								if(getComboVectorIndex(tempComboName) >= 0)
								{
									comboDataVector[globalComboIndex].loadComboStructFromName((char *)tempComboName.c_str());
									usleep(200000);
									validComboIndexName = true;
								}
								else
								{
									if(debugOutput) cout << "combo not found." << endl;
								}

#elif(COMBO_DATA_ARRAY == 1)
								//for(int i = 0; i < 100; i++) used for stress testing
								if(getComboIndex(tempComboName) >= 0)
								{
									//if(debugOutput) cout << "loadComboStructFromName try #" << i << endl;
									comboDataArray[globalComboIndex].loadComboStructFromName((char *)tempComboName.c_str());
									usleep(200000);
									validComboIndexName = true;
								}
								else
								{
									if(debugOutput) cout << "combo not found." << endl;
								}

#elif(COMBO_DATA_MAP == 1)
								if(comboDataMap.find(tempComboName) != comboDataMap.end())
								{
									comboDataMap.erase(globalComboName);
									addComboStructToMapAndList(globalComboName);//comboDataMap[globalComboName].loadComboStructFromName((char *)globalComboName.c_str());
									validComboIndexName = true;
								}
								else
								{
									if(debugOutput) cout << "invalid combo name: " << globalComboName << endl;
									validComboIndexName = false;
								}
#endif
								usleep(500000);
							}

							if(validComboIndexName == true)
							{
								string comboNameArray;
#if(COMBO_DATA_VECTOR == 1)
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#elif(COMBO_DATA_ARRAY == 1)
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#elif(COMBO_DATA_MAP == 1)
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#endif
								hostUi.sendComboList(comboNameArray);
								/****************** Load saved combo *****************/
#if(COMBO_DATA_VECTOR == 1 || COMBO_DATA_ARRAY == 1)
								if(procCont.load(globalComboIndex) >= 0)
								{
#if(COMBO_DATA_VECTOR == 1)
									if(debugOutput) cout << "combo loaded: " << globalComboIndex << ":" << comboDataVector[globalComboIndex].comboName << endl;
#elif(COMBO_DATA_ARRAY == 1)
									if(debugOutput) cout << "combo loaded: " << globalComboIndex << ":" << comboDataArray[globalComboIndex].comboName << endl;
#endif
									strcpy(ofxMainStatus,"combo loaded");
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}


#elif(COMBO_DATA_MAP == 1)
								if(procCont.load(globalComboName) >= 0)
								{
									if(debugOutput) cout << "combo loaded: " << globalComboName << ":" << comboDataMap[globalComboName].comboName << endl;
#endif
									strcpy(ofxMainStatus,"combo loaded");
								}
								else
								{
									if(debugOutput) cout << "could not get combo" << endl;
									strcpy(ofxMainStatus,"load failed");
									status = -1;
								}
							}
							procCont.enableEffects();

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
							//comboData.saveCombo();//saveCombo(requestData);

							if(deleteComboFromFileSystem(hostUiRequestData) == 0)
							{
								comboList = getComboListFromFileSystem();

								procCont.disableEffects();
#if(COMBO_DATA_VECTOR == 1)
								comboDataVector.clear();
								loadComboStructVector(); // this could be done more efficiently, but will do for now
#elif(COMBO_DATA_ARRAY == 1)
								loadComboStructArray(comboList); // this could be done more efficiently, but will do for now
#elif(COMBO_DATA_MAP == 1)
								//loadComboStructMapAndList(); // this could be done more efficiently, but will do for now
								deleteComboStructFromMapAndList(hostUiRequestData);
#endif

#if(COMBO_DATA_VECTOR == 1)
								comboList = getComboVectorList();
								if(debugOutput) cout << "combo vector list size: " << comboList.size() << endl;
#elif(COMBO_DATA_ARRAY == 1)
								comboList = getComboList();
								if(debugOutput) cout << "combolist size: " << comboList.size() << endl;
#elif(COMBO_DATA_MAP == 1)
								comboList = getComboMapList();
								if(debugOutput) cout << "combo map list size: " << comboList.size() << endl;
#endif

#if(COMBO_DATA_VECTOR == 1 || COMBO_DATA_ARRAY == 1)
								while(globalComboIndex >= comboList.size())
								{
									if(debugOutput) cout << "globalComboIndex is too large (decrementing): " << globalComboIndex << endl;
									globalComboIndex--;
								}
#endif


								string comboNameArray;

#if(COMBO_DATA_VECTOR == 1)
								//string comboNameArray;
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#elif(COMBO_DATA_ARRAY == 1)
								//string comboNameArray;
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#elif(COMBO_DATA_MAP == 1)
								//string comboNameArray;
								for(unsigned int i = 0; i < comboList.size(); i++)
								{
									comboNameArray.append(comboList.at(i).c_str());
									if(i < comboList.size()-1) comboNameArray.append(",");
								}
#endif
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
						//hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
#if(COMBO_DATA_VECTOR == 1)
						hostUi.sendCurrentData(comboDataVector[globalComboIndex].unsortedParameterArray);
#elif(COMBO_DATA_ARRAY == 1)
						hostUi.sendCurrentData(comboDataArray[globalComboIndex].unsortedParameterArray);
#elif(COMBO_DATA_MAP == 1)
						hostUi.sendCurrentData(comboDataMap[globalComboName].unsortedParameterArray);
#endif
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
						//pedalUi->sendCurrentStatus(ofxMainStatus); // keep pedal UI receiving status
						//strcpy(ofxMainStatus," ");
						//hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
	#if(pedalUiDbg >= 1)
						if(debugOutput) cout << "sent current status to HOST: " << ofxMainStatus << endl;
	#endif
					}
					else if(hostUiRequestCommand.compare("hostUiActive") == 0)
					{
						//hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
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

        //if(((smSectionIndex = pedalUi.checkForNewData()) > 0))
        //else if(((smSectionIndex = pedalUi->checkForNewPedalData()) > 0))
		{
        	//enteringLoop = false;

        	{
				if(pedalUi.checkForNewPedalData() == 1)
				{



//					pedalUi.dataProcessingStatus(0);
////					pedalUi->dataProcessingStatus(0);
////					if(enteringLoop)
////					{
////						pedalUiRequest = "getCombo:";
////						pedalUiRequest.append(initComboName);
////						enteringLoop = false;
////					}
////					else
					{
						pedalUiRequest = pedalUi.getUserRequest();

					}

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
						//if(debugOutput) cout << "pedalUiRequest: " << pedalUiRequest << endl;
						if(0 < colonPosition && colonPosition < pedalUiRequest.length())
						{
							pedalUiRequestCommand = pedalUiRequest.substr(0,colonPosition);
							pedalUiRequestData = pedalUiRequest.substr(colonPosition+1);
						}
						else pedalUiRequestCommand = pedalUiRequest;

						if(pedalUiRequestCommand.compare("listCombos") == 0)
						{
							//pedalUi.sendComboUiData(combo.pedalUiJson);
							string comboNameArray;
#if(COMBO_DATA_VECTOR == 1)
							comboList = getComboVectorList();
							for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
							{
								comboNameArray.append(comboList.at(i).c_str());
								if(i < comboList.size()-1) comboNameArray.append(",");
							}
#elif(COMBO_DATA_ARRAY == 1)
							comboList = getComboList();
							for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
							{
								comboNameArray.append(comboList.at(i).c_str());
								if(i < comboList.size()-1) comboNameArray.append(",");
							}
#elif(COMBO_DATA_MAP == 1)
							comboList = getComboMapList();
							for(std::vector<string>::size_type i = 0; i < comboList.size(); i++)
							{
								comboNameArray.append(comboList.at(i).c_str());
								if(i < comboList.size()-1) comboNameArray.append(",");
							}
#endif
							pedalUi.sendComboList(comboNameArray);
							//pedalUi->sendComboList(comboNameArray);

#if(pedalUiDbg >= 1)
							if(debugOutput) cout << comboNameArray << endl;
							if(debugOutput) cout << "sent listCombos" << endl;
#endif
						}
						else if(pedalUiRequestCommand.compare("getCombo") == 0)
						{
							//bypassAll();
#if(COMBO_DATA_VECTOR == 1)
							if((pedalUiRequestData.size() > 1) && (getComboVectorIndex(pedalUiRequestData) >= 0))
#elif(COMBO_DATA_ARRAY == 1)
							if((pedalUiRequestData.size() > 1) && (getComboArrayIndex(pedalUiRequestData) >= 0))
#elif(COMBO_DATA_MAP == 1)
							if((pedalUiRequestData.size() > 1) && (comboDataMap.find(pedalUiRequestData) != comboDataMap.end()))
#endif
							{
								//globalComboName = pedalUiRequestData;
#if(COMBO_DATA_VECTOR == 1)
								int tempComboIndex = getComboVectorIndex(globalComboName);
								//if(tempComboIndex >= 0)
								if(0 <= tempComboIndex && tempComboIndex < comboList.size())
#elif(COMBO_DATA_ARRAY == 1)
								int tempComboIndex = getComboArrayIndex(globalComboName);
								//if(tempComboIndex >= 0)
								if(0 <= tempComboIndex && tempComboIndex < comboList.size())
#elif(COMBO_DATA_MAP == 1)
								if(comboDataMap.find(pedalUiRequestData) != comboDataMap.end())
#endif
								{
									procCont.disableEffects();

#if(COMBO_DATA_VECTOR == 1)
									globalComboIndex = tempComboIndex;
									//if(0 <= globalComboIndex && globalComboIndex < comboList.size())
#elif(COMBO_DATA_ARRAY == 1)
									globalComboIndex = tempComboIndex;
									//if(0 <= globalComboIndex && globalComboIndex < comboList.size())
#elif(COMBO_DATA_MAP == 1)
									globalComboName = pedalUiRequestData;
#endif

#if(COMBO_DATA_VECTOR == 1  || COMBO_DATA_ARRAY == 1)
									if(debugOutput) cout << globalComboName << ":" << globalComboIndex << endl;
									if(procCont.load(globalComboIndex) >= 0)
#elif(COMBO_DATA_MAP == 1)
									if(debugOutput) cout << globalComboName << endl;
									//comboDataMap[globalComboName].loadComboStructFromName((char *)globalComboName.c_str());
									if(procCont.load(globalComboName) >= 0)
#endif
									{
										//comboDataArray[globalComboIndex].getPedalUi();
										comboDataMap[globalComboName].getPedalUi();
#if(COMBO_DATA_VECTOR == 1)
										pedalUi.sendComboUiData(comboDataVector[globalComboIndex].pedalUiJson);
#elif(COMBO_DATA_ARRAY == 1)
										pedalUi.sendComboUiData(comboDataArray[globalComboIndex].pedalUiJson);
#elif(COMBO_DATA_MAP == 1)
										pedalUi.sendComboUiData(comboDataMap[pedalUiRequestData].pedalUiJson);
#endif
										if(debugOutput) cout << "sent pedalUiJson" << endl;

										strcpy(ofxMainStatus,"combo running");
									}
									else
									{
										strcpy(ofxMainStatus,"load failed");
										status = -1;
									}
									procCont.enableEffects();
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
							//uint8_t colon = 0;
							uint8_t equals = 0;

							string effectString, paramString, valueString, procParamString;
							volatile int absParamIndex;
							volatile int valueIndex;
							int size = pedalUiRequestData.size();
							//colon = requestData.find(":");
							equals = pedalUiRequestData.find("=");
							if(equals < size ) // error check to make sure "colon" and "equals" aren't
								// screwed up.
							{
								//effectString = requestData.substr(0,colon);

								// get control parameter index and value
								paramString = pedalUiRequestData.substr(0,equals);
								valueString = pedalUiRequestData.substr(equals+1);
								absParamIndex = atoi(paramString.c_str());
								valueIndex = atoi(valueString.c_str());
								// find corresponding controller and enter index and value into Control struct
								// (control will write value to process parameter during process run)

#if(COMBO_DATA_VECTOR == 1)
								if((0 <= absParamIndex && absParamIndex < comboDataVector[globalComboIndex].controlParameterArray.size()) && (0 <= valueIndex && valueIndex <= 255))
#elif(COMBO_DATA_ARRAY == 1)
								if((0 <= absParamIndex && absParamIndex < comboDataArray[globalComboIndex].controlParameterArray.size()) && (0 <= valueIndex && valueIndex <= 255))
#elif(COMBO_DATA_MAP == 1)
								if((0 <= absParamIndex && absParamIndex < comboDataMap[globalComboName].controlParameterArray.size()) && (0 <= valueIndex && valueIndex < 100))
#endif
								{
#if(COMBO_DATA_VECTOR == 1)
									comboDataVector[globalComboIndex].updateControl(absParamIndex, valueIndex);
#elif(COMBO_DATA_ARRAY == 1)
									comboDataArray[globalComboIndex].updateControl(absParamIndex, valueIndex);
#elif(COMBO_DATA_MAP == 1)
									comboDataMap[globalComboName].updateControl(absParamIndex, valueIndex);
#endif
									//// enter into controlsStruct element

									procCont.updateControlParameter(absParamIndex, valueIndex);
									//comboData.updateProcess(absParamIndex, valueIndex);
									//procCont.updateProcessParameter(absParamIndex, valueIndex);
#if(pedalUiDbg >= 1)
									if(debugOutput) cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
									//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
									if(debugOutput) cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
#endif
									//updateCombo(paramIndex,valueIndex);
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
							//if(pedalUiRequestData.empty() == false)
							{
								bool validComboIndexName;

								//string tempComboName = pedalUiRequestData; // new combo data is saved here.
								procCont.disableEffects();
#if(COMBO_DATA_VECTOR == 1)
								//if(getComboVectorIndex(tempComboName) >= 0)
								{
									comboDataVector[globalComboIndex].saveCombo();//saveCombo(requestData);
									usleep(200000);
									validComboIndexName = true;
								}
								/*else
								{
									if(debugOutput) cout << "combo not found." << endl;
								}*/

#elif(COMBO_DATA_ARRAY == 1)
								//if(getComboArrayIndex(tempComboName) >= 0)
								{
									//if(debugOutput) cout << "loadComboStructFromName try #" << i << endl;
									comboDataArray[globalComboIndex].saveCombo();//saveCombo(requestData);
									usleep(200000);
									validComboIndexName = true;
								}
								/*else
								{
									if(debugOutput) cout << "combo not found." << endl;
								}*/
#elif(COMBO_DATA_MAP == 1)
								//if(comboDataMap.find(tempComboName) != comboDataMap.end())
								{
									comboDataMap[globalComboName].saveCombo();//saveCombo(requestData);
									usleep(200000);
									validComboIndexName = true;
								}
								/*else
								{
									if(debugOutput) cout << "invalid combo name: " << tempComboName << endl;
									validComboIndexName = false;
								}*/
#endif
								strcpy(ofxMainStatus,"combo saved");
								procCont.enableEffects();
							}
							/*else
							{
								if(debugOutput) cout << "no data given to save." << endl;
							}*/

						}
						else if(pedalUiRequestCommand.compare("getCurrentValues") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
#if(COMBO_DATA_VECTOR == 1)
							pedalUi.sendCurrentData(comboDataVector[globalComboIndex].unsortedParameterArray);
#elif(COMBO_DATA_ARRAY == 1)
							pedalUi.sendCurrentData(comboDataArray[globalComboIndex].unsortedParameterArray);
#elif(COMBO_DATA_MAP == 1)
							pedalUi.sendCurrentData(comboDataMap[globalComboName].unsortedParameterArray);
#endif
							//if(debugOutput) cout << "sent current data" << endl;
						}
						else if(pedalUiRequestCommand.compare("getCurrentStatus") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							if(pedalUiRequestData.compare("host") == 0)
							{
								hostGuiActive = true;
								if(hostUi.isConnected() == 0)
								{
									hostUi.connect();
								}
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
							//procCont.getOutputAmplitudes();
							//pedalUi->sendCurrentStatus(ofxMainStatus);
							//strcpy(ofxMainStatus," ");
							//if(debugOutput) cout << "sent current data" << endl;
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

			//baseUi.clearSharedMemorySection();
#if(pedalUiDbg >= 1)
        	//if(debugOutput) cout << "*********************************** END **********************************************" << endl;
#endif
			/*baseUi.dataProcessingStatus(1);
			baseUi.waitForAccessRelease();
			baseUi.dataProcessingStatus(0);*/

		}

        procCont.readFootswitches();
		//if(comboRunning == true)
		{
			//procCont.updateFootswitch(footswitchStatusArray);
		}

	}
	procCont.stop();

	stopCombo();


	return 0;
}



