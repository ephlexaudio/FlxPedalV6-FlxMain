/*
 * PedalUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

#include "PedalUiInt.h"
#include <cstdlib>
#include <string>

#include "config.h"
#include "utilityFunctions.h"


extern bool debugOutput;


#if(dbg >= 1)
	cout << "***** ENTERING: PedalUiInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt:::" << endl;
#endif



using namespace std;


PedalUiInt::PedalUiInt()
{
#if(dbg >= 1)
	if(debugOutput) cout << "********** ENTERING PedalUiInt constructor: "  << endl;
#endif


	this->pedalUiTxFd = -1;
	this->pedalUiRxFd = -1;

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
	for(int i = 0; (this->pedalUiRxFd == -1 || this->pedalUiTxFd == -1) && i < 10; i++)
	{
		this->pedalUiRxFd = open(rxFifoPath, O_RDWR | O_NONBLOCK);
		usleep(10000);
		this->pedalUiTxFd = open(txFifoPath, O_RDWR | O_NONBLOCK);
		usleep(10000);
#if(dbg >= 2)
		if(debugOutput) cout << "waiting for FIFOs" << endl;
		if(debugOutput) cout << "********** OFX_MAIN: " << strerror(errno) << endl;
#endif
	}

	this->toPedalUiMemory = NULL;
	this->fromPedalUiMemory = NULL;
	this->createIPCFiles();

#if(dbg >= 1)
	if(debugOutput) cout << "********** EXITING PedalUiInt constructor: "  <<  endl;
#endif

}

PedalUiInt::~PedalUiInt()
{
	if(debugOutput) cout << "~PedalUiInt" << endl;
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

	this->closeIPCFiles();


}



#define dbg 0



#define dbg 0
string PedalUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::getUserRequest" << endl;
#endif
	char responseCharArray[200];
	uint8_t responseCharArrayIndex = 0;
	string responseString;


	int dataStatus = 0; //1: new data available
	int tempBufferIndex = 4;
	char tempBuffer[100];
	clearBuffer(tempBuffer,100);

	errno = 0;
	int dataReadSize = read(this->pedalUiRxFd, tempBuffer, 100);
	if( dataReadSize > 0)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "tempBuffer: " << tempBuffer << endl;
#endif

		for(tempBufferIndex = 0; tempBufferIndex < 100; tempBufferIndex++)
		{
			if((' ' <= tempBuffer[tempBufferIndex]) && (tempBuffer[tempBufferIndex] <= '~'))
			{
				responseString += tempBuffer[tempBufferIndex];
#if(dbg >= 2)
				if(debugOutput) cout << "tempBuffer[" << tempBufferIndex << "]: valid data: " << tempBuffer[tempBufferIndex] << "(" << (int)(tempBuffer[tempBufferIndex]) << ")" << endl;
#endif
			}
			else if(tempBufferIndex > 5)
			{
				dataStatus = 1;
				break;
			}
			else // corrupt data
			{
#if(dbg >= 2)
				if(debugOutput) cout << "tempBuffer[" << tempBufferIndex << "]: corrupt data: " << tempBuffer[tempBufferIndex] << "(" << (int)(tempBuffer[tempBufferIndex]) << ")" << endl;
#endif

			}
		}
	}
	else
	{
		dataStatus = 0;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::getUserRequest: " << responseString << endl;
#endif

	return responseString;
}


#define dbg 0

int PedalUiInt::sendComboPedalUiData(Json::Value uiJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendComboUiData: " <<  endl;
#endif
	uint8_t status = 0;
	string uiData;

	uiData.clear();
	uiData = getCompactedJSONData(uiJson);


	errno = 0;
	int txSentCount = write(this->pedalUiTxFd, uiData.c_str(), uiData.size());
#if(dbg>=2)
	if(debugOutput) cout << "this->pedalUiTxFd:" << pedalUiTxFd << endl;
	if(debugOutput) cout << "uiData:" << uiData << endl;
	if(debugOutput) cout << "uiData length:" << uiData.size() << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == uiData.size()) status = 0;
	else status = -1;

	uiData.clear();
#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendComboUiData: " << status << endl;
#endif
	return status;
}


#define dbg 0
int PedalUiInt::sendFlxUtilPedalUiData(Json::Value uiJson)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendFlxUtilUiData: " <<  endl;
#endif
	string uiData;

	uiData.clear();
	uiData = getCompactedJSONData(uiJson);

	errno = 0;
	int txSentCount = write(this->pedalUiTxFd, uiData.c_str(), uiData.size());
#if(dbg>=2)
	if(debugOutput) cout << "uiData:" << uiData << endl;
	if(debugOutput) cout << "uiData length:" << uiData.size() << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == uiData.size()) status = 0;
	else status = -1;

	uiData.clear();
#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendFlxUtilUiData: " << status << endl;
#endif
	return status;
}


int PedalUiInt::sendComboList(string comboList)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendComboList" << endl;
#endif
	uint8_t status = 0;


	write(this->pedalUiTxFd,comboList.c_str(), comboList.length());

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::sendComboList: " << status << endl;
#endif
	return status;

}

/*************************** IPC to replace getCurrentStatus funtion*******************/

