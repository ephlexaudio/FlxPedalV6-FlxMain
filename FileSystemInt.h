/*
 * FileSystemInt.h
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */

#ifndef FILESYSTEMINT_H_
#define FILESYSTEMINT_H_

#include <cstring>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <stdio.h>
#include <fcntl.h>
#include <json/json.h>
#include <errno.h>

#define JSON_BUFFER_LENGTH 32000

#include "utilityFunctions.h"
namespace std
{

	class FileSystemInt
	{
		Json::Reader jsonReader;
		Json::FastWriter jsonWriter;

	public:
		FileSystemInt ();
		~FileSystemInt ();

		// for Host UI
		string saveComboDataFromHostToFileSystem(string comboJson);
		string loadComboDataFromFileSystemToHost(string comboName);
		// for ComboMap
		std::vector<string> getComboListFromFileSystem(void);
		Json::Value loadValidatedComboJsonDataFromFileSystemToComboMap(std::string comboName);
		string saveComboJsonDataFromComboMapToFileSystem(Json::Value comboJsonData);
		int deleteComboFromFileSystem(std::string comboName);
		// for PedalUtilityData
		Json::Value loadPedalUtilityJsonDataFromFileSystem();
		int savePedalUtilityJsonDataToFileSystem(Json::Value comboJsonData);


	};

} /* namespace std */

#endif /* FILESYSTEMINT_H_ */
