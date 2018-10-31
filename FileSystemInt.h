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

	public:
		FileSystemInt ();
		~FileSystemInt ();


		std::vector<string> getComboListFromFileSystem(void);
		std::string getComboDataFromFileSystem(std::string comboName);
		string saveComboToFileSystem(std::string comboData);
		int deleteComboFromFileSystem(std::string comboName);
	};

} /* namespace std */

#endif /* FILESYSTEMINT_H_ */
