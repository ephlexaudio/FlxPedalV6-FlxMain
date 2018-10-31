/*
 * ComboMap.cpp
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */

#include "ComboMap.h"

extern bool debugOutput;

namespace std
{

	ComboMap::ComboMap ()
	{

	}

	ComboMap::~ComboMap ()
	{
		// TODO Auto-generated destructor stub
	}


#define dbg 0
ComboDataInt ComboMap::getComboObject(string comboName)
{
	ComboDataInt combo;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboMap::getComboObject" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	if(comboDataMap.find(comboName) != comboDataMap.end())
	{
		combo = comboDataMap[comboName];
	}
#if(dbg >= 1)
		if(debugOutput) cout << "***** EXITING ComboMap::getComboObject: " << comboObject.getName() << endl;
#endif

	return combo;
}


#define dbg 0
int ComboMap::loadComboMapAndList(void)
{
	int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING ComboMap::loadComboStructMapAndList" << endl;
#endif



	this->comboNameVector = fsInt.getComboListFromFileSystem();

	if(this->comboNameVector.empty() == true)
	{
	#if(dbg>=2)
		if(debugOutput) cout << "popen failed." << endl;
	#endif
		status = -1;
	}
	else
	{
		comboDataMap.clear();

		for(auto & comboName : this->comboNameVector)
		{
			addNewComboObjectToMapAndList(comboName);
		}

	}

#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING ComboMap::loadComboStructMapAndList: " << status << endl;
#endif
	return status;
}






std::vector<string> ComboMap::getComboNameList(void)
{
		return this->comboNameVector;
}


//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
int ComboMap::addNewComboObjectToMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboMap::addComboStructToMapAndList" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif
	ComboDataInt tempCombo;
	tempCombo.loadIndexMappedComboData(comboName);
	if(tempCombo.getName().empty() == false)
	{
		comboDataMap[comboName] = tempCombo;
		fsInt.getComboListFromFileSystem();
	}
	else
	{
		if(debugOutput) cout << "failed to add combo object: " << comboName << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboMap::addComboStructToMapAndList: " << status << endl;
#endif
	return status;
}

//************* Use this when adding individual comboStructs to comboStruct map and comboList *****************
int ComboMap::addNewComboObjectToMap(string comboName)
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboMap::addComboStructToMapAndList" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif
	ComboDataInt tempCombo;
	tempCombo.loadIndexMappedComboData(comboName);
	if(tempCombo.getName().empty() == false)
	{
		comboDataMap[comboName] = tempCombo;
	}
	else
	{
		if(debugOutput) cout << "failed to add combo object: " << comboName << endl;
		status = -1;
	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboMap::addComboStructToMapAndList: " << status << endl;
#endif
	return status;
}

#define dbg 0
//************* Use this when re-creating entire comboStruct map *****************
int ComboMap::loadCombosFromFileSystemToComboMap()
{
	int status = 0;
#if(dbg >= 1)
	if(debugOutput) cout << "*****ENTERING ComboMap::loadCombosFromFileSystemToComboMap" << endl;
	if(debugOutput) cout << "comboName: " << comboName << endl;
#endif

	try
	{
		this->comboDataMap.clear();
		this->comboNameVector = fsInt.getComboListFromFileSystem();
		for(auto & comboName : this->comboNameVector)
		{
			ComboDataInt tempComboData;
			tempComboData.loadIndexMappedComboData(comboName);
			this->comboDataMap[comboName] = tempComboData;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboMap::loadCombosFromFileSystemToComboMap: " << e.what() << endl;
	}


#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING ComboMap::loadCombosFromFileSystemToComboMap: " << status << endl;
#endif
	return status;
}


int ComboMap::deleteComboObjectFromMapAndList(string comboName)
{
	int status = 0;
#if(dbg >= 1)
if(debugOutput) cout << "*****ENTERING ComboMap::deleteComboStructFromMapAndList" << endl;
if(debugOutput) cout << "comboName: " << comboName << endl;
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
			if(debugOutput) cout << "combo object not found:" << comboName << endl;
			status = -1;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboMap::deleteComboStructFromMapAndList: " << e.what() << endl;
	}

#if(dbg >= 1)
if(debugOutput) cout << "***** EXITING ComboMap::deleteComboStructFromMapAndList: " << status << endl;
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

int ComboMap::saveCombo(string comboName, ComboStruct comboData)
{
	int status = 0;

	status = this->comboDataMap[comboName].saveCombo(comboData);

	return status;
}

/*int ComboMap::setProcessUtilities(ProcessUtility procUtil)
{
	int status = 0;

	ComboDataInt::setProcessUtilityData(procUtil);

	return status;
}*/
} /* namespace std */
