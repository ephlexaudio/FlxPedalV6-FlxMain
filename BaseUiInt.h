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
#include <sys/stat.h>

#include <iostream>
#include <fcntl.h>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "ComboDataInt.h"
#include "GPIOClass.h"
#define SEND_BUFFER_SIZE 250

#define RX_DATA_SIZE 100
#define TX_DATA_SIZE 1000



using namespace std;

class BaseUiInt
{


protected:
	char uiData[TX_DATA_SIZE];
	uint8_t status;
	char request[RX_DATA_SIZE];
	char sendBuffer[SEND_BUFFER_SIZE];

public:
	BaseUiInt();

	~BaseUiInt();


	int checkForNewData(void);
	string getUserRequest(void);
	virtual int sendComboUiData(Json::Value uiData){return 0;}
	int sendComboList(string comboLists);
	int sendCurrentStatus(char *currentStatus);
	int sendCurrentData(vector<IndexedProcessParameter> currentParams);
};



#endif /* PEDALUI_H_ */
