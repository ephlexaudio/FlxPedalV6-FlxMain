/*
 * FileSystemInt.cpp
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */
#include "config.h"
#include "FileSystemInt.h"

//extern bool debugOutput;

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


#define dbg 1
std::vector<string> FileSystemInt::getComboListFromFileSystem(void)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::getComboListFromFileSystem" << endl;
#endif
	 int status = 0;
	std::vector<string> comList;
	FILE *fdComboList;
	char buffer[20];

	try
	{
		fdComboList = popen("ls /home/Combos","r");
		clearBuffer(buffer,20);

		if(fdComboList == NULL)
		{
	#if(dbg>=2)
			 cout << "popen failed." << endl;
	#endif
			 status = -1;
		}
		else
		{
			fflush(fdComboList);
			while(fgets(buffer,20,fdComboList) != NULL)
			{
				strcpy(buffer,strtok(buffer,"."));
				comList.push_back(buffer);
	#if(dbg>=2)
				 cout << buffer << endl;
	#endif

				clearBuffer(buffer,20);

			}
		}

		if(fdComboList != NULL) pclose(fdComboList);

	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::getComboListFromFileSystem: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::getComboListFromFileSystem: " << status << endl;
#endif
	return comList;
}

#define dbg 0
string FileSystemInt::saveComboDataFromHostToFileSystem(std::string comboJson)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::saveComboToFileSystem" << endl;
#endif
	 int status = 0;
	string name;
	FILE *saveComboFD;
	Json::Value tempJsonCombo;
	//int charCount = 0;
	char comboDataBuffer[FILE_SIZE];
	Json::Reader dataReader;
	try
	{
		clearBuffer(comboDataBuffer,FILE_SIZE);
		char fileNameBuffer[50];
		clearBuffer(fileNameBuffer,50);
	#if(dbg>=2)
		 cout << "pre parsed JSON string: " << comboJson.c_str() << endl;
	#endif
		if(dataReader.parse(comboJson, tempJsonCombo))
		{
			snprintf(fileNameBuffer, 49,"/home/Combos/%s.txt", tempJsonCombo["name"].asString().c_str());
	#if(dbg>=2)
			 std::cout << "saveComboToFileSystem: " << fileNameBuffer << '\n';
	#endif
			if((saveComboFD = fopen(fileNameBuffer,"w")) != NULL )
			{
				int bytesWritten = fwrite(comboJson.c_str(),1,comboJson.size(),saveComboFD);

#if(dbg>=2)
				cout << "saveComboToFileSystem size: " << comboJson.size() << endl;
				cout << "saveComboToFileSystem bytes written: " << bytesWritten << endl;
	#endif
				fclose(saveComboFD);
				name = tempJsonCombo["name"].asString();
			}
			else
			{
				 cout << "failed to open combo file for writing." << endl;
			}
		}
		else
		{
			 cout << "error parsing combo data. Possible corruption." << endl;
		}

	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::saveComboToFileSystem: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::saveComboToFileSystem: " << name << "\tstatus: " << status <<  endl;
#endif
	return name;
}


#define dbg 0
string FileSystemInt::loadComboDataFromFileSystemToHost(string comboName)
{
#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: FileSystemFuncts::getComboDataFromFileSystem" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	int status = 0;

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
		status = -1;
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
	if(debugOutput) cout << "***** EXITING: FileSystemFuncts::getComboDataFromFileSystem: " << status << endl;
#endif
	return jsonBufferString;
}



#define dbg 1
Json::Value FileSystemInt::loadValidatedComboJsonDataFromFileSystemToComboMap(std::string comboName)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::loadValidatedComboJsonDataFromFileSystemToComboMap" << endl;
	 cout << "comboName: " << comboName << endl;
