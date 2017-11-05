/*
 * BaseUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */
#include "config.h"
#include "BaseUiInt.h"
//#include "ComboDataInt.h"
#include "utilityFunctions.h"

extern bool debugOutput;

#if(dbg >= 1)
	if(debugOutput) cout << "********** ENTERING BaseUiInt::: " << endl;
#endif
#if(dbg >= 1)
	if(debugOutput) cout << "********** EXITING BaseUiInt::: "  <<  endl;
#endif

//extern int comboTime;
/*
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::" << endl;
#endif
*/

/*#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define SHARED_MEMORY_SECTION_SIZE 4096

#define TX_DATA_SIZE 1500
#define RX_DATA_SIZE 1500
#define SEND_BUFFER_SIZE 250



#define SHARED_MEMORY_SOURCE 4
#define SHARED_MEMORY_READY 29*/
#define RQST_DATA_FILTER_COUNT 5
const char* rxFifoPath = "/home/pedalUiTx";
const char* txFifoPath = "/home/pedalUiRx";
#define RX_DATA_SIZE 100
#define TX_DATA_SIZE 1000


extern int procCount;
extern bool hostGuiActive;
extern int globalComboIndex;

/*
 *  Use this signature to ONLY detect new data (not access it), and
 *  wait for shared memory access release
 */
#define dbg 1
BaseUiInt::BaseUiInt()
{
#if(dbg >= 1)
	if(debugOutput) cout << "********** ENTERING BaseUiInt constructor: "  << endl;
#endif


	this->status = 0;
	/*if(mkfifo("/home/pedalUiTx",666) >= 0)
	{
		if((this->pedalUiTx = open("/home/pedalUiTx",O_RDWR | O_NONBLOCK, 'w')) >= 0)
		{

		}
		else
		{
			if(debugOutput) cout << "error opening pedalUiTx FIFO" << endl;
		}
	}
	else
	{
		if(debugOutput) cout << "error creating pedalUiTx FIFO" << endl;
		this->status = -1;
	}

	if(status >= 0)
	{
		if(mkfifo("/home/pedalUiRx",666) >= 0)
		{
			if((this->pedalUiRx = open("/home/pedalUiRx",O_RDWR | O_NONBLOCK),'r') >= 0)
			{

			}
			else
			{
				if(debugOutput) cout << "error opening pedalUiRx FIFO" << endl;
			}
		}
		else
		{
			if(debugOutput) cout << "error creating pedalUiRx FIFO" << endl;
			this->status = -1;
		}
	}*/
	this->pedalUiTxFd = -1;
	this->pedalUiRxFd = -1;
	bool exit = false;
	char rxData[RX_DATA_SIZE];
	int rxDataSize = 0;
	char txData[TX_DATA_SIZE];
	int txDataSize = 0;
	string rxCommand;
	string rxCommandData;
	errno = 0;
	if(mkfifo(rxFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		printf("********** OFX_MAIN: mkfifo rxFifoPath errno: %d\n", errno);
	}
	if(mkfifo(txFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		printf("********** OFX_MAIN: mkfifo txFifoPath errno: %d\n", errno);
	}

	errno = 0;
	for(int i = 0; (this->pedalUiRxFd == -1 || this->pedalUiTxFd == -1) && i < 10; i++)
	{
		this->pedalUiRxFd = open(rxFifoPath, O_RDONLY | O_NONBLOCK);
		usleep(100000);
		this->pedalUiTxFd = open(txFifoPath, O_WRONLY | O_NONBLOCK);
		usleep(100000);
		if(debugOutput) cout << "waiting for FIFOs" << endl;
		if(debugOutput) cout << "********** OFX_MAIN: " << strerror(errno) << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "********** EXITING BaseUiInt constructor: "  <<  endl;
#endif
}



BaseUiInt::~BaseUiInt()
{
	int ret;

	if(this->pedalUiTxFd >= 0)
	{
		if(debugOutput) cout << "closing and removing /home/pedalUiTx." << endl;
		close(this->pedalUiTxFd);
		system("rm /home/pedalUiTx");
	}
	else
	{
		if(debugOutput) cout << "/home/pedalUiTx not found." << endl;
	}

	if(this->pedalUiRxFd >= 0)
	{
		if(debugOutput) cout << "closing and removing /home/pedalUiRx." << endl;
		close(this->pedalUiRxFd);
		system("rm /home/pedalUiRx");
	}
	else
	{
		if(debugOutput) cout << "/home/pedalUiRx not found." << endl;
	}

	this->status = 0;

}


#define dbg 0
int BaseUiInt::checkForNewData()
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::checkForNewData 2" << endl;
#endif

	int pinString;
	int charCount = 0;
	volatile int dataStatus = 0;
	int/*uint8_t*/ pinValue = 0;
	int tempBufferIndex = 4;
	char tempBuffer[100];
	//newData.getval_gpio(pinString);
	//pinValue = atoi(pinString.c_str());
	clearBuffer(this->request,RX_DATA_SIZE);
	errno = 0;
	//if(this->getData(SHARED_MEMORY_SECTION_SIZE*sectionIndex, tempBuffer, 100) == 0)
	int dataReadSize = read(this->pedalUiRxFd, tempBuffer, 100);
	if( dataReadSize > 0)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "tempBuffer: " << tempBuffer << endl;
#endif
		//strncpy(this->request, tempBuffer, 100);
		dataStatus = 1;
		for(tempBufferIndex = 0; tempBufferIndex < 100; tempBufferIndex++)
		{
			if(' ' <= tempBuffer[tempBufferIndex] && tempBuffer[tempBufferIndex] <= '~')
			{
				//strcat(this->request, (const char*)tempBuffer[tempBufferIndex]);
				this->request[charCount++] = tempBuffer[tempBufferIndex];
				//charCount++;
#if(dbg >= 2)
				if(debugOutput) cout << "tempBuffer[" << tempBufferIndex << "]: valid data: " << tempBuffer[tempBufferIndex] << "(" << (int)(tempBuffer[tempBufferIndex]) << ")" << endl;
#endif
			}
			/*else if(charCount > RQST_DATA_FILTER_COUNT && tempBuffer[tempBufferIndex] == 255) // found end marker, data is ready
			{
				dataStatus = 1;
				this->request[charCount] = 0;
#if(dbg >= 2)
				if(debugOutput) cout << "found end marker, data is ready" << endl;
#endif
				break;
			}*/
			else if(tempBuffer[tempBufferIndex] == 0)
			{
				dataStatus = 1;
				break;
			}
			else // corrupt data
			{
#if(dbg >= 2)
				if(debugOutput) cout << "tempBuffer[" << tempBufferIndex << "]: corrupt data: " << tempBuffer[tempBufferIndex] << "(" << (int)(tempBuffer[tempBufferIndex]) << ")" << endl;
#endif
				//break;
			}
		}
	}
	else
	{
#if(dbg >= 2)
		if(debugOutput) cout << "error getting data: " << strerror(errno) << endl;
#endif
		dataStatus = 0;
	}

	//if(debugOutput) cout << "status: " << status << endl;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::checkForNewData 2: " << dataStatus << ":" << dataReadSize << endl;
#endif

	return dataStatus;
}



#define dbg 0
string BaseUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::getUserRequest" << endl;
#endif
	char responseCharArray[200];
	uint8_t responseCharArrayIndex = 0;
	string responseString;

	uint8_t status = 0;

	for(uint8_t i = 0; i < 50; i++)
	{
		if((' ' <= this->request[i] && this->request[i] <= '~') || this->request[i] == 0)
		{
			responseCharArray[responseCharArrayIndex++] = this->request[i];
		}
	}

	responseString = string(responseCharArray);


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::getUserRequest: " << responseString << endl;
#endif

	return responseString;
}

