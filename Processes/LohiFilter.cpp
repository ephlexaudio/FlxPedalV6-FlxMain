/*
 * LohiFilter.cpp
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

LohiFilter::LohiFilter():
lp_y {0.000,0.000,0.000,0.000},
lp_x {0.000,0.000,0.000,0.000},hp_y {0.000,0.000,0.000,0.000},
hp_x {0.000,0.000,0.000,0.000},couplingFilter_y {0.000,0.000,0.000},
couplingFilter_x {0.000,0.000,0.000},
lp_b {0.000,0.000,0.000,0.000},lp_a {0.000,0.000,0.000,0.000},
hp_b {0.000,0.000,0.000},hp_a {0.000,0.000,0.000},couplingFilter_b {0.000,0.000,0.000},
couplingFilter_a {0.000,0.000,0.000}
{
#if(dbg >= 1)
	 cout << "ENTERING LohiFilter::LohiFilter(Default)" << endl;
#endif

	status = 0;
	bufferSize = 256;
	inputCouplingMode = 1;
	this->name = "LohiFilter(Default)";

	this->inputBufferIndex = 0;
	this->outputBufferIndex[0] = 0;
	this->outputBufferIndex[1] = 0;
	this->paramControllerBufferIndex = 0;
#if(dbg >= 1)
	 cout << "EXITING LohiFilter::LohiFilter(Default)"  << endl;
#endif

}
LohiFilter::LohiFilter(ProcessObjectData processData):
lp_y {0.000,0.000,0.000,0.000},
lp_x {0.000,0.000,0.000,0.000},hp_y {0.000,0.000,0.000,0.000},
hp_x {0.000,0.000,0.000,0.000},couplingFilter_y {0.000,0.000,0.000},
couplingFilter_x {0.000,0.000,0.000},
lp_b {0.000,0.000,0.000,0.000},lp_a {0.000,0.000,0.000,0.000},
hp_b {0.000,0.000,0.000},hp_a {0.000,0.000,0.000},couplingFilter_b {0.000,0.000,0.000},
couplingFilter_a {0.000,0.000,0.000}
{
#if(dbg >= 1)
	 cout << "ENTERING LohiFilter::LohiFilter" << endl;
#endif
	status = 0;
	bufferSize = 256;
	inputCouplingMode = 1;
	this->processData = processData;
	this->name = this->processData.processName;
	this->inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
	this->outputBufferIndex[0] = this->processData.outputConnectedBufferIndexArray[0];
	this->outputBufferIndex[1] = this->processData.outputConnectedBufferIndexArray[1];
	this->paramControllerBufferIndex = this->processData.parameterArray[0].paramContBufferIndex;

#if(dbg >= 1)
	 cout << "EXITING LohiFilter::LohiFilter: " << this->getName() << endl;
#endif

}
LohiFilter::~LohiFilter()
{

}

#define dbg 0


 void LohiFilter::load()
 {
#if(dbg >= 1)
	 cout << "ENTERING LohiFilter::load" << endl;
#endif
		try
		{
			std::fill(std::begin(lp_y), std::end(lp_y), 0.0000);
			std::fill(std::begin(lp_x), std::end(lp_x), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);

				int coefIndex = 0;
				for(auto & coef : couplingFilter)
				{
					if(coefIndex < 3) couplingFilter_b[coefIndex] = coef;
					else couplingFilter_a[coefIndex - 3] = coef;
					coefIndex++;
				}
				for (int signalBufferIndex = 0; signalBufferIndex < this->processData.processOutputCount; signalBufferIndex++)
				{
					this->clearProcBuffer(&this->processSignalBufferArray->at(signalBufferIndex));
				}
		}
		catch(exception &e)
		{
			cout << "exception in LohiFilter::load: " << e.what() << endl;
		}
#if(dbg >= 1)
	 cout << "EXITING LohiFilter::load"  << endl;
#endif

 }

#define dbg 0
 void LohiFilter::run()
 {

		try
		{
			 int param1 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex).parameterValueIndex;
			 double tempInput = 0.000;

			lp_b[0] = lp[param1][0];
			lp_b[1] = lp[param1][1];
			lp_b[2] = lp[param1][2];
			lp_a[1] = lp[param1][4];
			lp_a[2] = lp[param1][5];

			hp_b[0] = hp[param1][0];
			hp_b[1] = hp[param1][1];
			hp_b[2] = hp[param1][2];
			hp_a[1] = hp[param1][4];
			hp_a[2] = hp[param1][5];

			for (size_t i = 0; i < bufferSize; i++)
			{
				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					this->processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] =
									this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
					this->processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] =
									this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
				}
				else
				{
					if (inputCouplingMode == 0)
					{
						tempInput = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
					}
					else if (inputCouplingMode == 1)
					{
						couplingFilter_x[0] =
										this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
						couplingFilter_y[0] = couplingFilter_b[0] * couplingFilter_x[0]
										+ couplingFilter_b[1] * couplingFilter_x[1]
										+ couplingFilter_b[2] * couplingFilter_x[2]
										- couplingFilter_a[1] * couplingFilter_y[1]
										- couplingFilter_a[2] * couplingFilter_y[2];

						tempInput = couplingFilter_y[0];

						couplingFilter_x[2] = couplingFilter_x[1];
						couplingFilter_x[1] = couplingFilter_x[0];

						couplingFilter_y[2] = couplingFilter_y[1];
						couplingFilter_y[1] = couplingFilter_y[0];
					}

					//********************* Lowpass filter ********************
					lp_x[0] = tempInput;

					lp_y[0] = lp_b[0] * lp_x[0]
									+ lp_b[1] * lp_x[1]
									+ lp_b[2] * lp_x[2]
									- lp_a[1] * lp_y[1]
									- lp_a[2] * lp_y[2];
					lp_x[2] = lp_x[1];
					lp_x[1] = lp_x[0];

					lp_y[2] = lp_y[1];
					lp_y[1] = lp_y[0];

					this->processSignalBufferArray->at(this->outputBufferIndex[0]).buffer[i] =
									lp_y[0];

					//********************* Highpass filter ********************
					hp_x[0] = tempInput;


					hp_y[0] = hp_b[0] * hp_x[0]
									+ hp_b[1] * hp_x[1]
									+ hp_b[2] * hp_x[2]
									- hp_a[1] * hp_y[1]
									- hp_a[2] * hp_y[2];
					hp_x[2] = hp_x[1];
					hp_x[1] = hp_x[0];

					hp_y[2] = hp_y[1];
					hp_y[1] = hp_y[0];


					this->processSignalBufferArray->at(this->outputBufferIndex[1]).buffer[i] =
									hp_y[0];
				}
			}
		}
		catch(exception &e)
		{
			cout << "exception in LohiFilter::run: " << e.what() << endl;
		}


 }
 void LohiFilter::stop()
 {
		try
		{
			std::fill(std::begin(lp_y), std::end(lp_y), 0.0000);
			std::fill(std::begin(lp_x), std::end(lp_x), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(hp_y), std::end(hp_y), 0.0000);
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);

			for (int signalBufferIndex = 0; signalBufferIndex < this->processData.processOutputCount; signalBufferIndex++)
			{
				this->clearProcBuffer(&this->processSignalBufferArray->at(this->outputBufferIndex[signalBufferIndex]));
			}
		}
		catch(exception &e)
		{
			cout << "exception in LohiFilter::stop: " << e.what() << endl;
		}

 }


}
