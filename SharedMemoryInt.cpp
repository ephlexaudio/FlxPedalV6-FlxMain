/*
 * SharedMemoryInt.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */


#include "config.h"
#include "SharedMemoryInt.h"
#include "utilityFunctions.h"

#define SHARED_MEMORY_PROCESSED 28/*"12"*/
#define SHARED_MEMORY_SECTION_SIZE 4096

//#define SHARED_MEMORY_FILE_ADDRESS 32768
//#define SHARED_MEMORY_FILE_SIZE 32000

#define HOST_SHARED_MEMORY_SECTION_ADDRESS 8192
#define HOST_SHARED_MEMORY_SECTION_SIZE 4096

#define HOST_SHARED_MEMORY_FILE_ADDRESS 32768
#define HOST_SHARED_MEMORY_FILE_SIZE 32000

//#define SHARED_MEMORY_SUBSECTION_SIZE 2048
#define TX_BUFFER_SIZE 1500
#define RX_BUFFER_SIZE 1500

/**************************************
#if(dbg >= 1)
	cout << "***** ENTERING: SharedMemoryInt::" << endl;
	cout << ": " <<  << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: SharedMemoryInt::: " << status << endl;
#endif

#if(dbg >=2)
#endif
********************************************/

#define MCU_SHARED_MEMORY_SECTION_INDEX 0
#define CM0_SHARED_MEMORY_SECTION_INDEX 1


//extern struct Request request;
using namespace std;

static const char *device0 = "/dev/spidev0.0";
//static const char *device1 = "/dev/spidev0.1";
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 100000;

SharedMemoryInt::SharedMemoryInt()
{
	this->status = 0;
	//int status = 0;
	char tempStr[5];
	this->spiFD = open(device0, O_RDWR);
	if (this->spiFD < 0)
	{
		this->status = -1;
		pabort("can't open device");
	}

	/*
	 * spi mode
	 */
	int ret;

	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_WR_MODE, &mode);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't set spi mode");
		}

	}

	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_RD_MODE, &mode);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't get spi mode");
		}

	}

	/*
	 * bits per word
	 */
	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_WR_BITS_PER_WORD, &bits);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't set bits per word");
		}

	}

	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_RD_BITS_PER_WORD, &bits);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't get bits per word");
		}

	}

	/*
	 * max speed hz
	 */
	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't set max speed hz");
		}
	}

	if(this->status >= 0)
	{
		ret = ioctl(this->spiFD, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
		if (ret == -1)
		{
			this->status = -1;
			pabort("can't get max speed hz");
		}

	}
	printf("spi file descriptor: %d", this->spiFD );
	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);


	strcpy(tempStr, "out");
	//dataProcessed(uint8_t pinNumber);
	dataProcessed = GPIOClass(SHARED_MEMORY_PROCESSED);
	dataProcessed.export_gpio();
	dataProcessed.setdir_gpio(tempStr);
	dataProcessed.setval_gpio(0/*"0"*/);//writePin(SHARED_MEMORY_PROCESSED, 0);

}

SharedMemoryInt::~SharedMemoryInt()
{
	int ret;

#if(dbg >= 1)
	cout << "***** ENTERING: SharedMemoryInt::" << endl;
#endif

#if(dbg >= 1)
	cout << "***** EXITING: SharedMemoryInt::: " << status << endl;
#endif

	this->status = 0;
	printf("spi file descriptor: %d", this->spiFD );

	ret = close(this->spiFD);
	if(ret == -1)
	{
		this->status = -1;
		pabort("couldn't close SPI port");
	}
	//newData.unexport_gpio();
	//dataProcessed.unexport_gpio();



}

#define dbg 0
void SharedMemoryInt::dataProcessingStatus(uint8_t pinStatus)
{
#if(dbg >= 1)
	cout << "***** ENTERING: SharedMemoryInt::dataProcessingStatus" << endl;
	cout << "status: " << status << endl;
#endif

	if(pinStatus == 0) dataProcessed.setval_gpio(0/*"0"*/);
	else dataProcessed.setval_gpio(1/*"1"*/);

#if(dbg >= 1)
	cout << "***** EXITING: SharedMemoryInt::dataProcessingStatus" << endl;
#endif

}



