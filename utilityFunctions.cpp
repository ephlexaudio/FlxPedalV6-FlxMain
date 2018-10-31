/*
 * utilityFunctions.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#include "config.h"
#include "utilityFunctions.h"


long startStamp;
long stopStamp;
struct timeval tv;
extern bool debugOutput;

#define JSON_BUFFER_LENGTH 32000

void startTimer(void)
{
	gettimeofday(&tv, NULL);
	startStamp = 1000000*tv.tv_sec+tv.tv_usec;
}

int stopTimer(const char *description)
{
	gettimeofday(&tv, NULL);
	stopStamp = 1000000*tv.tv_sec+tv.tv_usec;
	if(description != NULL)
	{
		if(debugOutput) if(debugOutput) cout << "******" << description << "time: " << stopStamp - startStamp << endl;
	}

	return (int)(stopStamp - startStamp);
}



void clearBuffer(char *buffer, int length)
{
	uint16_t index;

	for(index = 0; index < length; index++)
	{
		buffer[index] = 0;
	}
}

void pabort(const char *s)
{
	perror(s);
}



string getCompactedJSONData(Json::Value data)
{
	string reducedData;
	reducedData = data.toStyledString();
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\n'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\r'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), '\t'), reducedData.end());
	reducedData.erase(std::remove(reducedData.begin(), reducedData.end(), ' '), reducedData.end());

	return reducedData;
}

string removeReturnRelatedCharacters(string dirtyString)
{

	dirtyString.erase(remove(dirtyString.begin(), dirtyString.end(), '\n'), dirtyString.end());
	dirtyString.erase(remove(dirtyString.begin(), dirtyString.end(), '\r'), dirtyString.end());

	return dirtyString;
}

string getStringListString(vector<string> stringVector)
{
	string vectorString;
	int i = 0;
	for(auto & stringName : stringVector)
	{
		vectorString.append(stringName);
		if(i < stringVector.size()-1) vectorString.append(",");
		i++;
	}
	return vectorString;
}

#define dbg 0
bool validateString(string dataString)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: validateString" << endl;
	if(debugOutput) cout << "dataString: " << dataString << endl;
#endif
	bool dataStringIsValid = true;
	int  charIndex = 0;
	for(auto & dataStringChar : dataString)
	{
#if(dbg >= 2)
			cout << dataStringChar << "(" << (int)dataStringChar << ")";
#endif
			if(dataStringChar < ' ' || '~' < dataStringChar)
			{
				dataStringIsValid = false;
				break;
			}
			charIndex++;
#if(dbg >= 2)
			if(charIndex == dataString.size())
				cout << endl;
#endif
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: validateString: " << dataStringIsValid << endl;
#endif

	return dataStringIsValid;
}


#define dbg 0
int validateJsonBuffer(char *jsonBuffer)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: validateJsonBuffer" << endl;
#endif
	int status = 0;
	Json::Value jsonClean;
	int newlineIndex = 0;
	Json::Reader jsonDirtyReader;
	Json::FastWriter jsonCleanWriter;

	char dirtyBuffer[FILE_SIZE];
	string dirtyBufferString;
	char cleanBuffer[FILE_SIZE];
	string cleanBufferString;
	string tempBufferString;

	clearBuffer(dirtyBuffer,FILE_SIZE);
	clearBuffer(cleanBuffer,FILE_SIZE);

	tempBufferString = string(jsonBuffer);
	/************ SANITIZE IN CASE OF CORRUPTION *********************/

	newlineIndex = tempBufferString.find("\n");
#if(dbg >=2)
	if(debugOutput) cout << "tempBufferString: " << tempBufferString << endl;
	if(debugOutput) cout << "tempBufferString length: " << tempBufferString.size() << endl;
	if(debugOutput) cout << "newline found at: " << newlineIndex << endl;
#endif
	if(newlineIndex == (tempBufferString.length()-1)) newlineIndex = std::string::npos; // ignore newline at end of string
	if((newlineIndex != std::string::npos))
	{
		dirtyBufferString = tempBufferString.substr(0,newlineIndex);
	}
	else
	{
#if(dbg >=2)
		if(debugOutput) cout << "no newline found" << endl;
#endif
		dirtyBufferString.assign(tempBufferString);
	}
	dirtyBufferString.erase(remove(dirtyBufferString.begin(),dirtyBufferString.end(),'\n'),dirtyBufferString.end());


#if(dbg >=2)
	if(debugOutput) cout << "Dirty string: " << dirtyBufferString << endl;
	if(debugOutput) cout << "Dirty string length: " << dirtyBufferString.size() << endl;
#endif


	if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
	{
#if(dbg >=2)
		if(debugOutput) cout << "JSON name: " << jsonClean["name"] << endl;
#endif
		cleanBufferString = jsonCleanWriter.write(jsonClean);
		cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
#if(dbg >=2)
		if(debugOutput) cout << "Clean string: " << cleanBufferString << endl;
		if(debugOutput) cout << "Clean string length: " << cleanBufferString.size() << endl;
#endif



		if(dirtyBufferString.size() == cleanBufferString.size() && (newlineIndex == std::string::npos))
		{
#if(dbg >=2)
			if(debugOutput) cout << "jsonString is clean" << endl;
#endif
			status = 1;
		}
		else
		{

			{
				dirtyBufferString.clear();
				dirtyBufferString.assign(cleanBufferString);
#if(dbg >=2)
				if(debugOutput) cout << "confirming clean..." << endl;
#endif
				if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
				{
					cleanBufferString = jsonCleanWriter.write(jsonClean);
					cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
#if(dbg >=2)
					if(debugOutput) cout << "JSON name: " << jsonClean["name"] << endl;
					if(debugOutput) cout << "Clean string: " << cleanBufferString << endl;
					if(debugOutput) cout << "Clean string length: " << cleanBufferString.size() << endl;
#endif

					if(dirtyBufferString.size() == cleanBufferString.size())
					{
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
						strncpy(jsonBuffer,cleanBufferString.c_str(),JSON_BUFFER_LENGTH);
#if(dbg >=2)
						if(debugOutput) cout << "jsonString needed cleaning/repair" << endl;
#endif
						status = 0; // jsonString needed cleaning/repair
					}
					else
					{
#if(dbg >=2)
						if(debugOutput) cout << "jsonString cleaning failed" << endl;
#endif
						status = -1;
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
					}

				}
				else
				{
#if(dbg >=2)
					if(debugOutput) cout << "jsonString cleaning failed" << endl;
#endif
					status = -1;
				}

			}
		}
	}
	else
	{
#if(dbg >=2)
		if(debugOutput) cout << "jsonString could not be parsed" << endl;
#endif
		status = -1;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: validateJsonBuffer: " << status << endl;
#endif


	return status;
}
