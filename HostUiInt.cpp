/*
 * HostUiInt.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#include "config.h"
//#include "BaseUiInt.h"
#include "HostUiInt.h"


#define FILE_SIZE 32000
//#define JSON_STRING_SIZE 32000
#define SHARED_MEMORY_FILE_ADDRESS 32768
extern vector<string> componentVector;
extern vector<string> controlTypeVector;
extern bool debugOutput;

/*
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::" << endl;
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
#define dbg 1

int HostUiInt::connect()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::connect" << endl;
#endif

	status = usb.connect();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::connect: " << status << endl;
#endif

	return status;
}

int HostUiInt::disconnect()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::disconnect" << endl;
#endif

	status = usb.disconnect();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::disconnect:" << status << endl;
#endif

	return status;
}

#define dbg 1
int HostUiInt::sendComponentData(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendComponentData" << endl;
#endif
	string componentDataString;
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

	sprintf(this->hostUiResponseCharArray,"<ComponentData>[");
	for(vector<string>::size_type componentIndex = 0; componentIndex < componentVector.size(); componentIndex++)
	{
		strcat(this->hostUiResponseCharArray,componentVector[componentIndex].c_str());
		if(componentIndex < componentVector.size()-1)
			strcat(this->hostUiResponseCharArray,",");
	}
	strcat(this->hostUiResponseCharArray,"]</ComponentData>");

	#if(dbg >= 2)
		if(debugOutput) cout << "Data retrieved, sending to host...." << this->hostUiResponseCharArray << endl;
	#endif

	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "combo data: " << this->hostUiResponseCharArray << endl;
#endif

	}
	else
	{
		if(debugOutput) cout << "error sending data to host" << endl;
		status = -1;
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendComponentData:" << status << endl;
#endif

	return status;
}

#define dbg 1
int HostUiInt::sendControlTypeData(void)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendControlTypeData" << endl;
#endif
	string controlTypeDataString;
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

	sprintf(this->hostUiResponseCharArray,"<ControlTypeData>[");
	for(vector<string>::size_type controlTypeIndex = 0; controlTypeIndex < controlTypeVector.size(); controlTypeIndex++)
	{
		strcat(this->hostUiResponseCharArray,controlTypeVector[controlTypeIndex].c_str());
		if(controlTypeIndex < controlTypeVector.size()-1)
			strcat(this->hostUiResponseCharArray,",");
	}
	strcat(this->hostUiResponseCharArray,"]</ControlTypeData>");

	#if(dbg >= 2)
		if(debugOutput) cout << "Data retrieved, sending to host...." << this->hostUiResponseCharArray << endl;
	#endif

	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
#if(dbg >= 2)
		if(debugOutput) cout << "combo data: " << this->hostUiResponseCharArray << endl;
#endif

	}
	else
	{
		if(debugOutput) cout << "error sending data to host" << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendControlTypeData:" << status << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::isConnected()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::isConnected" << endl;
#endif


	status = usb.isConnected();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::isConnected: " << status << endl;
#endif
	return status;
}

#define dbg 0
int HostUiInt::checkForNewHostData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::checkForNewHostData" << endl;
#endif

	int status = usb.newData();

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::checkForNewHostData: " << status << endl;
#endif
	return status;
}

#define dbg 0
string HostUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::getUserRequest" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::getUserRequest" << usb.readData() << endl;
#endif
	return string(usb.readData());
}


#define dbg 1
int HostUiInt::sendComboList(string comboLists)
{

	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendComboList" << endl;
#endif

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	sprintf(this->hostUiResponseCharArray,"<ComboList>%s</ComboList>",(char *)comboLists.c_str());
	errno = 0;
	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
		status = 0;
#if(dbg >= 2)
		if(debugOutput) cout << this->hostUiResponseCharArray << endl;
#endif
	}
	else
	{
		status = -1;
#if(dbg >= 2)
		if(debugOutput) cout << "combo list end fail: " << errno << endl;
#endif
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendComboList: " << status << endl;
#endif


	return status;
}

#define dbg 0
int HostUiInt::sendCurrentStatus(char *currentStatus)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendCurrentStatus" << endl;
#endif

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	sprintf(this->hostUiResponseCharArray,"<CurrentStatus>%s</CurrentStatus>",currentStatus);
	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendCurrentStatus: " << status << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendCurrentData(vector<IndexedParameter> currentParams)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendCurrentData" << endl;
#endif

	char paramString[50];
	if(debugOutput) cout << "currentParams size: " << currentParams.size() << endl;
	clearBuffer(paramString,50);
	sprintf(paramString,"<CurrentData>{paramValue:0}</CurrentData>");
	if(usb.writeData(paramString) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendCurrentData: " << status << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendComboToHost(string comboName)
{
	#if(dbg >= 1)
		if(debugOutput) cout << "***** ENTERING: HostUiInt::sendComboToHost" << endl;
		if(debugOutput) cout << "comboName: " << comboName << endl;
	#endif

	int status = 0;
	string comboString;

	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
	//strcpy(this->hostUiResponseCharArray, getComboData(comboName).c_str());

	comboString = getComboDataFromFileSystem(comboName);
	if(comboString.empty() == false)
	{

		sprintf(this->hostUiResponseCharArray,"<ComboData>%s</ComboData>",comboString.c_str());

	#if(dbg >= 2)
		if(debugOutput) cout << "Data retrieved, sending to host...." << this->hostUiResponseCharArray << endl;
	#endif

		if(usb.writeData(this->hostUiResponseCharArray) >= 0)
		{
	#if(dbg >= 2)
			if(debugOutput) cout << "combo data: " << this->hostUiResponseCharArray << endl;
	#endif

		}
		else
		{
			if(debugOutput) cout << "error sending data to host" << endl;
			status = -1;
		}

	}
	else
	{
		status = -1;
		if(debugOutput) cout << "error reading data" << endl;
	}

	//clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendComboToHost: " << status << endl;
#endif

	return status;
}

#define dbg 1
string HostUiInt::getComboFromHost(string comboJson)
{
	#if(dbg >= 1)
		if(debugOutput) cout << "***** ENTERING: HostUiInt::getComboFromHost" << endl;
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
		if(debugOutput) cout << "USB data size: " << strlen(/*comboJson.c_str()*/usb.readData()) << endl;
		if(strlen(usb.readData()) < 4000)
			done = true;

		while(!done)
		{
			//if(debugOutput) cout << "waiting for next packet..." << endl;
			if(usb.newData() == 1)
			{
				strcat(this->hostUiRawRequestCharArray, usb.readData());
				if(debugOutput) cout << "USB data size: " << strlen(usb.readData()) << endl;
				if(strlen(usb.readData()) < 4000) done = true;
			}
			/*else
			{
				if(debugOutput) cout << "error getting data from host." << endl;
				status = 1;
				break;
			}*/
		}
		//if(this->getData(SHARED_MEMORY_FILE_ADDRESS, this->comboJson, SHARED_MEMORY_FILE_SIZE) == 0)
		{
			// Filter data first
			int filteredDataIndex = 0;
	#if(dbg >= 2)
				if(debugOutput) cout << "filtering data" << endl;
	#endif
			for(int dataIndex = 0; this->hostUiRawRequestCharArray[dataIndex] != 0 && dataIndex < FILE_SIZE; dataIndex++)
			{
				if(' ' <= this->hostUiRawRequestCharArray[dataIndex] && this->hostUiRawRequestCharArray[dataIndex] <= '~')
				{
					filteredComboData[filteredDataIndex] = this->hostUiRawRequestCharArray[dataIndex];
		#if(dbg >= 2)
					if(debugOutput) cout << filteredComboData[filteredDataIndex] << ',';
		#endif
					filteredDataIndex++;
				}
			}
#if(dbg >= 2)
			if(debugOutput) cout << "data filtered" << endl;
			if(debugOutput) cout << "filtered combo data: " << filteredComboData << endl;
#endif
			comboName = saveComboToFileSystem(string(filteredComboData));
			if(comboName.empty() == true)
			{
				if(debugOutput) cout << "error saving combo." << endl;
				status = -1;
			}
		}
	}

#if(dbg >= 2)
	if(debugOutput) cout << "combo data: " << this->hostUiRawRequestCharArray << endl;
#endif
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::getComboFromHost: " << comboName << endl;
#endif

	return comboName;
}



int HostUiInt::sendSimpleResponse(char *response)
{
	int status = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: HostUiInt::sendSimpleResponse" << endl;
	if(debugOutput) cout << "response: " << response << endl;
#endif
	sprintf(this->hostUiResponseCharArray,"<SimpleResponse>%s</SimpleResponse>",response);
	if(usb.writeData(this->hostUiResponseCharArray) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}
	clearBuffer(this->hostUiResponseCharArray,FILE_SIZE);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: HostUiInt::sendSimpleResponse: " << status << endl;
#endif


	return status;
}
