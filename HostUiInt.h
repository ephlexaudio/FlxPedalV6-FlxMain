/*
 * HostUiInt.h
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#ifndef HOSTUIINT_H_
#define HOSTUIINT_H_

#include "UsbInt.h"
#include <vector>
#include "utilityFunctions.h"
#include "FileSystemFuncts.h"
#include "GPIOClass.h"
#include "structs.h"
//using namespace std;
#define FILE_SIZE 32000
class HostUiInt
{
private:
	//char comboData[FILE_SIZE];
	UsbInt usb;
	char hostUiRawRequestCharArray[FILE_SIZE];
	char hostUiRequestCharArray[FILE_SIZE];
	char hostUiResponseCharArray[FILE_SIZE];


public:

	HostUiInt();
	virtual ~HostUiInt();
	// Files are retrieved from the directory and sent straight to the host
	int connect();
	int disconnect();
	int open();
	bool isPortOpen();
	int close();
	bool isConnected();
	int sendControlTypeData();
	int sendComponentData();
	int checkForNewHostData(void);
	string getUserRequest(void);
	int sendComboList(string comboLists);
	int sendCurrentStatus(char *currentStatus);
	int sendCurrentData(vector<IndexedProcessParameter> currentParams);
	int sendComboToHost(string comboName);
	string getComboFromHost(string comboData);
	int sendSimpleResponse(char *response);
	void clearAllInternalCharArrays(void);
};

#endif /* HOSTUIINT_H_ */
