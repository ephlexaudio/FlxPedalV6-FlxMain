/*
 * HostUiInt.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#include "HostUiInt.h"
#include "config.h"


using namespace std;



HostUiInt::HostUiInt()
{
	// TODO Auto-generated constructor stub

}

HostUiInt::~HostUiInt() {
	// TODO Auto-generated destructor stub
}


#define dbg 0
string HostUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::getUserRequest" << endl;
#endif
	string newData = this->readNewData();
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::getUserRequest" << newData << endl;
#endif
	return newData;
}


#define dbg 0
int HostUiInt::sendComboList(string comboLists)
{

	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendComboList" << endl;
#endif
	string hostUiResponse;

	hostUiResponse = "<ComboList>";
	hostUiResponse += comboLists;
	hostUiResponse += "</ComboList>";
	errno = 0;
	if(this->writeData(hostUiResponse) >= 0)
	{
		status = 0;
#if(dbg >= 2)
		 cout << hostUiResponse << endl;
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
	 cout << "***** EXITING: HostUiInt::sendComboList: " << status << endl;
#endif


	return status;
}


#define dbg 0
int HostUiInt::sendComponentData(vector<string> componentDataVector)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendComponentData" << endl;
#endif
	string hostUiResponse;
	string componentDataString;

	hostUiResponse = "<ComponentData>[";
	size_t componentIndex = 0;
	for(auto & component : componentDataVector)
	{
		hostUiResponse += component;
		if(componentIndex < componentDataVector.size()-1)
			hostUiResponse += ",";
		componentIndex++;
	}
	hostUiResponse += "]</ComponentData>";

	#if(dbg >= 2)
		 cout << "Data retrieved, sending to host...." << hostUiResponse << endl;
	#endif

	if(this->writeData((char *)hostUiResponse.c_str()) >= 0)
	{
#if(dbg >= 2)
		 cout << "combo data: " << hostUiResponse << endl;
#endif

	}
	else
	{
		 cout << "error sending data to host" << endl;
		status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendComponentData:" << status << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendControlTypeData(vector<string>controlTypeDataVector)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendControlTypeData" << endl;
#endif
	string hostUiResponse;
	string controlTypeDataString;

	hostUiResponse = "<ControlTypeData>[";
	size_t controlTypeIndex = 0;
	for(auto & controlType : controlTypeDataVector)
	{
		hostUiResponse += controlType;
		if(controlTypeIndex < controlTypeDataVector.size()-1)
			hostUiResponse += ",";
		controlTypeIndex++;
	}
	hostUiResponse += "]</ControlTypeData>";

	#if(dbg >= 2)
		 cout << "Data retrieved, sending to host...." << hostUiResponse << endl;
	#endif

	if(this->writeData((char *)hostUiResponse.c_str()) >= 0)
	{
#if(dbg >= 2)
		 cout << "combo data: " << hostUiResponse << endl;
#endif
	}
	else
	{
		 cout << "error sending data to host" << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendControlTypeData:" << status << endl;
#endif

	return status;
}


#define dbg 0
int HostUiInt::sendComboToHost(string comboName)
{
	#if(dbg >= 1)
		 cout << "******************************************" << endl;
		 cout << "***** ENTERING: HostUiInt::sendComboToHost" << endl;
		 cout << "comboName: " << comboName << endl;
		 cout << "******************************************" << endl;
		startTimer();
	#endif

	int status = 0;
	string comboString;
	string hostUiResponse;


	comboString = fsInt.loadComboDataFromFileSystemToHost(comboName);
	if(comboString.empty() == false)
	{
		int checkSum = 0;
		for(auto & c : comboString)
		{
			if(c > 31)
			{
				checkSum += c;
			}

		}

		char checkSumCharArray[10] = "";
		snprintf(checkSumCharArray,10,"%d",checkSum);
		hostUiResponse = "<ComboData>";
		hostUiResponse += comboString + "#" + string(checkSumCharArray);
		hostUiResponse += "</ComboData>";

	#if(dbg >= 2)
		 cout << "Data retrieved, sending to host...." << hostUiResponse << endl;
	#endif

		if(this->writeData(hostUiResponse) >= 0)
		{
	#if(dbg >= 2)
			 cout << "combo data: " << hostUiResponse << endl;
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


#if(dbg >= 1)
	 cout << "******************************************" << endl;
	 cout << "***** EXITING: HostUiInt::sendComboToHost: " << status << endl;
	 stopTimer("sendComboToHost");
	 cout << "******************************************" << endl;
#endif

	return status;
}

#define dbg 1
string HostUiInt::getComboFromHost(string hostComboJson)
{
	#if(dbg >= 1)
		 cout << "******************************************" << endl;
		 cout << "***** ENTERING: HostUiInt::getComboFromHost" << endl;
		 cout << "******************************************" << endl;
		startTimer();
	#endif

	string hostUiRequest;
	string comboData;
	string comboName;
	bool done = false;
	string usbData;
	unsigned newDataSize = 0;
	comboData.clear();
	comboName.clear();

	/* The Linux USB buffer is 4000 bytes in size.
	 * comboJson is the first 4000 bytes of the combo file data from the host.  The file size will usually be
	 * greater than 4000, so the while loop below reads in the 4000-byte blocks, until it gets a block
	 * less than 4000 in size, indicating the end of the combo file.
	 */


	comboData = hostComboJson;
	 cout << "USB data size: " << comboData.size() << endl;
	if(comboData.size() < 3990) // "saveCombo:" is removed, cutting string from 4000 to 3990
		done = true;

	while(!done)
	{
		string newData = this->readNewData();

		if(newData.empty() == false)
		{
			newDataSize = newData.size();
			comboData += newData;
			 cout << "USB data size: " << newDataSize << endl;
			if(newDataSize < 4000) done = true;
		}
	}
	hostUiRequest = comboData;

	comboName = fsInt.saveComboDataFromHostToFileSystem(hostUiRequest);
	if(comboName.empty() == true)
	{
		 cout << "error saving combo." << endl;
	}


#if(dbg >= 2)
	 cout << "combo data: " << hostUiRequest << endl;
#endif

#if(dbg >= 1)
	 cout << "******************************************" << endl;
	 cout << "***** EXITING: HostUiInt::getComboFromHost: " << comboName << endl;
	 cout << stopTimer("getComboFromHost");
	 cout << "******************************************" << endl;
#endif

	return comboName;
}

#define dbg 0
int HostUiInt::sendCurrentStatus(string currentStatus)
{
	int status = 0;

#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendCurrentStatus" << endl;
#endif


	string hostUiReponseString;

	hostUiReponseString = "<CurrentStatus>" + currentStatus + "</CurrentStatus>";
	if(this->writeData(hostUiReponseString) >= 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendCurrentStatus: " << status << endl;
#endif

	return status;
}

int HostUiInt::setHostPcUtility(HostPcUtility hostUtil)
{
	int status = 0;

		try
		{
			cout << "hostUtil.os.option: " << hostUtil.os.option << endl;
			this->configureUsbGadgetForOs(hostUtil.os.option);
		}
		catch(exception &e)
		{
			cout << "exception in HostUiInt::processUtilityChange: " << e.what() << endl;
			status = -1;
		}

	return status;
}




