/*
 * mainFunctions.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: mike
 */

#include "config.h"
//#include "ProcessInt.h"
//#include "ConnectionCont.h"
#include "ComboDataInt.h"
//#include "PedalUiInt.h"
#include "FileSystemFuncts.h"
#include "utilityFunctions.h"
#include "mainFunctions.h"

#include "ProcessingControl.h"
//#include "Server.h"
//CONNECTION connArray;
GPIOClass footswitch[2];
#define FOOTSWITCH_COUNT 2

#define FOOTSWITCH_1 6
#define FOOTSWITCH_2 12

/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1*/

using namespace std;

#define dbg 0

struct linuxProcess {
	int pid;
	int pgid;
	char command[20];
};
int jackPid;

int footswitchBypassAll[10] = {0,0,0,0,0,0,0,0,0,0};


jackctl_server_t *ofxJackServer;
const JSList *ofxJackParameters;
const JSList *ofxJackDrivers;
const JSList *ofxJackInternals;
const JSList *node_ptr;
jackctl_sigmask_t *sigmask;


jack_client_t *ofxJackClient;

jack_status_t jackStatus;
const char **ports;
std::vector<jack_port_t *> portList;

//extern void setupGpio(void);
extern std::vector<linuxProcess> linuxProcessList;
#define FILE_LENGTH 1000
extern jack_client_t *ofxJackClient;

//extern ComboDataInt comboData;
//extern PedalUiInt pedalUi;
//extern ConnectionCont connCont;
//extern std::vector<string> componentList;
#if(COMBO_DATA_VECTOR == 1)
extern std::vector<string> comboList;//comboVectorList;
#elif(COMBO_DATA_ARRAY == 1)
extern std::vector<string> comboList;
#elif(COMBO_DATA_MAP == 1)
extern std::vector<string> comboList;
#endif

#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions:::" << status << endl;
#endif


//extern std::vector<ProcessInt> procIntArray;
extern int ofxPid;
extern int childPgid;
extern int procCount;
extern bool startUp;
extern int footswitchStatusArray[10];
extern int effectCount;
extern int wrapperParamData[10];
ProcessingControl procCont;

#if(COMBO_DATA_VECTOR == 1)
	extern vector<ComboDataInt> comboDataVector;
#elif(COMBO_DATA_ARRAY == 1)
	extern ComboDataInt comboDataArray[15];
#elif(COMBO_DATA_MAP == 1)
	extern map<string, ComboDataInt> comboDataMap;
#endif

extern struct _jackParams jackParams;
extern int comboDataCount;
/*extern void *toProcessMemory;
extern void *fromProcessMemory;
extern int toProcFD;
extern int fromProcFD;*/


#define dbg 0



int openJack(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::openJack" << endl;
#endif


	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4E},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x00},	// disable DAC soft mute
			{0x0C ,0x5F},	// disabling POWEROFF
			{0x0C ,0x5E},	// powering line inputs
			{0x0C ,0x5A},	// powering ADC
			{0x0C ,0x52},	// powering DAC
			{0x0C ,0x42},	// powering outputs
			{0x12 ,0x01}	// setting to active
	};
	uint8_t i2cInitLength = 13;

	int i2cDevFile;
	int i2cAdapter=1;
	char i2cDevFileName[20];
	uint8_t i2cBuffer[2];
	uint8_t i2cStatus = 0;

    int pipe_stdin[2], pipe_stdout[2];
    int p,p2;


    cout << "jackctl_server_create" << endl;
    ofxJackServer = jackctl_server_create(NULL, NULL);

	//*************  Finish setting codec registers ************
	  snprintf(i2cDevFileName, 19, "/dev/i2c-%d", i2cAdapter);
	  i2cDevFile = open(i2cDevFileName, O_WRONLY);
	  if (i2cDevFile < 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg==1)
		  printf("error creating I2C device file.\n");
#endif
		  status = 1;
	  }
	  int addr = 0x1a; // The I2C address

	  if((ioctl(i2cDevFile, I2C_SLAVE_FORCE, addr) < 0) && status == 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg==1)
		  printf("error opening I2C device file: %d\n", errno);
