/*
 * PedalUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

#include "PedalUiInt.h"

#include "config.h"
#include "utilityFunctions.h"

extern int pedalUiTxFd;
extern int pedalUiRxFd;

extern bool debugOutput;

#define FILE_LENGTH 1000
#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define MCU_SHARED_MEMORY_SECTION_SIZE 4096

#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define CM0_SHARED_MEMORY_SECTION_SIZE 4096

#define HOST_SHARED_MEMORY_SECTION_ADDRESS 32768

#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500
#define FILE_BUFFER_SIZE 16000

#if(dbg >= 1)
	cout << "***** ENTERING: PedalUiInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt:::" << endl;
#endif



using namespace std;


PedalUiInt::PedalUiInt():BaseUiInt()
{
	this->status = 0;

}

PedalUiInt::~PedalUiInt()
{
	if(debugOutput) cout << "~PedalUiInt" << endl;
}



#define dbg 0

int PedalUiInt::checkForNewPedalData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::checkForNewPedalData" << endl;
#endif

	int status = 0;

	status = this->checkForNewData();
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::checkForNewPedalData: " << status << endl;
#endif

	return status;
}


#define dbg 0

int PedalUiInt::sendComboUiData(Json::Value uiJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendComboUiData: " <<  endl;
#endif
	uint8_t status = 0;
	uint8_t effectIndex = 0;
	uint8_t effectCount = 0;
	uint8_t guiParamIndex = 0;
	uint8_t guiParamCount = 0;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

	string uiJsonString = uiJson.toStyledString();
#if(dbg>=2)
	if(debugOutput) cout << "uiJsonString: " + uiJsonString << endl;
#endif
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\n'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\r'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\t'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), ' '), uiJsonString.end());
	strncpy(this->uiData, uiJsonString.c_str(), 1000);


	errno = 0;
	int txSentCount = write(pedalUiTxFd, this->uiData, strlen(this->uiData));
#if(dbg>=2)
	if(debugOutput) cout << "this->pedalUiTxFd:" << pedalUiTxFd << endl;
	if(debugOutput) cout << "this->uiData:" << this->uiData << endl;
	if(debugOutput) cout << "this->uiData length:" << strlen(this->uiData) << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == strlen(this->uiData)) status = 0;
	else status = -1;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendComboUiData: " << status << endl;
#endif
	return status;
}


#define dbg 1
int PedalUiInt::sendFlxUtilUiData(Json::Value uiJson)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendFlxUtilUiData: " <<  endl;
#endif

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;


	string uiJsonString = uiJson.toStyledString();
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\n'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\r'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\t'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), ' '), uiJsonString.end());
	strncpy(this->uiData, uiJsonString.c_str(), 1000);

	errno = 0;
	int txSentCount = write(pedalUiTxFd, this->uiData, strlen(this->uiData));
#if(dbg>=2)
	if(debugOutput) cout << "this->uiData:" << this->uiData << endl;
	if(debugOutput) cout << "this->uiData length:" << strlen(this->uiData) << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == strlen(this->uiData)) status = 0;
	else status = -1;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendFlxUtilUiData: " << status << endl;
#endif
	return status;
}
