/*
 * Filter3Band.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: buildrooteclipse
 */

#include "Processes.h"


#include "../valueArrays.h"
#include "../config.h"
//extern bool debugOutput;


namespace std
{
#define dbg 0

	Filter3Band::Filter3Band():lp_y {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	lp_x {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	hp_y {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	hp_x {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	couplingFilter_y {0.000,0.000,0.000},
	couplingFilter_x {0.000,0.000,0.000},
	rolloffFilter_y {0.000,0.000,0.000},
	rolloffFilter_x {0.000,0.000,0.000},
	 lp_b {0.000,0.000,0.000},
	 lp_a {0.000,0.000,0.000},
	 hp_b {0.000,0.000,0.000},
	 hp_a {0.000,0.000,0.000},
	 couplingFilter_b {0.000,0.000,0.000},
	 couplingFilter_a {0.000,0.000,0.000},
	 rolloffFilter_b {0.000,0.000,0.000},
	 rolloffFilter_a {0.000,0.000,0.000}

	{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band::Filter3Band(Default)" << endl;
#endif

		 status = 0;
		 bufferSize = 256;
		 inputCouplingMode = 1;
		 this->name = "Filter3Band(Default)";
		this->inputBufferIndex = 0;
		this->outputBufferIndex[0] = 0;
		this->outputBufferIndex[1] = 0;
		this->outputBufferIndex[2] = 0;
		this->paramControllerBufferIndex[0] = 0;
		this->paramControllerBufferIndex[1] = 0;
		this->paramControllerBufferIndex[2] = 0;
#if(dbg >= 1)
	 cout << "EXITING Filter3Band::Filter3Band(Default)"  << endl;
#endif

	}

	Filter3Band::Filter3Band(ProcessObjectData processData): lp_y {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	lp_x {{0.000,0.000,0.000},{0.000,0.000,0.000}},hp_y {{0.000,0.000,0.000},{0.000,0.000,0.000}},
	hp_x {{0.000,0.000,0.000},{0.000,0.000,0.000}},couplingFilter_y {0.000,0.000,0.000},
	couplingFilter_x {0.000,0.000,0.000},rolloffFilter_y {0.000,0.000,0.000},
	rolloffFilter_x {0.000,0.000,0.000},lp_b {0.000,0.000,0.000},lp_a {0.000,0.000,0.000},
	hp_b {0.000,0.000,0.000},hp_a {0.000,0.000,0.000},couplingFilter_b {0.000,0.000,0.000},
	couplingFilter_a {0.000,0.000,0.000},rolloffFilter_b {0.000,0.000,0.000},
	rolloffFilter_a {0.000,0.000,0.000}
	{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band::Filter3Band" << endl;
#endif
		status = 0;
		bufferSize = 256;
		inputCouplingMode = 1;
		this->processData = processData;
		this->name = this->processData.processName;
		this->inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
		this->outputBufferIndex[0] = this->processData.outputConnectedBufferIndexArray[0];
		this->outputBufferIndex[1] = this->processData.outputConnectedBufferIndexArray[1];
		this->outputBufferIndex[2] = this->processData.outputConnectedBufferIndexArray[2];

		this->paramControllerBufferIndex[0] = this->processData.parameterArray[0].paramContBufferIndex;
		this->paramControllerBufferIndex[1] = this->processData.parameterArray[1].paramContBufferIndex;
		this->paramControllerBufferIndex[2] = this->processData.parameterArray[2].paramContBufferIndex;
#if(dbg >= 1)
	 cout << "EXITING Filter3Band::Filter3Band: " << this->getName() << endl;
#endif

	}

	Filter3Band::~Filter3Band()
	{

	}


#define dbg 0
	void Filter3Band::load()
	{
#if(dbg >= 1)
	 cout << "ENTERING Filter3Band::load" << endl;
#endif
		try
		{
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
			std::fill(std::begin(rolloffFilter_y), std::end(rolloffFilter_y), 0.0000);
			std::fill(std::begin(rolloffFilter_x), std::end(rolloffFilter_x), 0.0000);
			std::fill(std::begin(lp_y[0]), std::end(lp_y[0]), 0.0000);
			std::fill(std::begin(lp_x[0]), std::end(lp_x[0]), 0.0000);
			std::fill(std::begin(lp_y[1]), std::end(lp_y[1]), 0.0000);
			std::fill(std::begin(lp_x[1]), std::end(lp_x[1]), 0.0000);
			std::fill(std::begin(hp_y[0]), std::end(hp_y[0]), 0.0000);
			std::fill(std::begin(hp_y[0]), std::end(hp_y[0]), 0.0000);
			std::fill(std::begin(hp_y[1]), std::end(hp_y[1]), 0.0000);
			std::fill(std::begin(hp_y[1]), std::end(hp_y[1]), 0.0000);


				int coefIndex = 0;
				for(auto & coef : couplingFilter)
				{
					if(coefIndex < 3) couplingFilter_b[coefIndex] = coef;
					else couplingFilter_a[coefIndex - 3] = coef;
					coefIndex++;
				}

		}
		catch(exception &e)
		{
			cout << "exception in Filter3Band::load: " << e.what() << endl;
		}




#if(dbg >= 1)
	 cout << "EXITING Filter3Band::load"  << endl;
#endif

	}
#define dbg 0
	void Filter3Band::run()
	{
		try
		{
			int lowMidIndex = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[0]).parameterValueIndex;
				int midHighIndex = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[1]).parameterValueIndex;
				int RolloffIndex = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[2]).parameterValueIndex;


				double tempInput = 0.000;


				lp_b[0][0] = lp[lowMidIndex][0];
				lp_b[0][1] = lp[lowMidIndex][1];
				lp_b[0][2] = lp[lowMidIndex][2];
				lp_a[0][1] = lp[lowMidIndex][4];
				lp_a[0][2] = lp[lowMidIndex][5];

				hp_b[0][0] = hp[lowMidIndex][0];
				hp_b[0][1] = hp[lowMidIndex][1];
				hp_b[0][2] = hp[lowMidIndex][2];
				hp_a[0][1] = hp[lowMidIndex][4];
				hp_a[0][2] = hp[lowMidIndex][5];

				lp_b[1][0] = lp[midHighIndex][0];
				lp_b[1][1] = lp[midHighIndex][1];
				lp_b[1][2] = lp[midHighIndex][2];
				lp_a[1][1] = lp[midHighIndex][4];
				lp_a[1][2] = lp[midHighIndex][5];

				hp_b[1][0] = hp[midHighIndex][0];
				hp_b[1][1] = hp[midHighIndex][1];
				hp_b[1][2] = hp[midHighIndex][2];
				hp_a[1][1] = hp[midHighIndex][4];
				hp_a[1][2] = hp[midHighIndex][5];

				rolloffFilter_b[0] = lp[RolloffIndex][0];
				rolloffFilter_b[1] = lp[RolloffIndex][1];
				rolloffFilter_b[2] = lp[RolloffIndex][2];
				rolloffFilter_a[1] = lp[RolloffIndex][4];
				rolloffFilter_a[2] = lp[RolloffIndex][5];

				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					for (int i = 0; i < bufferSize; i++)
					{
						this->processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] =
										this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
						this->processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] = 0.0000;
						this->processSignalBufferArray->at(this->outputBufferIndex[2]).buffer[i] = 0.0000;
					}
				}
				else
				{
					for (int i = 0; i < bufferSize; i++)
					{
						if (inputCouplingMode == 0)
						{
							tempInput = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
						}
						else if (inputCouplingMode == 1)
						{
							couplingFilter_x[0] = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
							couplingFilter_y[0] = couplingFilter_b[0] * couplingFilter_x[0] + couplingFilter_b[1] * couplingFilter_x[1] + couplingFilter_b[2] * couplingFilter_x[2]
																			   - couplingFilter_a[1] * couplingFilter_y[1] - couplingFilter_a[2] * couplingFilter_y[2];

							tempInput = couplingFilter_y[0];

							couplingFilter_x[2] = couplingFilter_x[1];
							couplingFilter_x[1] = couplingFilter_x[0];

							couplingFilter_y[2] = couplingFilter_y[1];
							couplingFilter_y[1] = couplingFilter_y[0];
						}

						//************************** FIRST BAND DIVIDER (LOW/MID) *************************


						lp_x[0][0] = tempInput;
						hp_x[0][0] = tempInput;

						lp_y[0][0] = lp_b[0][0] * lp_x[0][0] + lp_b[0][1] * lp_x[0][1] + lp_b[0][2] * lp_x[0][2]
										- lp_a[0][1] * lp_y[0][1] - lp_a[0][2] * lp_y[0][2];

						this->processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] =
										lp_y[0][0];


						lp_x[0][2] = lp_x[0][1];
						lp_x[0][1] = lp_x[0][0];

						lp_y[0][2] = lp_y[0][1];
						lp_y[0][1] = lp_y[0][0];




						hp_y[0][0] = hp_b[0][0] * hp_x[0][0] + hp_b[0][1] * hp_x[0][1] + hp_b[0][2] * hp_x[0][2]
										- hp_a[0][1] * hp_y[0][1] - hp_a[0][2] * hp_y[0][2];



						hp_x[0][2] = hp_x[0][1];
						hp_x[0][1] = hp_x[0][0];

						hp_y[0][2] = hp_y[0][1];
						hp_y[0][1] = hp_y[0][0];


						//************************** SECOND BAND DIVIDER (MID/HIGH) *************************
						lp_x[1][0] = hp_y[0][0];

						lp_y[1][0] = lp_b[1][0] * lp_x[1][0] + lp_b[1][1] * lp_x[1][1] + lp_b[1][2] * lp_x[1][2]
										- lp_a[1][1] * lp_y[1][1] - lp_a[1][2] * lp_y[1][2];

						lp_x[1][2] = lp_x[1][1];
						lp_x[1][1] = lp_x[1][0];

						lp_y[1][2] = lp_y[1][1];
						lp_y[1][1] = lp_y[1][0];

						this->processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] =
										lp_y[1][0];

						hp_x[1][0] = tempInput;

						hp_y[1][0] = hp_b[1][0] * hp_x[1][0]
										+ hp_b[1][1] * hp_x[1][1]
										+ hp_b[1][2] * hp_x[1][2]
										- hp_a[1][1] * hp_y[1][1]
										- hp_a[1][2] * hp_y[1][2];
						hp_x[1][2] = hp_x[1][1];
						hp_x[1][1] = hp_x[1][0];

						hp_y[1][2] = hp_y[1][1];
						hp_y[1][1] = hp_y[1][0];


						rolloffFilter_x[0] = hp_y[1][0];
						rolloffFilter_y[0] = rolloffFilter_b[0] * rolloffFilter_x[0]
										+ rolloffFilter_b[1] * rolloffFilter_x[1]
										+ rolloffFilter_b[2] * rolloffFilter_x[2]
										- rolloffFilter_a[1] * rolloffFilter_y[1]
										- rolloffFilter_a[2] * rolloffFilter_y[2];


						this->processSignalBufferArray->at(this->outputBufferIndex[2]).buffer[i] =
										rolloffFilter_y[0];


						rolloffFilter_x[2] = rolloffFilter_x[1];
						rolloffFilter_x[1] = rolloffFilter_x[0];

						rolloffFilter_y[2] = rolloffFilter_y[1];
						rolloffFilter_y[1] = rolloffFilter_y[0];
					}
				}

		}
		catch(exception &e)
		{
			cout << "exception in Filter3Band::run: " << e.what() << endl;
		}
	}


	void Filter3Band::stop()
	{
		try
		{
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
			std::fill(std::begin(rolloffFilter_y), std::end(rolloffFilter_y), 0.0000);
			std::fill(std::begin(rolloffFilter_x), std::end(rolloffFilter_x), 0.0000);
			std::fill(std::begin(lp_y[0]), std::end(lp_y[0]), 0.0000);
			std::fill(std::begin(lp_x[0]), std::end(lp_x[0]), 0.0000);
			std::fill(std::begin(lp_y[1]), std::end(lp_y[1]), 0.0000);
			std::fill(std::begin(lp_x[1]), std::end(lp_x[1]), 0.0000);
			std::fill(std::begin(hp_y[0]), std::end(hp_y[0]), 0.0000);
			std::fill(std::begin(hp_y[0]), std::end(hp_y[0]), 0.0000);
			std::fill(std::begin(hp_y[1]), std::end(hp_y[1]), 0.0000);
			std::fill(std::begin(hp_y[1]), std::end(hp_y[1]), 0.0000);


			for (int signalBufferIndex = 0; signalBufferIndex < this->processData.processOutputCount; signalBufferIndex++)
			{
				this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex[signalBufferIndex]));
			}
		}
		catch(exception &e)
		{
			cout << "exception in Filter3Band::stop: " << e.what() << endl;
		}

	}

}
