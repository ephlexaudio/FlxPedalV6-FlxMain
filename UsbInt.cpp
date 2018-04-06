/*
 * UsbInt.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "UsbInt.h"
#define FILE_SIZE 32000
/*
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::" << endl;
#endif
*/

using namespace std;

extern bool debugOutput;

UsbInt::UsbInt() {
	// TODO Auto-generated constructor stub
	this->hostUiFD = 0;
	//this->usbDetect = GPIOClass(USB_CONNECTED);
	this->portOpenStatus = 0;
}

UsbInt::~UsbInt() {
	// TODO Auto-generated destructor stub
}

#define dbg 1
int UsbInt::connect()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::connect: " <<  endl;
#endif

	if(this->connectionStatus == 0)
	{
		this->hostUiFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);
		if(this->hostUiFD >= 0)
		{
			this->connectionStatus = 1;
			status = 0;
#if(dbg >= 2)
			if(debugOutput) cout << "this->hostUiFD:"  << this->hostUiFD << endl;
#endif
		}
		else
		{
#if(dbg >= 2)
			if(debugOutput) cout << "failed to open USB:"  << errno << endl;
#endif
		}
	}
	else
	{
		if(debugOutput) cout << "USB already connected." << endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "********** EXITING UsbInt::connect: " << status << endl;
#endif

	return status;
}

#define dbg 1
int UsbInt::disconnect()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::disconnect: " <<  endl;
#endif

	if(this->connectionStatus != 0)
	{
		if(close(this->hostUiFD) >= 0)
		{
			this->connectionStatus = 0;
			status = 0;
#if(dbg >= 2)
			if(debugOutput) cout << "this->hostUiFD:"  << this->hostUiFD << endl;
#endif
		}
		else
		{
#if(dbg >= 2)
			if(debugOutput) cout << "failed to close USB: " << errno << endl;
#endif
		}
	}
	else
	{
		if(debugOutput) cout << "USB already closed." << endl;
		status = 0;
	}

#if(dbg >= 1)
	cout << "********** EXITING UsbInt::disconnect: " << status << endl;
#endif

	return status;
}

bool UsbInt::isPortOpen()
{
	bool status;

	if(this->portOpenStatus == 1) status = true;
	else status = false;

	return status;
}

#define dbg 0

int UsbInt::openPort()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::openPort: " <<  endl;
#endif

	//if(this->portOpenStatus == 0)
	{
		this->hostUiFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);
		if(this->hostUiFD >= 0)
		{
			this->portOpenStatus = 1;
			status = 0;
		}
		else
		{
			if(debugOutput) cout << "failed to open USB:"  << errno << endl;
		}
	}
	/*else
	{
		if(debugOutput) cout << "USB already connected." << endl;
		status = 0;
	}*/
#if(dbg >= 1)
	cout << "********** EXITING UsbInt::openPort: " << status << endl;
#endif

	return status;
}

#define dbg 0
int UsbInt::closePort()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::closePort: " <<  endl;
#endif

	if(close(this->hostUiFD) >= 0)
	{
		this->portOpenStatus = 0;
		status = 0;
	}
	else
	{
		if(debugOutput) cout << "failed to close USB: " << errno << endl;
	}
#if(dbg >= 1)
	cout << "********** EXITING UsbInt::closePort: " << status << endl;
#endif
	return status;
}

#define dbg 0

int UsbInt::isConnected()
{
	//bool status = false;
	int usbLine = 0;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::isUsbCableConnected: " <<  endl;
#endif

	//this->usbDetect.getval_gpio(usbLine);
	//if(usbLine == 1) status = true;

#if(dbg >= 1)
	cout << "********** EXITING UsbInt::isUsbCableConnected: " << status << endl;
#endif

	//return status;
	return this->connectionStatus;

}


#define dbg 0
int UsbInt::newData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::newData" << endl;
#endif
	clearBuffer(this->usbInputBuffer,FILE_SIZE);
	clearBuffer(this->usbCleanInputBuffer,FILE_SIZE);
	int status = 0;
	ssize_t size_read = read(this->hostUiFD, this->usbInputBuffer, FILE_SIZE);
	int cleanDataIndex = 0;

	for(int usbInputBufferIndex = 0; usbInputBufferIndex < strlen(this->usbInputBuffer); usbInputBufferIndex++)
	{
		if(this->usbInputBuffer[usbInputBufferIndex] > 0) this->usbCleanInputBuffer[cleanDataIndex++] = this->usbInputBuffer[usbInputBufferIndex];
	}
	if(size_read > 1)
	{
#if(dbg >= 1)
		if(debugOutput) cout << "USB received size: " << strlen(this->usbInputBuffer) << ":\t" << this->usbInputBuffer << endl;
		if(debugOutput) cout << "clean data size: " << strlen(this->usbCleanInputBuffer )<< ":\t" << this->usbCleanInputBuffer << endl;
#endif
		status = 1;
	}
	/*else
		status = strlen(this->usbCleanInputBuffer );*/

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::newData" << endl;
#endif
	return status;
}

char *UsbInt::readData(void)
{
	if(strlen(this->usbCleanInputBuffer) >= 0)
		return this->usbCleanInputBuffer;
	else
		return 0;
}

#define dbg 0
int UsbInt::writeData(char *input)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::writeData" << endl;
#endif
	clearBuffer(this->usbOutputBuffer,FILE_SIZE);

	//sprintf(this->usbOutputBuffer,"%s\r\n", input);
	sprintf(this->usbOutputBuffer,"%s\n", input);

#if(dbg >= 2)

	if(debugOutput) cout << "dataString: " << this->usbOutputBuffer << endl;
#endif

	ssize_t size_write = write(this->hostUiFD, this->usbOutputBuffer, strlen(this->usbOutputBuffer));

	if(size_write >= 0) status = 0;
	else status = -1;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::writeData" << endl;
#endif

	return status;
}
