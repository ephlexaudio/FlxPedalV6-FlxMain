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
#define hostUiDbg 0
#define pedalUiDbg 0


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
#include <hid.h>
#include "utilityFunctions.h"
#include "BaseUiInt.h"
#include "ComboDataInt.h"
#include "PedalUiInt.h"
#include "HostUiInt.h"
#include "mainFunctions.h"
#include "DataFuncts.h"
#include "GPIOClass.h"
#include "ProcessingControl.h"

using namespace std;

#define FILE_LENGTH 1000
#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define MCU_SHARED_MEMORY_SECTION_SIZE 4096

#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define CM0_SHARED_MEMORY_SECTION_SIZE 4096

#define HOST_SHARED_MEMORY_SECTION_ADDRESS 8192
#define HOST_SHARED_MEMORY_SECTION_SIZE 4096

#define HOST_SHARED_MEMORY_FILE_ADDRESS 32768
#define HOST_SHARED_MEMORY_FILE_SIZE 16000

#define HOST_UI_ACTIVE_COUNT 3000

#define OFX_MAIN_READY 45

struct linuxProcess {
	int pid;
	int pgid;
	char command[20];
};



std::vector<linuxProcess> linuxProcessList;

std::vector<string> componentDataList;
std::vector<string> componentNameList;
std::vector<string> comboNameList;



static void signal_handler(int sig)
{

	//system("echo \"0\" > /sys/class/gpio/gpio13/value");
	fprintf(stderr, "signal received:%d, OfxMain exiting ...\n", sig);
	exit(0);
}



ComboDataInt comboData;
//
int ofxPid;
int childPgid = 0;
int procCount = 0;
int effectCount = 0;
int comboIndex = 0;
unsigned int bufferSize = 0;
bool justPoweredUp = true;
bool comboRunning = false;
bool hostGuiActive = false;
extern GPIOClass footswitch[2];
int footswitchStatusArray[10] = {0,0,0,0,0,0,0,0,0,0};
int wrapperParamData[10] = {1,0,0,0,0,0,0,0,0,0};
bool inputsSwitched;

struct _jackParams jackParams;
struct _processingParams processingParams;
extern ProcessingControl procCont;

