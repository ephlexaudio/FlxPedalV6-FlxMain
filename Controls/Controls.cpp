/*
 * Controls.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: buildrooteclipse
 */

#include "Controls.h"


#include "../valueArrays.h"

#include "../structs.h"
#include "../config.h"




using namespace std;


Controls::Controls()
{
#if(dbg >= 1)
	 cout << "ENTERING Controls::Controls(Default)" << endl;
#endif
	
	this->processParamControlBufferArray = NULL;
	this->output = 0.0;
	this->outputInv = 0.0;
	this->int_output = 0;
	this->int_outputInv = 0;

#if(dbg >= 1)
	 cout << "EXITING Controls::Controls(Default)" << endl;
#endif
}

Controls::Controls(ControlObjectData controlData)
{
#if(dbg >= 1)
	 cout << "ENTERING Controls::Controls" << endl;
#endif
	this->output = 0.0;
	this->outputInv = 0.0;
	this->int_output = 0;
	this->int_outputInv = 0;
	
	this->controlData = controlData;
	this->name = this->controlData.controlName;

	this->processParamControlBufferArray = NULL;
	this->outputToParamControlBufferIndex.clear();//push_back(0);
#if(dbg >= 1)
	 cout << "EXITING Controls::Controls" << endl;
#endif
}
Controls::~Controls()
{

}

string Controls::getName()
{
	return this->name;
}

void Controls::setParameter(int paramIndex, int valueIndex)
{
#if(dbg >= 1)
	 cout << "ENTERING Controls::setParameter" << endl;
#endif
	cout << "setting parameter[" << paramIndex << "]: " << valueIndex << endl;
	this->controlData.parameter[paramIndex].value = valueIndex;
#if(dbg >= 1)
	 cout << "EXITING Controls::setParameter: " << paramIndex << ":" << valueIndex << endl;
#endif
}

int Controls::getParameter(int paramIndex)
{
	return this->controlData.parameter[paramIndex].value;
}

string Controls::getParameterName(int paramIndex)
{
	string paramName = this->controlData.parameter[paramIndex].parameterName;
	return paramName;
}


void Controls::setProcessParameterControlBufferArray(array<ProcessParameterControlBuffer,60> &procParamControlBufferArray)
{
#if(dbg >= 1)
	 cout << "ENTERING Controls::setProcessParameterControlBufferArray" << endl;
#endif
	this->processParamControlBufferArray = &procParamControlBufferArray;
#if(dbg >= 1)
	 cout << "EXITING Controls::setProcessParameterControlBufferArray: " << endl;
#endif

}




