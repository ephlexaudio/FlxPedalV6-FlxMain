/*
 * BaseUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */
#include "config.h"
#include "BaseUiInt.h"
#include "utilityFunctions.h"

extern bool debugOutput;
int pedalUiTxFd;
int pedalUiRxFd;


#if(dbg >= 1)
	if(debugOutput) cout << "********** ENTERING BaseUiInt::: " << endl;
#endif
#if(dbg >= 1)
	if(debugOutput) cout << "********** EXITING BaseUiInt::: "  <<  endl;
#endif


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
#define dbg 2
BaseUiInt::BaseUiInt()
{
#if(dbg >= 1)
	if(debugOutput) cout << "********** ENTERING BaseUiInt constructor: "  << endl;
#endif


	this->status = 0;
	pedalUiTxFd = -1;
	pedalUiRxFd = -1;
	errno = 0;
	if(mkfifo(rxFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		cout << "********** OFX_MAIN: mkfifo rxFifoPath errno: " << errno << endl;
	}
	errno = 0;
	if(mkfifo(txFifoPath,S_IWUSR | S_IRUSR) != 0)
	{
		cout << "********** OFX_MAIN: mkfifo txFifoPath errno: " << errno << endl;
	}

	errno = 0;
	for(int i = 0; (pedalUiRxFd == -1 || pedalUiTxFd == -1) && i < 10; i++)
	{
		pedalUiRxFd = open(rxFifoPath, O_RDWR | O_NONBLOCK);
		usleep(10000);
		pedalUiTxFd = open(txFifoPath, O_RDWR | O_NONBLOCK);
		usleep(10000);
#if(dbg >= 2)
		if(debugOutput) cout << "waiting for FIFOs" << endl;
		if(debugOutput) cout << "********** OFX_MAIN: " << strerror(errno) << endl;
#endif
	}

#if(dbg >= 1)
	if(debugOutput) cout << "********** EXITING BaseUiInt constructor: "  <<  endl;
#endif
}



BaseUiInt::~BaseUiInt()
{
	int ret;

	if(pedalUiTxFd >= 0)
	{
		if(debugOutput) cout << "closing and removing /home/pedalUiTx." << endl;
		close(pedalUiTxFd);
		system("rm /home/pedalUiTx");
	}
	else
	{
		if(debugOutput) cout << "/home/pedalUiTx not found." << endl;
	}

	if(pedalUiRxFd >= 0)
	{
		if(debugOutput) cout << "closing and removing /home/pedalUiRx." << endl;
		close(pedalUiRxFd);
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
	int dataStatus = 0;
	int pinValue = 0;
	int tempBufferIndex = 4;
	char tempBuffer[100];
	clearBuffer(this->request,RX_DATA_SIZE);
	errno = 0;
	int dataReadSize = read(pedalUiRxFd, tempBuffer, 100);
	if( dataReadSize > 0)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "tempBuffer: " << tempBuffer << endl;
#endif

		dataStatus = 1;
		for(tempBufferIndex = 0; tempBufferIndex < 100; tempBufferIndex++)
		{
			if(' ' <= tempBuffer[tempBufferIndex] && tempBuffer[tempBufferIndex] <= '~')
			{

				this->request[charCount++] = tempBuffer[tempBufferIndex];

#if(dbg >= 2)
				if(debugOutput) cout << "tempBuffer[" << tempBufferIndex << "]: valid data: " << tempBuffer[tempBufferIndex] << "(" << (int)(tempBuffer[tempBufferIndex]) << ")" << endl;
#endif
			}
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



#define dbg 0


#define dbg 0
int BaseUiInt::sendComboList(string comboList)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: BaseUiInt::sendComboList" << endl;
#endif
	uint8_t status = 0;


	write(pedalUiTxFd,comboList.c_str(), comboList.length());

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

	strcat(this->uiData, hostUiStatusString);

#if(dbg >= 2)
	if(debugOutput) cout << "sendCurrentData: " << this->uiData << "\tsize: " << strlen(this->uiData) << endl;
#endif

	write(pedalUiTxFd, this->uiData, TX_DATA_SIZE);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::sendCurrentStatus: " << status << endl;
#endif
	return status;
}

#define dbg 0
int BaseUiInt::sendCurrentData(vector<IndexedProcessParameter> currentParams)
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

	write(pedalUiTxFd, this->uiData, strlen(this->uiData));

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: BaseUiInt::sendCurrentData: " << status << endl;
#endif
	return status;
}
