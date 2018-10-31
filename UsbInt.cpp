/*
 * UsbInt.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "UsbInt.h"

using namespace std;

extern bool debugOutput;

UsbInt::UsbInt()
{
	// TODO Auto-generated constructor stub
	this->usbFD = 0;
	this->portOpenStatus = false;
	this->connectionStatus = false;

	char pinDirection[4] = "in;";
	usbDetect = GPIOClass(USB_CONNECTED);
	usbDetect.export_gpio();
	usbDetect.setdir_gpio(pinDirection);

	clearBuffer(gadget,10);
	FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
	while((fgets(gadget,5,gadgetDetect)) != NULL);
	pclose(gadgetDetect);

	if(gadget[0] == 'g')
	{
		this->usbConfigStatus = true;

	}
	else
	{
		this->usbConfigStatus = false;

	}

}

UsbInt::~UsbInt() {
	// TODO Auto-generated destructor stub
}


#define dbg 0
bool UsbInt::isUsbCableConnected()
{

	bool status = false;
	int usbLine = 0;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt:isUsbCableConnected: " <<  endl;
#endif

	usbDetect.getval_gpio(usbLine);
	if(usbLine == 1) status = true;

	this->connectionStatus = status;
#if(dbg >= 1)
	cout << "********** EXITING UsbInt::isUsbCableConnected: " << status << endl;
#endif

	return status;
}


void UsbInt::acmEcmUsbInit()
{
	system("/root/HybridUsbInit");

}

void UsbInt::acmRndisUsbInit()
{
	system("/root/AcmRndisUsbInit");
}

#define dbg 0
int UsbInt::configureUsbGadgetForOs(int selectedOs)
{
	errno = 0;
	this->osSelected = selectedOs;
	int status = 0;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::configureUsbGadgetForOs: " <<  endl;
#endif

	try
	{
		FILE *gadgetDetect = popen("ls /sys/kernel/config/usb_gadget/","r");
		while((fgets(gadget,5,gadgetDetect)) != NULL);
		pclose(gadgetDetect);

		if(gadget[0] != 'g')
		{
						switch(osSelected)
						{
							case 0:
								status = -1;
								break;
							case 1:		// Windows selected
								cout << "this->acmRndisUsbInit()" << endl;
								this->acmRndisUsbInit();
								break;
							case 2:		// Mac OS X selected
								cout << "this->acmEcmUsbInit()" << endl;
								this->acmEcmUsbInit();
								break;
							case 3:		// Linux selected
								cout << "this->acmEcmUsbInit()" << endl;
								this->acmEcmUsbInit();
								break;
							default:
								status = -1;
						}
		}

	}
	catch(exception &e)
	{
		cout << "exception in UsbInt::configureUsbGadgetForOs: " << e.what() << endl;
		status = -1;
	}

	if(status == 0) this->usbConfigStatus = true;
	else this->usbConfigStatus = false;
#if(dbg >= 1)
	cout << "********** EXITING UsbInt::configureUsbGadgetForOs: " << status << endl;
#endif

	return status;
}


bool UsbInt::isUsbConfigured()
{
	return this->usbConfigStatus;

}
bool UsbInt::isPortOpen()
{

	return this->portOpenStatus;
}

#define dbg 1

int UsbInt::openPort()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::openPort: " <<  endl;
#endif

	{
		this->usbFD = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);
		if(this->usbFD >= 0)
		{
			this->portOpenStatus = true;
			status = 0;
		}
		else
		{
			if(debugOutput) cout << "failed to open USB:"  << errno << endl;
			this->portOpenStatus = false;
		}
	}
#if(dbg >= 1)
	cout << "********** EXITING UsbInt::openPort: " << status << endl;
#endif

	return status;
}

#define dbg 1
int UsbInt::closePort()
{
	errno = 0;
	int status = -1;
#if(dbg >= 1)
	cout << "********** ENTERING UsbInt::closePort: " <<  endl;
#endif

	if(close(this->usbFD) >= 0)
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


#define dbg 0
string UsbInt::readNewData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::getNewData" << endl;
#endif
	char usbInputBuffer[FILE_SIZE];
	clearBuffer(usbInputBuffer,FILE_SIZE);

	bool stringIsValid = false;
	int status = 0;
	string usbString;

	ssize_t size_read = read(this->usbFD, usbInputBuffer, FILE_SIZE);

	if(size_read > 1)
	{
		usbString = string(usbInputBuffer);
		stringIsValid = validateString(usbString);
#if(dbg >= 1)
		if(debugOutput) cout << "USB received size: " << strlen(this->usbInputBuffer) << ":\t" << this->usbInputBuffer << endl;
		if(debugOutput) cout << "clean data size: " << strlen(this->usbCleanInputBuffer )<< ":\t" << this->usbCleanInputBuffer << endl;
#endif
		status = 0;
	}
	else status = -1;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::getNewData" << status << endl;
#endif
	if(stringIsValid) return usbString;
	else return string("");
}


#define dbg 0
int UsbInt::writeData(string input)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: UsbInt::writeData" << endl;
#endif

#if(dbg >= 2)

	if(debugOutput) cout << "dataString: " << this->usbOutputBuffer << endl;
#endif

	ssize_t size_write = write(this->usbFD, input.c_str(), input.size());

	if(size_write >= 0) status = 0;
	else status = -1;

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: UsbInt::writeData" << endl;
#endif

	return status;
}