#endif
		  status = 1;
	  }

	  if(status == 0)
	  {
		  do{
			  errno = 0;
			  i2cStatus = write(i2cDevFile,i2cInitSequence[0],2);
#if(dbg==1)
			  printf("i2c deactivate result: %d\n", errno);
#endif
			  nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);

		  }while ((errno != 0));


		  for(uint8_t i = 1; i < i2cInitLength; i++)
		  {
			  i2cStatus = write(i2cDevFile,i2cInitSequence[i],2);
#if(dbg==1)
			  printf("i2c init result: %d\n", errno);
#endif

		  } //while(i2cStatus < 2);
	  }
	  close(i2cDevFile);
#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::openJack:" << status << endl;
#endif

    return status;
}

#define dbg 1
int startJack(void)
{
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::startJack" << endl;
#endif


	cout << "mainFunctions JACK period: " << jackParams.period << "\tJACK buffer: " << jackParams.buffer << endl;

	char jackInitString[100];
	sprintf(jackInitString, "jackd -d alsa -p %d -n %d &", jackParams.period, jackParams.buffer);
	system(jackInitString);
	//system("jackd -d alsa &");
	sleep(2);
	cout << "setting up I2C port..." << endl;
	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4A},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x00},	// disable DAC soft mute
			{0x0C ,0x5F},	// disabling POWEROFF
			{0x0C ,0x5E},	// powering line inputs
			{0x0C ,0x5A},	// powering ADC
			{0x0C ,0x52},	// powering DAC
			{0x0C ,0x42},	// powering outputs
			{0x12 ,0x01}	// setting to active
	};
	uint8_t i2cInitLength = 13;

	int i2cDevFile;
	int i2cAdapter=1;
	char i2cDevFileName[20];
	uint8_t i2cBuffer[2];
	uint8_t i2cStatus = 0;

    /*cout << "jackctl_server_create" << endl;
    ofxJackServer = jackctl_server_create(NULL, NULL);*/

	//*************  Finish setting codec registers ************
	  snprintf(i2cDevFileName, 19, "/dev/i2c-%d", i2cAdapter);
	  i2cDevFile = open(i2cDevFileName, O_WRONLY);
	  if (i2cDevFile < 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg==1)
		  printf("error creating I2C device file.\n");
#endif
		  status = 1;
	  }
	  int addr = 0x1a; // The I2C address

	  if((ioctl(i2cDevFile, I2C_SLAVE_FORCE, addr) < 0) && status == 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg==1)
		  printf("error opening I2C device file: %d\n", errno);
#endif
		  status = 1;
	  }

	  if(status == 0)
	  {
		  do{
			  errno = 0;
			  i2cStatus = write(i2cDevFile,i2cInitSequence[0],2);
#if(dbg==1)
			  printf("i2c deactivate result: %d\n", errno);
#endif
			  nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);

		  }while ((errno != 0));


		  for(uint8_t i = 1; i < i2cInitLength; i++)
		  {
			  i2cStatus = write(i2cDevFile,i2cInitSequence[i],2);
#if(dbg==1)
			  printf("i2c init result: %d\n", errno);
#endif

		  } //while(i2cStatus < 2);
	  }
	  close(i2cDevFile);
#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::startJack:" << status << endl;
#endif
	return status;

}

int stopJack(void)
{
	int status = 0;

	status = system("killall -9 jackd");

    return status;
}

int closeJack(void)
{
	int status = 0;

    //jackctl_server_destroy(ofxJackServer);

	status = system("killall -9 jackd");

	//sleep(1);
    return status;
}

