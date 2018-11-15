/*
 * Normal.cpp
 *
 *  Created on: Nov 2, 2018
 *      Author: buildrooteclipse
 */


#include "Controls.h"

#include "../valueArrays.h"
#include "../config.h"


namespace std
{


#define dbg 0


Normal::Normal()
{
#if(dbg >= 1)
	 cout << "ENTERING Normal::Normal(Default)" << endl;
#endif
	 this->name = "Normal(Default)";
#if(dbg >= 1)
	 cout << "EXITING Normal::Normal(Default)" << endl;
#endif
}

Normal::Normal(ControlObjectData controlData)
{
#if(dbg >= 1)
	 cout << "ENTERING Normal::Normal" << endl;
#endif
	this->controlData = controlData;
	this->name = this->controlData.controlName;
#if(dbg >= 1)
	 cout << "EXITING Normal::Normal: " << this->getName() << endl;
#endif

}

Normal::~Normal()
{

}

#define dbg 0
void Normal::load()
{
#if(dbg >= 1)
	 cout << "ENTERING Normal::load" << endl;
#endif
		try
		{
			for(int controlIndex = 0; controlIndex < this->controlData.outputConnectionCount; controlIndex++)
			{
				this->outputToParamControlBufferIndex.push_back(this->controlData.outputToParamControlBufferIndex[controlIndex]);
			}
			for(int controlIndex = 0; controlIndex < this->controlData.outputInvConnectionCount; controlIndex++)
			{
				this->outputInvToParamControlBufferIndex.push_back(this->controlData.outputInvToParamControlBufferIndex[controlIndex]);
			}

		}
		catch(exception &e)
		{
			cout << "exception in Normal::load" << e.what();

		}
#if(dbg >= 1)
	 cout << "EXITING Normal::load" << endl;
#endif

}


void Normal::run(int controlVoltageIndex, bool envTrigger)
{

	try
	{
		if(this->controlData.parameter[0].cvEnabled == true)
		{
			this->int_output = controlVoltageIndex;  // direct transfer of parameter value
			this->int_outputInv = 99-controlVoltageIndex;  // direct transfer of parameter value
		}
		else
		{
			this->int_output = this->controlData.parameter[0].value;  // direct transfer of parameter value
			this->int_outputInv = 99-this->controlData.parameter[0].value;  // direct transfer of parameter value
		}
#if(dbg >= 2)
		 cout << "PARAM CONTROL: Normal" << endl;
#endif

		if(this->int_output > 99)
		{
			this->int_output = 99;
		}
		if(this->int_outputInv > 99)
		{
			this->int_outputInv = 99;
		}
		if(this->int_output < 0)
		{
			this->int_output = 0;
		}
		if(this->int_outputInv < 0)
		{
			this->int_outputInv = 0;
		}

		for(auto & bufferIndex : this->outputToParamControlBufferIndex)
		{
			processParamControlBufferArray->at(bufferIndex).parameterValueIndex = this->int_output;
		}

		for(auto & bufferIndex : this->outputInvToParamControlBufferIndex)
		{
			processParamControlBufferArray->at(bufferIndex).parameterValueIndex = this->int_outputInv;
		}

	}
	catch(std::exception &e)
	{
		cout << "exception in Normal::run: " << e.what() << endl;
	}

}


void Normal::stop()
{

}

}
