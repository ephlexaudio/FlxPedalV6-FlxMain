/*
 * MainFuncts.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: mike
 */
#include "config.h"
#include "FileSystemFuncts.h"

/*extern std::vector<string> jsonComponentList;
extern std::vector<string> jsonComboList;

extern std::vector<string> jsonProcessList;*/


/**********************************************************************************/
/**********************************************************************************/
/**************************************
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::" << endl;
#endif

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::" << endl;
#endif

#if(dbg >=2)
#endif
********************************************/
#define JSON_BUFFER_LENGTH 32000
#define FILE_SIZE 32000
#define COMBO_DATA_VECTOR 0
#define COMBO_DATA_ARRAY 0
#define COMBO_DATA_MAP 1

extern bool debugOutput;

struct _segFaultInfo {
	string function;
	int line;
};

#define dbg 0
int validateJsonString(std::string jsonString)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::validateJsonString" << endl;
	if(debugOutput) cout << "jsonString: " << jsonString << endl;
#endif

	int status = 0;
	Json::Value jsonClean;
	int newlineIndex =0;
	Json::Reader jsonDirtyReader;
	Json::FastWriter jsonCleanWriter;

	char dirtyBuffer[FILE_SIZE];
	string dirtyBufferString;
	char cleanBuffer[FILE_SIZE];
	string cleanBufferString;
	string tempBufferString;
	//char outputBuffer[FILE_SIZE];

	clearBuffer(dirtyBuffer,FILE_SIZE);
	clearBuffer(cleanBuffer,FILE_SIZE);

	tempBufferString.assign(jsonString);
	/************ SANITIZE IN CASE OF CORRUPTION *********************/

	newlineIndex = tempBufferString.find("\n");
	if(newlineIndex != std::string::npos)
	{
		if(debugOutput) cout << "newline found" << endl;
#if(dbg >=2)
#endif
		dirtyBufferString = tempBufferString.substr(0,newlineIndex);
	}
	else
	{
		if(debugOutput) cout << "no newline found" << endl;
#if(dbg >=2)
#endif
		dirtyBufferString.assign(tempBufferString);
	}


#if(dbg >=2)
	if(debugOutput) cout << "Dirty string: " << dirtyBufferString << endl;
	if(debugOutput) cout << "Dirty string length: " << dirtyBufferString.size() << endl;
#endif
	if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
	{
		if(debugOutput) cout << "JSON name: " << jsonClean["name"] << endl;
		cleanBufferString = jsonCleanWriter.write(jsonClean);

#if(dbg >=2)
		if(debugOutput) cout << "Clean string: " << cleanBufferString << endl;
		if(debugOutput) cout << "Clean string length: " << cleanBufferString.size() << endl;
#endif

		if(dirtyBufferString.size() == cleanBufferString.size())
		{
			if(debugOutput) cout << "jsonString is clean" << endl;
			status = 1;
		}
		else /*if(dirtyBufferString.size() > cleanBufferString.size())*/
		{
			/*if(dirtyBufferString.size()/2 > cleanBufferString.size()) // something went REALLY wrong
			{
				if(debugOutput) cout << "jsonString is highly corrupted and cannot be repaired" << endl;
				status = -1; // jsonString is highly corrupted and cannot be repaired
			}
			else*/
			{
				dirtyBufferString.clear();
				dirtyBufferString.assign(cleanBufferString);
				if(debugOutput) cout << "confirming clean..." << endl;
				if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
				{
					if(debugOutput) cout << "JSON name2: " << jsonClean["name"] << endl;
					cleanBufferString = jsonCleanWriter.write(jsonClean);
#if(dbg >=2)
					if(debugOutput) cout << "Clean string2: " << cleanBufferString << endl;
					if(debugOutput) cout << "Clean string length2: " << cleanBufferString.size() << endl;
#endif

					if(dirtyBufferString.size() == cleanBufferString.size() ||
							dirtyBufferString.size() == cleanBufferString.size()-1)
					{
						jsonString.clear();
						jsonString.assign(cleanBufferString);
						if(debugOutput) cout << "jsonString needed cleaning/repair" << endl;
						status = 0; // jsonString needed cleaning/repair
					}
					else
					{
						if(debugOutput) cout << "jsonString cleaning failed" << endl;
						status = -1;
						jsonString.clear();
					}

				}
				else
				{
					if(debugOutput) cout << "jsonString cleaning failed" << endl;
					status = -1;
					jsonString.clear();
				}

			}
		}
	}
	else
	{
		if(debugOutput) cout << "jsonString could not be parsed" << endl;
		status = -1;
		jsonString.clear();
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::validateJsonString: " << status << endl;
#endif

	return status;
}

