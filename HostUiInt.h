/*
 * HostUiInt.h
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#ifndef HOSTUIINT_H_
#define HOSTUIINT_H_

#include "UsbInt.h"
#include "utilityFunctions.h"
#include "FileSystemFuncts.h"
#include "GPIOClass.h"
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
	int isConnected();

	int checkForNewHostData(void);
	string getUserRequest(void);
	int sendComboList(string comboLists);
	int sendCurrentStatus(char *currentStatus);
	int sendCurrentData(vector<IndexedParameter> currentParams);
	int sendComboToHost(string comboName);
	string getComboFromHost(string comboData);
	int sendSimpleResponse(char *response);

};

#endif /* HOSTUIINT_H_ */
