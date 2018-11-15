/*
 * Waveshaper.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: buildrooteclipse
 */


#include "Processes.h"

#include "../config.h"
#include "../valueArrays.h"

namespace std
{

#define dbg 0
	Waveshaper::Waveshaper(): k {{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000},
	{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000},
	{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000}},
	v {0.000,0.000,0.000,0.000,0.000},
	x {0.000,0.000,0.000,0.000,0.000,0.000},y {0.000,0.000,0.000,0.000,0.000,0.000},
	m{0.000,0.000,0.000,0.000,0.000},b{0.000,0.000,0.000,0.000,0.000},
	couplingFilter_y{0.000,0.000,0.000},
	couplingFilter_x{0.000,0.000,0.000},
	noiseFilter_y{0.000,0.000,0.000}, noiseFilter_x{0.000,0.000,0.000},
	antiAliasingFilter_y{{0.000,0.000,0.000},{0.000,0.000,0.000}},
	antiAliasingFilter_x{{0.000,0.000,0.000},{0.000,0.000,0.000}},
	couplingFilter_b{0.000,0.000,0.000}, couplingFilter_a{0.000,0.000,0.000},
	noiseFilter_b{0.000,0.000,0.000}, noiseFilter_a{0.000,0.000,0.000},
	antiAliasingFilter_b{0.000,0.000,0.000}, antiAliasingFilter_a{0.000,0.000,0.000}
	{
	#if(dbg >= 1)
		cout << "ENTERING Waveshaper::Waveshaper(Default)" << endl;
	#endif

		status = 0;
		bufferSize = 256;
		inputCouplingMode = 1;
		waveshaperMode = 0;
		antiAliasingNumber = 1;
		this->name = "Waveshaper(Default)";
		inputBufferIndex = 0;
		outputBufferIndex = 0;
		paramControllerBufferIndex[0] = 0;
		paramControllerBufferIndex[1] = 0;
		paramControllerBufferIndex[2] = 0;

		outMeasure = 0;
	#if(dbg >= 1)
		cout << "EXITING Waveshaper::Waveshaper(Default)"  << endl;
	#endif


	}
	Waveshaper::Waveshaper(ProcessObjectData processData): k {{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000},
	{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000},
	{0.000,0.000,0.000,0.000,0.000,0.000},{0.000,0.000,0.000,0.000,0.000,0.000}},
	v {0.000,0.000,0.000,0.000,0.000},
	x {0.000,0.000,0.000,0.000,0.000,0.000},y {0.000,0.000,0.000,0.000,0.000,0.000},
	m{0.000,0.000,0.000,0.000,0.000},b{0.000,0.000,0.000,0.000,0.000},
	couplingFilter_y{0.000,0.000,0.000},
	couplingFilter_x{0.000,0.000,0.000},
	noiseFilter_y{0.000,0.000,0.000}, noiseFilter_x{0.000,0.000,0.000},
	antiAliasingFilter_y{{0.000,0.000,0.000},{0.000,0.000,0.000}},
	antiAliasingFilter_x{{0.000,0.000,0.000},{0.000,0.000,0.000}},
	couplingFilter_b{0.000,0.000,0.000}, couplingFilter_a{0.000,0.000,0.000},
	noiseFilter_b{0.000,0.000,0.000}, noiseFilter_a{0.000,0.000,0.000},
	antiAliasingFilter_b{0.000,0.000,0.000}, antiAliasingFilter_a{0.000,0.000,0.000}
	{
#if(dbg >= 1)
		cout << "ENTERING Waveshaper::Waveshaper" << endl;
#endif
		status = 0;
		bufferSize = 256;
		inputCouplingMode = 1;
		waveshaperMode = 0;
		antiAliasingNumber = 1;
		this->processData = processData;
		this->name = this->processData.processName;
		inputBufferIndex = this->processData.inputConnectedBufferIndexArray[0];
		outputBufferIndex = this->processData.outputConnectedBufferIndexArray[0];
		paramControllerBufferIndex[0] = this->processData.parameterArray[0].paramContBufferIndex;
		paramControllerBufferIndex[1] = this->processData.parameterArray[1].paramContBufferIndex;
		paramControllerBufferIndex[2] = this->processData.parameterArray[2].paramContBufferIndex;

		outMeasure = 0;
#if(dbg >= 1)
		cout << "EXITING Waveshaper::Waveshaper: " << this->getName() << endl;
#endif

	}


	Waveshaper::~Waveshaper()
	{

	}

#define dbg 0