#define dbg 0
int initializePedal(char *fileName)
{
	int status = 0;
	char gpioStr[5];
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::initializePedal" << endl;
	cout << "fileName: " << fileName << endl;
#endif



#if(dbg==1)
	printf("Getting combo data from file...\n");
#endif


	/*if(status == 0)
	{
		comboData.getCombo(fileName);
		if(comboData.getPedalUi() != 0)
		{
			status = 1;
		}
	}*/

	/*************  Initialize PCB UI Interface  *************/

	/*************  Initialize Footswitches  *************/
	strcpy(gpioStr,"in");
	footswitch[0] = GPIOClass(FOOTSWITCH_1/*"4"*/);//, "in");
	footswitch[0].export_gpio();
	footswitch[0].setdir_gpio(gpioStr);

	footswitch[1] = GPIOClass(FOOTSWITCH_2/*"5"*/);//, "out");
	footswitch[1].export_gpio();
	footswitch[1].setdir_gpio(gpioStr);


	char spiData[10];


	  system("rm ipc_*");
#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::initializePedal:" << status << endl;
#endif

	  return status;
}



#define dbg 1
int getRunningProcesses(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::getRunningProcesses" << endl;
#endif


	char processDescriptionStr[100];

	for(int i = 0; i < 100; i++) processDescriptionStr[i] = 0;

	FILE *getProcFD = popen("ps -o pid,comm,pgid","r");
	//FILE *getProcFD = popen("ps -o pid","r");
	linuxProcessList.clear();
	while(fgets(processDescriptionStr, 100, getProcFD) != NULL)
	{

		if(processDescriptionStr[0] != 'P')
		{
			int state = 0;
			int indexOffset = 0;
			char pidStr[6] = " ";
			int pidStrIndex = 0;
			char comStr[20] = " ";
			int comStrIndex = 0;
			char pgidStr[6] = " ";
			int pgidStrIndex = 0;

			//puts(processDescriptionStr);
			for(int procDescStrIndex = 0; procDescStrIndex < 100; procDescStrIndex++)
			{
				if(processDescriptionStr[procDescStrIndex] == 0)
				{
					processDescriptionStr[procDescStrIndex] = ' ';
				}

				if(processDescriptionStr[procDescStrIndex] == ' ')
				{
					indexOffset = procDescStrIndex;
				}
				else
				{
					if(state == 0)
					{

						pidStr[pidStrIndex++] = processDescriptionStr[procDescStrIndex];

						if(processDescriptionStr[procDescStrIndex+1] == ' ')
						{
							state = 1;
						}
					}
					else if(state == 1)
					{

						comStr[comStrIndex++] = processDescriptionStr[procDescStrIndex];
						if(processDescriptionStr[procDescStrIndex+1] == ' ')
						{
							state = 2;
						}
					}
					else if(state == 2)
					{
						pgidStr[pgidStrIndex++] = processDescriptionStr[procDescStrIndex];
						if(processDescriptionStr[procDescStrIndex+1] == '\n')
						{
							state = 3;
						}
					}
					else if(state == 3)
					{
						//printf("pid: %s \tcomm: %s\tpgid: %s\n", pidStr, comStr, pgidStr);
						linuxProcess tempProcess;
						tempProcess.pid = atoi(pidStr);
						for(int i = 0; i < 20; i++) tempProcess.command[i] = 0;
						strcpy(tempProcess.command,comStr);
						tempProcess.pgid = atoi(pgidStr);
						//printf("pid: %d \tcomm: %s\tpgid: %d\n", tempProcess.pid, tempProcess.command, tempProcess.pgid);
						if(strcmp(tempProcess.command,"OfxMain") == 0)
						{
							childPgid = tempProcess.pid;
						}
						else if(childPgid != 0 && tempProcess.pgid == childPgid) // every process in linuxProcessList will be shutdown.
																	// keep OfxMain out of it.
						{
							linuxProcessList.push_back(tempProcess);
						}
						processDescriptionStr[procDescStrIndex] = 0;
						state = 0;
						break;

					}
				}
			}
		}
		else for(int i = 0; i < 100; i++) processDescriptionStr[i] = 0;
	}

	  /*for (std::vector<linuxProcess>::iterator it = linuxProcessList.begin() ; it != linuxProcessList.end(); ++it)
	  {
		  linuxProcess proc = *it;
		  printf("pid: %d \tcomm: %s\tpgid: %d\n", proc.pid, proc.command, proc.pgid);

	  }*/


	  for(std::vector<linuxProcess>::size_type i = 0; i < linuxProcessList.size(); i++)
	  {
		  linuxProcess proc = linuxProcessList.at(i);
#if(dbg==1)
		  printf("pid: %d \tcomm: %s\tpgid: %d\tpriority: %d\n", proc.pid, proc.command, proc.pgid, getpriority(PRIO_PROCESS,proc.pid));
#endif
	  }
	  cout << "********************************************" << endl;
	  //printf("pid: %d \tcomm: %s\tpgid: %d\tpriority: %d\n", ofxPid, "OfxMain", getpriority(PRIO_PROCESS,ofxPid));
	  cout << "pid: " << ofxPid << " \tcomm:  OfxMain\tpriority: " << getpriority(PRIO_PROCESS,ofxPid) << endl;
	  printf("********************************************\n");
	  /*for(int i = 0; i < linuxProcessList.size(); i++)
	  {
		  linuxProcess proc = linuxProcessList.at(i);
	  }*/

	  if(getProcFD != NULL) pclose(getProcFD);
#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::getRunningProcesses:" << status << endl;
#endif
	return status;
}



