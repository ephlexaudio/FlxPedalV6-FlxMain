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

int startJack(void);
int stopJack(void);


int loadComponentSymbols(void);
int loadControlTypeSymbols(void);

int stopCombo(void);
ComboDataInt getComboObject(string comboName);
int listComboMapObjects(void);

int loadComboStructMapAndList(void);
std::vector<string> getComboMapList(void);
int addComboStructToMapAndList(string comboName);
int addComboStructToMap(string comboName);
int deleteComboStructFromMapAndList(string comboName);



#endif /* MAINFUNCTIONS_H_ */
