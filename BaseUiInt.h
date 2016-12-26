/*
 * PedalUi.h
 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#ifndef BASEUIINT_H_
#define BASEUIINT_H_

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <linux/types.h>

#include <iostream>
#include <fcntl.h>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "SharedMemoryInt.h"
#include "ComboDataInt.h"
#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500
#define SEND_BUFFER_SIZE 250


//static void pabort(const char *s);

/*struct Request {
	int requestCommand;
	char requestData[200];
};*/
using namespace std;

class BaseUiInt : public SharedMemoryInt
{

private:

	unsigned int cmSectionStartAddress;
	unsigned int cmSectionSize;
	unsigned int uiSectionStartAddress;
	unsigned int uiSectionSize;
	char gpioStr[5];

protected:
	char uiData[TX_BUFFER_SIZE];
	char request[RX_BUFFER_SIZE];
	char sendBuffer[SEND_BUFFER_SIZE];
	//uint8_t status;

public:
	BaseUiInt();
	BaseUiInt(unsigned int cmSectionStartAddress, unsigned int cmSectionSize,
			unsigned int uiSectionStartAddress, unsigned int uiSectionSize);

	/*virtual*/ ~BaseUiInt()/*{cout << "~BaseUiInt" << endl;}*/;
	GPIOClass newData;// = GPIOClass(SHARED_MEMORY_READY);//, "in");
	GPIOClass dataSource;// = GPIOClass(SHARED_MEMORY_READY);//, "in");


	int checkForNewData(void);
	int checkForNewData(int sectionIndex);
	string getUserRequest(void);
	int processUserRequest(char *request);
	virtual int sendComboUiData(Json::Value uiData){return 0;}
	int sendComboList(string comboLists);
	int sendCurrentStatus(char *currentStatus);
	int sendCurrentData(vector<IndexedParameter> currentParams);
	int clearSharedMemorySection(void);
	void waitForAccessRelease(void);
	/*int checkMemory(void);
	int sendData(char *data, uint16_t length);
	int getData(char *data, uint16_t length);*/
};



#endif /* PEDALUI_H_ */