#define dbg 0
int validateJsonBuffer(char *jsonBuffer)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::validateJsonBuffer" << endl;
	//if(debugOutput) cout << "jsonBuffer: " << jsonBuffer << endl;
#endif
	int status = 0;
	Json::Value jsonClean;
	int newlineIndex =0;
	Json::Reader jsonDirtyReader;
	Json::FastWriter jsonCleanWriter;

	char dirtyBuffer[FILE_SIZE];
	string dirtyBufferString;
	char cleanBuffer[FILE_SIZE];
	string cleanBufferString;
	string tempBufferString;
	//char outputBuffer[FILE_SIZE];

	clearBuffer(dirtyBuffer,FILE_SIZE);
	clearBuffer(cleanBuffer,FILE_SIZE);

	tempBufferString = string(jsonBuffer);
	/************ SANITIZE IN CASE OF CORRUPTION *********************/

	newlineIndex = tempBufferString.find("\n");
#if(dbg >=2)
	if(debugOutput) cout << "tempBufferString: " << tempBufferString << endl;
	if(debugOutput) cout << "tempBufferString length: " << tempBufferString.size() << endl;
#endif
	if(debugOutput) cout << "newline found at: " << newlineIndex << endl;
	if(newlineIndex == (tempBufferString.length()-1)) newlineIndex = std::string::npos; // ignore newline at end of string
	if((newlineIndex != std::string::npos))
	{
		//if(debugOutput) cout << "newline found at: " << newlineIndex << endl;
		dirtyBufferString = tempBufferString.substr(0,newlineIndex);
	}
	else
	{
		if(debugOutput) cout << "no newline found" << endl;
		dirtyBufferString.assign(tempBufferString);
	}
	dirtyBufferString.erase(remove(dirtyBufferString.begin(),dirtyBufferString.end(),'\n'),dirtyBufferString.end());


#if(dbg >=2)
	if(debugOutput) cout << "Dirty string: " << dirtyBufferString << endl;
	if(debugOutput) cout << "Dirty string length: " << dirtyBufferString.size() << endl;
#endif

	//printAsciiNumbers(dirtyBufferString);

	if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
	{
		if(debugOutput) cout << "JSON name: " << jsonClean["name"] << endl;
		cleanBufferString = jsonCleanWriter.write(jsonClean);
		cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
#if(dbg >=2)
		if(debugOutput) cout << "Clean string: " << cleanBufferString << endl;
		if(debugOutput) cout << "Clean string length: " << cleanBufferString.size() << endl;
#endif

		//printAsciiNumbers(cleanBufferString);

		if(dirtyBufferString.size() == cleanBufferString.size() && (newlineIndex == std::string::npos))
		{
			if(debugOutput) cout << "jsonString is clean" << endl;
			status = 1;
		}
		else /*if(dirtyBufferString.size() > cleanBufferString.size())*/
		{
			/*if(dirtyBufferString.size()/2 > cleanBufferString.size()) // something went REALLY wrong
			{
				if(debugOutput) cout << "jsonString is highly corrupted and cannot be repaired" << endl;
				status = -1; // jsonString is highly corrupted and cannot be repaired
			}
			else*/
			{
				dirtyBufferString.clear();
				dirtyBufferString.assign(cleanBufferString);
				if(debugOutput) cout << "confirming clean..." << endl;
				if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
				{
					if(debugOutput) cout << "JSON name: " << jsonClean["name"] << endl;
					cleanBufferString = jsonCleanWriter.write(jsonClean);
					cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
#if(dbg >=2)
					if(debugOutput) cout << "Clean string: " << cleanBufferString << endl;
					if(debugOutput) cout << "Clean string length: " << cleanBufferString.size() << endl;
#endif

					if(dirtyBufferString.size() == cleanBufferString.size())
					{
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
						strncpy(jsonBuffer,cleanBufferString.c_str(),JSON_BUFFER_LENGTH);
						if(debugOutput) cout << "jsonString needed cleaning/repair" << endl;
						status = 0; // jsonString needed cleaning/repair
					}
					else
					{
						if(debugOutput) cout << "jsonString cleaning failed" << endl;
						status = -1;
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
					}

				}
				else
				{
					if(debugOutput) cout << "jsonString cleaning failed" << endl;
					status = -1;
				}

			}
		}
	}
	else
	{
		if(debugOutput) cout << "jsonString could not be parsed" << endl;
		status = -1;
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::validateJsonBuffer: " << status << endl;
#endif


	return status;
}

