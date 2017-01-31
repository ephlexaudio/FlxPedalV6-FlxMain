/*
 * HostUiInt.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "BaseUiInt.h"
#include "HostUiInt.h"


#define FILE_SIZE 32000
//#define JSON_STRING_SIZE 32000
#define SHARED_MEMORY_FILE_ADDRESS 32768

/*
#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::" << endl;
#endif
*/
using namespace std;

HostUiInt::HostUiInt()
{
	// TODO Auto-generated constructor stub
	UsbInt usb = UsbInt();
	clearBuffer(this->hostUiRawRequestCharArray,FILE_SIZE);
	clearBuffer(this->hostUiRequestCharArray,FILE_SIZE);
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

}

HostUiInt::~HostUiInt() {
	// TODO Auto-generated destructor stub
}

int HostUiInt::connect()
{
	int status = 0;

	status = usb.connect();

	return status;
}

int HostUiInt::disconnect()
{
	int status = 0;

	status = usb.disconnect();

	return status;
}

int HostUiInt::isConnected()
{
	int status = 0;

	status = usb.isConnected();

	return status;
}

#define dbg 0
int HostUiInt::checkForNewHostData(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::checkForNewHostData" << endl;
#endif

	int status = usb.newData();

#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::checkForNewHostData" << endl;
#endif
	return status;
}

string HostUiInt::getUserRequest(void)
{

	return string(usb.readData());
}


#define dbg 2
int HostUiInt::sendComboList(string comboLists)
{

	int status = 0;
#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::sendComboList" << endl;
#endif

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	sprintf(this->hostUiResponseCharArray,"ComboList:%s",(char *)comboLists.c_str());
	errno = 0;
	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
		status = 0;
#if(dbg >= 2)
		cout << this->hostUiResponseCharArray << endl;
#endif
	}
	else
	{
		status = -1;
#if(dbg >= 2)
		cout << "combo list end fail: " << errno << endl;
#endif
	}

#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::sendComboList" << endl;
#endif


	return status;
}

#define dbg 0
int HostUiInt::sendCurrentStatus(char *currentStatus)
{
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::sendCurrentStatus" << endl;
#endif

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	sprintf(this->hostUiResponseCharArray,"CurrentStatus:%s",currentStatus);
	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}


#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::sendCurrentStatus" << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendCurrentData(vector<IndexedParameter> currentParams)
{
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::sendCurrentData" << endl;
#endif

	char paramString[50];
	cout << "currentParams size: " << currentParams.size() << endl;
	clearBuffer(paramString,50);
	sprintf(paramString,"CurrentData:{paramValue:0}");
	if(usb.writeData(paramString) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}

#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::sendCurrentData" << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendComboToHost(string comboName)
{
	#if(dbg >= 1)
		cout << "***** ENTERING: HostUiInt::sendComboToHost" << endl;
	#endif

	int status = 0;
	string comboString;

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	//strcpy(this->hostUiResponseCharArray, getComboData(comboName).c_str());

	comboString = getComboDataFromFS(comboName);
	if(comboString.empty() == false)
	{

		sprintf(this->hostUiResponseCharArray,"ComboData:%s",comboString.c_str());

	#if(dbg >= 2)
		cout << "Data retrieved, sending to host...." << this->hostUiResponseCharArray << endl;
	#endif

		if(usb.writeData(this->hostUiResponseCharArray) >= 0)
		{
	#if(dbg >= 2)
			cout << "combo data: " << this->hostUiResponseCharArray << endl;
	#endif

		}
		else
		{
			cout << "error sending data to host" << endl;
			status = -1;
		}

	}
	else
	{
		status = -1;
		cout << "error reading data" << endl;
	}

	//clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

	#if(dbg >= 1)
		cout << "***** EXITING: HostUiInt::sendComboToHost" << endl;
	#endif

	return status;
}

#define dbg 1
string HostUiInt::getComboFromHost(string comboJson)
{
	#if(dbg >= 1)
		cout << "***** ENTERING: HostUiInt::getComboFromHost" << endl;
	#endif

		clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
		clearBuffer(this->hostUiRawRequestCharArray,FILE_SIZE);

	char filteredComboData[FILE_SIZE];
	string comboName;
	int status = 0;


	{
		//int retVal;
		bool done = false;

		strcpy(this->hostUiRawRequestCharArray, comboJson.c_str()/*usb.readData()*/);
		cout << "USB data size: " << strlen(comboJson.c_str()/*usb.readData()*/) << endl;
		if(strlen(usb.readData()) < 4000)
			done = true;

		while(!done)
		{
			//cout << "waiting for next packet..." << endl;
			if(usb.newData() == 1)
			{
				strcat(this->hostUiRawRequestCharArray, usb.readData());
				cout << "USB data size: " << strlen(usb.readData()) << endl;
				if(strlen(usb.readData()) < 4000) done = true;
			}
			/*else
			{
				cout << "error getting data from host." << endl;
				status = 1;
				break;
			}*/
		}
		//if(this->getData(SHARED_MEMORY_FILE_ADDRESS, this->comboJson, SHARED_MEMORY_FILE_SIZE) == 0)
		{
			// Filter data first
			int filteredDataIndex = 0;
	#if(dbg >= 2)
				cout << "filtering data" << endl;
	#endif
			for(int dataIndex = 0; this->hostUiRawRequestCharArray[dataIndex] != 0 && dataIndex < FILE_SIZE; dataIndex++)
			{
				if(' ' <= this->hostUiRawRequestCharArray[dataIndex] && this->hostUiRawRequestCharArray[dataIndex] <= '~')
				{
					filteredComboData[filteredDataIndex] = this->hostUiRawRequestCharArray[dataIndex];
		#if(dbg >= 2)
					cout << filteredComboData[filteredDataIndex] << ',';
		#endif
					filteredDataIndex++;
				}
			}
#if(dbg >= 2)
			cout << "data filtered" << endl;
			cout << "filtered combo data: " << filteredComboData << endl;
#endif
			comboName = saveComboToFS(string(filteredComboData));
			if(comboName.empty() == true)
			{
				cout << "error saving combo." << endl;
				status = -1;
			}
		}
	}

#if(dbg >= 2)
	cout << "combo data: " << this->hostUiRawRequestCharArray << endl;
#endif
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

	#if(dbg >= 1)
		cout << "***** EXITING: HostUiInt::getComboFromHost" << endl;
	#endif

	return comboName;
}



int HostUiInt::sendSimpleResponse(char *response)
{
	int status = 0;

#if(dbg >= 1)
	cout << "***** ENTERING: HostUiInt::sendSimpleResponse" << endl;
#endif

	if(usb.writeData(response) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}

#if(dbg >= 1)
	cout << "***** EXITING: HostUiInt::sendSimpleResponse" << endl;
#endif


	return status;
}