#endif
	 int status = 0;
	errno = 0;
	char comboString[50];
	char jsonBuffer[FILE_SIZE];

	string jsonBufferString;
	Json::Value comboJsonData;
	try
	{
		sprintf(comboString,"/home/Combos/%s.txt", comboName.c_str());
	#if(dbg>=1)
		cout << "comboString: " << comboString << endl;
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

			// ******************* Convert string to JSON **************************
			try
			{
				int result = validateJsonBuffer(jsonBuffer);
	#if(dbg >= 1)
				cout << "parsing jsonString in effectComboJson" << endl;

	#endif
				if(result >= 0)
				{
					jsonReader.parse(jsonBuffer,comboJsonData);
					cout << "comboJsonData name: " << comboJsonData["name"].asString();
					jsonBufferString = string(jsonBuffer);
					cout << "JSON combo data OK: " << endl;
				}
				else
				{
					status = -1;
					cout << "JSON parse failed." << endl;
				}
			}
			catch(exception &e)
			{
				cout << "exception in ComboDataInt::loadValidatedComboJsonDataFromFileSystemToComboMap: " << e.what() <<  endl;
				status = -1;
			}
			fclose(fdCombo);
		}
	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::loadValidatedComboJsonDataFromFileSystemToComboMap: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::getComboDataFromFileSystem: " << status << endl;
#endif
	return comboJsonData;
}




#define dbg 2
string FileSystemInt::saveComboJsonDataFromComboMapToFileSystem(Json::Value comboJsonData)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::saveComboJsonDataFromComboMapToFileSystem" << endl;
#endif
#if(dbg >= 2)
	 cout << comboJsonData.toStyledString() << endl;
#endif
	 int status = 0;
	string name;
	FILE *saveComboFD;
	//int charCount = 0;
	char comboDataBuffer[FILE_SIZE];
	Json::Reader dataReader;
	string comboDataString;
	try
	{
		clearBuffer(comboDataBuffer,FILE_SIZE);
		char fileNameBuffer[50];
		clearBuffer(fileNameBuffer,50);
		comboDataString = jsonWriter.write(comboJsonData);

		snprintf(fileNameBuffer, 49,"/home/Combos/%s.txt", comboJsonData["name"].asString().c_str());
#if(dbg>=2)
		 std::cout << "saveComboToFileSystem: " << fileNameBuffer << '\n';
#endif
		if((saveComboFD = fopen(fileNameBuffer,"w")) != NULL )
		{
			int bytesWritten = fwrite(comboDataString.c_str(),1,comboDataString.length(),saveComboFD);

#if(dbg>=2)
			cout << "saveComboToFileSystem size: " << comboDataString.size() << endl;
			cout << "saveComboToFileSystem bytes written: " << bytesWritten << endl;
#endif
			fclose(saveComboFD);
			name = comboJsonData["name"].asString();
		}
		else
		{
			 cout << "failed to open combo file for writing." << endl;
		}
	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::saveComboJsonDataFromComboMapToFileSystem: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::saveComboJsonDataFromComboMapToFileSystem: " << name << "\tstatus: " << status << endl;
#endif
	return name;
}


int FileSystemInt::deleteComboFromFileSystem(std::string comboName)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::deleteComboFromFileSystem" << endl;
	 cout << "comboName: " << comboName << endl;
#endif
		try
		{
			char cliString[50];
			char cliResult[100];
			sprintf(cliString, "rm /home/Combos/%s.txt", comboName.c_str());
		#if(hostUiDbg == 1)
			 cout << "CLI string for delete: " << cliString << endl;
		#endif
			strcpy(cliResult,strerror(system(cliString)));
			 cout << "delete result: " << cliResult << endl;

			if(strncmp(cliResult,"Success",7) == 0)
			{
				status = 0;
			}
			else
			{
				status = -1;
			}
		}
		catch(exception &e)
		{
			cout << "exception in FileSystemInt::deleteComboFromFileSystem: " << e.what() << endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: FileSystemInt::deleteComboFromFileSystem: " << status << endl;
#endif

	return status;
}