	void Waveshaper::load()
	{
#if(dbg >= 1)
		cout << "ENTERING Waveshaper::load" << endl;
#endif
		try
		{
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
			std::fill(std::begin(noiseFilter_y), std::end(noiseFilter_y), 0.0000);
			std::fill(std::begin(noiseFilter_x), std::end(noiseFilter_x), 0.0000);
			std::fill(std::begin(antiAliasingFilter_y[0]), std::end(antiAliasingFilter_y[0]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_x[0]), std::end(antiAliasingFilter_x[0]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_y[1]), std::end(antiAliasingFilter_y[1]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_x[1]), std::end(antiAliasingFilter_x[1]), 0.0000);


			// These constants are used to mathematically model an analog distortion circuit
			const double divider = 5.5;
			const double r1 = 1000.0;
			const double r2 = 2000.0;
			double r3 = r2 / divider;
			double r4 = r3 / divider;
			double r5 = r4 / divider;
			double r6 = r5 / divider;

			if (waveshaperMode == 0)
			{
				k[5][0] = r2 * r3 * r4 * r5 * r6;
				k[5][1] = r1 * r3 * r4 * r5 * r6;
				k[5][2] = r1 * r2 * r4 * r5 * r6;
				k[5][3] = r1 * r2 * r3 * r5 * r6;
				k[5][4] = r1 * r2 * r3 * r4 * r6;
				k[5][5] = r1 * r2 * r3 * r4 * r5;

				k[4][0] = r2 * r3 * r4 * r5;
				k[4][1] = r1 * r3 * r4 * r5;
				k[4][2] = r1 * r2 * r4 * r5;
				k[4][3] = r1 * r2 * r3 * r5;
				k[4][4] = r1 * r2 * r3 * r4;
				k[4][5] = 0.0000;

				k[3][0] = r2 * r3 * r4;
				k[3][1] = r1 * r3 * r4;
				k[3][2] = r1 * r2 * r4;
				k[3][3] = r1 * r2 * r3;
				k[3][4] = 0.000;
				k[3][5] = 0.000;

				k[2][0] = r2 * r3;
				k[2][1] = r1 * r3;
				k[2][2] = r1 * r2;
				k[2][3] = 0.000;
				k[2][4] = 0.000;
				k[2][5] = 0.000;

				k[1][0] = r2;
				k[1][1] = r1;
				k[1][2] = 0.000;
				k[1][3] = 0.000;
				k[1][4] = 0.000;
				k[1][5] = 0.000;

				k[0][0] = 1.000;
				k[0][1] = 0.000;
				k[0][2] = 0.000;
				k[0][3] = 0.000;
				k[0][4] = 0.000;
				k[0][5] = 0.000;

			}
			else
			{
				x[0] = 0.000;
				y[0] = 0.000;
				x[1] = 0.100;
				y[1] = 0.400;
				x[2] = 0.200;
				y[2] = 0.800;
				x[3] = 0.300;
				y[3] = 1.200;
				x[4] = 0.400;
				y[4] = 1.200;
				x[5] = 0.500;
				y[5] = 1.200;

				for (int j = 0; j < 5; j++)
				{
					m[j] = (y[j + 1] - y[j]) / (x[j + 1] - x[j]);
					b[j] = y[j] - m[j] * x[j];
	#if(dbg >= 2)
					cout << "m[" << j << "] = " << m[j] << "\tb[" << j << "] = " << b[j] << endl;
	#endif
				}
			}

			int coefIndex = 0;
			for(auto & coef : couplingFilter)
			{
				if(coefIndex < 3) couplingFilter_b[coefIndex] = coef;
				else couplingFilter_a[coefIndex - 3] = coef;
				coefIndex++;
			}
			coefIndex = 0;
			for(auto & coef : noiseFilter)
			{
				if(coefIndex < 3) noiseFilter_b[coefIndex] = coef;
				else noiseFilter_a[coefIndex - 3] = coef;
				coefIndex++;
			}
			coefIndex = 0;
			for(auto & coef : antiAliasingFilter)
			{
				if(coefIndex < 3) antiAliasingFilter_b[coefIndex] = coef;
				else antiAliasingFilter_a[coefIndex - 3] = coef;
				coefIndex++;
			}

			this->clearProcBuffer(&this->processSignalBufferArray->at(outputBufferIndex));

			outMeasure = 0.000;

		}
		catch(exception &e)
		{
			cout << "exception in Waveshaper::load: " << e.what() << endl;
		}

#if(dbg >= 1)
		cout << "EXITING Waveshaper::load"  << endl;
#endif

	}

#define dbg 0
	void Waveshaper::run()
	{
		try
		{

			int param1 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[0]).parameterValueIndex;
			int edge = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[1]).parameterValueIndex;
			int param3 = this->processParamControlBufferArray->at(this->paramControllerBufferIndex[2]).parameterValueIndex;
			double gain = logAmp[param1] * 5.0;
			double mix = linAmp[param3] * 0.1;
			double k1, k2, k3, k4, k5, k6;
			double v1, v2, v3, v4, v5;
			double clean , dist;
			double tempInput;
			bool kIndexFound = false;
			int kIndex = 0;
			int i,j;
			int upSampleBufferSize = antiAliasingNumber * bufferSize;

