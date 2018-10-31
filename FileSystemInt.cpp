/*
 * FileSystemInt.cpp
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */
#include "config.h"
#include "FileSystemInt.h"

extern bool debugOutput;

namespace std
{

	FileSystemInt::FileSystemInt ()
	{
		// TODO Auto-generated constructor stub

	}

	FileSystemInt::~FileSystemInt ()
	{
		// TODO Auto-generated destructor stub
	}


#define dbg 0
std::vector<string> FileSystemInt::getComboListFromFileSystem(void)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComboFileList" << endl;
#endif

	std::vector<string> comList;
	FILE *fdComboList = popen("ls /home/Combos","r");
	char buffer[20];

	clearBuffer(buffer,20);

	if(fdComboList == NULL)
	{
#if(dbg>=2)
		if(debugOutput) cout << "popen failed." << endl;
#endif
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

			clearBuffer(buffer,20);

		}
	}

	if(fdComboList != NULL) pclose(fdComboList);
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemInt::getComboList" << endl;
#endif
	return comList;
}



#define dbg 0
std::string FileSystemInt::getComboDataFromFileSystem(std::string comboName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemInt::getComboData" << endl;
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

		fclose(fdCombo);
	}
#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemInt::getComboData" << endl;
#endif
	return jsonBufferString;
}

#define dbg 1
string FileSystemInt::saveComboToFileSystem(std::string comboJson)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemInt::saveComboToFileSystem" << endl;
#endif
	string name;
	FILE *saveComboFD;
	Json::Value tempJsonCombo;
	int charCount = 0;
	char comboDataBuffer[FILE_SIZE];
	Json::Reader dataReader;

	clearBuffer(comboDataBuffer,FILE_SIZE);
	char fileNameBuffer[50];
	clearBuffer(fileNameBuffer,50);
#if(dbg>=2)
	if(debugOutput) cout << "pre parsed JSON string: " << comboJson.c_str() << endl;
#endif
	if(dataReader.parse(comboJson, tempJsonCombo))
	{
		snprintf(fileNameBuffer, 49,"/home/Combos/%s.txt", tempJsonCombo["name"].asString().c_str());
#if(dbg>=2)
		if(debugOutput) std::cout << "saveComboToFileSystem: " << fileNameBuffer << '\n';
#endif
		if((saveComboFD = fopen(fileNameBuffer,"w")) != NULL )
		{


			int bytesWritten = fwrite(comboJson.c_str(),1,comboJson.size(),saveComboFD);
#if(dbg>=2)
			if(debugOutput) std::cout << "OfxMain/FileSystemFuncts/saveComboToFileSystem size: " << comboJson.size() << endl;
			if(debugOutput) std::cout << "OfxMain/FileSystemFuncts/saveComboToFileSystem bytes written: " << bytesWritten << endl;
#endif
			fclose(saveComboFD);
			name = tempJsonCombo["name"].asString();
		}
		else
		{
			if(debugOutput) cout << "failed to open combo file for writing." << endl;
		}
	}
	else
	{
		if(debugOutput) cout << "error parsing combo data. Possible corruption." << endl;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: FileSystemInt::saveComboToFileSystem: " << name << endl;
#endif
	return name;
}


int FileSystemInt::deleteComboFromFileSystem(std::string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemInt::deleteComboFromFileSystem" << endl;
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
		if(debugOutput) cout << "***** EXITING: FileSystemInt::deleteComboFromFileSystem: " << status << endl;
#endif

	return status;
}

} /* namespace std */