#define dbg 0
uint8_t SharedMemoryInt::sendData(uint16_t address, char *data, uint16_t length)
{
#if(dbg >= 1)
	cout << "***** ENTERING: SharedMemoryInt::sendData" << endl;
	cout << "address: " << address << "\tlength: " << length << endl;
#endif
	uint8_t status = 0;
	//address += index*SHARED_MEMORY_SECTION_SIZE;
	uint8_t addressHigh = address>>8;
	uint8_t addressLow = 0x00FF & address;
	struct spi_ioc_transfer spi;
	if(address >= HOST_SHARED_MEMORY_FILE_ADDRESS)
	{
#if(dbg >= 2)
		cout << "sendData file data: " <<  data << endl;
#endif
		for(int i = 0; i < HOST_SHARED_MEMORY_FILE_SIZE; i++) this->sharedMemoryFileTxBuffer[i] = 0;
		this->sharedMemoryFileTxBuffer[HOST_SHARED_MEMORY_FILE_SIZE-1] = 255;

		for(int i = 0; i < HOST_SHARED_MEMORY_FILE_SIZE; i++) this->sharedMemoryFileRxBuffer[i] = 0;
		this->sharedMemoryFileRxBuffer[HOST_SHARED_MEMORY_FILE_SIZE-1] = 255;

		/*this->sharedMemoryFileTxBuffer[0] = 0x02;
		this->sharedMemoryFileTxBuffer[1] = addressHigh;
		this->sharedMemoryFileTxBuffer[2] = addressLow;
		this->sharedMemoryFileTxBuffer[3] = 0; // status byte on MCU, keep blank here*/
		int fileDivisionCount = (length+4)/4000;
		//if(length > HOST_SHARED_MEMORY_FILE_SIZE - 10) length -= 10;

		// make sure all bytes of file division blocks used are send to prevent garbage data
		// being sent in unused bytes
		memcpy(this->sharedMemoryFileTxBuffer,data,(fileDivisionCount+1)*4000);
		this->sharedMemoryFileTxBuffer[length+4] = 255;

		int fileDivisionIndex = 0;

		for(fileDivisionIndex = 0; fileDivisionIndex <= fileDivisionCount; fileDivisionIndex++)
		{
			addressHigh = address>>8;
			addressLow = 0x00FF & address;

			this->fileDivisionTxBuffer[0] = 0x02;
			this->fileDivisionTxBuffer[1] = addressHigh;
			this->fileDivisionTxBuffer[2] = addressLow;
			if(fileDivisionIndex == 0)
			{
				this->fileDivisionTxBuffer[3] = 0; // status byte on MCU, keep blank here
				memcpy(this->fileDivisionTxBuffer+4,this->sharedMemoryFileTxBuffer+fileDivisionIndex*4000,4000);
				this->fileDivisionTxBuffer[4004] = 255;
			}
			else
			{
				memcpy(this->fileDivisionTxBuffer+3,this->sharedMemoryFileTxBuffer+fileDivisionIndex*4000,4000);
				this->fileDivisionTxBuffer[4003] = 0;
				this->fileDivisionTxBuffer[4004] = 255;
			}


			spi.tx_buf        = (unsigned long)(this->fileDivisionTxBuffer); // transmit from "txBuffer"
			spi.rx_buf        = (unsigned long)(this->fileDivisionRxBuffer); // receive into "rxBuffer"
			spi.len           = 4005;//sizeof(this->rxBuffer);
			spi.delay_usecs   = 0;
			spi.speed_hz      = speed;
			spi.bits_per_word = bits;
			spi.cs_change = 0;
			spi.tx_nbits = 0;
			spi.rx_nbits = 0;
			spi.pad = 0;

			errno = 0;
			status = ioctl(this->spiFD, SPI_IOC_MESSAGE(1), &spi) ;

#if(dbg >= 2)
			cout << "spi transmit status: " << strerror(errno) << endl;
#endif
			address += 4000;
		}
	}
	else
	{
#if(dbg >= 2)
	cout << "file length: " << length << endl;
	cout << "sendData data: " << data << endl;
#endif
		for(int i = 0; i < TX_BUFFER_SIZE; i++)
		{
			this->sharedMemoryTxBuffer[i] = 0;
		}
		this->sharedMemoryTxBuffer[TX_BUFFER_SIZE-1] = 255;

		for(int i = 0; i < RX_BUFFER_SIZE; i++) this->sharedMemoryRxBuffer[i] = 0;
		this->sharedMemoryRxBuffer[RX_BUFFER_SIZE-1] = 255;

		this->sharedMemoryTxBuffer[0] = 0x02;
		this->sharedMemoryTxBuffer[1] = addressHigh;
		this->sharedMemoryTxBuffer[2] = addressLow;
		this->sharedMemoryTxBuffer[3] = 0; // status byte on MCU, keep blank here

		if(length > TX_BUFFER_SIZE - 10) length -= 10;
		memcpy(this->sharedMemoryTxBuffer+4,data,length);
		this->sharedMemoryTxBuffer[length+4] = 255;

		spi.tx_buf        = (unsigned long)(this->sharedMemoryTxBuffer); // transmit from "txBuffer"
		spi.rx_buf        = (unsigned long)(this->sharedMemoryRxBuffer); // receive into "rxBuffer"
		spi.len           = length+5;//204;//sizeof(this->rxBuffer);
		spi.delay_usecs   = 0;
		spi.speed_hz      = speed;
		spi.bits_per_word = bits;
		spi.cs_change = 0;
		spi.tx_nbits = 0;
		spi.rx_nbits = 0;
		spi.pad = 0;

		errno = 0;
		status = ioctl(this->spiFD, SPI_IOC_MESSAGE(1), &spi) ;

#if(dbg >= 2)
		cout << "spi transmit status: " << strerror(errno) << endl;
#endif

	}

#if(dbg >= 2)
	puts((const char*)(this->sharedMemoryRxBuffer));
#endif
#if(dbg >= 1)
	cout << "***** EXITING: SharedMemoryInt::sendData: " << status << endl;
#endif
	return status;
}

