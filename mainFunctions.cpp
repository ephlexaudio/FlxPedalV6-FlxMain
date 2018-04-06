/*
 * mainFunctions.cpp
 *
 *  Created on: Mar 2, 2016
 *      Author: mike
 */

#include "config.h"
#include "ComboDataInt.h"
#include "FileSystemFuncts.h"
#include "utilityFunctions.h"
#include "mainFunctions.h"

#include "ProcessingControl.h"
#include "PedalUtilityData.h"
//#include "Server.h"
//CONNECTION connArray;

/*#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1*/

using namespace std;

#define dbg 0
extern bool debugOutput;

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


#if(COMBO_DATA_VECTOR == 1)
extern std::vector<string> comboList;//comboVectorList;
#elif(COMBO_DATA_ARRAY == 1)
extern std::vector<string> comboList;
#elif(COMBO_DATA_MAP == 1)
extern std::vector<string> comboList;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: mainFunctions::" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: mainFunctions:::" << status << endl;
#endif


//extern std::vector<ProcessInt> procIntArray;
extern int ofxPid;
extern int childPgid;
extern int procCount;
extern bool startUp;
extern int footswitchStatusArray[10];
extern int effectCount;
extern int wrapperParamData[10];
extern PedalUtilityData pedalUtilityData;
extern ProcessingControl procCont;

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
	if(debugOutput) cout << "***** ENTERING: mainFunctions::openJack" << endl;
#endif


	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4E},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x04},	// disable DAC soft mute
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


    if(debugOutput) cout << "jackctl_server_create" << endl;
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
	if(debugOutput) cout << "***** EXITING: mainFunctions::openJack:" << status << endl;
#endif

    return status;
}

#define dbg 0
int startJack(void)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: mainFunctions::startJack" << endl;
#endif


	if(debugOutput) cout << "mainFunctions JACK period: " << pedalUtilityData.getJack_Period()/*jackParams.period*/ << "\tJACK buffer: " << pedalUtilityData.getJack_Buffer()/*jackParams.buffer*/ << endl;

	char jackInitString[100];
	sprintf(jackInitString, "jackd -d alsa -p %d -n %d &", pedalUtilityData.getJack_Period(), pedalUtilityData.getJack_Buffer()/*jackParams.period, jackParams.buffer*/);
	system(jackInitString);
	//system("jackd -d alsa &");
	sleep(2);
	if(debugOutput) cout << "setting up I2C port..." << endl;
	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4A},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x07},	// disable DAC soft mute, disable HPF, enable de-emphasis
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

    /*if(debugOutput) cout << "jackctl_server_create" << endl;
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
	if(debugOutput) cout << "***** EXITING: mainFunctions::startJack:" << status << endl;
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
int stopCombo(void)
{
	//if(debugOutput) cout << "stopCombo connectionArray size: " << connCont.connectionArray.size() << endl;

	//if(debugOutput) cout << "closing dummy client: " << jack_client_close(ofxJackClient) << endl;

	printf("stopCombo\n");
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: mainFunctions::stopCombo" << endl;
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
	if(debugOutput) cout << "***** EXITING: mainFunctions::stopCombo:" << status << endl;
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
	if(debugOutput) cout << "*****ENTERING mainFunctions::getComboObject" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	status = comboObject.loadComboStructFromName((char *)comboName.c_str());


	//if(status >= 0)
	{
#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING mainFunctions::getComboObject: " << comboObject.comboName << endl;
#endif
		return comboObject;
	}
	/*else
	{
#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING mainFunctions::getComboObject: " << nullObject.comboName << endl;
#endif
		return nullObject;
	}*/
}


#define dbg 0
int listComboMapObjects(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING listComboMapObjects" << endl;
#endif

	for(vector<string>::size_type comboListIndex = 0; comboListIndex < comboList.size(); comboListIndex++)
	{
		if(debugOutput) cout << "name: " << comboList[comboListIndex] << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING listComboMapObjects: " << status << endl;
#endif
	return status;
}

#define dbg 0
#if(COMBO_DATA_MAP == 1)
int loadComboStructMapAndList(void)
{
	int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING mainFunctions::loadComboStructMapAndList" << endl;
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
		if(debugOutput) cout << "popen failed." << endl;
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
if(debugOutput) cout << "***** EXITING mainFunctions::loadComboStructMapAndList: " << status << endl;
#endif
	return status;
}


std::vector<string> getComboMapList(void)
{
	std::vector<string> tempComboList;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING mainFunctions::getComboMapList" << endl;
#endif

		for(map<string, ComboDataInt>::iterator comboMapIt = comboDataMap.begin(); comboMapIt != comboDataMap.end(); comboMapIt++)
		{
			if(debugOutput) cout << "combo name: " << comboMapIt->first << endl;
			if(comboMapIt->first.empty() == false)
			{
				tempComboList.push_back(comboMapIt->first);
			}
			else
			{
				if(debugOutput) cout << "combo name is empty." << endl;
			}
		}

#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING mainFunctions::getComboMapList" << endl;
#endif
		return tempComboList;
}

int getComboMapIndex(string comboName)
{
	int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING mainFunctions::getComboMapIndex" << endl;
if(debugOutput) cout << "comboName: " << comboName << endl;
#endif



#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING mainFunctions::getComboMapIndex: " << status << endl;
#endif
	return status;
}

//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
int addComboStructToMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING mainFunctions::addComboStructToMapAndList" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	ComboDataInt tempCombo = getComboObject(comboName);
	if(tempCombo.comboName.empty() == false)
	{
		//comboDataMap.insert({comboName, tempCombo});
		comboDataMap[comboName] = tempCombo;
		comboList = getComboMapList();
	}
	else
	{
		if(debugOutput) cout << "failed to add combo object: " << comboName << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING mainFunctions::addComboStructToMapAndList: " << status << endl;
#endif
	return status;
}

#define dbg 1
//************* Use this when re-creating entire comboStruct map *****************
int addComboStructToMap(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING mainFunctions::addComboStructToMap" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	ComboDataInt tempCombo = getComboObject(comboName);
	if(tempCombo.comboName.empty() == false)
	{
		//comboDataMap.insert({comboName, tempCombo});
		comboDataMap[comboName] = tempCombo;
	}
	else
	{
		if(debugOutput) cout << "failed to add combo object: " << comboName << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING mainFunctions::addComboStructToMap: " << status << endl;
#endif
	return status;
}


int deleteComboStructFromMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING mainFunctions::deleteComboStructFromMapAndList" << endl;
if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	if(comboDataMap.find(comboName) != comboDataMap.end())
	{
		comboDataMap.erase(comboName);
		comboList = getComboMapList();
	}
	else
	{
		if(debugOutput) cout << "combo object not found:" << comboName << endl;
		status = -1;
	}

#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING mainFunctions::deleteComboStructFromMapAndList: " << status << endl;
#endif
	return status;
}
#endif


int deleteComboNameFromList(string comboName)
{
int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING mainFunctions::deleteComboNameFromList" << endl;
if(debugOutput) cout << "comboName: " << comboName << endl;
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
		if(debugOutput) cout << "combo object not found:" << comboName << endl;
		status = -1;
	}
}

	#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING mainFunctions::deleteComboNameFromList: " << status << endl;
#endif
	return status;
}