#define dbg 1
/*int loadCombo(void)
{
	int status = 0;
	startTimer();
	//procCount = combo.effectComboJson["processes"].size();

#if(dbg==1)
	printf("Getting combo data from file...\n");
#endif
	char pedalUiStr[10];
#if(dbg==1)
	printf("combo title: %s\n", (const char*)(comboData.pedalUiJson["title"].asString().c_str()));
#endif
	effectCount = comboData.pedalUiJson["effects"].size();
#if(dbg==1)
	printf("number of effects:%d\n", effectCount);
#endif

	if(status == 0)
	{
		if(comboData.getConnections2() != 0)
		{
			status = -1;
#if(dbg==1)
			printf("getConnections failed.\n");
#endif
		}
		else
		{
#if(dbg==1)
			printf("number of connections:%d\n", comboData.connectionsJson.size());
#endif
		}
	}
#if(dbg==1)
	else printf("skipping getConnections.\n");
#endif


	if(status == 0)
	{
		if(comboData.getProcesses() != 0)
		{
			status = -1;
#if(dbg==1)
			printf("getProcesses failed.\n");
#endif
		}
		else
		{
#if(dbg==1)
			printf("number of processes:%d\n", comboData.processesStruct.size());
#endif
		}
	}
	if(status == 0)
	{
		if(comboData.getControlConnections() != 0)
		{
			status = -1;
#if(dbg==1)
			printf("getControlConnections failed.\n");
#endif
		}
		else
		{
#if(dbg==1)
			printf("number of controlConnections:%d\n", comboData.controlConnectionsStruct.size());
#endif
		}
	}
	if(status == 0)
	{
		if(comboData.getControls() != 0)
		{
			status = -1;
#if(dbg==1)
			printf("getControls failed.\n");
#endif
		}
		else
		{
#if(dbg==1)
			printf("number of controls:%d\n", comboData.controlsStruct.size());
#endif
		}
	}


#if(dbg==1)
	else printf("skipping getProcesses.\n");
#endif
	if(status == 0)
	{
		if(procCont.load(comboData.processesStruct, comboData.connectionsJson, comboData.controlsStruct, comboData.controlConnectionsStruct) != 0)//loadCombo();
		{
			status = -1;
		}
	}


	stopTimer("loadCombo");
	return status;
}*/

// run and connect processes
#define dbg 0
int runCombo(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::runCombo" << endl;
#endif

	//int procCount = 0;
	char procOutput[15];
	int connCount = 0;
	startTimer();
	//startJack();

	procCont.start();

	//sleep(1);
	int tempFootswitchStatus[] = {1,1,1,0,0,0,0,0,0,0};
	procCont.updateFootswitch(tempFootswitchStatus);

	/*************  Create interface object for each process  *************/
//#if(dbg==1)
	printf("Creating interface object for each process...\n");
//#endif

	char name[15];
	char fileName[15];

	stopTimer("start processes");
	//sleep(1);
	/******************************Write to processes*******************************/

	startTimer();

	int j = 20;//for(int j = 0; j < 1; j++)

	stopTimer("write processes");

	startTimer();

	stopTimer("getRunningProcesses");
	//cout << "runCombo end connectionArray size: " << connCont.connectionArray.size() << endl;

#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::runCombo:" << status << endl;
#endif
	return status;
}