			double coupledBuffer[BUFFER_SIZE];

			double upSampleBuffer[10000];

			double filteredUpSampleBuffer[10000];

			double downSampleBuffer[10000];

			v[0] = brkpt[edge][0];
			v[1] = brkpt[edge][1];
			v[2] = brkpt[edge][2];
			v[3] = brkpt[edge][3];
			v[4] = brkpt[edge][4];


			if(antiAliasingNumber > 1)
			{

				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					for (size_t i = 0; i < bufferSize; i++)
					{
						this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
										this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
					}
				}
				else
				{
					cout << "Waveshaper running: " << endl;
					for (size_t i = 0; i < bufferSize; i++)
					{
						if (inputCouplingMode == 0)
						{
							tempInput = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
						}
						else if (inputCouplingMode == 1)
						{
							couplingFilter_x[0] = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
							couplingFilter_y[0] = couplingFilter_b[0] * couplingFilter_x[0]
												+ couplingFilter_b[1] * couplingFilter_x[1]
												+ couplingFilter_b[2] * couplingFilter_x[2]
												- couplingFilter_a[1] * couplingFilter_y[1]
												- couplingFilter_a[2] * couplingFilter_y[2];

							coupledBuffer[i] = couplingFilter_y[0];

							couplingFilter_x[2] = couplingFilter_x[1];
							couplingFilter_x[1] = couplingFilter_x[0];

							couplingFilter_y[2] = couplingFilter_y[1];
							couplingFilter_y[1] = couplingFilter_y[0];
						}
					}

					for (size_t i = 0; i < bufferSize; i++)
					{
						upSampleBuffer[antiAliasingNumber * i] = coupledBuffer[i]
																			   * antiAliasingNumber;
						for (j = 1; j < antiAliasingNumber; j++)
							upSampleBuffer[antiAliasingNumber * i + j] = 0.00000000;
					}

#if(UP_SAMPLE_FILTER == 1)
					for (i = 0; i < upSampleBufferSize; i++)
					{

						antiAliasingFilter_x[0][0] = upSampleBuffer[i];
						antiAliasingFilter_y[0][0] = antiAliasingFilter_b[0]* antiAliasingFilter_x[0][0]
													+ antiAliasingFilter_b[1]* antiAliasingFilter_x[0][1]
													+ antiAliasingFilter_b[2]* antiAliasingFilter_x[0][2]
													- antiAliasingFilter_a[1]* antiAliasingFilter_y[0][1]
													- antiAliasingFilter_a[2]* antiAliasingFilter_y[0][2];

						filteredUpSampleBuffer[i] = antiAliasingFilter_y[0][0];
						antiAliasingFilter_x[0][2] = antiAliasingFilter_x[0][1];
						antiAliasingFilter_x[0][1] = antiAliasingFilter_x[0][0];

						antiAliasingFilter_y[0][2] = antiAliasingFilter_y[0][1];
						antiAliasingFilter_y[0][1] = antiAliasingFilter_y[0][0];
					}
#endif

					for (i = 0; i < upSampleBufferSize; i++)
					{
#if(UP_SAMPLE_FILTER == 1)
						tempInput = filteredUpSampleBuffer[i];
#else
						tempInput = upSampleBuffer[i];
#endif

						if (waveshaperMode == 0)
						{
							kIndexFound = false;
							if (0.000 <= tempInput)
							{
								v1 = v[0];
								v2 = v[1];
								v3 = v[2];
								v4 = v[3];
								v5 = v[4];

								while (kIndexFound == false)
								{
									k1 = k[kIndex][0];
									k2 = k[kIndex][1];
									k3 = k[kIndex][2];
									k4 = k[kIndex][3];
									k5 = k[kIndex][4];
									k6 = k[kIndex][5];
									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																										/ (k1 + k2 + k3 + k4 + k5 + k6);

									if (kIndex == 0)
									{
										if (dist > v[kIndex])
											kIndex++;
										else
											kIndexFound = true;
									}
									else if (kIndex == 5)
									{
										if (dist < v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									else
									{
										if (dist > v[kIndex])
											kIndex++;
										else if (dist < v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																										/ (k1 + k2 + k3 + k4 + k5 + k6);
								}
							}
							else
							{
								v1 = -v[0];
								v2 = -v[1];
								v3 = -v[2];
								v4 = -v[3];
								v5 = -v[4];
								while (kIndexFound == false)
								{
									k1 = k[kIndex][0];
									k2 = k[kIndex][1];
									k3 = k[kIndex][2];
									k4 = k[kIndex][3];
									k5 = k[kIndex][4];
									k6 = k[kIndex][5];

									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																										/ (k1 + k2 + k3 + k4 + k5 + k6);

									if (kIndex == 0)
									{
										if (dist < -v[kIndex])
											kIndex++;
										else
											kIndexFound = true;
									}
									else if (kIndex == 5)
									{
										if (dist > -v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									else
									{
										if (dist < -v[kIndex])
											kIndex++;
										else if (dist > -v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																										/ (k1 + k2 + k3 + k4 + k5 + k6);
								}
							}
						}

						clean = tempInput;

#if(DOWN_SAMPLE_FILTER == 1)
						antiAliasingFilter_x[1][0] = ((1.0 - mix)*clean + mix*dist);
						antiAliasingFilter_y[1][0] = antiAliasingFilter_b[0]*antiAliasingFilter_x[1][0] + antiAliasingFilter_b[1]*antiAliasingFilter_x[1][1] + antiAliasingFilter_b[2]*antiAliasingFilter_x[1][2] - antiAliasingFilter_a[1]*antiAliasingFilter_y[1][1] - antiAliasingFilter_a[2]*antiAliasingFilter_y[1][2];
						downSampleBuffer[i] = antiAliasingFilter_y[1][0];

						antiAliasingFilter_x[1][2] = antiAliasingFilter_x[1][1];
						antiAliasingFilter_x[1][1] = antiAliasingFilter_x[1][0];
						antiAliasingFilter_y[1][2] = antiAliasingFilter_y[1][1];
						antiAliasingFilter_y[1][1] = antiAliasingFilter_y[1][0];
#else
						downSampleBuffer[i] = ((1.0 - mix) * clean + mix * dist);
#endif
					}

					for (size_t i = 0; i < bufferSize; i++)
					{
						this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
										downSampleBuffer[antiAliasingNumber * i];
					}

				}

			}
			else
			{

				if (this->footswitchStatusArray->at(this->processData.footswitchNumber) == false)
				{
					for (size_t i = 0; i < bufferSize; i++)
					{

						this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
										this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
					}
				}
				else
				{
					for (size_t i = 0; i < bufferSize; i++)
					{

						if (inputCouplingMode == 0)
						{
							tempInput = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
						}
						else if (inputCouplingMode == 1)
						{
							couplingFilter_x[0] = this->processSignalBufferArray->at(this->inputBufferIndex).buffer[i];
							couplingFilter_y[0] = couplingFilter_b[0]  * couplingFilter_x[0]
												+ couplingFilter_b[1]* couplingFilter_x[1]
												+ couplingFilter_b[2] * couplingFilter_x[2]
												- couplingFilter_a[1] * couplingFilter_y[1]
												- couplingFilter_a[2] * couplingFilter_y[2];

							tempInput = couplingFilter_y[0];

							couplingFilter_x[2] = couplingFilter_x[1];
							couplingFilter_x[1] = couplingFilter_x[0];

							couplingFilter_y[2] = couplingFilter_y[1];
							couplingFilter_y[1] = couplingFilter_y[0];
						}

						if (waveshaperMode == 0)
						{
							kIndexFound = false;
							if (0.000 <= tempInput)
							{
								v1 = v[0];
								v2 = v[1];
								v3 = v[2];
								v4 = v[3];
								v5 = v[4];

								while (kIndexFound == false)
								{
									k1 = k[kIndex][0];
									k2 = k[kIndex][1];
									k3 = k[kIndex][2];
									k4 = k[kIndex][3];
									k5 = k[kIndex][4];
									k6 = k[kIndex][5];
									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																											/ (k1 + k2 + k3 + k4 + k5 + k6);

									if (kIndex == 0)
									{
										if (dist > v[kIndex])
											kIndex++;
										else
											kIndexFound = true;
									}
									else if (kIndex == 5)
									{
										if (dist < v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									else
									{
										if (dist > v[kIndex])
											kIndex++;
										else if (dist < v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
								}
							}
							else
							{
								v1 = -v[0];
								v2 = -v[1];
								v3 = -v[2];
								v4 = -v[3];
								v5 = -v[4];
								while (kIndexFound == false)
								{
									k1 = k[kIndex][0];
									k2 = k[kIndex][1];
									k3 = k[kIndex][2];
									k4 = k[kIndex][3];
									k5 = k[kIndex][4];
									k6 = k[kIndex][5];

									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																											/ (k1 + k2 + k3 + k4 + k5 + k6);

									if (kIndex == 0)
									{
										if (dist < -v[kIndex])
											kIndex++;
										else
											kIndexFound = true;
									}
									else if (kIndex == 5)
									{
										if (dist > -v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									else
									{
										if (dist < -v[kIndex])
											kIndex++;
										else if (dist > -v[kIndex - 1])
											kIndex--;
										else
											kIndexFound = true;
									}
									dist = (tempInput * gain * k1 + v1 * k2
													+ v2 * k3 + v3 * k4 + v4 * k5
													+ v5 * k6)
																											/ (k1 + k2 + k3 + k4 + k5 + k6);
								}
							}
						}
						else
						{
							if (tempInput * gain >= 0.0000)
							{
								if (tempInput * gain <= x[1])
									dist = tempInput * gain * m[0] + b[0];
								else if (tempInput * gain <= x[2])
									dist = tempInput * gain * m[1] + b[1];
								else if (tempInput * gain <= x[3])
									dist = tempInput * gain * m[2] + b[2];
								else if (tempInput * gain <= x[4])
									dist = tempInput * gain * m[3] + b[3];
								else
									dist = tempInput * gain * m[4] + b[4];
							}
							else
							{
								if (tempInput * gain >= -x[1])
									dist = tempInput * gain * m[0] - b[0];
								else if (tempInput * gain >= -x[2])
									dist = tempInput * gain * m[1] - b[1];
								else if (tempInput * gain >= -x[3])
									dist = tempInput * gain * m[2] - b[2];
								else if (tempInput * gain >= -x[4])
									dist = tempInput * gain * m[3] - b[3];
								else
									dist = tempInput * gain * m[4] - b[4];
							}
						}

						clean = tempInput;

						noiseFilter_x[0] = 0.1 * ((1.0 - mix) * clean + mix * dist);
						noiseFilter_y[0] = noiseFilter_b[0] * noiseFilter_x[0]
											+ noiseFilter_b[1] * noiseFilter_x[1]
											+ noiseFilter_b[2] * noiseFilter_x[2]
											- noiseFilter_a[1] * noiseFilter_y[1]
											- noiseFilter_a[2] * noiseFilter_y[2];
						this->processSignalBufferArray->at(this->outputBufferIndex).buffer[i] =
										noiseFilter_y[0];

						noiseFilter_x[2] = noiseFilter_x[1];
						noiseFilter_x[1] = noiseFilter_x[0];

						noiseFilter_y[2] = noiseFilter_y[1];
						noiseFilter_y[1] = noiseFilter_y[0];

					}
				}
			}
		}
		catch(exception &e)
		{
			cout << "exception in Waveshaper::run: " << e.what() << endl;
		}


	}
	void Waveshaper::stop()
	{
		try
		{
			std::fill(std::begin(couplingFilter_y), std::end(couplingFilter_y), 0.0000);
			std::fill(std::begin(couplingFilter_x), std::end(couplingFilter_x), 0.0000);
			std::fill(std::begin(noiseFilter_y), std::end(noiseFilter_y), 0.0000);
			std::fill(std::begin(noiseFilter_x), std::end(noiseFilter_x), 0.0000);
			std::fill(std::begin(antiAliasingFilter_y[0]), std::end(antiAliasingFilter_y[0]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_x[0]), std::end(antiAliasingFilter_x[0]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_y[1]), std::end(antiAliasingFilter_y[1]), 0.0000);
			std::fill(std::begin(antiAliasingFilter_x[1]), std::end(antiAliasingFilter_x[1]), 0.0000);
			this->clearProcBuffer(&this->processSignalBufferArray->at(outputBufferIndex));

		}
		catch(exception &e)
		{
			cout << "exception in Waveshaper::stop: " << e.what() << endl;
		}
	}

}
