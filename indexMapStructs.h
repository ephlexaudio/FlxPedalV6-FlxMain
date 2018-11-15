/*
 * structs4.h
 *
 *  Created on: Oct 3, 2018
 *      Author: buildrooteclipse
 */

#ifndef INDEXMAPSTRUCTS_H_
#define INDEXMAPSTRUCTS_H_


#include <vector>
#include <map>
#include <sys/types.h>


namespace std
{


struct ProcessParameterIndexing{
	string paramName;
	int paramIndex;
	string parentProcess;
	int connectedBufferIndex;
};

struct ProcessControlParameterIndexing{
	string contParamName;
	int contParamIndex;
	string parentControl;  // create parent control index in getPedalUi
};


struct ProcessIndexing{
	string processName;
	int processSequenceIndex;
	int processTypeInt;
	string parentEffect;
	map<string,ProcessParameterIndexing> paramIndexMap;
};


struct ControlIndexing{
	string controlName;  // make the name more descriptive (ie. control:mixerb_0:level1)
	int controlIndex;
	int controlTypeInt;
	string parentEffect;
	map<string,ProcessControlParameterIndexing> paramIndexMap;
};



}


#endif /* INDEXMAPSTRUCTS_H_ */