/*int updateCombo(int paramIndex, int paramValueIndex)
{
	int status = 0;
	procCont.updateParameter(paramIndex,paramValueIndex);

	return status;
}*/


#define dbg 1
int stopCombo(void)
{
	//cout << "stopCombo connectionArray size: " << connCont.connectionArray.size() << endl;

	//cout << "closing dummy client: " << jack_client_close(ofxJackClient) << endl;

	printf("stopCombo\n");
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::stopCombo" << endl;
#endif

	//int procCount = 0;
	char procOutput[15];
	int connCount = 0;
	stopJack();
	//procCont.stop();//stopCombo();
	sleep(1);
	char disconnString[100];
	startTimer();
	//procCount = 1;//combo.effectComboJson["processes"].size();



	  stopTimer("stopCombo");
#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::stopCombo:" << status << endl;
#endif
	return status;
}

#define dbg 1
ComboDataInt getComboObject(string comboName)
{
	ComboDataInt comboObject;
	ComboDataInt nullObject;
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::getComboObject" << endl;
	cout << "comboName: " << comboName << endl;
#endif

	status = comboObject.loadComboStructFromName((char *)comboName.c_str());


	//if(status >= 0)
	{
#if(dbg >= 1)
		cout << "***** EXITING mainFunctions::getComboObject: " << comboObject.comboName << endl;
#endif
		return comboObject;
	}
	/*else
	{
#if(dbg >= 1)
		cout << "***** EXITING mainFunctions::getComboObject: " << nullObject.comboName << endl;
#endif
		return nullObject;
	}*/
}


#define dbg 1
int listComboMapObjects(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING listComboMapObjects" << endl;
#endif

	for(vector<string>::size_type comboListIndex = 0; comboListIndex < comboList.size(); comboListIndex++)
	{
		cout << "name: " << comboList[comboListIndex] << endl;
	}

#if(dbg >= 1)
	cout << "***** EXITING listComboMapObjects: " << status << endl;
#endif
	return status;
}


