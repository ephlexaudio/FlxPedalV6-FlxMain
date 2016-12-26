/*
 * utilityFunctions.h
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <time.h>
#include <sys/time.h>

#include <linux/types.h>

using namespace std;

void startTimer(void);
int stopTimer(const char *description);

void pabort(const char *s);
void delay(unsigned long delay);
void clearBuffer(char *buffer, int length);
void zero2Space(char* buffer, uint16_t length);
void cleanString(char* src, char* dest);
void printAsciiNumbers(char *charArrayData);
void printAsciiNumbers(string stringData);
#endif /* UTILITYFUNCTIONS_H_ */
