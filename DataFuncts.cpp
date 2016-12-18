/*
 * MainFuncts.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: mike
 */

#include "DataFuncts.h"

/*extern std::vector<string> jsonComponentList;
extern std::vector<string> jsonComboList;

extern std::vector<string> jsonProcessList;*/


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
#define JSON_BUFFER_LENGTH 16000

int validateJsonString(std::string jsonString)
{
	int status = 0;
	Json::Value jsonClean;
	int newlineIndex =0;
	Json::Reader jsonDirtyReader;
	Json::FastWriter jsonCleanWriter;

	char dirtyBuffer[16000];
	string dirtyBufferString;
	char cleanBuffer[16000];
	string cleanBufferString;
	string tempBufferString;
	//char outputBuffer[20000];

	clearBuffer(dirtyBuffer,16000);
	clearBuffer(cleanBuffer,16000);

	tempBufferString.assign(jsonString);
	/************ SANITIZE IN CASE OF CORRUPTION *********************/

	newlineIndex = tempBufferString.find("\n");
	if(newlineIndex != std::string::npos)
	{
		cout << "newline found" << endl;
		dirtyBufferString = tempBufferString.substr(0,newlineIndex);
	}
	else
	{
		cout << "no newline found" << endl;
		dirtyBufferString.assign(tempBufferString);
	}


	cout << "Dirty string: " << dirtyBufferString << endl;
	cout << "Dirty string length: " << dirtyBufferString.size() << endl;
	if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
	{
		cout << "JSON name: " << jsonClean["name"] << endl;
		cleanBufferString = jsonCleanWriter.write(jsonClean);
		cout << "Clean string: " << cleanBufferString << endl;
		cout << "Clean string length: " << cleanBufferString.size() << endl;

		if(dirtyBufferString.size() == cleanBufferString.size())
		{
			cout << "jsonString is clean" << endl;
			status = 1;
		}
		else /*if(dirtyBufferString.size() > cleanBufferString.size())*/
		{
			if(dirtyBufferString.size()/2 > cleanBufferString.size()) // something went REALLY wrong
			{
				cout << "jsonString is highly corrupted and cannot be repaired" << endl;
				status = -1; // jsonString is highly corrupted and cannot be repaired
			}
			else
			{
				dirtyBufferString.clear();
				dirtyBufferString.assign(cleanBufferString);
				cout << "confirming clean..." << endl;
				if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
				{
					cout << "JSON name2: " << jsonClean["name"] << endl;
					cleanBufferString = jsonCleanWriter.write(jsonClean);
					cout << "Clean string2: " << cleanBufferString << endl;
					cout << "Clean string length2: " << cleanBufferString.size() << endl;

					if(dirtyBufferString.size() == cleanBufferString.size() ||
							dirtyBufferString.size() == cleanBufferString.size()-1)
					{
						jsonString.clear();
						jsonString.assign(cleanBufferString);
						cout << "jsonString needed cleaning/repair" << endl;
						status = 0; // jsonString needed cleaning/repair
					}
					else
					{
						cout << "jsonString cleaning failed" << endl;
						status = -1;
						jsonString.clear();
					}

				}
				else
				{
					cout << "jsonString cleaning failed" << endl;
					status = -1;
					jsonString.clear();
				}

			}
		}
	}
	else
	{
		cout << "jsonString could not be parsed" << endl;
		status = -1;
		jsonString.clear();
	}


	return status;
}

