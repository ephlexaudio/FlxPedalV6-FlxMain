/*
 * Controls.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: buildrooteclipse
 */

#include "config.h"
#include "Controls.h"
#include "valueArrays.h"

extern bool debugOutput;
extern vector<string> controlTypeVector;
int envGenCount = 0;
int lfoCount = 0;

#define dbg 0

int control(/*int*/ char action, bool envTrigger, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent)
{
	int targetParameterValueIndex = 0;

#if(dbg >= 1)
	if(debugOutput) cout << "***** ENTERING: control" << endl;
	if(debugOutput) cout << "action: " << action << endl;
#endif

	static EnvGenContext envGenContext[20];
	static LfoContext lfoContext[20];

	if(action == 'c')
	{
		if(controlEvent->type == 0)
		{
			controlTypeVector.push_back(normSymbol);
		}
		else if(controlEvent->type == 1)
		{
			controlTypeVector.push_back(envGenSymbol);
		}
		else if(controlEvent->type == 2)
		{
			controlTypeVector.push_back(lfoSymbol);
		}
	}
	else if(action == 'l')
	{
		if(controlEvent->type == 0)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Adding normal parameter controller" << endl;
#endif
		}
		else if(controlEvent->type == 1)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Adding EnvGen parameter controller: " << envGenCount << endl;
#endif
			//int attack = controlEvent->parameter[0];
			//int peakValueIndex = 50;//controlEvent->parameter[4];

			/*controlEvent->controlContext = (EnvGenContext *)calloc(1, sizeof(EnvGenContext));
			if(controlEvent->controlContext == NULL)
			{
				if(debugOutput) std::cout << "EnvGenContext control calloc failed." << std::endl;
			}
			else
			{
				if(debugOutput) std::cout << "EnvGenContext control calloc succeeded." << std::endl;
				((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
			}*/
			controlEvent->controlTypeIndex = envGenCount;
			envGenCount++;
			envGenContext[controlEvent->controlTypeIndex].envStage = 0;
			envGenContext[controlEvent->controlTypeIndex].stageTimeValue = 0;
		}
		else if(controlEvent->type == 2)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Adding LFO parameter controller: " << envGenCount << endl;
