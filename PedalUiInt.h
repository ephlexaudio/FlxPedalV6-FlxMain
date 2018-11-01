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
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

using namespace std;

/*#define SPI_TX_BUFFER_SIZE 1500
#define SPI_RX_BUFFER_SIZE 1500*/




class PedalUiInt {
private:
	struct _ipcData {
		int change;
		string comboName;
		string currentStatus;
		bool usbPortOpen;
		bool hostGuiActive;
		int exit;
	};

	int pedalUiTxFd;
	int pedalUiRxFd;
	const char* rxFifoPath = "/home/pedalUiTx";
	const char* txFifoPath = "/home/pedalUiRx";

	int toPedalUiFD;
	int fromPedalUiFD;
	_ipcData *toPedalUiMemory;
	_ipcData *fromPedalUiMemory;
	char toPedalUiIPCPath[50];
	char fromPedalUiIPCPath[50];

	int createIPCFiles(void);

public:
	PedalUiInt();
	~PedalUiInt();
	string getUserRequest(void);
	int sendComboPedalUiData(Json::Value uiJson);
	int sendFlxUtilPedalUiData(Json::Value uiJson);
	int sendComboList(string comboList);
	int openIPCFiles(void);
	int closeIPCFiles(void);
	int sendUsbPortOpen(bool usbPortOpen);
	int sendHostGuiActive(bool hostGuiActive);
};



#endif /* PEDALUI_H_ */