int validateJsonBuffer(char *jsonBuffer)
{
	int status = 0;
	Json::Value jsonClean;
	int newlineIndex =0;
	Json::Reader jsonDirtyReader;
	Json::FastWriter jsonCleanWriter;

	char dirtyBuffer[16000];
	string dirtyBufferString;
	char cleanBuffer[16000];
	string cleanBufferString;
	string tempBufferString;
	//char outputBuffer[20000];

	clearBuffer(dirtyBuffer,16000);
	clearBuffer(cleanBuffer,16000);

	tempBufferString = string(jsonBuffer);
	/************ SANITIZE IN CASE OF CORRUPTION *********************/

	newlineIndex = tempBufferString.find("\n");
	cout << "tempBufferString: " << tempBufferString << endl;
	cout << "tempBufferString length: " << tempBufferString.size() << endl;
	cout << "newline found at: " << newlineIndex << endl;
	if(newlineIndex == (tempBufferString.length()-1)) newlineIndex = std::string::npos; // ignore newline at end of string
	if((newlineIndex != std::string::npos))
	{
		//cout << "newline found at: " << newlineIndex << endl;
		dirtyBufferString = tempBufferString.substr(0,newlineIndex);
	}
	else
	{
		cout << "no newline found" << endl;
		dirtyBufferString.assign(tempBufferString);
	}
	dirtyBufferString.erase(remove(dirtyBufferString.begin(),dirtyBufferString.end(),'\n'),dirtyBufferString.end());


	cout << "Dirty string: " << dirtyBufferString << endl;
	cout << "Dirty string length: " << dirtyBufferString.size() << endl;

	//printAsciiNumbers(dirtyBufferString);

	if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
	{
		cout << "JSON name: " << jsonClean["name"] << endl;
		cleanBufferString = jsonCleanWriter.write(jsonClean);
		cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
		cout << "Clean string: " << cleanBufferString << endl;
		cout << "Clean string length: " << cleanBufferString.size() << endl;

		//printAsciiNumbers(cleanBufferString);

		if(dirtyBufferString.size() == cleanBufferString.size() && (newlineIndex == std::string::npos))
		{
			cout << "jsonString is clean" << endl;
			status = 1;
		}
		else /*if(dirtyBufferString.size() > cleanBufferString.size())*/
		{
			if(dirtyBufferString.size()/2 > cleanBufferString.size()) // something went REALLY wrong
			{
				cout << "jsonString is highly corrupted and cannot be repaired" << endl;
				status = -1; // jsonString is highly corrupted and cannot be repaired
			}
			else
			{
				dirtyBufferString.clear();
				dirtyBufferString.assign(cleanBufferString);
				cout << "confirming clean..." << endl;
				if(jsonDirtyReader.parse(dirtyBufferString,jsonClean) == true)
				{
					cout << "JSON name: " << jsonClean["name"] << endl;
					cleanBufferString = jsonCleanWriter.write(jsonClean);
					cleanBufferString.erase(remove(cleanBufferString.begin(),cleanBufferString.end(),'\n'),cleanBufferString.end());
					cout << "Clean string: " << cleanBufferString << endl;
					cout << "Clean string length: " << cleanBufferString.size() << endl;

					if(dirtyBufferString.size() == cleanBufferString.size())
					{
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
						strncpy(jsonBuffer,cleanBufferString.c_str(),JSON_BUFFER_LENGTH);
						cout << "jsonString needed cleaning/repair" << endl;
						status = 0; // jsonString needed cleaning/repair
					}
					else
					{
						cout << "jsonString cleaning failed" << endl;
						status = -1;
						clearBuffer(jsonBuffer, JSON_BUFFER_LENGTH);
					}

				}
				else
				{
					cout << "jsonString cleaning failed" << endl;
					status = -1;
				}

			}
		}
	}
	else
	{
		cout << "jsonString could not be parsed" << endl;
		status = -1;
	}


	return status;
}

Json::Reader dataReader;
#define dbg 0
std::vector<string> getComponentList(void)
{
#if(dbg>=1)
		fprintf(stderr,"entering OfxMain/DataFuncts.cpp getComponentList.\n");
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
#if(dbg>=1)
		fprintf(stderr,"exiting OfxMain/DataFuncts.cpp getComponentList.\n");
#endif
	return compList;
}


#define dbg 0
std::string getComponentData(std::string componentName)
{
#if(dbg>=1)
		fprintf(stderr,"entering OfxMain/DataFuncts.cpp getComponentList.\n");
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
#if(dbg>=1)
		fprintf(stderr,"exiting OfxMain/DataFuncts.cpp getComponentList.\n");
#endif
	return std::string(outputString);
}



/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/


