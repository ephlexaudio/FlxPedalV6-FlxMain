/*
 * UsbInt.h
 *
 *  Created on: Nov 16, 2016
 *      Author: buildrooteclipse
 */

#ifndef USBINT_H_
#define USBINT_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <string>
#include <cstring>
#include <iostream>
#include <errno.h>
#include <signal.h>
#include "config.h"
#include "utilityFunctions.h"
#include "ComputeModulePinOuts2.h"
#include "GPIOClass.h"




class UsbInt
{
private:

	string osSelected = "Lin";
	char gadget[10] = "";
	int usbFD;
	bool connectionStatus;
	bool portOpenStatus;
	bool usbConfigStatus;
	void acmEcmUsbInit();
	void acmRndisUsbInit();
	GPIOClass usbDetect;
public:
	UsbInt();
	virtual ~UsbInt();
	bool isUsbCableConnected();
	int configureUsbGadgetForOs(string selectedOs);
	int openPort();
	bool isUsbConfigured();
	bool isPortOpen();
	int closePort();
	string readNewData(void);
	int writeData(string input);

};

#endif /* USBINT_H_ */