#if(COMBO_DATA_VECTOR == 1)
	#define dbg 1
	int loadComboStructVectorAndList()
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "*****ENTERING loadComboStructVector" << endl;
	#endif

		FILE *fdComboList = popen("ls /home/Combos","r");
		char buffer[20];

		for(int i = 0; i<20;i++)
		{
			buffer[i] = 0;
		}


		if(fdComboList == NULL)
		{
	#if(dbg>=2)
			cout << "popen failed." << endl;
	#endif
			status = 1;
		}
		else
		{
			fflush(fdComboList);
			while(fgets(buffer,20,fdComboList) != NULL)
			{
				strcpy(buffer,strtok(buffer,"."));
				addComboStructToVector(string(buffer));
				for(int i = 0; i<20;i++) buffer[i] = 0;
			}
		}

	#if(dbg >= 1)
		cout << "***** EXITING loadComboStructVector" << endl;
	#endif
		return status;
	}


	std::vector<string> getComboVectorList(void)
	{
		std::vector<string> tempComboList;

	#if(dbg >= 1)
		cout << "*****ENTERING getComboVectorList" << endl;
	#endif

		for(int comboVectorIndex = 0; comboVectorIndex < comboDataVector.size(); comboVectorIndex++)
		{
			cout << "combo name: " << comboDataVector[comboVectorIndex].comboName << endl;
			if(comboDataVector[comboVectorIndex].comboName.empty() == false)
			{
				tempComboList.push_back(comboDataVector[comboVectorIndex].comboName);
			}
			else
			{
				cout << "combo name is empty." << endl;
			}
		}

	#if(dbg >= 1)
		cout << "***** EXITING getComboVectorList" << endl;
	#endif
		return tempComboList;
	}

	#define dbg 1
	int getComboVectorIndex(string comboName)
	{
		int index;
	#if(dbg >= 1)
		cout << "*****ENTERING getComboVectorIndex" << endl;
	#endif

		for(int i = 0; i < comboDataVector.size(); i++)
		{
			if(comboDataVector[i].comboName.empty() == false)
			{
				cout << "comparing: " << comboDataVector[i].comboName << " with " << comboName << endl;
				if(comboDataVector[i].comboName.compare(comboName) == 0)
				{
	#if(dbg >= 1)
					cout << "EXITING getComboVectorIndex." << endl;
	#endif
					return i;
				}
			}
			else
			{
				cout << "combo name was empty." << endl;
			}
		}
	#if(dbg >= 1)
		cout << "***** EXITING getComboVectorIndex" << endl;
	#endif
		return -1;
	}

	#define dbg 1
	int addComboStructToVectorAndList(string comboName)
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "*****ENTERING addComboStructToVector" << endl;
	#endif
		ComboDataInt tempCombo = getComboObject(comboName);
		if(tempCombo.comboName.empty() == false)
		{
			comboDataVector.push_back(tempCombo);
		}
		else
		{
			cout << "failed to add combo object: " << comboName << endl;
			status = -1;
		}

	#if(dbg >= 1)
		cout << "***** EXITING addComboStructToVector" << endl;
	#endif
		return status;
	}

	#define dbg 1
	int deleteComboStructFromVectorAndList(char *comboName)
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "*****ENTERING deleteComboStructFromVector" << endl;
	#endif
		int index = 0;

		index = getComboVectorIndex(comboName);
		if(index >= 0)
		{
			comboDataVector.erase(comboDataVector.begin() + index);
		}
		else
		{
			cout << "combo object not found:" << comboName << endl;
			status = -1;
		}

	#if(dbg >= 1)
		cout << "***** EXITING deleteComboStructFromVector" << endl;
	#endif
		return status;
	}

