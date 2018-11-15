/*
 * EnvGen.cpp
 *
 *  Created on: Nov 2, 2018
 *      Author: buildrooteclipse
 */

#include "../config.h"
#include "Controls.h"
#include "../valueArrays.h"


namespace std
{



#define dbg 0

EnvGen::EnvGen()
{
#if(dbg >= 1)
	 cout << "ENTERING EnvGen::EnvGen(Default)" << endl;
#endif
	envStage = 0;
	this->name = "EnvGen(Default)";
	stageTimeValue = 0;
	slewRate = 0.0000;
	this->envTrigger = false;
#if(dbg >= 1)
	 cout << "EXITING EnvGen::EnvGen(Default)" << endl;
#endif

}

EnvGen::EnvGen(ControlObjectData controlData)
{
#if(dbg >= 1)
	 cout << "ENTERING EnvGen::EnvGen" << endl;
#endif
	envStage = 0;
	stageTimeValue = 0;
	slewRate = 0.0000;
	this->controlData = controlData;
	this->name = this->controlData.controlName;
	this->envTrigger = false;

#if(dbg >= 1)
	 cout << "EXITING EnvGen::EnvGen: " << this->getName() << endl;
#endif

}

EnvGen::~EnvGen()
{

}
#define dbg 0
void EnvGen::load()
{
#if(dbg >= 1)
	 cout << "ENTERING EnvGen::load" << endl;
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
		cout << "exception in EnvGen::load" << e.what();

	}

#if(dbg >= 1)
	 cout << "EXITING EnvGen::load" << endl;
#endif

}


#define dbg 0
void EnvGen::run(int controlVoltageIndex, bool envTrigger)
{
	try
	{

		int  attack;
		int  decay;

		if(this->controlData.parameter[0].cvEnabled == true)
		{
			attack = controlVoltageIndex;
		}
		else
		{
			attack = this->controlData.parameter[0].value;
		}

		if(this->controlData.parameter[1].cvEnabled == true)
		{
			decay = controlVoltageIndex;
		}
		else
		{
			decay = this->controlData.parameter[1].value;
		}


		int  attackPeakValueIndex = 100;
		int  decayBottomValueIndex = 10;


		switch(envStage)
		{
		case 0: // *************** idle ***************
			this->output = 0.0;
			this->outputInv = 100.0;
			// ********************* pick detected, go to attack *********************
			if(envTrigger == true)
			{
				envStage = 1;
#if(dbg >= 3)
			 cout << "case 0: output: " << this->output << endl;
#endif
				slewRate = (1.0*attackPeakValueIndex - 1.0*decayBottomValueIndex)/(188.0*envTime[attack]);
#if(dbg >= 1)
				 cout << "CONTROLS: ATTACK after IDLE" << endl;
#endif
			}
			break;
		case 1:	//********************* attack *********************

			if(this->output < double(attackPeakValueIndex))
			{
				this->output += slewRate;
				this->outputInv -= slewRate;
#if(dbg >= 2)
			 cout << "CONTROLS: case 1: output: " << this->output << endl;
			cout << "envTime[" << attack << "]: " << envTime[attack] << endl;
#endif

			}
			// ********************* peak reached, go to decay *********************
			else if(this->output >= double(attackPeakValueIndex))
			{

				envStage = 2;
				slewRate = (attackPeakValueIndex - decayBottomValueIndex)/(188*envTime[decay]);
#if(dbg >= 1)
				 cout << "CONTROLS: DECAY after ATTACK" << endl;
#endif
			}

			// ********************* string muted, go to idle *********************
			break;
		case 2:	// ********************* decay *********************
#if(dbg >= 2)
			 cout << "CONTROLS: case 2: output: " << this->output << endl;
			cout << "envTime[" << decay << "]: " << envTime[decay] << endl;
#endif
			if(this->output > double(decayBottomValueIndex))
			{
				this->output -= slewRate;
				this->outputInv += slewRate;
			}
			else //********************* output is below decay bottom value *********************
			{
				envStage = 0;
#if(dbg >= 3)
				 cout << "CONTROLS: RELEASE after decay" << endl;
#endif
			}
			// ********************* string muted, go to idle *********************
			break;

		default:;
		}
		this->int_output  = static_cast<int>(this->output);
		this->int_outputInv  = static_cast<int>(this->outputInv);

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
		cout << "exception in EnvGen::run: " << e.what() << endl;
	}

}
void EnvGen::stop()
{

}

}
