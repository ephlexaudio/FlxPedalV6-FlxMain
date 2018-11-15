/*
 * UsbInt.cpp
 *
 *  Created on: Nov 16, 2016
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "UsbInt.h"

namespace std
{



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
int UsbInt::configureUsbGadgetForOs(string selectedOs)
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
			if(selectedOs.compare("Win") == 0)
			{
				cout << "this->acmRndisUsbInit()" << endl;
				this->acmRndisUsbInit();
			}
			else if(selectedOs.compare("Mac") == 0)
			{
				cout << "this->acmEcmUsbInit()" << endl;
				this->acmEcmUsbInit();
			}
			else if(selectedOs.compare("Lin") == 0)
			{
				cout << "this->acmEcmUsbInit()" << endl;
				this->acmEcmUsbInit();
			}
			else
			{
				cout << "invalid option" << endl;
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
			 cout << "failed to open USB:"  << errno << endl;
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
		 cout << "failed to close USB: " << errno << endl;
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
	 cout << "***** ENTERING: UsbInt::getNewData" << endl;
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
		 cout << "USB received size: " << strlen(usbInputBuffer) << ":\t" << usbInputBuffer << endl;
#endif
		status = 0;
	}
	else status = -1;

#if(dbg >= 1)
	 cout << "***** EXITING: UsbInt::getNewData: " << status << endl;
#endif
	if(stringIsValid >= 0)return usbString;
	else return string("");
}


#define dbg 0
int UsbInt::writeData(string input)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: UsbInt::writeData" << endl;
#endif

#if(dbg >= 2)

	 cout << "dataString: " << this->usbOutputBuffer << endl;
#endif

	ssize_t size_write = write(this->usbFD, input.c_str(), input.size());

	if(size_write >= 0) status = 0;
	else status = -1;

#if(dbg >= 1)
	 cout << "***** EXITING: UsbInt::writeData" << endl;
#endif

	return status;
}
}
