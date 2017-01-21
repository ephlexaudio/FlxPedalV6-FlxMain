/*
 * PedalUiInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */

//#include "BaseUiInt.h"
#include "PedalUiInt.h"
//#include "ComboDataInt.h"
#include "utilityFunctions.h"



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

using namespace std;

//extern int procCount;
//extern bool hostGuiActive;
//extern int comboIndex;

PedalUiInt::PedalUiInt(unsigned int cmSectionStartAddress, unsigned int cmSectionSize,
		unsigned int uiSectionStartAddress, unsigned int uiSectionSize):BaseUiInt(
				cmSectionStartAddress,cmSectionSize,uiSectionStartAddress,uiSectionSize)
{
	//this->status = 0;

	//newData(SHARED_MEMORY_READY);

}

PedalUiInt::~PedalUiInt()
{
	cout << "~PedalUiInt" << endl;
}

#define dbg 0

int PedalUiInt::checkForNewPedalData(void)
{
#if(dbg >= 1)
	cout << "***** ENTERING: PedalUiInt::checkForNewPedalData" << endl;
#endif

	volatile int status = 0;

	/*if(this->checkForNewData(1) == 1)
	{
		status = 1;
#if(dbg >= 2)
		cout << "new data from pedal" << endl;
#endif
	}*/
	status = BaseUiInt::checkForNewData(1);
#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::checkForNewPedalData" << endl;
#endif

	return status;
}


#define dbg 1

int PedalUiInt::sendComboUiData(Json::Value uiJson)
{
#if(dbg >= 1)
	cout << "***** ENTERING: PedalUiInt::sendComboUiData" << endl;
#endif
	//if(this->status == 1) return 1;
	uint8_t status = 0;
	uint8_t effectIndex = 0;
	uint8_t effectCount = 0;
	uint8_t guiParamIndex = 0;
	uint8_t guiParamCount = 0;

	//const int length = 10;
	//struct spi_ioc_transfer spi;

	//int retVal;

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;
	//this->uiData[0] = 3;


	strcpy((char *)this->uiData, "{title:");
	strncat((char *)this->uiData, uiJson["title"].asCString(), strlen(uiJson["title"].asString().c_str()));

	{
		strcat((char *)this->uiData, ",effects:[");
		effectCount = uiJson["effects"].size();
		for(effectIndex = 0; effectIndex < effectCount; effectIndex++)
		{
			guiParamCount = uiJson["effects"][effectIndex]["params"].size();
			if(guiParamCount > 0)
			{
				strcat((char *)this->uiData, "{abbr:");
				strncat((char *)this->uiData, uiJson["effects"][effectIndex]["abbr"].asString().c_str(),
						strlen(uiJson["effects"][effectIndex]["abbr"].asCString()));
				strcat((char *)this->uiData, ",name:");
				strncat((char *)this->uiData, uiJson["effects"][effectIndex]["name"].asString().c_str(),
						strlen(uiJson["effects"][effectIndex]["name"].asCString()));
				strcat((char *)this->uiData, ",params:[");

				for(guiParamIndex = 0; guiParamIndex < guiParamCount; guiParamIndex++)
				{
					if(uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().size() > 1
        					&& uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().compare("none") != 0)
					{
						strcat((char *)this->uiData, "{");
						char paramStringBuffer[15];
						char stringBuffer[200];
						clearBuffer(stringBuffer,200);

						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiJson["effects"][effectIndex]["params"][guiParamIndex]["abbr"].asString().c_str());
	#if(dbg>=2)
						strcpy(stringBuffer,"abbr:");//puts("abbr:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiJson["effects"][effectIndex]["params"][guiParamIndex]["name"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", name:");//puts("name:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiJson["effects"][effectIndex]["params"][guiParamIndex]["value"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", value:");//puts("value:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiJson["effects"][effectIndex]["params"][guiParamIndex]["type"].asString().c_str());
	#if(dbg>=2)
						strcat(stringBuffer,", type:");//puts("type:");
						strcat(stringBuffer, paramStringBuffer);//puts(paramStringBuffer);
	#endif
						strncat((char *)this->uiData, paramStringBuffer, strlen(paramStringBuffer));

						strcat((char *)this->uiData, ",");
						clearBuffer(paramStringBuffer, 15);
						strcpy(paramStringBuffer, uiJson["effects"][effectIndex]["params"][guiParamIndex]["index"].asString().c_str());
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
				if(effectIndex != effectCount-1 && uiJson["effects"][effectIndex+1]["params"].size() != 0)
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
	this->sendData(MCU_SHARED_MEMORY_SECTION_ADDRESS, this->uiData, strlen(this->uiData));

	for(int i = 0; i < TX_BUFFER_SIZE; i++) this->uiData[i] = 0;

#if(dbg >= 1)
	cout << "***** EXITING: PedalUiInt::sendComboUiData" << endl;
#endif
	return status;
}

