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
#include "DataFuncts.h"
#include "GPIOClass.h"
//using namespace std;

class HostUiInt
{
private:
	//char comboData[16000];
	UsbInt usb;
	char hostUiRawRequestCharArray[16000];
	char hostUiRequestCharArray[16000];
	char hostUiResponseCharArray[16000];


public:

	HostUiInt();
	virtual ~HostUiInt();

	// Files are retrieved from the directory and sent straight to the host

	int checkForNewHostData(void);
	string getUserRequest(void);
	int sendComboList(string comboLists);
	int sendCurrentStatus(char *currentStatus);
	int sendCurrentData(vector<IndexedParameter> currentParams);
	int sendComboToHost(string comboName);
	int getComboFromHost(string comboData);
	int sendSimpleResponse(char *response);

};

#endif /* HOSTUIINT_H_ */
