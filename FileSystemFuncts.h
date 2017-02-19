/*
 * MainFuncts.h
 *
 *  Created on: Mar 12, 2016
 *      Author: mike
 */

#ifndef DATAFUNCTS_H_
#define DATAFUNCTS_H_

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

#include "utilityFunctions.h"

using namespace std;


int validateJsonString(std::string jsonString);
int validateJsonBuffer(char *jsonBuffer);

std::vector<string> getComponentList(void);
std::string getComponentData(std::string componentName);

std::vector<string> getComboListFromFileSystem(void);
//std::vector<string> getComboFileList(void);
std::string getComboDataFromFileSystem(std::string comboName);
string saveComboToFileSystem(std::string comboData);
int deleteComboFromFileSystem(std::string comboName);


#endif /* DATAFUNCTS_H_ */
