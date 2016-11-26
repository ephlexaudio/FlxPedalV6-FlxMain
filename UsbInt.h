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
#include "utilityFunctions.h"

using namespace std;

class UsbInt {
private:
	char usbInputBuffer[16000];
	char usbOutputBuffer[16000];
	int hostUiFD;
public:
	UsbInt();
	~UsbInt();

	int newData(void);
	char* readData(void);
	int writeData(char* input);
};

#endif /* USBINT_H_ */