#define dbg 1
int main(int argc, char *argv[])
{
	cout << "starting..." << endl;
	int exit = 0;
	int count = 0;
	bool enteringLoop = false;
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
	long hostUiActiveCount = 0;
	char gpioStr[5];
	char simpleResponseStr[20];
	string comboName;

	BaseUiInt baseUi;

	PedalUiInt pedalUi(CM0_SHARED_MEMORY_SECTION_ADDRESS,CM0_SHARED_MEMORY_SECTION_SIZE,
			MCU_SHARED_MEMORY_SECTION_ADDRESS,MCU_SHARED_MEMORY_SECTION_SIZE);
	/*PedalUiInt *pedalUi = new PedalUiInt(CM0_SHARED_MEMORY_SECTION_ADDRESS,CM0_SHARED_MEMORY_SECTION_SIZE,
			MCU_SHARED_MEMORY_SECTION_ADDRESS,MCU_SHARED_MEMORY_SECTION_SIZE);*/
	string pedalUiRequest;
	string pedalUiRequestCommand;
	string pedalUiRequestData;
	bool pedalUiRequestIsValid = false;

	/*HostUiInt hostUi(CM0_SHARED_MEMORY_SECTION_ADDRESS,CM0_SHARED_MEMORY_SECTION_SIZE,
			HOST_SHARED_MEMORY_SECTION_ADDRESS,HOST_SHARED_MEMORY_SECTION_SIZE);*/
	HostUiInt hostUi;
	/*int hostUiFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);
	char hostUiRawRequestCharArray[16000];
	char hostUiRequestCharArray[16000];
	char hostUiResponseCharArray[16000];*/
	string hostUiRequest;
	string hostUiRequestCommand;
	string hostUiRequestData;
	hostUiRequest.resize(16000);
	hostUiRequestData.resize(16000);
	bool hostUiRequestIsValid = false;

	string footswitch1Status;
	string footswitch2Status;
	pedalUi.dataProcessingStatus(0);
	//pedalUi->dataProcessingStatus(0);
	char ofxMainStatus[15];
	GPIOClass ofxMainRdy = GPIOClass(OFX_MAIN_READY/*"13"*/);
	ofxMainRdy.export_gpio();
	strcpy(gpioStr,"out");
	gpioStatus = ofxMainRdy.setdir_gpio(gpioStr);
	gpioStatus = ofxMainRdy.setval_gpio(0/*"0"*/);
	char ofxParamString[200];
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

	printf("OfxMain PID: %d.\n", ofxPid);
	SharedMemoryInt sharedMem;

	//usbMain();
	/*UsbServerInt usb;

	usb.startServer();

	usb.stopServer();*/


	/******* Get JACK initialization data ***************/
	int ofxParamsFD = open("/home/ofxParams.txt",O_RDONLY);
	/* read file into temp string */
	read(ofxParamsFD, ofxParamString, 200);

	ofxParamJsonData.clear();
	if(ofxParamJsonReader.parse(ofxParamString, ofxParamJsonData) == false)
	{
		cout << "failed to read JACK initialization data file.\n" << endl;
	}
	jackParams.period = atoi(ofxParamJsonData["jack"]["period"].asString().c_str());
	bufferSize = jackParams.period;
	jackParams.buffer = atoi(ofxParamJsonData["jack"]["buffer"].asString().c_str());
	cout << "JACK period: " << jackParams.period << "\tJACK buffer: " << jackParams.buffer << endl;

	processingParams.noiseGate.highThres = atof(ofxParamJsonData["noiseGate"]["highThres"].asString().c_str());
	processingParams.noiseGate.lowThres = atof(ofxParamJsonData["noiseGate"]["lowThres"].asString().c_str());
	processingParams.noiseGate.gain = atof(ofxParamJsonData["noiseGate"]["gain"].asString().c_str());
	cout << "Noise gate low threshold: " << processingParams.noiseGate.lowThres << "\tNoise gate high threshold: " << processingParams.noiseGate.highThres << endl;
	processingParams.trigger.highThres = atof(ofxParamJsonData["trigger"]["highThres"].asString().c_str());
	processingParams.trigger.lowThres = atof(ofxParamJsonData["trigger"]["lowThres"].asString().c_str());
	cout << "Trigger low threshold: " << processingParams.trigger.lowThres << "\tTrigger high threshold: " << processingParams.trigger.highThres << endl;
	for(int i = 0; i < argc; i++)
	{
		printf("argument[%d]: %s\n", i, argv[i]);
	}

	if(argc > 1)
	{
		/*if(strcmp(argv[1],"-s") == 0)
		{
		}
		else*/
		{
			strcpy(initComboName, argv[1]);
		}
	}
	else
	{
		strcpy(initComboName, "eq");
	}



	/*****************END: Start up and init of pedal *******************/

	initializePedal(initComboName);
	/*****************START: Start up server and connect ***************/
	//componentList = getComponentList();
	//comboList = getComboList();


	char choice = 0;
	openJack();
	startJack();

	//std::string serverRequest;
	char serverRequestData[20];

	std::vector<string> pedalUiTestData;
	pedalUiTestData.push_back("listCombos");
	pedalUiTestData.push_back("getCombo:0eq");

	pinString = "1";

	clearBuffer(ofxMainStatus,15);
	strcpy(ofxMainStatus, "starting up");
	gpioStatus = ofxMainRdy.setval_gpio(1/*pinString*/);
	int smSectionIndex = 0;

	while(exit == 0)
	{
		//************* Process requests from Host UI Interface  *************
		/*clearBuffer(hostUiRawRequestCharArray,16000);
		clearBuffer(hostUiRequestCharArray,16000);
		clearBuffer(hostUiResponseCharArray,16000);
    	ssize_t size_read = read(hostUiFD, hostUiRawRequestCharArray, 16000);*/

        if(hostUi.checkForNewHostData() == 1)
        {
        	//enteringLoop = false;
        	hostUiRequest = hostUi.getUserRequest();
        	//cout << "usb data received: " << hostUiRequestCharArray << endl;
        	//cout << "usb data size: " << strlen(hostUiRequestCharArray) << endl;
        	/*cleanString(hostUiRawRequestCharArray,hostUiRequestCharArray);
        	strcpy(hostUiResponseCharArray,"received: ");
        	strncat(hostUiResponseCharArray, (const char*)hostUiRequestCharArray, 16000);
        	hostUiRequest = hostUiRequestCharArray;*/
#if(hostUiDbg >= 1)
			cout << "HOST UI REQUEST: " << hostUiRequest << endl;
			cout << "hostUiRequest size: " << hostUiRequest.size() << endl;
			/*for(int charIndex = 0; charIndex < hostUiRequest.size(); charIndex++)
			{
				cout << hostUiRequest[charIndex] << ',';
			}
			cout << endl;*/
#endif
			hostUiRequestIsValid = true;
			hostUiRequest.erase(remove(hostUiRequest.begin(), hostUiRequest.end(), '\n'), hostUiRequest.end());
			hostUiRequest.erase(std::remove(hostUiRequest.begin(), hostUiRequest.end(), '\r'), hostUiRequest.end());
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
				//cout << "hostUiRequest: " << hostUiRequest << endl;
				if(0 < colonPosition && colonPosition < hostUiRequest.length())
				{
					hostUiRequestCommand = hostUiRequest.substr(0,colonPosition);
					hostUiRequestData = hostUiRequest.substr(colonPosition+1);
				}
				else hostUiRequestCommand = hostUiRequest;

				if(hostUiRequestCommand.compare("listCombos") == 0)
				{
					//hostUi.sendComboUiData(combo.hostUiJson);
					comboNameList = getComboList();
					string comboNameArray;
					for(unsigned int i = 0; i < comboNameList.size(); i++)
					{
						comboNameArray.append(comboNameList.at(i).c_str());
						if(i < comboNameList.size()-1) comboNameArray.append(",");
					}
					hostUi.sendComboList(comboNameArray);
					//strcpy(hostUiResponseCharArray, comboNameArray.c_str());

#if(hostUiDbg == 1)
					cout << comboNameArray << endl;
					cout << "sent listCombos" << endl;
#endif
				}
				else if(hostUiRequestCommand.compare("getCombo") == 0)
				{
					//bypassAll();

					if(hostUiRequestData.size() > 1)
					{
						if(comboData.getCombo((char *)hostUiRequestData.c_str()) == 0)
						{
							if(comboRunning == true)
							{
								stopCombo();
								comboRunning = false;
							}
							//hostUiRequestData = string(initComboName);
#if(hostUiDbg == 1)
							cout << "requested Combo: " << hostUiRequestData << endl;
#endif
#if(hostUiDbg == 1)
							cout << "after getCombo: effectCount:"<<effectCount<<"\tprocCount:"<<procCount<<endl;
#endif
							//									if(comboData.getPedalUi() != 0)
							//									{
							//										status = 1;
							//									}
							//									else
							{
								hostUi.sendComboToHost(hostUiRequestData);
#if(hostUiDbg == 1)
								cout << "sent hostUiJson" << endl;
#endif
							}

							loadCombo();
							runCombo();
							sleep(1);
							comboRunning = true;
							//server.sendCurrentData();

							strcpy(ofxMainStatus,"combo running");
						}
						else
						{
							cout << "could not get combo" << endl;
						}
					}
				}
				else if(hostUiRequestCommand.compare("changeValue") == 0)
				{
					//uint8_t colon = 0;
					//uint8_t equals = 0;

					string procContString, paramString, valueString, procParamString;
					volatile int absParamIndex;
					volatile int valueIndex;
					//int size = hostUiRequestData.size();

					//colon = requestData.find(":");
					/*equals = hostUiRequestData.find("=");
					if(equals < size ) // error check to make sure "colon" and "equals" aren't
						// screwed up.*/
					if(hostUiJsonReader.parse(hostUiRequestData.c_str(),hostUiJsonData) == true)
					{
						//effectString = requestData.substr(0,colon);

						// get control parameter index and value
						/*paramString = hostUiRequestData.substr(0,equals);
						valueString = hostUiRequestData.substr(equals+1);
						absParamIndex = atoi(paramString.c_str());
						valueIndex = atoi(valueString.c_str());*/
						if(hostUiJsonData["control"].isNull() == false)
						{
							procContString = hostUiJsonData["control"].asString();
							paramString = hostUiJsonData["parameter"].asString();
							valueString = hostUiJsonData["value"].asString();

							cout << "control: " << procContString;
							cout << "\tparameter: " << paramString;
							cout << "\tvalue: " << valueString << endl;

							absParamIndex = comboData.getControlParameterIndex(procContString, paramString);
							valueIndex = atoi(valueString.c_str());
							if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
							{

								comboData.updateControl(absParamIndex, valueIndex);
								//// enter into controlsStruct element

								procCont.updateControlParameter(absParamIndex, valueIndex);
								//comboData.updateProcess(absParamIndex, valueIndex);
								//procCont.updateProcessParameter(absParamIndex, valueIndex);
								cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
								//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
	#if(hostUiDbg == 1)
								cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
	#endif
								//updateCombo(paramIndex,valueIndex);
							}
							else
							{
								cout << "absParamIndex and/or valueIndex are out of range." << endl;
							}


						}
						else if(hostUiJsonData["process"].isNull() == false)
						{
							procContString = hostUiJsonData["process"].asString();
							paramString = hostUiJsonData["parameter"].asString();
							valueString = hostUiJsonData["value"].asString();

							cout << "process: " << procContString;
							cout << "\tparameter: " << paramString;
							cout << "\tvalue: " << valueString << endl;

							absParamIndex = comboData.getProcessParameterIndex(procContString, paramString);
							valueIndex = atoi(valueString.c_str());
							if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
							{
								comboData.updateProcess(absParamIndex, valueIndex);
								//// enter into controlsStruct element

								procCont.updateProcessParameter(absParamIndex, valueIndex);
								//comboData.updateProcess(absParamIndex, valueIndex);
								//procCont.updateProcessParameter(absParamIndex, valueIndex);
								cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
								//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
	#if(hostUiDbg == 1)
								cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
	#endif
								//updateCombo(paramIndex,valueIndex);
							}
							else
							{
								cout << "absParamIndex and/or valueIndex are out of range." << endl;
							}
						}
						else
						{
							cout << "invalid value change request." << endl;
						}

						// find corresponding controller and enter index and value into Control struct
						// (control will write value to process parameter during process run)

						/*if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
						{
							comboData.updateControl(absParamIndex, valueIndex);
							//// enter into controlsStruct element

							procCont.updateControlParameter(absParamIndex, valueIndex);
							//comboData.updateProcess(absParamIndex, valueIndex);
							//procCont.updateProcessParameter(absParamIndex, valueIndex);
							cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
							//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
#if(dbg == 1)
							cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
#endif
							//updateCombo(paramIndex,valueIndex);
						}
						else
						{
							cout << "absParamIndex and/or valueIndex are out of range." << endl;
						}*/

					}
					strcpy(simpleResponseStr,"ChangedValue");
					hostUi.sendSimpleResponse(simpleResponseStr);
				}
				else if(hostUiRequestCommand.compare("saveCombo") == 0)
				{
					//comboData.saveCombo();//saveCombo(requestData);
					hostUi.getComboFromHost(hostUiRequestData);
					strcpy(simpleResponseStr,"SavedCombo");
					hostUi.sendSimpleResponse(simpleResponseStr);
					strcpy(ofxMainStatus,"combo saved");
				}
				else if(hostUiRequestCommand.compare("deleteCombo") == 0)
				{
					//comboData.saveCombo();//saveCombo(requestData);
					char cliString[50];
					sprintf(cliString, "rm /home/Combos/%s.txt", hostUiRequestData.c_str());
#if(hostUiDbg == 1)
					cout << "CLI string for delete: " << cliString << endl;
					cout << "delete result: " << strerror(system(cliString)) << endl;
#endif

					comboNameList = getComboList();
					string comboNameArray;
					for(unsigned int i = 0; i < comboNameList.size(); i++)
					{
						comboNameArray.append(comboNameList.at(i).c_str());
						if(i < comboNameList.size()-1) comboNameArray.append(",");
					}
					hostUi.sendComboList(comboNameArray);
					strcpy(ofxMainStatus,"combo deleted");
				}
				else if(hostUiRequestCommand.compare("getCurrentValues") == 0)
				{
					if(newDataFromServer)
					{
						newDataFromServer = false;
					}
					hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
					hostUi.sendCurrentData(comboData.unsortedParameterArray);
#if(pedalUiDbg >= 1)
					cout << "sent current data to HOST " << endl;
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
					hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
#if(pedalUiDbg >= 1)
					cout << "sent current status to HOST: " << ofxMainStatus << endl;
#endif
				}
				else if(hostUiRequestCommand.compare("hostUiActive") == 0)
				{
					hostUiActiveCount = HOST_UI_ACTIVE_COUNT;
				}

				else
				{
					cout << "sent nothing to host" << endl;
				}
			}
			else
			{
				cout << "hostUiRequest is invalid or corrupted." << endl;
			}
			hostUiRequest.clear();
			hostUiRequestCommand.clear();
			hostUiRequestData.clear();
			hostUiRequest.resize(16000);
			hostUiRequestData.resize(16000);

        }
		//************* Process requests from PCB UI Interface  *************

        else if(((smSectionIndex = baseUi.checkForNewData()) > 0))
        //else if(((smSectionIndex = pedalUi->checkForNewPedalData()) > 0))
		{
        	//enteringLoop = false;

#if(pedalUiDbg >= 1)
			//cout << "section index: " << smSectionIndex << endl;
#endif
			/*if(smSectionIndex == 2)
			{
				if(hostUi.checkForNewHostData() == 1)
				{
#if(dbg >= 1)
					cout << "*************** NEW DATA FROM HOST *****************" << endl;
#endif
					hostUi.dataProcessingStatus(0);
					hostUiRequest = hostUi.getUserRequest();
					// validate request (make sure it isn't corrupted)
					hostUiRequestIsValid = true;

#if(dbg >= 1)
					cout << "hostUiRequest size: " << hostUiRequest.size() << endl;
					for(int charIndex = 0; charIndex < hostUiRequest.size(); charIndex++)
					{
						cout << hostUiRequest[charIndex] << ',';
					}
					cout << endl;
#endif

					for(int charIndex = 0; charIndex < hostUiRequest.size(); charIndex++)
					{
						if(hostUiRequest[charIndex] < ' ' || '~' < hostUiRequest[charIndex])
						{
							hostUiRequestIsValid = false;
							break;
						}
					}

					if(hostUiRequestIsValid)
					{
						uint8_t colonPosition = hostUiRequest.find(":");
						//cout << "hostUiRequest: " << hostUiRequest << endl;
						if(0 < colonPosition && colonPosition < hostUiRequest.length())
						{
							hostUiRequestCommand = hostUiRequest.substr(0,colonPosition);
							hostUiRequestData = hostUiRequest.substr(colonPosition+1);
						}
						else hostUiRequestCommand = hostUiRequest;

						if(hostUiRequestCommand.compare("listCombos") == 0)
						{
#if(dbg == 1)
							cout << "HOST UI REQUEST: " << hostUiRequest << endl;
#endif
							//hostUi.sendComboUiData(combo.hostUiJson);
							comboNameList = getComboList();
							string comboNameArray;
							for(int i = 0; i < comboNameList.size(); i++)
							{
								comboNameArray.append(comboNameList.at(i).c_str());
								if(i < comboNameList.size()-1) comboNameArray.append(",");
							}
							hostUi.sendComboList(comboNameArray);

#if(dbg == 1)
							cout << comboNameArray << endl;
							cout << "sent listCombos" << endl;
#endif
						}
						else if(hostUiRequestCommand.compare("getCombo") == 0)
						{
#if(dbg == 1)
							cout << "HOST UI REQUEST: " << hostUiRequest << endl;
#endif
							//bypassAll();

							if(comboRunning == true)
							{
								stopCombo();
								comboRunning = false;
							}
							//hostUiRequestData = string(initComboName);
#if(dbg == 1)
							cout << "requested Combo: " << hostUiRequestData << endl;
#endif
							if(hostUiRequestData.size() > 1)
							{
								if(comboData.getCombo((char *)hostUiRequestData.c_str()) == 0)
								{
#if(dbg == 1)
									cout << "after getCombo: effectCount:"<<effectCount<<"\tprocCount:"<<procCount<<endl;
#endif
									//									if(comboData.getPedalUi() != 0)
									//									{
									//										status = 1;
									//									}
									//									else
									{
										hostUi.sendComboToHost(hostUiRequestData);
#if(dbg == 1)
										cout << "sent hostUiJson" << endl;
#endif
									}

									loadCombo();
									runCombo();
									sleep(1);
									comboRunning = true;
									//server.sendCurrentData();

									strcpy(ofxMainStatus,"combo running");
								}
								else
								{
									cout << "could not get combo" << endl;
								}
							}
						}
						else if(hostUiRequestCommand.compare("changeValue") == 0)
						{
#if(dbg == 1)
							cout << "HOST UI REQUEST: " << hostUiRequest << endl;
#endif
							uint8_t colon = 0;
							uint8_t equals = 0;

							string effectString, paramString, valueString, procParamString;
							volatile int absParamIndex;
							volatile int valueIndex;
							int size = hostUiRequestData.size();
							//colon = requestData.find(":");
							equals = hostUiRequestData.find("=");
							if(equals < size ) // error check to make sure "colon" and "equals" aren't
								// screwed up.
							{
								//effectString = requestData.substr(0,colon);

								// get control parameter index and value
								paramString = hostUiRequestData.substr(0,equals);
								valueString = hostUiRequestData.substr(equals+1);
								absParamIndex = atoi(paramString.c_str());
								valueIndex = atoi(valueString.c_str());
								// find corresponding controller and enter index and value into Control struct
								// (control will write value to process parameter during process run)

								if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
								{
									comboData.updateControl(absParamIndex, valueIndex);
									//// enter into controlsStruct element

									procCont.updateControlParameter(absParamIndex, valueIndex);
									//comboData.updateProcess(absParamIndex, valueIndex);
									//procCont.updateProcessParameter(absParamIndex, valueIndex);
									cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
									//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
#if(dbg == 1)
									cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
#endif
									//updateCombo(paramIndex,valueIndex);
								}
								else
								{
									cout << "absParamIndex and/or valueIndex are out of range." << endl;
								}

							}
						}
						else if(hostUiRequestCommand.compare("saveCombo") == 0)
						{
#if(dbg == 1)
							cout << "HOST UI REQUEST: " << hostUiRequestData << endl;
#endif
							//comboData.saveCombo();//saveCombo(requestData);
							hostUi.getComboFromHost(hostUiRequestData);
							strcpy(ofxMainStatus,"combo saved");
						}
						else if(hostUiRequestCommand.compare("deleteCombo") == 0)
						{
#if(dbg == 1)
							cout << "HOST UI REQUEST: " << hostUiRequestData << endl;
#endif
							//comboData.saveCombo();//saveCombo(requestData);
							char cliString[50];
							sprintf(cliString, "rm /home/Combos/%s.txt", hostUiRequestData.c_str());
#if(dbg == 1)
							cout << "CLI string for delete: " << cliString << endl;
#endif
							cout << "delete result: " << strerror(system(cliString)) << endl;

//							comboNameList = getComboList();
//							string comboNameArray;
//							for(int i = 0; i < comboNameList.size(); i++)
//							{
//								comboNameArray.append(comboNameList.at(i).c_str());
//								if(i < comboNameList.size()-1) comboNameArray.append(",");
//							}
//							hostUi.sendComboList(comboNameArray);
							strcpy(ofxMainStatus,"combo deleted");
						}
						else if(hostUiRequestCommand.compare("getCurrentValues") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							hostUi.sendCurrentData(comboData.unsortedParameterArray);
							//cout << "sent current data" << endl;
						}
						else if(hostUiRequestCommand.compare("getCurrentStatus") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							hostUi.sendCurrentStatus(ofxMainStatus);
							strcpy(ofxMainStatus," ");
							//cout << "sent current data" << endl;
						}
						else
						{
							cout << "sent nothing to host" << endl;
						}
						cout << "Host request processed" << endl;
					}
					else
					{
						cout << "hostUiRequest is invalid or corrupted." << endl;
					}

					hostUiRequest.clear();
					hostUiRequestCommand.clear();
					hostUiRequestData.clear();

					//					// since hostUi and pedalUi classes are based off of shared memory class,
					//					// hostUi dataProcessingStatus and checkForNewData are already done with pedalUi below
					//					hostUi.dataProcessingStatus(1);
					//		#if(testing == 0)
					//					while(hostUi.checkForNewHostData() == 1)
					//					{;}
					//		#endif
					//					hostUi.dataProcessingStatus(0);

					//					//hostUi.clearSharedMemorySection();
					//
					//					hostUi.dataProcessingStatus(1);
					//					hostUi.waitForAccessRelease();
					//					hostUi.dataProcessingStatus(0);
				}
#define dbg 1
			}
			else if(smSectionIndex == 1)*/
			{
				if((pedalUi.checkForNewPedalData() == 1))
				//if((pedalUi->checkForNewPedalData() == 1))
				{


#if(pedalUiDbg >= 1)
					cout << "************ NEW DATA FROM PEDAL ****************" << endl;
#endif

					pedalUi.dataProcessingStatus(0);
					//pedalUi->dataProcessingStatus(0);
					/*if(enteringLoop)
					{
						pedalUiRequest = "getCombo:";
						pedalUiRequest.append(initComboName);
						enteringLoop = false;
					}
					else*/
					{
						pedalUiRequest = pedalUi.getUserRequest();
						//pedalUiRequest = pedalUi->getUserRequest();
					}

#if(pedalUiDbg >= 1)
					cout << "PEDAL UI REQUEST: " << pedalUiRequest << endl;
					cout << "pedalUiRequest size: " << pedalUiRequest.size() << endl;
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
						//cout << "pedalUiRequest: " << pedalUiRequest << endl;
						if(0 < colonPosition && colonPosition < pedalUiRequest.length())
						{
							pedalUiRequestCommand = pedalUiRequest.substr(0,colonPosition);
							pedalUiRequestData = pedalUiRequest.substr(colonPosition+1);
						}
						else pedalUiRequestCommand = pedalUiRequest;

						if(pedalUiRequestCommand.compare("listCombos") == 0)
						{
							//pedalUi.sendComboUiData(combo.pedalUiJson);
							comboNameList = getComboList();
							string comboNameArray;
							for(std::vector<string>::size_type i = 0; i < comboNameList.size(); i++)
							{
								comboNameArray.append(comboNameList.at(i).c_str());
								if(i < comboNameList.size()-1) comboNameArray.append(",");
							}
							pedalUi.sendComboList(comboNameArray);
							//pedalUi->sendComboList(comboNameArray);

#if(pedalUiDbg >= 1)
							cout << comboNameArray << endl;
							cout << "sent listCombos" << endl;
#endif
						}
						else if(pedalUiRequestCommand.compare("getCombo") == 0)
						{
							//bypassAll();

							//pedalUiRequestData = string(initComboName);
							if(pedalUiRequestData.size() > 1)
							{
								if(comboData.getCombo((char *)pedalUiRequestData.c_str()) == 0)
								{
									if(comboData.getPedalUi() != 0)
									{
										status = 1;
									}
									else
									{
										pedalUi.sendComboUiData(comboData.pedalUiJson);
										//pedalUi->sendComboUiData(comboData.pedalUiJson);
										cout << "sent pedalUiJson" << endl;
									}

									if(comboRunning == true)
									{
										stopCombo();
										comboRunning = false;
									}
									loadCombo();
									runCombo();
									sleep(1);
									comboRunning = true;
									//server.sendCurrentData();

									strcpy(ofxMainStatus,"combo running");
								}
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

								if((absParamIndex >= 0) && (0 <= valueIndex && valueIndex <= 255))
								{
									comboData.updateControl(absParamIndex, valueIndex);
									//// enter into controlsStruct element

									procCont.updateControlParameter(absParamIndex, valueIndex);
									//comboData.updateProcess(absParamIndex, valueIndex);
									//procCont.updateProcessParameter(absParamIndex, valueIndex);
#if(pedalUiDbg >= 1)
									cout << "paramString: " << paramString << '\t' << "valueString: " << valueString << endl;
									//sprintf(procParamString, "%s:%d", paramName.c_str(), valueIndex);
									cout << "controlParamString: " << paramString.c_str() << ":" << valueIndex << endl;
#endif
									//updateCombo(paramIndex,valueIndex);
								}
								else
								{
									cout << "absParamIndex and/or valueIndex are out of range." << endl;
								}
							}
						}
						else if(pedalUiRequestCommand.compare("saveCombo") == 0)
						{
							comboData.saveCombo();//saveCombo(requestData);
							strcpy(ofxMainStatus,"combo saved");
						}
						else if(pedalUiRequestCommand.compare("getCurrentValues") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							pedalUi.sendCurrentData(comboData.unsortedParameterArray);
							//pedalUi->sendCurrentData(comboData.unsortedParameterArray);
							//cout << "sent current data" << endl;
						}
						else if(pedalUiRequestCommand.compare("getCurrentStatus") == 0)
						{
							if(newDataFromServer)
							{
								newDataFromServer = false;
							}
							pedalUi.sendCurrentStatus(ofxMainStatus);
							//pedalUi->sendCurrentStatus(ofxMainStatus);
							//strcpy(ofxMainStatus," ");
							//cout << "sent current data" << endl;
						}
						else if(pedalUiRequestCommand.compare("powerOff") == 0)
						{
							system("poweroff");
							exit = 1;  // not sure what real effect this will have,
									   // since it comes after poweroff
						}

						else
						{
							cout << "sent nothing to pedal" << endl;
						}
					}
					else
					{
						cout << "pedalUiRequest is invalid or corrupted." << endl;
					}

					pedalUiRequest.clear();
					pedalUiRequestCommand.clear();
					pedalUiRequestData.clear();
					//					pedalUi.clearSharedMemorySection();

					//					pedalUi.dataProcessingStatus(1);
					//					pedalUi.waitForAccessRelease();
					//					pedalUi.dataProcessingStatus(0);
				}
			}

			//baseUi.clearSharedMemorySection();

			baseUi.dataProcessingStatus(1);
			baseUi.waitForAccessRelease();
			baseUi.dataProcessingStatus(0);
			/*pedalUi->dataProcessingStatus(1);
			pedalUi->waitForAccessRelease();
			pedalUi->dataProcessingStatus(0);*/
		}




		/*cout << "********************* PROCESS DATA BUFFERS ****************************" << endl;
		for(int procIndex = 0; procIndex < procCount; procIndex++)
		{
			procCont.getProcessData(procIndex, dataBuffer);
			cout << "process[" << procIndex << "]: ";
			for(int i = 0; i < 256; i += 2)
			{
				cout << dataBuffer[i] << ",";
			}
			cout << endl;

			if(getProcessDataPassCount > 10)
			{
				procCont.clearProcessData(procIndex, dataBuffer);
				getProcessDataPassCount = 0;
			}
			else getProcessDataPassCount++;
		}


		cout << "********************* end PROCESS DATA BUFFERS ****************************" << endl;
		 */

		if(hostUiActiveCount > 0)
		{
			//cout << "hostUiActiveCount: " << hostUiActiveCount << endl;
			hostUiActiveCount--;
			hostGuiActive = true;
		}
		else
		{
			if(hostGuiActive == true)
			{
				cout << "**** HOST GUI DE-ACTIVATED." << endl;
			}
			hostGuiActive = false;
		}
		readFootswitches();
		if(comboRunning == true)
		{
			procCont.updateFootswitch(footswitchStatusArray);
		}

		/*for(int i = 0; i < procIntArray.size(); i++)
		{
			procIntArray.at(i).updateFootswitchStatus(footswitchStatusArray);
		}*/
	}
	stopCombo();

	//#if(testingPedalUi == 1)


	return 0;
}