Json::Reader dataReader;
#define dbg 0
std::vector<string> getComponentList(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComponentList" << endl;
#endif
	std::vector<string> compList;
	FILE *fdCompList = popen("ls /home/Components","r");
	int status = 0;
	char buffer[20];

	for(int i = 0; i<20;i++)
	{
		buffer[i] = 0;
	}

	if(fdCompList == NULL)
	{
#if(dbg>=2)
		printf("popen failed.\n");
#endif
		status = 1;
	}
	else
	{
		fflush(fdCompList);

		while(fgets(buffer,20,fdCompList) != NULL)
		{
			// is the component file a text file?  Get rid of ".txt" if so.
			puts(buffer);
			if(strchr(buffer,'.') >= 0)
			{
				strcpy(buffer,strtok(buffer,"."));
			}
			else
			{
				strcpy(buffer,strtok(buffer,"\n"));
			}
			compList.push_back(std::string(buffer));
			for(int i = 0; i<20;i++) buffer[i] = 0;
		}
	}

	if(fdCompList != NULL) pclose(fdCompList);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::getComponentList" << endl;
#endif
	return compList;
}


#define dbg 0
std::string getComponentData(std::string componentName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComponentData" << endl;
	if(debugOutput) cout << "componentName: " << componentName << endl;
#endif
	char compString[50];
	sprintf(compString,"/home/Components/%s.txt", componentName.c_str());
	//printf("compString: %s\n", compString);
	FILE *fdComp = fopen(compString, "r");
	char outputString[1000];
	if(fdComp == NULL)
	{
#if(dbg>=2)
		printf("open failed.\n");
#endif
	}
	else
	{
		while(fgets(outputString,1000,fdComp) != NULL)
		{
			//printf("componentData: %s\n", outputString);
		}
	}
	fclose(fdComp);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::getComponentData" << endl;
#endif
	return std::string(outputString);
}



/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/


//def update_combo_directory_data():
#define dbg 0
std::vector<string> getComboListFromFileSystem(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComboFileList" << endl;
#endif

	std::vector<string> comList;
	FILE *fdComboList = popen("ls /home/Combos","r");
	int status = 0;
	char buffer[20];

	for(int i = 0; i<20;i++)
	{
		buffer[i] = 0;
	}


	if(fdComboList == NULL)
	{
#if(dbg>=2)
		if(debugOutput) cout << "popen failed." << endl;
#endif
		status = 1;
	}
	else
	{
		fflush(fdComboList);
		while(fgets(buffer,20,fdComboList) != NULL)
		{
			strcpy(buffer,strtok(buffer,"."));
			comList.push_back(buffer);
#if(dbg>=2)
			if(debugOutput) cout << buffer << endl;
#endif

			for(int i = 0; i<20;i++) buffer[i] = 0;

		}
	}

	if(fdComboList != NULL) pclose(fdComboList);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::getComboList" << endl;
#endif
	return comList;
}



