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
#include <string>
#include <vector>
#include <array>
#include <stdio.h>
#include <fcntl.h>
#include <json/json.h>
#include <errno.h>


using namespace std;



std::vector<string> getComponentList(void);
std::string getComponentData(std::string componentName);
std::vector<string> getComboList(void);
std::string getComboData(std::string comboName);
int saveCombo(std::string comboData);
int deleteCombo(std::string comboName);


#endif /* DATAFUNCTS_H_ */
