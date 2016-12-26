/*
 * BaseUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

#include "BaseUiInt.h"
//#include "ComboDataInt.h"
#include "utilityFunctions.h"

extern int comboTime;
/*
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::" << endl;
#endif
*/
#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define SHARED_MEMORY_SECTION_SIZE 4096
/*#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1*/
#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500
#define SEND_BUFFER_SIZE 250

#define RQST_DATA_FILTER_COUNT 5

#define SHARED_MEMORY_SOURCE 4
#define SHARED_MEMORY_READY 29/*"44"*/



extern int procCount;
extern bool hostGuiActive;
extern int comboIndex;

/*
 *  Use this signature to ONLY detect new data (not access it), and
 *  wait for shared memory access release
 */
BaseUiInt::BaseUiInt()
{
	this->status = 0;
	this->uiSectionStartAddress = 0;
	this->uiSectionSize = 0;
	this->cmSectionStartAddress = 0;
	this->cmSectionSize = 0;

	strcpy(this->gpioStr, "in");
	newData = GPIOClass(SHARED_MEMORY_READY);
	newData.export_gpio();
	newData.setdir_gpio(this->gpioStr);

	dataSource = GPIOClass(SHARED_MEMORY_SOURCE);
	dataSource.export_gpio();
	dataSource.setdir_gpio(this->gpioStr);

}

/*
 * Use this signature for accessing data from different types of user interfaces.
 */
BaseUiInt::BaseUiInt(unsigned int cmSectionStartAddress, unsigned int cmSectionSize,
		unsigned int uiSectionStartAddress, unsigned int uiSectionSize):SharedMemoryInt()
{
	this->status = 0;
	this->uiSectionStartAddress = uiSectionStartAddress;
	this->uiSectionSize = uiSectionSize;
	this->cmSectionStartAddress = cmSectionStartAddress;
	this->cmSectionSize = cmSectionSize;
	cout << "uiSectionStartAddress: " << this->uiSectionStartAddress
			<< "\tuiSectionSize" << this->uiSectionSize
			<<  "\tcmSectionStartAddress" << this->cmSectionStartAddress
			<<  "\tcmSectionSize" << this->cmSectionSize << endl;

	/*newData = GPIOClass(SHARED_MEMORY_READY);
	newData.export_gpio();
	newData.setdir_gpio("in");*/
	strcpy(this->gpioStr, "in");
	dataSource = GPIOClass(SHARED_MEMORY_SOURCE);
	dataSource.export_gpio();
	dataSource.setdir_gpio(this->gpioStr);


}

BaseUiInt::~BaseUiInt()
{
	int ret;

	this->status = 0;

//	ret = close(this->spiFD);
//	if(ret == -1)
//	{
//		this->status = 1;
//		pabort("couldn't close SPI port");
//	}
}

/*
 * 	Use this signature to detect shared memory access.
 * 	Returns sectionIndex (0:no data, 1: data from pedal UI , 2: data from host UI)
 * 	if access is detected.
 */
#define dbg 0
int BaseUiInt::checkForNewData(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::checkForNewData" << endl;
#endif

	int pinString;
	int source = 0; // 0:no data, 1: data from pedal UI , 2: data from host UI
	int/*uint8_t*/ pinValue = 0;
	newData.getval_gpio(pinValue);

	if(pinValue == 0) // chip select activated to CM shared memory access
	{
		dataSource.getval_gpio(pinValue);
		if(pinValue == 0) // source is pedal UI
		{
			source = 1;
		}
		else if(pinValue == 1) // source is host UI
		{
			source = 2;
		}
#if(dbg >= 2)
		cout << "source: " << source << endl;
#endif
	}

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::checkForNewData" << endl;
#endif

	return source;
}

// Use this signature to access data in shared memory section
#define dbg 0
int BaseUiInt::checkForNewData(int sectionIndex)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::checkForNewData 2" << endl;
#endif

	int pinString;
	int charCount = 0;
	volatile int dataStatus = 0;
	int/*uint8_t*/ pinValue = 0;
	int tempBufferIndex = 4;
	char tempBuffer[100];
	//newData.getval_gpio(pinString);
	//pinValue = atoi(pinString.c_str());


	if(this->getData(SHARED_MEMORY_SECTION_SIZE*sectionIndex, tempBuffer, 100) == 0)
	{
		for(tempBufferIndex = 0; tempBufferIndex < 100; tempBufferIndex++)
		{
			if(' ' <= tempBuffer[tempBufferIndex] && tempBuffer[tempBufferIndex] <= '~')
			{
				this->request[charCount++] = tempBuffer[tempBufferIndex];
				//charCount++;
#if(dbg >= 2)
				cout << "tempBuffer[" << tempBufferIndex << "]: valid data" << endl;
#endif
			}
			else if(charCount > RQST_DATA_FILTER_COUNT && tempBuffer[tempBufferIndex] == 255) // found end marker, data is ready
			{
				dataStatus = 1;
				this->request[charCount] = 0;
#if(dbg >= 2)
				cout << "found end marker, data is ready" << endl;
#endif
				break;
			}
			else if(tempBuffer[tempBufferIndex] == 0)
			{
				// do nothing
			}
			else // corrupt data
			{
#if(dbg >= 2)
				cout << "tempBuffer[" << tempBufferIndex << "]: corrupt data" << endl;
#endif
				break;
			}
		}
	}
	else
	{
		cout << "error getting data." << endl;
		dataStatus = 0;
	}

	//cout << "status: " << status << endl;

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::checkForNewData 2" << endl;
#endif

	return dataStatus;
}



