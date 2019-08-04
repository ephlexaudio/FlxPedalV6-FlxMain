/*
 * Volume.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: buildrooteclipse
 */


#include "Processes.h"

#include "../config.h"
#include "../valueArrays.h"

using namespace std;


#define dbg 0
Volume::Volume()
{
#if(dbg >= 1)
	 cout << "ENTERING Volume::Volume(Default)" << endl;
#endif

	status = 0;
	bufferSize = 256;
	this->name = "Volume(Default)";
	inputBufferIndex = 0;
	outputBufferIndex = 0;
	paramControllerBufferIndex = 0;
#if(dbg >= 1)
	 cout << "EXITING Volume::Volume(Default)"  << endl;
#endif

}

Volume::Volume(ProcessObjectData processData)
{
	status = 0;
#if(dbg >= 1)
	 cout << "ENTERING Volume::Volume" << endl;
#endif

	bufferSize = 256;
	this->processData = processData;
	this->name = this->processData.processName;
	inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
	outputBufferIndex = this->processData.outputConnectedBufferIndexArray[0];
	paramControllerBufferIndex = this->processData.parameterArray[0].paramContBufferIndex;

#if(dbg >= 1)
	 cout << "EXITING Volume::Volume: " << this->getName() << endl;
#endif

}

Volume::~Volume()
{

}

#define dbg 0
void Volume::load()
{
#if(dbg >= 1)
	 cout << "ENTERING Volume::load" << endl;
#endif

		try
		{
			this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
		}
		catch(exception &e)
		{
			cout << "exception in Volume::load: " << e.what() << endl;
		}




#if(dbg >= 1)
	 cout << "EXITING Volume::load" << endl;
#endif

}

#define dbg 0
void Volume::run()
{
	int i = 0;
	try
	{
		int param1 = this->processParamControlBufferArray->at(paramControllerBufferIndex).parameterValueIndex;
			double vol = logAmp[param1];

				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					for (i = 0; i < bufferSize; i++)
					{
						this->processSignalBufferArray->at(outputBufferIndex).buffer[i] =
										this->processSignalBufferArray->at(inputBufferIndex).buffer[i];
					}
				}
				else
				{
					for (i = 0; i < bufferSize; i++)
					{
						this->processSignalBufferArray->at(outputBufferIndex).buffer[i] =
										vol * (this->processSignalBufferArray->at(inputBufferIndex).buffer[i]);
					}
				}
	}
	catch(exception &e)
	{
		cout << "exception in Volume::run: " << e.what() << endl;
	}
		status = 0;
}
void Volume::stop()
{
	try
	{
		this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
	}
	catch(exception &e)
	{
		cout << "exception in Volume::stop: " << e.what() << endl;
	}
}

