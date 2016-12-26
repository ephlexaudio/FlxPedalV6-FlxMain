/*
 * utilityFunctions.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#include "utilityFunctions.h"


long startStamp;
long stopStamp;
struct timeval tv;


void startTimer(void)
{
	gettimeofday(&tv, NULL);
	startStamp = 1000000*tv.tv_sec+tv.tv_usec;
}

int stopTimer(const char *description)
{
	gettimeofday(&tv, NULL);
	stopStamp = 1000000*tv.tv_sec+tv.tv_usec;
	if(description != NULL)
	{
		cout << "******" << description << "time: " << stopStamp - startStamp << endl;
	}

	return (int)(stopStamp - startStamp);
}

void delay(unsigned long delay)
{
 unsigned long i;

 for (i = 0; i < delay; i++){;}
}


void clearBuffer(char *buffer, int length)
{
	uint16_t index;

	for(index = 0; index < length; index++)
	{
		buffer[index] = 0;
	}
}

void pabort(const char *s)
{
	perror(s);
	//abort();
	//goto exit;
}

void zero2Space(char* buffer, uint16_t length)
{
	uint16_t i = 0;
	for(i = 0; i < length; i++)
	{
		if(buffer[i] == 0) buffer[i] = ' ';
	}
}

void cleanString(char* src, char* dest)
{
	int srcLength = strlen(src);
	int destIndex = 0;
	for(int srcIndex = 0; srcIndex < srcLength; srcIndex++)
	{
		if(' ' <= src[srcIndex] && src[srcIndex] <= '~' )
		{
			dest[destIndex++] = src[srcIndex];
		}
	}
	dest[destIndex] = 0;
}

void printAsciiNumbers(char *charArrayData)
{
	int charArrayLength = strlen(charArrayData);

	cout << endl;
	for(int i = 0; i < charArrayLength; i++)
	{
		cout << charArrayData[i] << ',';
	}
	cout << endl;
}

void printAsciiNumbers(string stringData)
{
	cout << endl;
	for(int i = 0; i < stringData.length(); i++)
	{
		cout << (int)(stringData[i]) << ',';
	}
	cout << endl;

}

