/*
 * mainFunctions.h
 *
 *  Created on: Mar 2, 2016
 *      Author: mike
 */

#ifndef MAINFUNCTIONS_H_
#define MAINFUNCTIONS_H_



#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <jack/jack.h>
#include <jack/control.h>
#include <algorithm>


struct _jackParams{
	int period; // number of frames/period
	int buffer; // number of periods
};


int openJack(void);
int startJack(void);
int stopJack(void);
int closeJack(void);

/*int createIPCFiles(void);
int openIPCFiles(void);
int closeIPCFiles(void);*/

int getRunningProcesses(void);
int initializePedal(char *fileName);
int loadCombo(void);
int runCombo(void);
//int updateCombo(int paramIndex, int paramValueIndex);
int stopCombo(void);
ComboDataInt getComboObject(string comboName);

int loadComboStructArray(vector<string> comboList);
std::vector<string> getComboArrayList(void);
int getComboListIndex(string comboName);
int addComboStructToArray(string comboName);
int deleteComboStructFromArray(char *comboName);

int loadComboStructVector(void);
std::vector<string> getComboVectorList(void);
int getComboVectorIndex(string comboName);
int addComboStructToVector(string comboName);
int deleteComboStructFromVector(string comboName);

int loadComboStructMapAndList(void);
std::vector<string> getComboMapList(void);
int getComboMapIndex(string comboName);
int addComboStructToMapAndList(string comboName);
int addComboStructToMap(string comboName);
int deleteComboStructFromMapAndList(string comboName);

int deleteComboNameFromList(string comboName);


int readFootswitches(void);
int bypassAll(void);
#endif /* MAINFUNCTIONS_H_ */