#endif
			/*controlEvent->controlContext = (LfoContext *)calloc(1, sizeof(LfoContext));
			if(controlEvent->controlContext == NULL)
			{
				if(debugOutput) std::cout << "LfoContext control calloc failed." << std::endl;
			}
			else
			{
				if(debugOutput) std::cout << "LfoContext control calloc succeeded." << std::endl;
				((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = 0;
			}*/

			controlEvent->controlTypeIndex = lfoCount;
			lfoCount++;
			lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = 0;

		}
	}
	else if(action == 'r')
	{

		if(controlEvent->type == 0)
		{
			controlEvent->int_output = controlEvent->parameter[0];  // direct transfer of parameter value
			controlEvent->int_outputInv = 100-controlEvent->parameter[0];  // direct transfer of parameter value
#if(dbg >= 2)
			if(debugOutput) cout << "PARAM CONTROL: Normal" << endl;
#endif
		}
		else if(controlEvent->type == 1) // ********************* envelope generator *********************
		{
			/*if(controlEvent->controlContext == NULL)
			{
				if(debugOutput) cout << "audioCallback control context not allocated" << endl;
				status = -1;
			}*/
			if(envTrigger == true && controlEvent->envTriggerStatus == false) // ********************* pick detected *********************
			{
				//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
				//((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = 0;
				//envGenContext[controlEvent->controlTypeIndex].envStage = 0;
				envGenContext[controlEvent->controlTypeIndex].stageTimeValue = 0;
#if(dbg >= 2)
				if(debugOutput) cout << "CONTROLS: envTriggerStatus: true." << endl;
#endif
				controlEvent->envTriggerStatus = true;
			}
			else if(envTrigger == false && controlEvent->envTriggerStatus == true) // ************** string muted ***************
			{
#if(dbg >= 2)
				if(debugOutput) cout << "CONTROLS: envTriggerStatus: false." << endl;
#endif
				controlEvent->envTriggerStatus = false;
			}


			//int stageTimeValue = ((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue;
			//double slewRate = ((EnvGenContext *)(controlEvent->controlContext))->slewRate;
			int stageTimeValue = envGenContext[controlEvent->controlTypeIndex].stageTimeValue;
			double slewRate = envGenContext[controlEvent->controlTypeIndex].slewRate;
			int attack = 99-controlEvent->parameter[0];
			int decay = 99-controlEvent->parameter[1];
			//int sustain = controlEvent->parameter[2];
			//int release = controlEvent->parameter[3];
			int attackPeakValueIndex = 100;//controlEvent->parameter[4];
			int decayBottomValueIndex = 10;//controlEvent->parameter[5];
			//int internalOutput = 50*controlEvent->output;

			//switch(((EnvGenContext *)(controlEvent->controlContext))->envStage)
			switch(envGenContext[controlEvent->controlTypeIndex].envStage)
			{
			case 0: // *************** idle ***************
				controlEvent->output = 0.0;
				controlEvent->outputInv = 100.0;
				// ********************* pick detected, go to attack *********************
				if(controlEvent->envTriggerStatus == true)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage++;
					envGenContext[controlEvent->controlTypeIndex].envStage = 1;
#if(dbg >= 3)
				if(debugOutput) cout << "case 0: output: " << controlEvent->output << endl;
#endif
					slewRate = /*10.1 - */envTime[attack]*0.2;
#if(dbg >= 2)
					if(debugOutput) cout << "CONTROLS: ATTACK after IDLE" << endl;
#endif
				}
				break;
			case 1:	//********************* attack *********************

				if(controlEvent->output < double(attackPeakValueIndex))
				{
					controlEvent->output += slewRate;
					controlEvent->outputInv -= slewRate;
#if(dbg >= 3)
				if(debugOutput) cout << "CONTROLS: case 1: output: " << controlEvent->output << endl;
#endif

					//********************* another pick detected, restart attack *********************
					/*if(controlEvent->envTriggerStatus == true)
					{
						envGenContext[controlEvent->controlTypeIndex].envStage = 0;
	#if(dbg >= 2)
						if(debugOutput) cout << "CONTROLS: ATTACK after ATTACK" << endl;
	#endif
					}*/
#if(dbg >= 2)
					//if(debugOutput) cout << "CONTROLS: after 'ATTACK after ATTACK' block" << endl;
#endif
				}
				// ********************* peak reached, go to decay *********************
				else if(/*controlEvent->output >= double(attackPeakValueIndex) && */controlEvent->envTriggerStatus == false)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 2;
					envGenContext[controlEvent->controlTypeIndex].envStage = 2;
					slewRate = /*10.1 -*/ envTime[decay]*0.2;
#if(dbg >= 2)
					if(debugOutput) cout << "CONTROLS: DECAY after ATTACK" << endl;
#endif
				}

				// ********************* string muted, go to idle *********************
				/*else //if(controlEvent->envTriggerStatus == false)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
					envGenContext[controlEvent->controlTypeIndex].envStage = 0;
					//slewRate = 10.1 - envTime[sustain];
#if(dbg >= 2)
					if(debugOutput) cout << "RELEASE" << endl;
#endif
				}*/

				break;
			case 2:	// ********************* decay *********************
#if(dbg >= 3)
				if(debugOutput) cout << "CONTROLS: case 2: output: " << controlEvent->output << endl;
#endif
				// ********************* pick detected, go back to attack *********************
				if(controlEvent->envTriggerStatus == true)
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 1;
					envGenContext[controlEvent->controlTypeIndex].envStage = 0;//1;
					slewRate = /*10.1 - */envTime[attack]*0.2;
#if(dbg >= 2)
					if(debugOutput) cout << "CONTROLS: ATTACK after decay" << endl;
#endif
				}
				else if(controlEvent->output > double(decayBottomValueIndex))
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;//envStage++;
					controlEvent->output -= slewRate;
					controlEvent->outputInv += slewRate;
				}
				else //********************* output is below decay bottom value *********************
				{
					//((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;//envStage++;
					envGenContext[controlEvent->controlTypeIndex].envStage = 0;//envStage++;
#if(dbg >= 2)
					if(debugOutput) cout << "CONTROLS: RELEASE after decay" << endl;
#endif
				}
				// ********************* string muted, go to idle *********************
				/*else if(controlEvent->envTriggerStatus == false)
				{
					((EnvGenContext *)(controlEvent->controlContext))->envStage = 0;
					//slewRate = 10.1 - envTime[sustain];
#if(dbg >= 2)
					if(debugOutput) cout << "RELEASE" << endl;
#endif
				}*/
				break;

			default:;
			}
			controlEvent->int_output  = (unsigned int)(controlEvent->output);
			controlEvent->int_outputInv  = (unsigned int)(controlEvent->outputInv);

			/*if(controlEvent->int_output < 0)
			{
				controlEvent->int_output = 0;
				controlEvent->int_outputInv = 99;
			}
			if(controlEvent->int_output > 99)
			{
				controlEvent->int_output = 99;
				controlEvent->int_outputInv = 0;
			}*/



			//((EnvGenContext *)(controlEvent->controlContext))->stageTimeValue = stageTimeValue;
			//((EnvGenContext *)(controlEvent->controlContext))->slewRate = slewRate;
			envGenContext[controlEvent->controlTypeIndex].stageTimeValue = stageTimeValue;
			envGenContext[controlEvent->controlTypeIndex].slewRate = slewRate;
		}
		else if(controlEvent->type == 2) // ********************* LFO *********************
		{
			/*if(controlEvent->controlContext == NULL)
			{
				if(debugOutput) cout << "audioCallback control context not allocated" << endl;
				status = -1;
			}*/

			//unsigned int cycleTimeValueIndex = ((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex;
			//double cyclePositionValue = ((LfoContext *)(controlEvent->controlContext))->cyclePositionValue;
			unsigned int cycleTimeValueIndex = lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex;
			double cyclePositionValue = lfoContext[controlEvent->controlTypeIndex].cyclePositionValue;
			unsigned int int_cyclePositionValue;
			unsigned int frequencyIndex = controlEvent->parameter[0];
			unsigned int amplitudeIndex = controlEvent->parameter[1];
			unsigned int offsetIndex = controlEvent->parameter[2];
			unsigned int cyclePositionCount = 500;
			//double waveValue;
			//if(debugOutput) cout << "output: " << controlEvent->output << endl;

			if(cyclePositionValue >= double(cyclePositionCount-1))
			{
				cyclePositionValue = 0.0;
			}
			else
			{
				cyclePositionValue += lfoFreq[frequencyIndex];
				int_cyclePositionValue = (unsigned int)cyclePositionValue;
				if(int_cyclePositionValue < 0) int_cyclePositionValue = 0;
				if(int_cyclePositionValue > (cyclePositionCount-1)) int_cyclePositionValue = (cyclePositionCount-1);
				controlEvent->output = lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
				controlEvent->outputInv = -lfoAmp[amplitudeIndex]*lfoSine[int_cyclePositionValue]+lfoOffset[offsetIndex];
			}


			controlEvent->int_output  = (unsigned int)(controlEvent->output);
			controlEvent->int_outputInv  = (unsigned int)(controlEvent->outputInv);

			/*if(controlEvent->int_output > 99)
			{
				controlEvent->int_output = 99;
			}
			if(controlEvent->int_outputInv > 99)
			{
				controlEvent->int_outputInv = 99;
			}*/

			{
#if(dbg >= 2)
				//if(debugOutput) cout << "PARAM CONTROL[" << controlEvent->name << "]: LFO: value index " << controlEvent->int_output << endl;
				if(debugOutput) cout << "output: " << controlEvent->output << "\tint_output: " << controlEvent->int_output << endl;
#endif
			}
			//((LfoContext *)(controlEvent->controlContext))->cycleTimeValueIndex = cycleTimeValueIndex;
			//((LfoContext *)(controlEvent->controlContext))->cyclePositionValue = cyclePositionValue;
			lfoContext[controlEvent->controlTypeIndex].cycleTimeValueIndex = cycleTimeValueIndex;
			lfoContext[controlEvent->controlTypeIndex].cyclePositionValue = cyclePositionValue;
		}


		if(controlEvent->int_output > 99)
		{
			controlEvent->int_output = 99;
		}
		if(controlEvent->int_outputInv > 99)
		{
			controlEvent->int_outputInv = 99;
		}

		for(int paramControlConnectionIndex = 0; paramControlConnectionIndex < controlEvent->paramContConnectionCount; paramControlConnectionIndex++)
		{
			int paramContProcessIndex = controlEvent->paramContConnection[paramControlConnectionIndex].processIndex;
			int paramContParameterIndex = controlEvent->paramContConnection[paramControlConnectionIndex].processParamIndex;

			procEvent[paramContProcessIndex].parameters[paramContParameterIndex] = controlEvent->int_output;
			/*if(debugOutput) cout << "procEvent[" << paramContProcessIndex << "].parameters[" << paramContParameterIndex << "]: " <<
					procEvent[paramContProcessIndex].parameters[paramContParameterIndex] << endl;*/
		}

		for(int paramControlConnectionIndexInv = 0; paramControlConnectionIndexInv < controlEvent->paramContConnectionCountInv; paramControlConnectionIndexInv++)
		{
			int paramContProcessIndex = controlEvent->paramContConnectionInv[paramControlConnectionIndexInv].processIndex;
			int paramContParameterIndex = controlEvent->paramContConnectionInv[paramControlConnectionIndexInv].processParamIndex;

			procEvent[paramContProcessIndex].parameters[paramContParameterIndex] = controlEvent->int_outputInv;
			/*if(debugOutput) cout << "procEvent[" << paramContProcessIndex << "].parameters[" << paramContParameterIndex << "]: " <<
					procEvent[paramContProcessIndex].parameters[paramContParameterIndex] << endl;*/
		}

	}
	else if(action == 'd')
	{

	}
	else if(action == 's')
	{

		if(controlEvent->type == 1 || controlEvent->type == 2)
		{
			/*if(controlEvent->controlContext == NULL || controlEvent == NULL)
			{
				if(debugOutput) std::cout << "controlContext missing." << std::endl;
			}
			else
			{
				if(debugOutput) std::cout << "freeing allocated controlContext." << std::endl;
				free(controlEvent->controlContext);
			}*/
		}
		if(controlEvent->type == 1 && envGenCount > 0)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Removing EnvGen parameter controller: " << envGenCount << endl;
#endif
			envGenCount--;
		}
		if(controlEvent->type == 2 && lfoCount > 0)
		{
#if(dbg >= 2)
			if(debugOutput) cout << "Removing LFO parameter controller: " << lfoCount << endl;
#endif
			lfoCount--;
		}
		targetParameterValueIndex = 0;

	}
	else
	{

	}

#if(dbg >= 1)
	if(debugOutput) cout << "***** EXITING: control: " << targetParameterValueIndex << endl;
#endif

	return targetParameterValueIndex;
}
