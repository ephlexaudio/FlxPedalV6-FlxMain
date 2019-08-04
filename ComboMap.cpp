/*
 * ComboMap.cpp
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */

#include "ComboMap.h"


using namespace std;


	ComboMap::ComboMap ()
	{

	}

	ComboMap::~ComboMap ()
	{
		// TODO Auto-generated destructor stub
	}


#define dbg 1
ComboDataInt ComboMap::getComboObject(string comboName)
{
	ComboDataInt combo;
	int status = 0;
#if(dbg >= 1)
	 cout << "*****ENTERING ComboMap::getComboObject" << endl;
	 cout << "comboName: " << comboName << endl;
#endif
	 try
	 {
			if(comboDataMap.find(comboName) != comboDataMap.end())
			{
				combo = comboDataMap[comboName];

			}
	 }
	 catch(exception &e)
	 {
		 cout << "exception in ComboMap::getComboObject: " << e.what() << endl;
		 status = -1;
	 }
#if(dbg >= 1)
		 cout << "***** EXITING ComboMap::getComboObject: " << combo.getName() << "\tstatus: " << status << endl;
#endif

	return combo;
}


#define dbg 1
int ComboMap::loadComboMapAndList(void)
{
	int status = 0;
#if(dbg >= 1)
 cout << "*****ENTERING ComboMap::loadComboMapAndList" << endl;
#endif


 try
 {
	 this->comboNameVector = fsInt.getComboListFromFileSystem();

	 if(this->comboNameVector.empty() == true)
	 {
#if(dbg>=2)
		 cout << "popen failed." << endl;
#endif
		 status = -1;
	 }
	 else
	 {
		 comboDataMap.clear();

		// bool exit = false;

		 for(auto & comboName : this->comboNameVector)
		 {
			 if(comboName.empty() == false && comboName.length() < 20)
			 {
				this->addNewComboObjectToMapAndList(comboName);
			 }
		 }
	 }
 }
 catch(exception &e)
 {
	 cout << "exception in ComboMap::loadComboMapAndList: " << e.what() << endl;
	 status = -1;
 }


#if(dbg >= 1)
 cout << "***** EXITING ComboMap::loadComboMapAndList: " << status << endl;
#endif
	return status;
}

std::vector<string> ComboMap::getComboNameList(void)
{
		return this->comboNameVector;
}


//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
#define dbg 0
int ComboMap::addNewComboObjectToMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "*****ENTERING ComboMap::addNewComboObjectToMapAndList" << endl;
	 cout << "comboName: " << comboName << endl;
#endif
	ComboDataInt tempCombo;
	 try
	 {
			Json::Value comboJsonData = fsInt.loadValidatedComboJsonDataFromFileSystemToComboMap(comboName);

			tempCombo.loadIndexMappedComboData2(comboJsonData);

			if(tempCombo.getName().empty() == false)
			{
				comboDataMap[comboName] = tempCombo;


				if(find(this->comboNameVector.begin(), this->comboNameVector.end(), comboName) == this->comboNameVector.end())
				{
					this->comboNameVector.push_back(comboName);
				}

			}
			else
			{
				 cout << "failed to add combo object: " << comboName << endl;
				status = -1;
			}
	 }
	 catch(exception &e)
	 {
		 cout << "exception in ComboMap::addNewComboObjectToMapAndList: " << e.what() << endl;
		 status = -1;
	 }

#if(dbg >= 1)
	 cout << "***** EXITING ComboMap::addNewComboObjectToMapAndList: " << comboName << ":" << status << endl;
#endif
	return status;
}