#define dbg 0
string BaseUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::getUserRequest" << endl;
#endif
	char responseCharArray[200];
	uint8_t responseCharArrayIndex = 0;
	string responseString;

	uint8_t status = 0;
	/*int retVal;

	retVal = this->getData(this->cmSectionStartAddress, this->request, 50);
	//uint8_t intString[50];*/

	// Clean up data
	for(uint8_t i = 0; i < 50; i++)
	{
		if((' ' <= this->request[i] && this->request[i] <= '~') || this->request[i] == 0)
		{
			responseCharArray[responseCharArrayIndex++] = this->request[i];
		}
	}

	/*if(retVal < 0)
	{
		responseString = string("error");
	}
	else
	{
		responseString = string(responseCharArray);
	}*/

	// Data already retrieved in checkForNewData
	responseString = string(responseCharArray);


#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::getUserRequest" << endl;
#endif

	return responseString;
}

int BaseUiInt::processUserRequest(char *request)
{

#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::processUserRequest" << endl;
#endif

	if(this->status == 1) return 1;
	uint8_t status = 0;

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::processUserRequest" << endl;
#endif

	return status;
}

#define dbg 0

/*uint8_t BaseUiInt::sendComboUiData(Json::Value uiData)
{
	//if(this->status == 1) return 1;
	uint8_t status = 0;
	uint8_t effectIndex = 0;
	uint8_t effectCount = 0;
	uint8_t guiParamIndex = 0;
	uint8_t guiParamCount = 0;

	const int length = 10;
	//struct spi_ioc_transfer spi;

	int retVal;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;
	for(int i = 0; i < RX_BUFFER_SIZE; i++) this->request[i] = 0;
	//this->uiData[0] = 3;


	strcpy((char *)this->uiData, "{title:");
	strncat((char *)this->uiData, uiData["title"].asCString(), strlen(uiData["title"].asString().c_str()));

	{
		strcat((char *)this->uiData, ",effects:[");
		effectCount = uiData["effects"].size();
		for(effectIndex = 0; effectIndex < effectCount; effectIndex++)
		{
			guiParamCount = uiData["effects"][effectIndex]["params"].size();
			if(guiParamCount > 0)
			{
				strcat((char *)this->uiData, "{abbr:");
				strncat((char *)this->uiData, uiData["effects"][effectIndex]["abbr"].asString().c_str(),
						strlen(uiData["effects"][effectIndex]["abbr"].asCString()));
				strcat((char *)this->uiData, ",name:");
				strncat((char *)this->uiData, uiData["effects"][effectIndex]["name"].asString().c_str(),
						strlen(uiData["effects"][effectIndex]["name"].asCString()));
				strcat((char *)this->uiData, ",params:[");

				for(guiParamIndex = 0; guiParamIndex < guiParamCount; guiParamIndex++)
				{
					if(uiData["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().size() > 1
        					&& uiData["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().compare("none") != 0)
					{
						strcat((char *)this->uiData, "{");
						char paramStringBuffer[15];
						char stringBuffer[SEND_BUFFER_SIZE];
						clearBuffer(stringBuffer,SEND_BUFFER_SIZE);

						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiData["effects"][effectIndex]["params"][guiParamIndex]["abbr"].asString().c_str());
	#if(dbg>=2)
						strcpy(stringBuffer,"abbr:");//puts("abbr:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiData["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", name:");//puts("name:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiData["effects"][effectIndex]["params"][guiParamIndex]["value"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", value:");//puts("value:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiData["effects"][effectIndex]["params"][guiParamIndex]["type"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", type:");//puts("type:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiData["effects"][effectIndex]["params"][guiParamIndex]["index"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", type:");//puts("type:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
						puts(stringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));
						strcat((char *)this->uiData, "}");


						if(guiParamIndex != guiParamCount-1) strcat((char *)this->uiData, ",");

					}
				}
				strcat((char *)this->uiData, "]}");
				if(effectIndex != effectCount-1 && uiData["effects"][effectIndex+1]["params"].size() != 0)
					strcat((char *)this->uiData, ",");
			}



		}
		strcat((char *)this->uiData, "]}");
	}
#if(dbg>=1)
	//printf("this->uiData: %s\n", this->uiData);
	cout << "this->uiData:" << this->uiData << endl;
	//printf("this->uiData length: %d\n", strlen(this->uiData));
	cout << "this->uiData length:" << strlen(this->uiData) << endl;
#endif
	this->sendData(this->uiSectionStartAddress, this->uiData, strlen(this->uiData));
	return status;
}*/

