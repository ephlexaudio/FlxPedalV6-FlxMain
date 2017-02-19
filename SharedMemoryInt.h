/*
 * SharedMemoryInt.h
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#ifndef SHAREDMEMORYINT_H_
#define SHAREDMEMORYINT_H_


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <json/json.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/stat.h>
#include "GPIOClass.h"

//#define SHARED_MEMORY_READY "44"
//#define SHARED_MEMORY_PROCESSED "12"
#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500
#define HOST_SHARED_MEMORY_FILE_SIZE 32000
//static void pabort(const char *s);

/*struct Request {
	int requestCommand;
	char requestData[200];
};*/

class SharedMemoryInt
{
public:
	SharedMemoryInt();
	~SharedMemoryInt();


private:


//protected:
	char sharedMemoryTxBuffer[TX_BUFFER_SIZE];
	char sharedMemoryRxBuffer[RX_BUFFER_SIZE];
	char sharedMemoryFileTxBuffer[HOST_SHARED_MEMORY_FILE_SIZE];
	char sharedMemoryFileRxBuffer[HOST_SHARED_MEMORY_FILE_SIZE];
	char fileDivisionTxBuffer[4005];
	char fileDivisionRxBuffer[4005];

public:

	GPIOClass dataProcessed;// = GPIOClass(SHARED_MEMORY_PROCESSED);//, "out");
	int spiFD;
	uint8_t status;
	void dataProcessingStatus(uint8_t pinStatus);
	uint8_t sendData(uint16_t address, char *data, uint16_t length);
	uint8_t getData(uint16_t address, char *data, uint16_t length);
	/*uint8_t sendData(uint8_t fromIndex, uint8_t toIndex, char *data, uint16_t length);
	uint8_t getData(uint8_t fromIndex, uint8_t toIndex, char *data, uint16_t length);*/

};



#endif /* SHAREDMEMORYINT_H_ */
