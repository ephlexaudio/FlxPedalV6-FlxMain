/*
 * UsbInt.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: buildrooteclipse
 */

#include "UsbInt.h"
#define FILE_SIZE 32000
/*
#if(dbg >= 1)
	cout << "***** ENTERING: UsbInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: UsbInt::" << endl;
#endif
*/

using namespace std;
UsbInt::UsbInt() {
	// TODO Auto-generated constructor stub
	hostUiFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);

}

UsbInt::~UsbInt() {
	// TODO Auto-generated destructor stub
}

#define dbg 0
int UsbInt::newData(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: UsbInt::newData" << endl;
#endif
	clearBuffer(this->usbInputBuffer,FILE_SIZE);

	ssize_t size_read = read(hostUiFD, this->usbInputBuffer, FILE_SIZE);
	if(size_read > 1)
	{
#if(dbg >= 1)
		cout << "USB received size: " << strlen(this->usbInputBuffer) << endl;
#endif
		return 1;
	}
	else
		return size_read;

#if(dbg >= 1)
	cout << "***** EXITING: UsbInt::newData" << endl;
#endif

}

char *UsbInt::readData(void)
{
	if(strlen(usbInputBuffer) >= 0)
		return usbInputBuffer;
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
	cout << "***** ENTERING: UsbInt::writeData" << endl;
#endif
#if(dbg >= 1)

	cout << "dataString: " << this->usbOutputBuffer << endl;
#endif

	ssize_t size_write = write(hostUiFD, this->usbOutputBuffer, strlen(this->usbOutputBuffer));

	if(size_write >= 0) status = 0;
	else status = -1;

#if(dbg >= 1)
	cout << "***** EXITING: UsbInt::writeData" << endl;
#endif

	return status;
}