#define dbg 0
int BaseUiInt::sendComboList(string comboList)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::sendComboList" << endl;
#endif
	uint8_t status = 0;

	for(int i = 0; i < SEND_BUFFER_SIZE; i++) this->sendBuffer[i] = 0;
	strncpy(this->sendBuffer, (char *)comboList.c_str(),SEND_BUFFER_SIZE-1);
	this->sendData(this->uiSectionStartAddress, this->sendBuffer, 150);

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::sendComboList" << endl;
#endif
	return status;
}

#define dbg 2
int BaseUiInt::sendCurrentStatus(char *currentStatus)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::sendCurrentStatus" << endl;
#endif
	uint8_t status = 0;
	char paramData[8];
	if(strlen(currentStatus) == 0) currentStatus[0] = ' ';
	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

	sprintf(this->uiData, "currentComboIndex:%d", comboIndex);
	strcat(this->uiData, "|currentStatus:");
	strcat(this->uiData, currentStatus);
	/*strcat(this->uiData, "|currentData:");
	for(int i = 0; i < currentParams.size(); i++)
	{
		if(i == currentParams.size()-1)
		{
			sprintf(paramData,"%d", currentParams.at(i).paramValue);
		}
		else
		{
			sprintf(paramData,"%d,", currentParams.at(i).paramValue);
		}

		strcat(this->uiData, paramData);
	}*/

	char hostUiStatusString[20];
	sprintf(hostUiStatusString,"|hostUiStatus:%d", (hostGuiActive?1:0));
	strcat(this->uiData, hostUiStatusString);
	clearBuffer(hostUiStatusString,20);
	sprintf(hostUiStatusString,"|comboTime:%d", comboTime);
	strcat(this->uiData, hostUiStatusString);

#if(dbg >= 2)
	cout << "sendCurrentData: " << this->uiData << "\tsize: " << strlen(this->uiData) << endl;
#endif

	this->sendData(this->uiSectionStartAddress, this->uiData, 150/*strlen(this->uiData)*/);

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::sendCurrentStatus" << endl;
#endif
	return status;
}

#define dbg 0
int BaseUiInt::sendCurrentData(vector<IndexedParameter> currentParams)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::sendCurrentData" << endl;
#endif
	uint8_t status = 0;
	char paramData[8];

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

	sprintf(this->uiData, "currentComboIndex:%d", comboIndex);
	/*strcat(this->uiData, "|currentData:");
	for(int i = 0; i < currentParams.size(); i++)
	{
		if(i == currentParams.size()-1)
		{
			sprintf(paramData,"%d", currentParams.at(i).paramValue);
		}
		else
		{
			sprintf(paramData,"%d,", currentParams.at(i).paramValue);
		}

		strcat(this->uiData, paramData);
	}*/


	char hostUiStatusString[20];
	sprintf(hostUiStatusString,"|hostUiStatus:%d", (hostGuiActive?1:0));
	strcat(this->uiData, hostUiStatusString);

	//cout << "sendCurrentData: " << this->uiData << "\tsize: " << strlen(this->uiData) << endl;

	this->sendData(this->uiSectionStartAddress, this->uiData, strlen(this->uiData));

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::sendCurrentData" << endl;
#endif
	return status;
}

#define dbg 0
int BaseUiInt::clearSharedMemorySection(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::clearSharedMemorySection" << endl;
#endif
	uint8_t status = 0;
	char paramData[8];



	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

	this->sendData(this->cmSectionStartAddress, this->uiData, 150);

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::clearSharedMemorySection" << endl;
#endif
	return status;
}

#define dbg 0
void BaseUiInt::waitForAccessRelease(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: BaseUiInt::waitForAccessRelease" << endl;
#endif
	int pinValue = 0;
	long loopCount = 0;
	while(pinValue == 0 && loopCount++ < 100)
	{
		newData.getval_gpio(pinValue);
#if(dbg >= 2)
		cout << "loopCount: " << loopCount << endl;
#endif
	}

#if(dbg >= 2)
	if(loopCount >= 100)
	{
		cout << "access release loopCount overflow." << endl;
	}
#endif

#if(dbg >= 1)
	cout << "***** EXITING: BaseUiInt::waitForAccessRelease" << endl;
#endif


}