int PedalUiInt::createIPCFiles(void)
{
	int status = 0;

	for(int i = 0; i < 50; i++) this->toPedalUiIPCPath[i] = 0;

	sprintf(this->toPedalUiIPCPath,"/ipc_toPedalUiInt");
	errno = 0;

	this->toPedalUiFD = shm_open(this->toPedalUiIPCPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(this->toPedalUiFD == -1)
	{
		printf("toProdFD open failed.\n");
		printf("errno %d:%s\n", errno, strerror(errno));
	}
	else if(ftruncate(this->toPedalUiFD,100) == -1)
	{
		printf("toProdFD ftruncate failed: %d\n", errno);
		errno = 0;
	}

	sprintf(this->fromPedalUiIPCPath,"/ipc_fromPedalUiInt");

	this->fromPedalUiFD = shm_open(this->fromPedalUiIPCPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(this->fromPedalUiFD == -1)
	{
		printf("fromPedalUiFD open failed.\n");
	}
	else if(ftruncate(this->fromPedalUiFD,100) == -1)
	{
		printf("fromPedalUiFD ftruncate failed: %d.\n", errno);
		errno = 0;
	}

	close(toPedalUiFD);
	close(fromPedalUiFD);

	return status;
}

int PedalUiInt::openIPCFiles(void)
{
	int status = 0;


	this->toPedalUiFD = shm_open(this->toPedalUiIPCPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(this->toPedalUiFD == -1)
	{
		printf("toProdFD open failed.\n");
	}
	else
	{
		this->toPedalUiMemory = (_ipcData *)mmap (0, sizeof(_ipcData), PROT_WRITE | PROT_READ,
	                        MAP_SHARED, this->toPedalUiFD, 0);
	}


	this->fromPedalUiFD = shm_open(this->fromPedalUiIPCPath, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(this->fromPedalUiFD == -1)
	{
		printf("fromPedalUiFD open failed.\n");
	}
	else
	{
		this->fromPedalUiMemory = (_ipcData *)mmap (0, sizeof(_ipcData), PROT_READ,
	                        MAP_SHARED, this->fromPedalUiFD, 0);
	}

	close(toPedalUiFD);
    close(fromPedalUiFD);


	return status;
}


int PedalUiInt::closeIPCFiles(void)
{
	munmap(this->fromPedalUiMemory, this->fromPedalUiFD);
	munmap(this->toPedalUiMemory, this->toPedalUiFD);

}
#define dbg 1
int PedalUiInt::sendUsbPortOpen(bool usbPortOpen)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendUsbPortOpen" << endl;
#endif
	this->toPedalUiMemory->exit = 0;
	this->toPedalUiMemory->change = 1;
	this->toPedalUiMemory->usbPortOpen = usbPortOpen;

	#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::sendUsbPortOpen: " << status << endl;
#endif
	return status;
}
int PedalUiInt::sendHostGuiActive(bool hostGuiActive)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendHostGuiActive" << endl;
#endif
	this->toPedalUiMemory->exit = 0;
	this->toPedalUiMemory->change = 1;
	this->toPedalUiMemory->hostGuiActive = hostGuiActive;


	#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::sendHostGuiActive: " << status << endl;
#endif
	return status;
}