int BaseUiInt::processUserRequest(char *request)
{

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::processUserRequest" << endl;
#endif

	if(this->status == 1) return 1;
	uint8_t status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::processUserRequest: " << status <<endl;
#endif

	return status;
}

#define dbg 0


#define dbg 0
int BaseUiInt::sendComboList(string comboList)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::sendComboList" << endl;
#endif
	uint8_t status = 0;

	/*for(int i = 0; i < SEND_BUFFER_SIZE; i++) this->sendBuffer[i] = 0;
	strncpy(this->sendBuffer, (char *)comboList.c_str(),SEND_BUFFER_SIZE-1);
	//this->sendData(this->uiSectionStartAddress, this->sendBuffer, 150);
	write(this->pedalUiTx,this->sendBuffer, 150);*/
	write(this->pedalUiTxFd,comboList.c_str(), comboList.length());

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::sendComboList: " << status << endl;
#endif
	return status;
}

#define dbg 0
int BaseUiInt::sendCurrentStatus(char *currentStatus)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::sendCurrentStatus" << endl;
#endif
	uint8_t status = 0;
	char paramData[8];
	if(strlen(currentStatus) == 0) currentStatus[0] = ' ';
	for(int i = 0; i < TX_DATA_SIZE; i++) this->uiData[i] = 0;

	sprintf(this->uiData, "currentComboIndex:%d", globalComboIndex);
	strcat(this->uiData, "|currentStatus:");
	strcat(this->uiData, currentStatus);

	char hostUiStatusString[20];
	sprintf(hostUiStatusString,"|hostUiStatus:%d", (hostGuiActive?1:0));
	strcat(this->uiData, hostUiStatusString);
	clearBuffer(hostUiStatusString,20);
	//sprintf(hostUiStatusString,"|comboTime:%d", comboTime);
	strcat(this->uiData, hostUiStatusString);

#if(dbg >= 2)
	if(debugOutput) cout << "sendCurrentData: " << this->uiData << "\tsize: " << strlen(this->uiData) << endl;
#endif

	//this->sendData(this->uiSectionStartAddress, this->uiData, 150/*strlen(this->uiData)*/);
	write(this->pedalUiTxFd, this->uiData, TX_DATA_SIZE/*strlen(this->uiData)*/);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::sendCurrentStatus: " << status << endl;
#endif
	return status;
}

#define dbg 0
int BaseUiInt::sendCurrentData(vector<IndexedParameter> currentParams)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::sendCurrentData" << endl;
#endif
	uint8_t status = 0;
	char paramData[8];

	for(int i = 0; i < TX_DATA_SIZE; i++) this->uiData[i] = 0;

	sprintf(this->uiData, "currentComboIndex:%d", globalComboIndex);

	char hostUiStatusString[20];
	sprintf(hostUiStatusString,"|hostUiStatus:%d", (hostGuiActive?1:0));
	strcat(this->uiData, hostUiStatusString);

	//if(debugOutput) cout << "sendCurrentData: " << this->uiData << "\tsize: " << strlen(this->uiData) << endl;

	//this->sendData(this->uiSectionStartAddress, this->uiData, strlen(this->uiData));
	write(this->pedalUiTxFd, this->uiData, strlen(this->uiData));

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::sendCurrentData: " << status << endl;
#endif
	return status;
}