#elif(COMBO_DATA_ARRAY == 1)
	#define dbg 1
	int getComboArrayIndex(string comboName)
	{

	#if(dbg >= 1)
		cout << "ENTERING getComboIndex." << endl;
	#endif
		for(int i = 0; i < comboList.size(); i++)
		{
			if(comboDataArray[i].comboName.empty() == false)
			{
				cout << "comparing: " << comboDataArray[i].comboName << " with " << comboName << endl;
				if(comboDataArray[i].comboName.compare(comboName) == 0)
				{
		#if(dbg >= 1)
			cout << "EXITING getComboIndex." << endl;
		#endif
					return i;
				}
			}
			else
			{
				cout << "combo name was empty." << endl;
			}
		}

	#if(dbg >= 1)
		cout << "EXITING getComboIndex." << endl;
	#endif
		return -1; // combo not found

	}

	#define dbg 1
	int loadComboStructArrayAndList(vector<string> comboList)
	{
		int status = 0;

	#if(dbg >= 1)
		cout << "*****ENTERING loadComboStructArray*****" << endl;
	#endif

		for(int comboListIndex = 0; comboListIndex < comboList.size(); comboListIndex++)
		{
			cout << "loading combo from file: " << comboList[comboListIndex] << endl;
			if(comboDataArray[comboListIndex].loadComboStructFromName((char *)comboList[comboListIndex].c_str()) < 0)
			{
				cout << "failed to open file: " << comboDataArray[comboListIndex].comboName.c_str();
			}
		}


	#if(dbg >= 1)
		cout << "***** EXITING loadComboStructArray*****" << endl;
	#endif
		return status;
	}

	#define dbg 1
	int addComboStructToArrayAndList(string comboName)
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "*****ENTERING addComboStruct" << endl;
	#endif
		int index = 0;

		comboDataArray[comboDataCount++].loadComboStructFromName((char *)comboName.c_str());
	#if(dbg >= 1)
		cout << "***** EXITING addComboStruct" << endl;
	#endif

		return status;
	}

	#define dbg 1
	int deleteComboStructFromArrayAndList(string comboName)
	{
		int status = 0;
	#if(dbg >= 1)
		cout << "*****ENTERING mainFunctions::deleteComboStruct" << endl;
	#endif
		int index = getComboIndex(comboName);


		cout << "removing " << index << ":" << comboName << endl;
		if(index >= 0)
		{
			comboDataArray[index].comboName.clear();
			comboDataArray[index].connectionsJson.clear();

			comboDataArray[index].effectComboJson.clear();
			comboDataArray[index].pedalUiJson.clear();
			comboDataArray[index].unsequencedProcessListJson.clear();
			comboDataArray[index].unsequencedProcessListStruct.clear();
			comboDataArray[index].unsequencedConnectionListJson.clear();

			comboDataArray[index].connectionsJson.clear();
			comboDataArray[index].processesJson.clear();
			comboDataArray[index].processesStruct.clear();
			comboDataArray[index].unsortedParameterArray.clear();
			comboDataArray[index].sortedParameterArray.clear();
			comboDataArray[index].controlParameterArray.clear();
			comboDataArray[index].controlsStruct.clear();
			comboDataArray[index].controlConnectionsStruct.clear();
			//std::vector<Parameter> parameterArray;

			for(int i = 0; i < 10; i++)
				comboDataArray[index].footswitchStatus[i] = 0;
			for(int i = 0; i < 2; i++)
				comboDataArray[index].inputProcBufferIndex[i] = 0;
			for(int i = 0; i < 2; i++)
				comboDataArray[index].outputProcBufferIndex[i] = 0;
			comboDataArray[index].processCount = 0;
			comboDataArray[index].controlCount = 0;
			comboDataArray[index].bufferCount = 0;
			status = 0;
		}
		else
		{
			cout << "invalid index." << endl;
			status = -1;
		}

	#if(dbg >= 1)
		cout << "***** EXITING mainFunctions::deleteComboStruct" << endl;
	#endif

		return status;
	}

#elif(COMBO_DATA_MAP == 1)
	int loadComboStructMapAndList(void)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::loadComboStructMapAndList" << endl;
#endif

/*	FILE *fdComboList = popen("ls /home/Combos","r");
	char buffer[20];

	for(int i = 0; i<20;i++)
	{
		buffer[i] = 0;
	}*/

	comboList = getComboListFromFileSystem();

	if(comboList.empty() == true)
	{
#if(dbg>=2)
		cout << "popen failed." << endl;
#endif
		status = -1;
	}
	else
	{
		comboDataMap.clear();
		//while(fgets(buffer,20,fdComboList) != NULL)
		for(vector<string>::size_type comboListIndex = 0; comboListIndex < comboList.size(); comboListIndex++)
		{
			addComboStructToMap(comboList[comboListIndex]);
		}

	}

#if(dbg >= 1)
	cout << "***** EXITING mainFunctions::loadComboStructMapAndList: " << status << endl;
#endif
		return status;
	}


	std::vector<string> getComboMapList(void)
	{
		std::vector<string> tempComboList;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::getComboMapList" << endl;
#endif

			for(map<string, ComboDataInt>::iterator comboMapIt = comboDataMap.begin(); comboMapIt != comboDataMap.end(); comboMapIt++)
			{
				cout << "combo name: " << comboMapIt->first << endl;
				if(comboMapIt->first.empty() == false)
				{
					tempComboList.push_back(comboMapIt->first);
				}
				else
				{
					cout << "combo name is empty." << endl;
				}
			}

#if(dbg >= 1)
	cout << "***** EXITING mainFunctions::getComboMapList" << endl;
#endif
			return tempComboList;
	}

	int getComboMapIndex(string comboName)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::getComboMapIndex" << endl;
	cout << "comboName: " << comboName << endl;
#endif



#if(dbg >= 1)
	cout << "***** EXITING mainFunctions::getComboMapIndex: " << status << endl;
