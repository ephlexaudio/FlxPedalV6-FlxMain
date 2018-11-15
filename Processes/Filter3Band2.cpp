/*
 * Filter3Band2.cpp
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

Filter3Band2::Filter3Band2():lp_y {0.000,0.000,0.000},
lp_x {0.000,0.000,0.000},bp_y {0.000,0.000,0.000,0.000,0.000},
bp_x {0.000,0.000,0.000,0.000,0.000},hp_y {0.000,0.000,0.000},
hp_x {0.000,0.000,0.000},couplingFilter_y {0.000,0.000,0.000},
couplingFilter_x {0.000,0.000,0.000},rolloffFilter_y {0.000,0.000,0.000},
rolloffFilter_x {0.000,0.000,0.000},lp_b {0.000,0.000,0.000},lp_a {0.000,0.000,0.000},
bp_b {0.000,0.000,0.000,0.000,0.000},bp_a {0.000,0.000,0.000,0.000,0.000},
hp_b {0.000,0.000,0.000},hp_a {0.000,0.000,0.000},couplingFilter_b {0.000,0.000,0.000},
couplingFilter_a {0.000,0.000,0.000},rolloffFilter_b {0.000,0.000,0.000},
rolloffFilter_a {0.000,0.000,0.000}
{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band2::Filter3Band2(Default)" << endl;
#endif

	status = 0;
	bufferSize = 256;
	inputCouplingMode = 1;

	this->inputBufferIndex = 0;
	this->outputBufferIndex[0] = 0;
	this->outputBufferIndex[1] = 0;
	this->outputBufferIndex[2] = 0;
	this->paramControllerBufferIndex[0] = 0;
	this->paramControllerBufferIndex[1] = 0;
#if(dbg >= 1)
	 cout << "EXITING Filter3Band2::Filter3Band2(Default)"  << endl;
#endif

}
Filter3Band2::Filter3Band2(ProcessObjectData processData):lp_y {0.000,0.000,0.000},
lp_x {0.000,0.000,0.000},bp_y {0.000,0.000,0.000,0.000,0.000},
bp_x {0.000,0.000,0.000,0.000,0.000},hp_y {0.000,0.000,0.000},
hp_x {0.000,0.000,0.000},couplingFilter_y {0.000,0.000,0.000},
couplingFilter_x {0.000,0.000,0.000},rolloffFilter_y {0.000,0.000,0.000},
rolloffFilter_x {0.000,0.000,0.000},lp_b {0.000,0.000,0.000},lp_a {0.000,0.000,0.000},
bp_b {0.000,0.000,0.000,0.000,0.000},bp_a {0.000,0.000,0.000,0.000,0.000},
hp_b {0.000,0.000,0.000},hp_a {0.000,0.000,0.000},couplingFilter_b {0.000,0.000,0.000},
couplingFilter_a {0.000,0.000,0.000},rolloffFilter_b {0.000,0.000,0.000},
rolloffFilter_a {0.000,0.000,0.000}
{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band2::Filter3Band2" << endl;
#endif
	status = 0;
	bufferSize = 256;
	inputCouplingMode = 1;
	this->processData = processData;

	this->inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
	this->outputBufferIndex[0] = this->processData.outputConnectedBufferIndexArray[0];
	this->outputBufferIndex[1] = this->processData.outputConnectedBufferIndexArray[1];
	this->outputBufferIndex[2] = this->processData.outputConnectedBufferIndexArray[2];
	this->paramControllerBufferIndex[0] = this->processData.parameterArray[0].paramContBufferIndex;
	this->paramControllerBufferIndex[1] = this->processData.parameterArray[1].paramContBufferIndex;
#if(dbg >= 1)
	 cout << "EXITING Filter3Band2::Filter3Band2"  << endl;
#endif
}

Filter3Band2::~Filter3Band2()
{

}

#define dbg 0
void Filter3Band2::load()
{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band2::load" << endl;
#endif
		try
		{
			std::fill(std::begin(lp_y), std::end(lp_y), 0.0000);
			std::fill(std::begin(lp_x), std::end(lp_x), 0.0000);
			std::fill(std::begin(bp_y), std::end(bp_y), 0.0000);
			std::fill(std::begin(bp_x), std::end(bp_x), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
			std::fill(std::begin(rolloffFilter_y), std::end(rolloffFilter_y), 0.0000);
			std::fill(std::begin(rolloffFilter_x), std::end(rolloffFilter_x), 0.0000);


			int coefIndex = 0;
			for(auto & coef : couplingFilter)
			{
				if(coefIndex < 3) couplingFilter_b[coefIndex] = coef;
				else couplingFilter_a[coefIndex - 3] = coef;
				coefIndex++;
			}

			for (int signalBufferIndex = 0; signalBufferIndex < this->processData.processOutputCount; signalBufferIndex++)
			{
				this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex[signalBufferIndex]));
			}
		}
		catch(exception &e)
		{
			cout << "exception in Filter3Band2::load: " << e.what() << endl;
		}

#if(dbg >= 1)
	 cout << "EXITING Filter3Band2::load"  << endl;
#endif

}
#define dbg 0
void Filter3Band2::run()
{
	int i = 0;
	try
	{
		int param1 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[0]).parameterValueIndex;
		int param2 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[1]).parameterValueIndex;
		double tempInput;

		lp_b[0] = lp2[param1][0];
		lp_b[1] = lp2[param1][1];
		lp_b[2] = lp2[param1][2];
		lp_a[1] = lp2[param1][4];
		lp_a[2] = lp2[param1][5];

		bp_b[0] = bp2[param1][0];
		bp_b[1] = bp2[param1][1];
		bp_b[2] = bp2[param1][2];
		bp_b[3] = bp2[param1][3];
		bp_b[4] = bp2[param1][4];
		bp_a[1] = bp2[param1][6];
		bp_a[2] = bp2[param1][7];
		bp_a[3] = bp2[param1][8];
		bp_a[4] = bp2[param1][9];

		hp_b[0] = hp2[param1][0];
		hp_b[1] = hp2[param1][1];
		hp_b[2] = hp2[param1][2];
		hp_a[1] = hp2[param1][4];
		hp_a[2] = hp2[param1][5];

		rolloffFilter_b[0] = lp2[param2][0];
		rolloffFilter_b[1] = lp2[param2][1];
		rolloffFilter_b[2] = lp2[param2][2];
		rolloffFilter_a[1] = lp2[param2][4];
		rolloffFilter_a[2] = lp2[param2][5];

		if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
		{
			for (i = 0; i < bufferSize; i++)
			{
				processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] =
								processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
				processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] = 0.0000;
				processSignalBufferArray->at(this->outputBufferIndex[2]).buffer[i] = 0.0000;
			}
		}
		else
		{
			for (i = 0; i < bufferSize; i++)
			{
				tempInput = 0;
				if (inputCouplingMode == 0)
				{
					tempInput = processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
				}
				else if (inputCouplingMode == 1)
				{
					couplingFilter_x[0] = processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
					couplingFilter_y[0] = couplingFilter_b[0] * couplingFilter_x[0]\
									+ couplingFilter_b[1] * couplingFilter_x[1]\
									+ couplingFilter_b[2] * couplingFilter_x[2]\
									- couplingFilter_a[1] * couplingFilter_y[1]\
									- couplingFilter_a[2] * couplingFilter_y[2];

					tempInput = couplingFilter_y[0];

					couplingFilter_x[2] = couplingFilter_x[1];
					couplingFilter_x[1] = couplingFilter_x[0];

					couplingFilter_y[2] = couplingFilter_y[1];
					couplingFilter_y[1] = couplingFilter_y[0];
				}

				lp_x[0] = tempInput;
				bp_x[0] = tempInput;
				hp_x[0] = tempInput;
				//*********************Band Divider (BD): Lowpass filter ********************

				lp_y[0] = lp_b[0] * lp_x[0] + lp_b[1] * lp_x[1] + lp_b[2] * lp_x[2]\
										 - lp_a[1] * lp_y[1] - lp_a[2] * lp_y[2];

				lp_x[2] = lp_x[1];
				lp_x[1] = lp_x[0];
				lp_y[2] = lp_y[1];
				lp_y[1] = lp_y[0];



				processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] = lp_y[0];

				//*********************** Bandpass filter ****************************

				bp_y[0] = bp_b[0] * bp_x[0] + bp_b[1] * bp_x[1] + bp_b[2] * bp_x[2] + bp_b[3] * bp_x[3] + bp_b[4] * bp_x[4]\
								- bp_a[1] * bp_y[1] - bp_a[2] * bp_y[2] - bp_a[3] * bp_y[3] - bp_a[4] * bp_y[4];

				bp_x[4] = bp_x[3];
				bp_x[3] = bp_x[2];
				bp_x[2] = bp_x[1];
				bp_x[1] = bp_x[0];

				bp_y[4] = bp_y[3];
				bp_y[3] = bp_y[2];
				bp_y[2] = bp_y[1];
				bp_y[1] = bp_y[0];

				processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] = bp_y[0];

				//*********************Band Divider (BD): Highpass filter ********************

				hp_y[0] = hp_b[0] * hp_x[0] + hp_b[1] * hp_x[1] + hp_b[2] * hp_x[2]\
								- hp_a[1] * hp_y[1] - hp_a[2] * hp_y[2];

				hp_x[2] = hp_x[1];
				hp_x[1] = hp_x[0];
				hp_y[2] = hp_y[1];
				hp_y[1] = hp_y[0];


				rolloffFilter_x[0] = hp_y[0];
				rolloffFilter_y[0] = rolloffFilter_b[0] * rolloffFilter_x[0]\
								+ rolloffFilter_b[1] * rolloffFilter_x[1]\
								+ rolloffFilter_b[2] * rolloffFilter_x[2]\
								- rolloffFilter_a[1] * rolloffFilter_y[1]\
								- rolloffFilter_a[2] * rolloffFilter_y[2];

				processSignalBufferArray->at(this->outputBufferIndex[2]).buffer[i] = rolloffFilter_y[0];

				rolloffFilter_x[2] = rolloffFilter_x[1];
				rolloffFilter_x[1] = rolloffFilter_x[0];

				rolloffFilter_y[2] = rolloffFilter_y[1];
				rolloffFilter_y[1] = rolloffFilter_y[0];
			}
		}
	}
	catch(exception &e)
	{
		cout << "exception in Filter3Band2::run: " << e.what() << endl;
	}


	status = 0;
}

void Filter3Band2::stop()
{
	try
	{
		std::fill(std::begin(lp_y), std::end(lp_y), 0.0000);
		std::fill(std::begin(lp_x), std::end(lp_x), 0.0000);
		std::fill(std::begin(bp_y), std::end(bp_y), 0.0000);
		std::fill(std::begin(bp_x), std::end(bp_x), 0.0000);
		std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
		std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
		std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
		std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
		std::fill(std::begin(rolloffFilter_y), std::end(rolloffFilter_y), 0.0000);
		std::fill(std::begin(rolloffFilter_x), std::end(rolloffFilter_x), 0.0000);


		for (int signalBufferIndex = 0; signalBufferIndex < this->processData.processOutputCount; signalBufferIndex++)
		{
			this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex[signalBufferIndex]));
		}
	}
	catch(exception &e)
	{
		cout << "exception in Filter3Band2::stop: " << e.what() << endl;
	}
}


}
