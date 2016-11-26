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
	sprintf(comboString,"/home/Combos/%s.txt", comboName.c_str());
#if(dbg>=1)
	printf("comboString: %s\n", comboString);
#endif
	FILE *fdCombo = fopen(comboString, "r");
	char outputString[20000];
	if(fdCombo == NULL)
	{
#if(dbg>=2)
		printf("open failed.\n");
		printf("errno %d:%s\n", errno, strerror(errno));
#endif
	}
	else
	{
		while(fgets(outputString,20000,fdCombo) != NULL)
		{
			puts(outputString);
		}
	}
	fclose(fdCombo);
#if(dbg>=1)
		fprintf(stderr,"exiting OfxMain/DataFuncts.cpp getComponentList.\n");
#endif
	return std::string(outputString);
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





