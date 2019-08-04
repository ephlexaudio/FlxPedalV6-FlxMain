/*
 * HostUiInt.h
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#ifndef HOSTUIINT_H_
#define HOSTUIINT_H_

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "utilityFunctions.h"
#include "FileSystemInt.h"
#include "structs.h"
#include "UsbInt.h"



class HostUiInt : public UsbInt
{
private:

	FileSystemInt fsInt;
public:

	HostUiInt();
	~HostUiInt();
	// Files are retrieved from the directory and sent straight to the host
	string getUserRequest(void);

	int sendControlTypeData(vector<string>controlTypeDataVector);
	int sendComponentData(vector<string> componentDataVector);
	int sendComboList(string comboLists);
	int sendComboToHost(string comboName);
	string getComboFromHost(string comboData);
	int sendCurrentStatus(string currentStatus);
	int setHostPcUtility(HostPcUtility hostUtil);

};

#endif /* HOSTUIINT_H_ */
