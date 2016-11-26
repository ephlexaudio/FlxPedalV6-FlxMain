/*
 * utilityFunctions.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#include "utilityFunctions.h"

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
