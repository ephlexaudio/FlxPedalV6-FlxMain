/*
 * mainFunctions.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: mike
 */


//#include "ProcessInt.h"
//#include "ConnectionCont.h"
#include "ComboDataInt.h"
//#include "PedalUiInt.h"
#include "DataFuncts.h"
#include "utilityFunctions.h"
#include "mainFunctions.h"

#include "ProcessingControl.h"
//#include "Server.h"
//CONNECTION connArray;
GPIOClass footswitch[2];
#define FOOTSWITCH_COUNT 2

#define FOOTSWITCH_1 6
#define FOOTSWITCH_2 12

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

extern ComboDataInt comboData;
//extern PedalUiInt pedalUi;
//extern ConnectionCont connCont;
//extern std::vector<string> componentList;
//extern std::vector<string> comboList;
//extern std::vector<ProcessInt> procIntArray;
extern int ofxPid;
extern int childPgid;
extern int procCount;
extern bool startUp;
extern int footswitchStatusArray[10];
extern int wrapperParamData[10];
ProcessingControl procCont;

extern struct _jackParams jackParams;
/*extern void *toProcessMemory;
extern void *fromProcessMemory;
extern int toProcFD;
extern int fromProcFD;*/


#define dbg 0

long startStamp;
long stopStamp;
struct timeval tv;


void startTimer(void)
{
	gettimeofday(&tv, NULL);
	startStamp = 1000000*tv.tv_sec+tv.tv_usec;
}

void stopTimer(const char *description)
{
	gettimeofday(&tv, NULL);
	stopStamp = 1000000*tv.tv_sec+tv.tv_usec;
	cout << "******" << description << "time: " << stopStamp - startStamp << endl;

}

int openJack(void)
{
	int status = 0;
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

	/*************  Finish setting codec registers ************/
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

    return status;
}

int startJack(void)
{
	int status = 0;


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
	/*************  Finish setting codec registers ************/
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

    jackctl_server_destroy(ofxJackServer);

	status = system("killall -9 jackd");

	//sleep(1);
    return status;
}

#define dbg 0
int initializePedal(char *fileName)
{
	int status = 0;
	char gpioStr[5];


#if(dbg==1)
	printf("Getting combo data from file...\n");
#endif


	if(status == 0)
	{
		comboData.getCombo(fileName);
		if(comboData.getPedalUi() != 0)
		{
			status = 1;
		}
	}

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

	  return status;
}


#define dbg 1
int getRunningProcesses(void)
{
	int status = 0;
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
	return status;
}



#define dbg 0
int loadCombo(void)
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
	//int effectCount = comboData.pedalUiJson["effects"].size();
#if(dbg==1)
	printf("number of effects:%d\n", effectCount);
#endif
	/*for(int i = 0; i < effectCount; i++)
	{
#if(dbg==1)
		printf("\teffect abbr: %s\n",(const char*)(comboData.pedalUiJson["effects"][i]["abbr"].asString().c_str()));
		printf("\teffect name: %s\n",(const char*)comboData.pedalUiJson["effects"][i]["name"].asString().c_str());
#endif

		//int paramCount = comboData.pedalUiJson["effects"][i]["params"].size();
#if(dbg==1)
		printf("\tnumber of parameters:%d\n", paramCount);
		for(int j = 0 ; j < paramCount; j++)
		{
			printf("\t\t param abbr: %s\n", comboData.pedalUiJson["effects"][i]["params"][j]["abbr"].asString().c_str());

			printf("\t\t param name: %s\n", comboData.pedalUiJson["effects"][i]["params"][j]["name"].asString().c_str());

			printf("\t\t param value: %s\n", comboData.pedalUiJson["effects"][i]["params"][j]["value"].asString().c_str());
		}
#endif
	}*/

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
}

// run and connect processes
#define dbg 0
int runCombo(void)
{
	int status = 0;
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
	//int procCount = 0;
	char procOutput[15];
	int connCount = 0;
	//stopJack();
	procCont.stop();//stopCombo();
	sleep(1);
	char disconnString[100];
	startTimer();
	//procCount = 1;//combo.effectComboJson["processes"].size();



	  stopTimer("stopCombo");
	return status;
}

#define dbg 0
int readFootswitches(void)
{
	int status = 0;
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