#define dbg 0
std::string getComboDataFromFileSystem(std::string comboName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComboData" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif
	errno = 0;
	char comboString[50];
	char jsonBuffer[FILE_SIZE];
	string jsonBufferString;

	sprintf(comboString,"/home/Combos/%s.txt", comboName.c_str());
#if(dbg>=1)
	printf("comboString: %s\n", comboString);
#endif
	clearBuffer(jsonBuffer,FILE_SIZE);
	FILE *fdCombo = fopen(comboString, "r");
	if(fdCombo == NULL)
	{
#if(dbg>=2)
		printf("open failed.\n");
		printf("errno %d:%s\n", errno, strerror(errno));
#endif
	}
	else
	{
		while(fgets(jsonBuffer,FILE_SIZE,fdCombo) != NULL)
		{
#if(dbg>=2)
			puts(jsonBuffer);
#endif
		}
		jsonBufferString = string(jsonBuffer);

		int result = validateJsonString(jsonBufferString);

		if(result == 0) // file needed cleaning, so replacing with cleaned file
		{
			if(debugOutput) cout << "file needed cleaning, so replacing with cleaned file" << endl;
			fclose(fdCombo);
			fdCombo = fopen(comboString, "w");
			if(fputs(jsonBufferString.c_str(),fdCombo) == -1)
			{
				jsonBufferString.clear();
				if(debugOutput) cout << "error writing jsonBufferString back to combo file." << endl;
			}
		}
		else if(result == -1) // file is too corrupted to clean
		{
			jsonBufferString.clear();
			if(debugOutput) cout << "error parsing jsonBufferString" << endl;
		}
		fclose(fdCombo);
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::getComboData" << endl;
#endif
	return jsonBufferString;
}

#define dbg 0
string saveComboToFileSystem(std::string comboJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::saveComboToFileSystem" << endl;
#endif
	int status = 0;
	string name;
	FILE *saveComboFD;
	Json::Value tempJsonCombo;
	int charCount = 0;
	char comboDataBuffer[FILE_SIZE];

	char parsedComboDataBuffer[FILE_SIZE];
	for(int i = 0; i < FILE_SIZE; i++)
	{
		comboDataBuffer[i]=0;
		parsedComboDataBuffer[i]=0;
	}
	char fileNameBuffer[40];

#if(dbg>=2)
	if(debugOutput) cout << "pre parsed JSON string: " << comboJson.c_str() << endl;
#endif
	if(dataReader.parse(comboJson.c_str(), tempJsonCombo))
	{
		sprintf(fileNameBuffer, "/home/Combos/%s.txt", tempJsonCombo["name"].asString().c_str());
#if(dbg>=2)
		if(debugOutput) std::cout << "saveComboToFileSystem: " << fileNameBuffer << '\n';
#endif
		//saveComboFD = fopen(fileNameString,"w");
		if((saveComboFD = fopen(fileNameBuffer,"w")) != NULL )
		{
			strcpy(comboDataBuffer, comboJson.c_str());
			for(int i = 0; comboDataBuffer[i] != 0; i++) charCount++;
			strncpy(parsedComboDataBuffer, comboDataBuffer, charCount);
#if(dbg>=2)
			if(debugOutput) std::cout << "OfxMain/FileSystemFuncts/saveComboToFileSystem size: " << strlen(parsedComboDataBuffer) << endl;
#endif
			int bytesWritten = fwrite(parsedComboDataBuffer,1,charCount,saveComboFD);
#if(dbg>=2)
			if(debugOutput) std::cout << "OfxMain/FileSystemFuncts/saveComboToFileSystem bytes written: " << bytesWritten << endl;
#endif
			fclose(saveComboFD);
			name = tempJsonCombo["name"].asString();
		}
		else
		{
			if(debugOutput) cout << "failed to open combo file for writing." << endl;
			status = 1;
		}
	}
	else
	{
		if(debugOutput) cout << "error parsing combo data. Possible corruption." << endl;
		status = 1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::saveComboToFileSystem: " << name << endl;
#endif
	return name;
}


int deleteComboFromFileSystem(std::string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::deleteComboFromFileSystem" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	char cliString[50];
	char cliResult[100];
	sprintf(cliString, "rm /home/Combos/%s.txt", comboName.c_str());
#if(hostUiDbg == 1)
	if(debugOutput) cout << "CLI string for delete: " << cliString << endl;
#endif
	strcpy(cliResult,strerror(system(cliString)));
	if(debugOutput) cout << "delete result: " << cliResult << endl;

	if(strncmp(cliResult,"Success",7) == 0)
	{
		status = 0;
	}
	else
	{
		status = -1;
	}
#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING: FileSystemFuncts::deleteComboFromFileSystem: " << status << endl;
#endif

	return status;
}