#endif
		return status;
	}

	//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
	int addComboStructToMapAndList(string comboName)
	{
		int status = 0;
#if(dbg >= 1)
		cout << "*****ENTERING mainFunctions::addComboStructToMapAndList" << endl;
		cout << "comboName: " << comboName << endl;
#endif

		ComboDataInt tempCombo = getComboObject(comboName);
		if(tempCombo.comboName.empty() == false)
		{
			comboDataMap.insert({comboName, tempCombo});
			comboList = getComboMapList();
		}
		else
		{
			cout << "failed to add combo object: " << comboName << endl;
			status = -1;
		}

#if(dbg >= 1)
		cout << "***** EXITING mainFunctions::addComboStructToMapAndList: " << status << endl;
#endif
		return status;
	}

	//************* Use this when re-creating entire comboStruct map *****************
	int addComboStructToMap(string comboName)
	{
		int status = 0;
#if(dbg >= 1)
		cout << "*****ENTERING mainFunctions::addComboStructToMap" << endl;
		cout << "comboName: " << comboName << endl;
#endif

		ComboDataInt tempCombo = getComboObject(comboName);
		if(tempCombo.comboName.empty() == false)
		{
			comboDataMap.insert({comboName, tempCombo});
		}
		else
		{
			cout << "failed to add combo object: " << comboName << endl;
			status = -1;
		}

#if(dbg >= 1)
		cout << "***** EXITING mainFunctions::addComboStructToMap: " << status << endl;
#endif
		return status;
	}


	int deleteComboStructFromMapAndList(string comboName)
	{
		int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::deleteComboStructFromMapAndList" << endl;
	cout << "comboName: " << comboName << endl;
#endif

		if(comboDataMap.find(comboName) != comboDataMap.end())
		{
			comboDataMap.erase(comboName);
			comboList = getComboMapList();
		}
		else
		{
			cout << "combo object not found:" << comboName << endl;
			status = -1;
		}

#if(dbg >= 1)
	cout << "***** EXITING mainFunctions::deleteComboStructFromMapAndList: " << status << endl;
#endif
		return status;
	}
#endif


int deleteComboNameFromList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	cout << "*****ENTERING mainFunctions::deleteComboNameFromList" << endl;
	cout << "comboName: " << comboName << endl;
#endif

	for(int i = 0; i < comboList.size(); i++)
	{
		if(comboList[i].compare(string(comboName)) == 0)
		{
			comboList.erase(comboList.begin() + i);
			break;
		}
		if(i == comboList.size() - 1)
		{
			cout << "combo object not found:" << comboName << endl;
			status = -1;
		}
	}

	#if(dbg >= 1)
cout << "***** EXITING mainFunctions::deleteComboNameFromList: " << status << endl;
#endif
	return status;
}





#define dbg 0
int readFootswitches(void)
{
	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: mainFunctions::readFootswitches" << endl;
#endif

	int/*string*/ footswitchStatus;

	for(int i = 0; i < FOOTSWITCH_COUNT; i++)
	{
		footswitch[i].getval_gpio(footswitchStatus);
		if(footswitchStatus != 1)//if(footswitchStatus.compare("1") == 0)
		{
			footswitchStatusArray[i] = 1; // footswitches are active low
		}
		else
		{
			footswitchStatusArray[i] = 0;
		}
	}

	//cout << "footswitchStatusArray: " << footswitchStatusArray[0] << "," << footswitchStatusArray[1] << "," << footswitchStatusArray[2] << "," << footswitchStatusArray[3] << endl;

#if(dbg >= 1)
	cout << "***** EXITING: mainFunctions::readFootswitches:" << status << endl;
#endif

	return status;
}

int bypassAll(void)
{
	int status = 0;

	/******* Set all processes to Bypass mode to prevent JACK XRun ***********/
	/*for(int i = 0; i < procIntArray.size(); i++)
	{
		procIntArray.at(i).updateFootswitchStatus(footswitchBypassAll);
	}*/

	return status;
}
