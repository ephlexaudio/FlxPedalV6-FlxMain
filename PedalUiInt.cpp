/*
 * PedalUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

//#include "BaseUiInt.h"
#include "PedalUiInt.h"

#include "config.h"
#include "utilityFunctions.h"


extern bool debugOutput;

#define FILE_LENGTH 1000
#define MCU_SHARED_MEMORY_SECTION_ADDRESS 0
#define MCU_SHARED_MEMORY_SECTION_SIZE 4096

#define CM0_SHARED_MEMORY_SECTION_ADDRESS 4096
#define CM0_SHARED_MEMORY_SECTION_SIZE 4096

#define HOST_SHARED_MEMORY_SECTION_ADDRESS 32768
//#define HOST_SHARED_MEMORY_SECTION_SIZE 32000

#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500
#define FILE_BUFFER_SIZE 16000

#if(dbg >= 1)
	cout << "***** ENTERING: PedalUiInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt:::" << endl;
#endif



using namespace std;

//extern int procCount;
//extern bool hostGuiActive;
//extern int comboIndex;

PedalUiInt::PedalUiInt()//:BaseUiInt()
{
	this->status = 0;

	//newData(SHARED_MEMORY_READY);

}

PedalUiInt::~PedalUiInt()
{
	if(debugOutput) cout << "~PedalUiInt" << endl;
}

#define dbg 0

int PedalUiInt::checkForNewPedalData(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::checkForNewPedalData" << endl;
#endif

	int status = 0;

	status = BaseUiInt::checkForNewData();
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: PedalUiInt::checkForNewPedalData: " << status << endl;
#endif

	return status;
}


#define dbg 2

int PedalUiInt::sendComboUiData(Json::Value uiJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendComboUiData: " <<  endl;
#endif
	uint8_t status = 0;
	uint8_t effectIndex = 0;
	uint8_t effectCount = 0;
	uint8_t guiParamIndex = 0;
	uint8_t guiParamCount = 0;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

	/*strcpy((char *)this->uiData, "{title:");
	strncat((char *)this->uiData, uiJson["title"].asCString(), strlen(uiJson["title"].asString().c_str()));

	{
		strcat((char *)this->uiData, "\",\"effects\":[");
		effectCount = uiJson["effects"].size();
		if(debugOutput) cout << "effectCount: " << effectCount << endl;
		for(effectIndex = 0; effectIndex < effectCount; effectIndex++)
		{
			guiParamCount = uiJson["effects"][effectIndex]["params"].size();
			if(debugOutput) cout << "guiParamCount: " << guiParamCount << endl;
			if(guiParamCount > 0)
			{
				strcat((char *)this->uiData, "{\"abbr\":\"");
				strncat((char *)this->uiData, uiJson["effects"][effectIndex]["abbr"].asString().c_str(),
						4);
				strcat((char *)this->uiData, "\",\"name\":\"");
				strncat((char *)this->uiData, uiJson["effects"][effectIndex]["name"].asString().c_str(),
						10);
				strcat((char *)this->uiData, "\",\"params\":[");

				for(guiParamIndex = 0; guiParamIndex < guiParamCount; guiParamIndex++)
				{
					if(uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().size() > 1
        					&& uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().compare("none") != 0)
					{
						strcat((char *)this->uiData, "{\"abbr\":\"");
						strncat((char *)this->uiData, uiJson["effects"][effectIndex]["params"][guiParamIndex]["abbr"].asString().c_str(),
								4);
						strcat((char *)this->uiData, "\",\"name\":\"");
						strncat((char *)this->uiData, uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().c_str(),
								10);

						strcat((char *)this->uiData, "\",\"value\":");
						sprintf(intCharArray,"%d",uiJson["effects"][effectIndex]["params"][guiParamIndex]["value"].asInt());
						strcat((char *)this->uiData, intCharArray);

						strcat((char *)this->uiData, ",\"type\":");
						sprintf(intCharArray,"%d",uiJson["effects"][effectIndex]["params"][guiParamIndex]["type"].asInt());
						strcat((char *)this->uiData, intCharArray);

						strcat((char *)this->uiData, ",\"index\":");
						sprintf(intCharArray,"%d",uiJson["effects"][effectIndex]["params"][guiParamIndex]["index"].asInt());
						strcat((char *)this->uiData, intCharArray);

						strcat((char *)this->uiData, "}");

						if(guiParamIndex != guiParamCount-1) strcat((char *)this->uiData, ",");

					}
				}
				strcat((char *)this->uiData, "]}");
				if(effectIndex != effectCount-1 && uiJson["effects"][effectIndex+1]["params"].size() != 0)
					strcat((char *)this->uiData, ",");
			}
		}
		strcat((char *)this->uiData, "]}");
	}*/

	//cout << "creating uiJson string." << endl;
	string uiJsonString = uiJson.toStyledString();
#if(dbg>=2)
	if(debugOutput) cout << "uiJsonString: " + uiJsonString << endl;
#endif
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\n'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\r'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\t'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), ' '), uiJsonString.end());
	/*cout << uiJsonString << endl;
	cout << "creating uiJson Cstring." << endl;
	char uiJsonCstring[1000];
	strncpy(uiJsonCstring, uiJsonString.c_str(), 1000);
	cout << uiJsonCstring << endl;
	cout << "copying Cstring to uiData." << endl;*/
	strncpy(this->uiData, uiJsonString.c_str(), 1000);


	errno = 0;
	//this->sendData(MCU_SHARED_MEMORY_SECTION_ADDRESS, this->uiData, strlen(this->uiData));
	int txSentCount = write(this->pedalUiTxFd, this->uiData, strlen(this->uiData));
#if(dbg>=2)
	if(debugOutput) cout << "this->uiData:" << this->uiData << endl;
	if(debugOutput) cout << "this->uiData length:" << strlen(this->uiData) << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == strlen(this->uiData)) status = 0;
	else status = -1;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendComboUiData: " << status << endl;
#endif
	return status;
}


#define dbg 1
int PedalUiInt::sendFlxUtilUiData(Json::Value uiJson)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendFlxUtilUiData: " <<  endl;
#endif

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;


	string uiJsonString = uiJson.toStyledString();
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\n'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\r'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), '\t'), uiJsonString.end());
	uiJsonString.erase(remove(uiJsonString.begin(), uiJsonString.end(), ' '), uiJsonString.end());
	strncpy(this->uiData, uiJsonString.c_str(), 1000);

	errno = 0;
	int txSentCount = write(this->pedalUiTxFd, this->uiData, strlen(this->uiData));
#if(dbg>=2)
	if(debugOutput) cout << "this->uiData:" << this->uiData << endl;
	if(debugOutput) cout << "this->uiData length:" << strlen(this->uiData) << endl;
	if(debugOutput) cout << "txSentCount: " << txSentCount << endl;
	if(txSentCount < 0)
		if(debugOutput) cout << "FIFO write error: " << strerror(errno) << endl;
#endif

	if(txSentCount == strlen(this->uiData)) status = 0;
	else status = -1;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendFlxUtilUiData: " << status << endl;
#endif
	return status;
}


#define dbg 1
Json::Value PedalUiInt::recieveFlxUtilUiData()
{
	Json::Value utilData;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: PedalUiInt::sendFlxUtilUiData: " <<  endl;
#endif



#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendFlxUtilUiData: " << status << endl;
#endif
	return utilData;
}
