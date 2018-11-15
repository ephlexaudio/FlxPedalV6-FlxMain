/*
 * Mixer.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: buildrooteclipse
 */


 #include "Processes.h"


#include "../valueArrays.h"
#include "../config.h"

namespace std
{

#define dbg 0
 Mixer::Mixer()
 {
#if(dbg >= 1)
	 cout << "ENTERING Mixer::Mixer(Default)" << endl;
#endif

		 status = 0;
		 bufferSize = 256;

		 inputBufferIndex[0] = 0;
		 inputBufferIndex[1] = 0;
		 inputBufferIndex[2] = 0;
		 outputBufferIndex = 0;
		 this->name = "Mixer(Default)";
		 this->paramControllerBufferIndex[0] = 0;
		 this->paramControllerBufferIndex[1] = 0;
		 this->paramControllerBufferIndex[2] = 0;
		 this->paramControllerBufferIndex[3] = 0;
#if(dbg >= 1)
	 cout << "EXITING Mixer::Mixer(Default)"  << endl;
#endif

 }

 Mixer::Mixer(ProcessObjectData processData)
 {
#if(dbg >= 1)
	 cout << "ENTERING Mixer::Mixer" << endl;
#endif
	 status = 0;
	 bufferSize = 256;
	 this->processData = processData;
	 this->name = this->processData.processName;
	 inputBufferIndex[0] = this->processData.inputConnectedBufferIndexArray[0];
	 inputBufferIndex[1] = this->processData.inputConnectedBufferIndexArray[1];
	 inputBufferIndex[2] = this->processData.inputConnectedBufferIndexArray[2];
	 outputBufferIndex = this->processData.outputConnectedBufferIndexArray[0];
	 this->paramControllerBufferIndex[0] = this->processData.parameterArray[0].paramContBufferIndex;
	 this->paramControllerBufferIndex[1] = this->processData.parameterArray[1].paramContBufferIndex;
	 this->paramControllerBufferIndex[2] = this->processData.parameterArray[2].paramContBufferIndex;
	 this->paramControllerBufferIndex[3] = this->processData.parameterArray[3].paramContBufferIndex;
#if(dbg >= 1)
	 cout << "EXITING Mixer::Mixer: " << this->getName() << endl;
#endif

 }

 Mixer::~Mixer()
 {

 }

#define dbg 0
 void Mixer::load()
 {
#if(dbg >= 1)
	 cout << "ENTERING Mixer::load" << endl;
#endif
		try
		{
			this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
		}
		catch(exception &e)
		{
			cout << "exception in Mixer::load: " << e.what() << endl;
		}



#if(dbg >= 1)
	 cout << "EXITING Mixer::load" << endl;
#endif

 }

#define dbg 0
 void Mixer::run()
 {
	 unsigned int i = 0;
		try
		{
			int param1 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[0]).parameterValueIndex;
			int param2 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[1]).parameterValueIndex;
			int param3 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[2]).parameterValueIndex;
			int param4 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[3]).parameterValueIndex;


			double level1 = logAmp[param1];
			double level2 = logAmp[param2];
			double level3 = logAmp[param3];
			double levelOut = logAmp[param4];


			if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
			{
				for (i = 0; i < bufferSize; i++)
				{
					this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
									this->processSignalBufferArray->at(this->inputBufferIndex[0]).buffer[i];
				}
			}
			else
			{
				for (i = 0; i < bufferSize; i++)
				{
					this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
									levelOut * (level1 * (this->processSignalBufferArray->at(this->inputBufferIndex[0]).buffer[i])
									+ level2 * (this->processSignalBufferArray->at(this->inputBufferIndex[1]).buffer[i])
									+ level3 * (this->processSignalBufferArray->at(this->inputBufferIndex[2]).buffer[i]));
				}
			}

		}
		catch(exception &e)
		{
			cout << "exception in Mixer::run: " << e.what() << endl;
		}
		status = 0;
 }
 void Mixer::stop()
 {
		try
		{
			this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex));
		}
		catch(exception &e)
		{
			cout << "exception in Mixer::stop: " << e.what() << endl;
		}
 }

}
