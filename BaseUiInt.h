/*
 * PedalUi.h
 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#ifndef BASEUIINT2_H_
#define BASEUIINT2_H_

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <linux/types.h>
#include <errno.h>

#include <iostream>
#include <fcntl.h>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
//#include "SharedMemoryInt.h"
#include "ComboDataInt.h"
/*#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500*/
#define SEND_BUFFER_SIZE 250

#define RX_DATA_SIZE 100
#define TX_DATA_SIZE 1000


using namespace std;

class BaseUiInt //: public SharedMemoryInt
{

private:


	char gpioStr[5];
protected:
	char uiData[TX_DATA_SIZE];
	char request[RX_DATA_SIZE];
	char sendBuffer[SEND_BUFFER_SIZE];
	uint8_t status;

public:
	BaseUiInt();

	~BaseUiInt();


	GPIOClass newData;// = GPIOClass(SHARED_MEMORY_READY);//, "in");
	GPIOClass dataSource;// = GPIOClass(SHARED_MEMORY_READY);//, "in");

	int pedalUiTxFd;
	int pedalUiRxFd;

	int checkForNewData(void);
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