#define dbg 0
uint8_t SharedMemoryInt::getData(uint16_t address, char *data, uint16_t length)
{
#if(dbg >= 1)
	cout << "***** ENTERING: SharedMemoryInt::getData" << endl;
	cout << "address: " << address << "\tlength: " << length << endl;
#endif

	uint8_t status = 0;
	//int retVal = 0;

	//address += index*SHARED_MEMORY_SECTION_SIZE;
	uint8_t addressHigh;
	uint8_t addressLow;

	struct spi_ioc_transfer spi;

	{

		for(int i = 0; i < TX_BUFFER_SIZE; i++) this->sharedMemoryTxBuffer[i] = 0;
		this->sharedMemoryTxBuffer[TX_BUFFER_SIZE-1] = 255;

		for(int i = 0; i < RX_BUFFER_SIZE; i++) this->sharedMemoryRxBuffer[i] = 0;
		this->sharedMemoryRxBuffer[RX_BUFFER_SIZE-1] = 255;

		addressHigh = address>>8;
		addressLow = 0x00FF & address;

		this->sharedMemoryTxBuffer[0] = 0x03;
		this->sharedMemoryTxBuffer[1] = addressHigh;
		this->sharedMemoryTxBuffer[2] = addressLow;
		this->sharedMemoryTxBuffer[3] = 0; // status byte on MCU, keep blank here
		this->sharedMemoryTxBuffer[length+4] = 255;

		{
			spi.tx_buf        = (unsigned long)(this->sharedMemoryTxBuffer); // transmit from "txBuffer"
			spi.rx_buf        = (unsigned long)(this->sharedMemoryRxBuffer); // receive into "rxBuffer"
			spi.len           = length+10;//sizeof(this->rxBuffer);
			spi.delay_usecs   = 0;
			spi.speed_hz      = speed;
			spi.bits_per_word = bits;
			spi.cs_change = 0;
			spi.tx_nbits = 0;
			spi.rx_nbits = 0;
			spi.pad = 0;
		}

		int done = 0;

		int loopCount = 0;
		while(done == 0 && loopCount < 5) // loop count prevents infinite loop if something goes wrong
		{
			errno = 0;

			if(ioctl(this->spiFD, SPI_IOC_MESSAGE(1), &spi) == -1)
			{
				cout << "spi ioctl failure: " << strerror(errno) << endl;
				return 1;
			}

	#if(dbg >= 2)
			cout << "spi data: ";
	#endif
			memcpy(data, this->sharedMemoryRxBuffer+4, length);
			for(int dataIndex = 0; dataIndex < length; dataIndex++)
			{
				//data[dataIndex] = this->sharedMemoryRxBuffer[dataIndex+4];
	#if(dbg >= 2)
				cout << data[dataIndex] << ',';
	#endif
				if(data[dataIndex] == 255)  // end marker found
				{
					done = 1;
				}
			}
			int dataIndex = 0;


	#if(dbg >= 2)
			cout << endl;
	#endif
			loopCount++;
		}
#if(dbg >= 2)
		puts((const char*)(data));
#endif
	}



#if(dbg >= 1)
	cout << "***** EXITING: SharedMemoryInt::getData: " << status << endl;
#endif

	return status;
}

