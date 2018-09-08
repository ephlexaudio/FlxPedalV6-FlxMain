/*
 * PedalUi.h
 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#ifndef PEDALUIINT_H_
#define PEDALUIINT_H_

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

#include "BaseUiInt.h"
#include "ComboDataInt.h"
#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500



class PedalUiInt : public BaseUiInt {
private:

public:
	PedalUiInt();
	~PedalUiInt();
	int checkForNewPedalData(void);
	int sendComboUiData(Json::Value uiJson);
	int sendFlxUtilUiData(Json::Value uiJson);
};



#endif /* PEDALUI_H_ */
