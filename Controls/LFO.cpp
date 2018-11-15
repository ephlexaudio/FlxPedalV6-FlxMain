/*
 * LFO.cpp
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

LFO::LFO()
{
#if(dbg >= 1)
	 cout << "ENTERING LFO::LFO(Default)" << endl;
#endif
	cycleTimeValueIndex = 0;
	this->name = "LFO(Default)";
	cyclePositionValue = 0.000;
	int_cyclePositionValue = 0;
	waveValue = 0.000;
#if(dbg >= 1)
	 cout << "EXITING LFO::LFO(Default)" << endl;
#endif
}
LFO::LFO(ControlObjectData controlData)
{
#if(dbg >= 1)
	 cout << "ENTERING LFO::LFO" << endl;
#endif
	cycleTimeValueIndex = 0;
	cyclePositionValue = 0.000;
	int_cyclePositionValue = 0;
	waveValue = 0.000;

	this->controlData = controlData;
	this->name = this->controlData.controlName;
#if(dbg >= 1)
	 cout << "EXITING LFO::LFO: " << this->getName() << endl;
#endif

}
LFO::~LFO()
{

}
#define dbg 0
void LFO::load()
{
#if(dbg >= 1)
	 cout << "ENTERING LFO::load" << endl;
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
			cout << "exception in LFO::load:" << e.what();

		}
#if(dbg >= 1)
	 cout << "EXITING LFO::load" << endl;
#endif

}




void LFO::run(int controlVoltageIndex, bool envTrigger)
{
	try
	{

		int frequencyIndex;
		int amplitudeIndex;
		int offsetIndex;

		if(this->controlData.parameter[0].cvEnabled == true)
		{
			frequencyIndex = controlVoltageIndex;
		}
		else
		{
			frequencyIndex = this->controlData.parameter[0].value;
		}

		if(this->controlData.parameter[1].cvEnabled == true)
		{
			amplitudeIndex = controlVoltageIndex;
		}
		else
		{
			amplitudeIndex = this->controlData.parameter[1].value;
		}

		if(this->controlData.parameter[2].cvEnabled == true)
		{
			offsetIndex = controlVoltageIndex;
		}
		else
		{
			offsetIndex = this->controlData.parameter[2].value;
		}
		int cyclePositionCount = 250;

		if(cyclePositionValue >= double(cyclePositionCount-1))
		{
			cyclePositionValue = 0.0;
#if(dbg >= 1)
			 cout << "Begin LFO Cycle. " << frequencyIndex << ":" << lfoFreq[frequencyIndex] << endl;
#endif

		}
		else
		{
			cyclePositionValue += lfoFreq[frequencyIndex] * 1.4;
			int_cyclePositionValue = static_cast<int>(cyclePositionValue);
			if(int_cyclePositionValue < 0) int_cyclePositionValue = 0;
			if(int_cyclePositionValue > (cyclePositionCount-1)) int_cyclePositionValue = (cyclePositionCount-1);
			this->output = lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
			this->outputInv = -lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
		}


		this->int_output  = static_cast<int>(this->output);
		this->int_outputInv  = static_cast<int>(this->outputInv);


		{
#if(dbg >= 3)
			 cout << "output: " << this->controlData.output << "\tint_output: " << this->int_output;
			 cout << "frequencyIndex: " << frequencyIndex << "\tamplitudeIndex: " << amplitudeIndex << "\toffsetIndex: " << offsetIndex <<endl;
#endif
		}

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
		};
#if(dbg >= 2)
		cout << "LFO outputToParamControlBufferIndex size: " <<  this->outputToParamControlBufferIndex.size();
		cout << "\t\tint_output: " << this->int_output << endl;
#endif
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
		cout << "exception in LFO::run: " << e.what() << endl;
	}

}


void LFO::stop()
{

}

}
