/*
 * Delay.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: buildrooteclipse
 */

#include "Processes.h"


#include "../valueArrays.h"
#include "../config.h"


using namespace std;



#define dbg 0

	Delay::Delay():delayTimeAveragingBuffer {0,0,0,0}
	{
#if(dbg >= 1)
	 cout << "ENTERING Delay::Delay(Default)" << endl;
#endif


		status = 0;
		bufferSize = 256;
		this->inputBufferIndex = 0;
		this->outputBufferIndex = 0;
		this->paramControllerBufferIndex[0] = 0;
		this->paramControllerBufferIndex[1] = 0;
		this->name = "Delay(Default)";
		inputPtr  = 0;
		outputPtr = 0;
		delayCoarse = 0;
		delayFine = 0;
#if(dbg >= 1)
	 cout << "EXITING Delay::Delay(Default)"  << endl;
#endif

	}

	Delay::Delay(ProcessObjectData processData):delayTimeAveragingBuffer {0,0,0,0}
	{
#if(dbg >= 1)
	 cout << "ENTERING Delay::Delay" << endl;
#endif
		status = 0;
		bufferSize = 256;
		this->processData = processData;
		this->name = this->processData.processName;

		this->inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
		this->outputBufferIndex = this->processData.outputConnectedBufferIndexArray[0];
		this->paramControllerBufferIndex[0] = this->processData.parameterArray[0].paramContBufferIndex;
		this->paramControllerBufferIndex[1] = this->processData.parameterArray[1].paramContBufferIndex;


		std::fill(std::begin(this->delayBuffer), std::end(this->delayBuffer), 0.0000);

		inputPtr  = 0;
		outputPtr = 0;
		delayCoarse = 0;
		delayFine = 0;
#if(dbg >= 1)
	 cout << "EXITING Delay::Delay: " << this->getName() << endl;
#endif
	}

	Delay::~Delay()
	{

	}

#define dbg 0

	void Delay::load()
	{
#if(dbg >= 1)
	 cout << "ENTERING Delay::load" << endl;
#endif
	try
	{
		 std::fill(std::begin(this->delayBuffer), std::end(this->delayBuffer), 0.0000);

		 this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
	}
	catch(exception &e)
	{
		cout << "exception in Delay::load: " << e.what() << endl;
	}
#if(dbg >= 1)
	 cout << "EXITING Delay::load"  << endl;
#endif

	}

#define dbg 0

	void Delay::run()
	{
		double tempInput = 0;
		int delay = 0;

		int i;
		try
		{
			int param1 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[0]).parameterValueIndex;
			int param2 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[1]).parameterValueIndex;
			int delayCoarse = delayTimeCoarse[param1];
			int delayFine = delayTimeFine[param2];
	#if(dbg >= 2)
			 cout << "param1: " << param1;
			 cout << "delayCoarse: " << delayCoarse;
	#endif

	#if(dbg >= 2)
			 cout << "param2: " << param2;
			 cout << "delayFine: " << delayFine << endl;
	#endif

			this->delayTimeAveragingBuffer[3] = this->delayTimeAveragingBuffer[2];
			this->delayTimeAveragingBuffer[2] = this->delayTimeAveragingBuffer[1];
			this->delayTimeAveragingBuffer[1] = this->delayTimeAveragingBuffer[0];
			this->delayTimeAveragingBuffer[0] = delayFine;

			int delayFineAveraged =
							(this->delayTimeAveragingBuffer[0]
											+ this->delayTimeAveragingBuffer[1]
											+ this->delayTimeAveragingBuffer[2]
											+ this->delayTimeAveragingBuffer[3])
											/ 4;

			delay = (delayCoarse + delayFineAveraged);

			if (inputPtr >= delay)
				outputPtr = inputPtr - delay;
			else
				outputPtr = DELAY_BUFFER_LENGTH - (delay - inputPtr);

			if (inputPtr >= DELAY_BUFFER_LENGTH)
				inputPtr = 0;
			if (outputPtr >= DELAY_BUFFER_LENGTH)
				outputPtr = 0;

	#if(dbg >= 3)
			 cout << "delay: " << delay;
			 cout << "inputPtr: " << inputPtr;
			 cout << "outputPtr: " << outputPtr << endl;
	#endif

				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					for (i = 0; i < bufferSize; i++)
					{

						processSignalBufferArray->at(this->processData.outputConnectedBufferIndexArray[0]).buffer[i] =
										processSignalBufferArray->at(this->processData.inputConnectedBufferIndexArray[0]).buffer[i];
						if (inputPtr >= DELAY_BUFFER_LENGTH)
							inputPtr = 0;
						this->delayBuffer[inputPtr++] =
										0.0;
					}
				}
				else
				{
					for (i = 0; i < bufferSize; i++)
					{

						tempInput =
										processSignalBufferArray->at(this->processData.inputConnectedBufferIndexArray[0]).buffer[i];

						this->delayBuffer[inputPtr] =
										tempInput;
						processSignalBufferArray->at(this->processData.outputConnectedBufferIndexArray[0]).buffer[i] =
										this->delayBuffer[outputPtr];

						if (inputPtr >= (DELAY_BUFFER_LENGTH))
							inputPtr = 0;
						else
							inputPtr++;

						if (outputPtr >= (DELAY_BUFFER_LENGTH))
							outputPtr = 0;
						else
						{
							outputPtr++;
						}
					}
				}

		}
		catch(exception &e)
		{
			cout << "exception in Delay::run: " << e.what() << endl;
		}


	}
	void Delay::stop()
	{
		inputPtr  = 0;
		outputPtr = 0;
		delayCoarse = 0;
		delayFine = 0;
		try
		{
			std::fill(std::begin(this->delayBuffer), std::end(this->delayBuffer), 0.0000);

			this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
		}
		catch(exception &e)
		{
			cout << "exception in Delay::stop: " << e.what() << endl;
		}


	}