//def update_combo_directory_data():
#define dbg 2
std::vector<string> getComboList(void)
{
#if(dbg>=1)
		cout << "entering OfxMain/DataFuncts.cpp getComponentList." << endl;
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
		cout << "popen failed." << endl;
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
			cout << buffer << endl;
#endif

			for(int i = 0; i<20;i++) buffer[i] = 0;

		}
	}

	if(fdComboList != NULL) pclose(fdComboList);
#if(dbg>=1)
	cout << "exiting OfxMain/DataFuncts.cpp getComponentList." << endl;
#endif
	return comList;
}



#define dbg 0
std::string getComboData(std::string comboName)
{
#if(dbg>=1)
		fprintf(stderr,"entering OfxMain/DataFuncts.cpp getComponentList.\n");
#endif
	errno = 0;
	char comboString[50];
	char jsonBuffer[16000];
	string jsonBufferString;

	sprintf(comboString,"/home/Combos/%s.txt", comboName.c_str());
#if(dbg>=1)
	printf("comboString: %s\n", comboString);
#endif
	clearBuffer(jsonBuffer,16000);
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
		while(fgets(jsonBuffer,16000,fdCombo) != NULL)
		{
			puts(jsonBuffer);
		}
		jsonBufferString = string(jsonBuffer);

		int result = validateJsonString(jsonBufferString);

		if(result == 0) // file needed cleaning, so replacing with cleaned file
		{
			cout << "file needed cleaning, so replacing with cleaned file" << endl;
			fclose(fdCombo);
			fdCombo = fopen(comboString, "w");
			if(fputs(jsonBufferString.c_str(),fdCombo) == -1)
			{
				jsonBufferString.clear();
				cout << "error writing jsonBufferString back to combo file." << endl;
			}
		}
		else if(result == -1) // file is too corrupted to clean
		{
			jsonBufferString.clear();
			cout << "error parsing jsonBufferString" << endl;
		}
		fclose(fdCombo);
	}
#if(dbg>=1)
		fprintf(stderr,"exiting OfxMain/DataFuncts.cpp getComponentList.\n");
#endif
	return jsonBufferString;
}

#define dbg 2
int saveCombo(std::string comboData)
{
#if(dbg>=1)
		cout << "entering OfxMain/DataFuncts.cpp saveCombo." << endl;
#endif
	int status = 0;
	FILE *saveComboFD;
	Json::Value tempJsonCombo;
	int charCount = 0;
	char comboDataString[16000];

	char parsedComboDataString[16000];
	for(int i = 0; i < 16000; i++)
	{
		comboDataString[i]=0;
		parsedComboDataString[i]=0;
	}
	char fileNameString[40];

	cout << "pre parsed JSON string: " << comboData.c_str() << endl;
	if(dataReader.parse(comboData.c_str(), tempJsonCombo))
	{
		sprintf(fileNameString, "/home/Combos/%s.txt", tempJsonCombo["name"].asString().c_str());
#if(dbg>=2)
		std::cout << "saveCombo: " << fileNameString << '\n';
#endif
		//saveComboFD = fopen(fileNameString,"w");
		if((saveComboFD = fopen(fileNameString,"w")) != NULL )
		{
			strcpy(comboDataString, comboData.c_str());
			for(int i = 0; comboDataString[i] != 0; i++) charCount++;
			strncpy(parsedComboDataString, comboDataString, charCount);
#if(dbg>=2)
			std::cout << "OfxMain/DataFuncts/saveCombo size: " << strlen(parsedComboDataString) << endl;
#endif
			int bytesWritten = fwrite(parsedComboDataString,1,charCount,saveComboFD);
#if(dbg>=2)
			std::cout << "OfxMain/DataFuncts/saveCombo bytes written: " << bytesWritten << endl;
#endif
			fclose(saveComboFD);
		}
		else
		{
			cout << "failed to open combo file for writing." << endl;
			status = 1;
		}
	}
	else
	{
		cout << "error parsing combo data. Possible corruption." << endl;
		status = 1;
	}

#if(dbg>=1)
	std::cout << "exiting OfxMain/DataFuncts.cpp saveCombo: " << endl;
#endif
	return status;
}


int deleteCombo(std::string comboName)
{
	int status = 0;



	return status;
}