//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
#define dbg 0
int ComboMap::addNewComboObjectToMap(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "*****ENTERING ComboMap::addNewComboObjectToMap" << endl;
	 cout << "comboName: " << comboName << endl;
#endif
	ComboDataInt tempCombo;
	 try
	 {
			Json::Value comboJsonData = fsInt.loadValidatedComboJsonDataFromFileSystemToComboMap(comboName);
			tempCombo.loadIndexMappedComboData2(comboJsonData);
			if(tempCombo.getName().empty() == false)
			{
				comboDataMap[comboName] = tempCombo;
			}
			else
			{
				 cout << "failed to add combo object: " << comboName << endl;
				status = -1;
			}
	 }
	 catch(exception &e)
	 {
		 cout << "exception in ComboMap::addNewComboObjectToMap: " << e.what() << endl;
		 status = -1;
	 }

#if(dbg >= 1)
	 cout << "***** EXITING ComboMap::addNewComboObjectToMap: " << status << endl;
#endif
	return status;
}

#define dbg 0
//************* Use this when re-creating entire comboStruct map *****************
int ComboMap::loadCombosFromFileSystemToComboMap()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "*****ENTERING ComboMap::loadCombosFromFileSystemToComboMap" << endl;

#endif

	try
	{
		this->comboDataMap.clear();
		this->comboNameVector = fsInt.getComboListFromFileSystem();
		for(auto & comboName : this->comboNameVector)
		{
			ComboDataInt tempComboData;
			tempComboData.loadIndexMappedComboData2(fsInt.loadValidatedComboJsonDataFromFileSystemToComboMap(comboName));
			this->comboDataMap[comboName] = tempComboData;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboMap::loadCombosFromFileSystemToComboMap: " << e.what() << endl;
		 status = -1;
	}


#if(dbg >= 1)
	 cout << "***** EXITING ComboMap::loadCombosFromFileSystemToComboMap: " << status << endl;
#endif
	return status;
}

#define dbg 0
int ComboMap::deleteComboObjectFromMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
 cout << "*****ENTERING ComboMap::deleteComboStructFromMapAndList" << endl;
 cout << "comboName: " << comboName << endl;
#endif

	try
	{
		if(comboDataMap.find(comboName) != comboDataMap.end())
		{
			comboDataMap.erase(comboName);
			this->comboNameVector = fsInt.getComboListFromFileSystem();
		}
		else
		{
			 cout << "combo object not found:" << comboName << endl;
			status = -1;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboMap::deleteComboStructFromMapAndList: " << e.what() << endl;
		 status = -1;
	}

#if(dbg >= 1)
 cout << "***** EXITING ComboMap::deleteComboStructFromMapAndList: " << status << endl;
#endif
	return status;
}

bool ComboMap::isInComboMap(string comboName)
{
	return comboDataMap.find(comboName) != comboDataMap.end();
}

void ComboMap::eraseFromMap(string comboName)
{
	comboDataMap.erase(comboName);
}

#define dbg 1
int ComboMap::saveCombo(ComboStruct comboData)
{

	int status = 0;
#if(dbg >= 1)
 cout << "*****ENTERING ComboMap::saveCombo" << endl;
 cout << "comboName: " << comboData.name << endl;
#endif

	 try
	 {
		 string name = comboData.name;

		this->comboDataMap[name].transferComboStructBackToEffectComboJson(comboData);
		Json::Value comboJson = this->comboDataMap[name].getEffectComboJson();

#if(dbg >= 2)
		for(auto & effectJson : comboJson["effectArray"])
		{
			cout << effectJson["name"].asString() << endl;
			for(auto & controlJson : effectJson["controlArray"])
			{
				cout << "\t\t" << controlJson["name"].asString() << ":" << controlJson["conParamArray"][0]["value"].asInt() << endl;
			}
		}
#endif
		string comboName = fsInt.saveComboJsonDataFromComboMapToFileSystem(this->comboDataMap[name].getEffectComboJson());
	 }
	 catch(exception &e)
	 {
		 cout << "exception in ComboMap::saveCombo: " << e.what() << endl;
		 status = -1;
	 }

#if(dbg >= 1)
 cout << "***** EXITING ComboMap::saveCombo: " << status << endl;
#endif

	return status;
}