Json::Value FileSystemInt::loadPedalUtilityJsonDataFromFileSystem()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::loadPedalUtilityJsonDataFromFileSystem" << endl;

#endif
	 int status = 0;
	errno = 0;
	char utilityString[50];
	char jsonBuffer[FILE_SIZE];

	string jsonBufferString;
	Json::Value utilityJsonData;
	try
	{
		sprintf(utilityString,"/home/FlxUtilityData.txt");
	#if(dbg>=1)
		cout << "utilityString: " << utilityString << endl;
	#endif
		clearBuffer(jsonBuffer,FILE_SIZE);
		FILE *fdUtility = fopen(utilityString, "r");
		if(fdUtility == NULL)
		{
	#if(dbg>=2)
			printf("open failed.\n");
			printf("errno %d:%s\n", errno, strerror(errno));
	#endif
		}
		else
		{
			while(fgets(jsonBuffer,FILE_SIZE,fdUtility) != NULL)
			{
	#if(dbg>=2)
				puts(jsonBuffer);
	#endif
			}

			// ******************* Convert string to JSON **************************
			try
			{
				int result = validateJsonBuffer(jsonBuffer);
	#if(dbg >= 1)
				cout << "parsing jsonString in effectComboJson" << endl;

	#endif
				if(result >= 0)
				{
					jsonReader.parse(jsonBuffer,utilityJsonData);
					cout << "utilityJsonData name: " << utilityJsonData["name"].asString();
					jsonBufferString = string(jsonBuffer);
					cout << "JSON utility data OK: " << endl;
				}
				else
				{
					status = -1;
					cout << "JSON parse failed." << endl;
				}
			}
			catch(exception &e)
			{
				cout << "exception in FileSystemInt::loadPedalUtilityJsonDataFromFileSystem (JSON validation/parsing): " << e.what() <<  endl;
				status = -1;
			}
			fclose(fdUtility);
		}
	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::loadPedalUtilityJsonDataFromFileSystem (file/data access): " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::loadPedalUtilityJsonDataFromFileSystem: " << status << endl;
#endif
	return utilityJsonData;
}


#define dbg 2
int FileSystemInt::savePedalUtilityJsonDataToFileSystem(Json::Value utilityJsonData)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: FileSystemInt::savePedalUtilityJsonDataToFileSystem" << endl;
#endif
#if(dbg >= 2)
	 cout << utilityJsonData.toStyledString() << endl;
#endif
	 int status = 0;
	string name;
	FILE *saveUtilityFD;
	//int charCount = 0;
	char utilityDataBuffer[FILE_SIZE];
	Json::Reader dataReader;
	string utilityDataString;
	try
	{
		clearBuffer(utilityDataBuffer,FILE_SIZE);
		char fileNameBuffer[50];
		clearBuffer(fileNameBuffer,50);
		utilityDataString = jsonWriter.write(utilityJsonData);

		snprintf(fileNameBuffer, 49,"/home/FlxUtilityData.txt");
#if(dbg>=2)
		 std::cout << "saveComboToFileSystem: " << fileNameBuffer << '\n';
#endif
		if((saveUtilityFD = fopen(fileNameBuffer,"w")) != NULL )
		{

			int bytesWritten = fwrite(utilityDataString.c_str(),1,utilityDataString.length(),saveUtilityFD);

#if(dbg>=2)
			cout << "saveComboToFileSystem size: " << utilityDataString.size() << endl;
			cout << "saveComboToFileSystem bytes written: " << bytesWritten << endl;
#endif
			fclose(saveUtilityFD);
		}
		else
		{
			 cout << "failed to open utility file for writing." << endl;
		}
	}
	catch(exception &e)
	{
		cout << "exception in FileSystemInt::savePedalUtilityJsonDataToFileSystem: " << e.what() << endl;
		status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: FileSystemInt::savePedalUtilityJsonDataToFileSystem: " << status << endl;
#endif
	return status;


}





} /* namespace std */
