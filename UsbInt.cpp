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
}

UsbInt::~UsbInt() {
	// TODO Auto-generated destructor stub
}

int UsbInt::connect()
{
	errno = 0;
	if(this->connectionStatus == 0)
	{
		this->hostUiFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);
		if(this->hostUiFD >= 0)
		{
			this->connectionStatus = 1;
		}
		else
		{
			if(debugOutput) cout << "failed to close USB:"  << errno << endl;
		}
	}
	else
	{
		if(debugOutput) cout << "USB already connected." << endl;
	}

}
int UsbInt::disconnect()
{
	errno = 0;
	if(close(this->hostUiFD) >= 0)
	{
		this->connectionStatus = 0;
	}
	else
	{
		if(debugOutput) cout << "failed to close USB: " << errno << endl;
	}
}

int UsbInt::isConnected()
{
	return this->connectionStatus;
}


#define dbg 0
int UsbInt::newData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::newData" << endl;
#endif
	clearBuffer(this->usbInputBuffer,FILE_SIZE);
	int status = 0;
	ssize_t size_read = read(this->hostUiFD, this->usbInputBuffer, FILE_SIZE);
	if(size_read > 1)
	{
#if(dbg >= 1)
		if(debugOutput) cout << "USB received size: " << strlen(this->usbInputBuffer) << endl;
#endif
		status = 1;
	}
	else
		status = size_read;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::newData" << endl;
#endif
	return status;
}

char *UsbInt::readData(void)
{
	if(strlen(this->usbInputBuffer) >= 0)
		return this->usbInputBuffer;
	else
		return 0;
}

#define dbg 0
int UsbInt::writeData(char *input)
{
	int status = 0;
	clearBuffer(this->usbOutputBuffer,FILE_SIZE);

	sprintf(this->usbOutputBuffer,"%s\r\n", input);

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::writeData" << endl;
#endif
#if(dbg >= 1)

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
