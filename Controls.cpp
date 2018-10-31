/*
 * Controls.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "Controls.h"
#include "valueArrays.h"
#include "ControlSymbols.h"


extern bool debugOutput;
extern vector<string> controlTypeVector;
int envGenCount = 0;
int lfoCount = 0;

#define dbg 0



#define dbg 0
int normal( char action, bool envTrigger, int controlVoltageIndex, struct ControlEvent *controlEvent,
	    array<ProcessParameterControlBuffer,60> &paramContBufferArray)

{
	int targetParameterValueIndex = 0;


	if(action == 'c')
	{
		controlTypeVector.push_back(normSymbol);
	}
	else if(action == 'l')
	{
#if(dbg >= 1)
			if(debugOutput) cout << "Adding normal parameter controller" << endl;
#endif
#if(dbg >= 1)
			if(debugOutput) cout << "outputToParamControlBufferIndex: " << controlEvent->outputToParamControlBufferIndex << endl;
			if(debugOutput)cout << "paramContBufferArray:[" << src.connectedBufferIndex << "," << dest.connectedBufferIndex << "]: " << src.objectName << ":" << src.portName << "->" << dest.objectName << ":" << dest.portName << " :     " << valueIndex << endl;
#endif
	}
	else if(action == 'r')
	{
		try
		{
			if(controlEvent->parameter[0].cvEnabled == true)
			{
				controlEvent->int_output = controlVoltageIndex;  // direct transfer of parameter value
				controlEvent->int_outputInv = 99-controlVoltageIndex;  // direct transfer of parameter value
			}
			else
			{
				controlEvent->int_output = controlEvent->parameter[0].value;  // direct transfer of parameter value
				controlEvent->int_outputInv = 99-controlEvent->parameter[0].value;  // direct transfer of parameter value
			}
	#if(dbg >= 2)
			if(debugOutput) cout << "PARAM CONTROL: Normal" << endl;
	#endif

			if(controlEvent->int_output > 99)
			{
				controlEvent->int_output = 99;
			}
			if(controlEvent->int_outputInv > 99)
			{
				controlEvent->int_outputInv = 99;
			}

			for(int i = 0; i < controlEvent->outputConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_output;
			}

			for(int i = 0; i < controlEvent->outputInvConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputInvToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_outputInv;
			}

		}
		catch(std::exception &e)
		{
			cout << "normal control running error: " << e.what() << endl;
		}

	}
	else if(action == 'd')
	{

	}
	else if(action == 's')
	{

	}


	return targetParameterValueIndex;
}

#define dbg 0
int envGen( char action, bool envTrigger, int  controlVoltageIndex,  struct ControlEvent *controlEvent,
	    array<ProcessParameterControlBuffer,60> &paramContBufferArray)

{
	static EnvGenContext envGenContext[20];
	int  targetParameterValueIndex = 0;

	if(action == 'c')
	{
		controlTypeVector.push_back(envGenSymbol);
	}
	else if(action == 'l')
	{
#if(dbg >= 1)
			if(debugOutput) cout << "Adding EnvGen parameter controller: " << envGenCount << endl;
#endif
			try
			{
				controlEvent->controlTypeIndex = envGenCount;
				envGenCount++;
				envGenContext[controlEvent->controlTypeIndex].envStage = 0;

			}
			catch(std::exception &e)
			{
				cout << "env control loading error: " << e.what() << endl;
			}


	}
	else if(action == 'r')
	{
		try
		{
			if(envTrigger == true ) // ********************* pick detected *********************
			{
	#if(dbg >= 3)
				if(debugOutput) cout << "CONTROLS: envTriggerStatus: true." << endl;
	#endif
				controlEvent->envTrigger = true;
			}
			else if(envTrigger == false ) // ************** string muted ***************
			{
	#if(dbg >= 3)
				if(debugOutput) cout << "CONTROLS: envTriggerStatus: false." << endl;
	#endif
				controlEvent->envTrigger = false;
			}


			double slewRate = envGenContext[controlEvent->controlTypeIndex].slewRate;
			int  attack;
			int  decay;

			if(controlEvent->parameter[0].cvEnabled == true)
			{
				attack = controlVoltageIndex;
			}
			else
			{
				attack = controlEvent->parameter[0].value;
			}

			if(controlEvent->parameter[1].cvEnabled == true)
			{
				decay = controlVoltageIndex;
			}
			else
			{
				decay = controlEvent->parameter[1].value;
			}


			int  attackPeakValueIndex = 100;
			int  decayBottomValueIndex = 10;


			switch(envGenContext[controlEvent->controlTypeIndex].envStage)
			{
			case 0: // *************** idle ***************
				controlEvent->output = 0.0;
				controlEvent->outputInv = 100.0;
				// ********************* pick detected, go to attack *********************
				if(controlEvent->envTrigger == true)
				{
					envGenContext[controlEvent->controlTypeIndex].envStage = 1;
	#if(dbg >= 3)
				if(debugOutput) cout << "case 0: output: " << controlEvent->output << endl;
	#endif
					slewRate = (1.0*attackPeakValueIndex - 1.0*decayBottomValueIndex)/(188.0*envTime[attack]);
	#if(dbg >= 1)
					if(debugOutput) cout << "CONTROLS: ATTACK after IDLE" << endl;
	#endif
				}
				break;
			case 1:	//********************* attack *********************

				if(controlEvent->output < double(attackPeakValueIndex))
				{
					controlEvent->output += slewRate;
					controlEvent->outputInv -= slewRate;
	#if(dbg >= 2)
				if(debugOutput) cout << "CONTROLS: case 1: output: " << controlEvent->output << endl;
				cout << "envTime[" << attack << "]: " << envTime[attack] << endl;
	#endif

				}
				// ********************* peak reached, go to decay *********************
				else if(controlEvent->output >= double(attackPeakValueIndex))
				{

					envGenContext[controlEvent->controlTypeIndex].envStage = 2;
					slewRate = (attackPeakValueIndex - decayBottomValueIndex)/(188*envTime[decay]);
	#if(dbg >= 1)
					if(debugOutput) cout << "CONTROLS: DECAY after ATTACK" << endl;
	#endif
				}

				// ********************* string muted, go to idle *********************
				break;
			case 2:	// ********************* decay *********************
	#if(dbg >= 2)
				if(debugOutput) cout << "CONTROLS: case 2: output: " << controlEvent->output << endl;
				cout << "envTime[" << decay << "]: " << envTime[decay] << endl;
	#endif
				if(controlEvent->output > double(decayBottomValueIndex))
				{
					controlEvent->output -= slewRate;
					controlEvent->outputInv += slewRate;
				}
				else //********************* output is below decay bottom value *********************
				{
					envGenContext[controlEvent->controlTypeIndex].envStage = 0;
	#if(dbg >= 3)
					if(debugOutput) cout << "CONTROLS: RELEASE after decay" << endl;
	#endif
				}
				// ********************* string muted, go to idle *********************
				break;

			default:;
			}
			controlEvent->int_output  = (int)(controlEvent->output);
			controlEvent->int_outputInv  = (int)(controlEvent->outputInv);




			envGenContext[controlEvent->controlTypeIndex].slewRate = slewRate;

			if(controlEvent->int_output > 99)
			{
				controlEvent->int_output = 99;
			}
			if(controlEvent->int_outputInv > 99)
			{
				controlEvent->int_outputInv = 99;
			}

			for(int i = 0; i < controlEvent->outputConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_output;
			}

			for(int i = 0; i < controlEvent->outputInvConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputInvToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_outputInv;
			}

		}
		catch(std::exception &e)
		{
			cout << "env control running error: " << e.what() << endl;
		}

	}
	else if(action == 'd')
	{

	}
	else if(action == 's')
	{
		if(envGenCount > 0)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Removing EnvGen parameter controller: " << envGenCount << endl;
#endif
			envGenCount--;
		}
		targetParameterValueIndex = 0;
	}


	return targetParameterValueIndex;
}


#define dbg 0
int lfo(char action, bool envTrigger, int controlVoltageIndex,  struct ControlEvent *controlEvent,
	array<ProcessParameterControlBuffer,60> &paramContBufferArray)
{

	int  targetParameterValueIndex = 0;
  static LfoContext lfoContext[20];
	if(action == 'c')
	{
		controlTypeVector.push_back(lfoSymbol);
	}
	else if(action == 'l')
	{
#if(dbg >= 1)
			if(debugOutput) cout << "Adding LFO parameter controller: " << envGenCount << endl;
#endif
			try
			{
				controlEvent->controlTypeIndex = lfoCount;
				lfoCount++;
				lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = 0;

			}
			catch(std::exception &e)
			{
				cout << "lfo control loading error: " << e.what() << endl;
			}


	}
	else if(action == 'r')
	{
		try
		{

			int cycleTimeValueIndex = lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex;
			double cyclePositionValue = lfoContext[controlEvent->controlTypeIndex].cyclePositionValue;
			int int_cyclePositionValue;
			int frequencyIndex;
			int amplitudeIndex;
			int offsetIndex;

			if(controlEvent->parameter[0].cvEnabled == true)
			{
				frequencyIndex = controlVoltageIndex;
			}
			else
			{
				frequencyIndex = controlEvent->parameter[0].value;
			}

			if(controlEvent->parameter[1].cvEnabled == true)
			{
				amplitudeIndex = controlVoltageIndex;
			}
			else
			{
				amplitudeIndex = controlEvent->parameter[1].value;
			}

			if(controlEvent->parameter[2].cvEnabled == true)
			{
				offsetIndex = controlVoltageIndex;
			}
			else
			{
				offsetIndex = controlEvent->parameter[2].value;
			}
			int cyclePositionCount = 250;

			if(cyclePositionValue >= double(cyclePositionCount-1))
			{
				cyclePositionValue = 0.0;
	#if(dbg >= 1)
				if(debugOutput) cout << "Begin LFO Cycle. " << frequencyIndex << ":" << lfoFreq[frequencyIndex] << endl;
	#endif

			}
			else
			{
				cyclePositionValue += lfoFreq[frequencyIndex] * 1.4;
				int_cyclePositionValue = (int)cyclePositionValue;
				if(int_cyclePositionValue < 0) int_cyclePositionValue = 0;
				if(int_cyclePositionValue > (cyclePositionCount-1)) int_cyclePositionValue = (cyclePositionCount-1);
				controlEvent->output = lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
				controlEvent->outputInv = -lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
			}


			controlEvent->int_output  = (int)(controlEvent->output);
			controlEvent->int_outputInv  = (int)(controlEvent->outputInv);


			{
	#if(dbg >= 3)
				if(debugOutput) cout << "output: " << controlEvent->output << "\tint_output: " << controlEvent->int_output;
				if(debugOutput) cout << "frequencyIndex: " << frequencyIndex << "\tamplitudeIndex: " << amplitudeIndex << "\toffsetIndex: " << offsetIndex <<endl;
	#endif
			}
			lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = cycleTimeValueIndex;
			lfoContext[controlEvent->controlTypeIndex].cyclePositionValue = cyclePositionValue;

			if(controlEvent->int_output > 99)
			{
				controlEvent->int_output = 99;
			}
			if(controlEvent->int_outputInv > 99)
			{
				controlEvent->int_outputInv = 99;
			}


			for(int i = 0; i < controlEvent->outputConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_output;
			}

			for(int i = 0; i < controlEvent->outputInvConnectionCount; i++)
			{
				paramContBufferArray[controlEvent->outputInvToParamControlBufferIndex[i]].parameterValueIndex = controlEvent->int_outputInv;
			}

		}
		catch(std::exception &e)
		{
			cout << "lfo control running error: " << e.what() << endl;
		}

	}
	else if(action == 'd')
	{

	}
	else if(action == 's')
	{
#if(dbg >= 2)
			if(debugOutput) cout << "Removing LFO parameter controller: " << lfoCount << endl;
#endif
			lfoCount--;
	}

  return targetParameterValueIndex;
}
