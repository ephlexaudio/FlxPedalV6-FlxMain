/*
 * Effects.h
 *
 *  Created on: Jun 29, 2016
 *      Author: mike
 */

#ifndef PROCESSES_PROCESSES_H_

#define PROCESSES_PROCESSES_H_


#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>

#include "../config.h"
#include "../structs.h"

namespace std
{
	class Processes {
	protected:
		ProcessObjectData processData;

		
		string name;
		string processSymbol;
		array<bool,10> *footswitchStatusArray;
		array<ProcessSignalBuffer,60> *processSignalBufferArray;
		array<ProcessParameterControlBuffer,60> *processParamControlBufferArray;
		int clearProcBuffer(ProcessSignalBuffer *processSignalBuffer);

	public:
		Processes();
		Processes(ProcessObjectData processData);
		virtual ~Processes();

		string getName();
		string getSymbol();
		void setProcessSignalBufferArray(array<ProcessSignalBuffer,60> &processSignalBufferArray);
		void setProcessParameterControlBufferArray(array<ProcessParameterControlBuffer,60> &procParamControlBufferArray);
		void setFootswitchStatusArray(array<bool,10> &footswitchStatusArray);
		virtual void load() = 0;
		virtual void run() = 0;
		virtual void stop() = 0;
	};


	class Delay: public Processes {
	private:
		int status;
		int bufferSize;

		int inputBufferIndex;
		int outputBufferIndex;
		int paramControllerBufferIndex[2];

		 int delayCoarse;
		 int delayFine;
		 long inputPtr;
		 long outputPtr;
		double delayBuffer[DELAY_BUFFER_LENGTH+10];
		 long delayTimeAveragingBuffer[4];

	public:
		Delay();
		Delay(ProcessObjectData processData);
		virtual ~Delay();

		void load();
		void run();
		void stop();
	};


	class Filter3Band: public Processes {
	private:
		int status;
		 int bufferSize;
		int inputCouplingMode;

		int inputBufferIndex;
		int outputBufferIndex[3];
		int paramControllerBufferIndex[3];


		double lp_y[2][3], lp_x[2][3];
		double hp_y[2][3], hp_x[2][3];
		double couplingFilter_y[3], couplingFilter_x[3];
		double rolloffFilter_y[3], rolloffFilter_x[3];

		double lp_b[2][3], lp_a[2][3];
		double hp_b[2][3], hp_a[2][3];
		double couplingFilter_b[3], couplingFilter_a[3];
		double rolloffFilter_b[3], rolloffFilter_a[3];

	public:
		Filter3Band();
		Filter3Band(ProcessObjectData processData);
		virtual ~Filter3Band();

		void load();
		void run();
		void stop();
	};




	class Filter3Band2: public Processes {
	private:
		int status;
		int bufferSize;
		int inputCouplingMode;

		int inputBufferIndex;
		int outputBufferIndex[3];
		int paramControllerBufferIndex[2];

		double lp_y[3], lp_x[3];
		double bp_y[5], bp_x[5];
		double hp_y[3], hp_x[3];
		double couplingFilter_y[3], couplingFilter_x[3];
		double rolloffFilter_y[3], rolloffFilter_x[3];

		double lp_b[3], lp_a[3];
		double bp_b[5], bp_a[5];
		double hp_b[3], hp_a[3];
		double couplingFilter_b[3], couplingFilter_a[3];
		double rolloffFilter_b[3], rolloffFilter_a[3];

	public:
		Filter3Band2();
		Filter3Band2(ProcessObjectData processData);
		virtual ~Filter3Band2();

		void load();
		void run();
		void stop();
	};


	class LohiFilter: public Processes {
	private:
		int status;
		unsigned int bufferSize;
		int inputCouplingMode;

		int inputBufferIndex;
		int outputBufferIndex[2];
		int paramControllerBufferIndex;

		double lp_y[4], lp_x[4];
		double hp_y[4], hp_x[4];
		double couplingFilter_y[3], couplingFilter_x[3];

		double lp_b[4], lp_a[4];
		double hp_b[4], hp_a[4];
		double couplingFilter_b[3], couplingFilter_a[3];



	public:
		LohiFilter();
		LohiFilter(ProcessObjectData processData);
		virtual ~LohiFilter();

		void load();
		void run();
		void stop();
	};


	class Mixer: public Processes {
	private:
		int status;
		unsigned int bufferSize;
		int inputBufferIndex[3];
		int outputBufferIndex;
		int paramControllerBufferIndex[4];
	public:
		Mixer();
		Mixer(ProcessObjectData processData);
		virtual ~Mixer();

		void load();
		void run();
		void stop();
	};


	class Volume: public Processes {
	private:
		int status;
		int bufferSize;
		int inputBufferIndex;
		int outputBufferIndex;
		int paramControllerBufferIndex;
	public:
		Volume();
		Volume(ProcessObjectData processData);
		virtual ~Volume();

		void load();
		void run();
		void stop();
	};


	class Waveshaper: public Processes {
	private:
		int status;
		unsigned int bufferSize;
		int inputCouplingMode;
		int waveshaperMode;
		int antiAliasingNumber;

		int inputBufferIndex;
		int outputBufferIndex;
		int paramControllerBufferIndex[3];

		double k[6][6];
		double v[5];
		double x[6],y[6];
		double m[5],b[5];
		double couplingFilter_y[3], couplingFilter_x[3];
		double noiseFilter_y[3], noiseFilter_x[3];
		double antiAliasingFilter_y[2][3], antiAliasingFilter_x[2][3];
		double outMeasure;
		double couplingFilter_b[3], couplingFilter_a[3];
		double noiseFilter_b[3], noiseFilter_a[3];
		double antiAliasingFilter_b[3], antiAliasingFilter_a[3];


	public:
		Waveshaper();
		Waveshaper(ProcessObjectData processData);
		virtual ~Waveshaper();

		void load();
		void run();
		void stop();
	};


}



#endif /* PROCESSES_H_ */
