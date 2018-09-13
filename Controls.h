/*
 * Controls.h
 *
 *  Created on: Mar 12, 2017
 *      Author: buildrooteclipse
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "config.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <json/json.h>

#include "structs.h"
#include "ControlSymbols.h"

//******************************** Controls for manipulating process parameters ********************************

int control(char action, bool envTrigger, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent);

int normal(char action, bool envTrigger, int controlVoltageIndex, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent);
int envGen(char action, bool envTrigger, int controlVoltageIndex, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent);
int lfo(char action, bool envTrigger, int controlVoltageIndex, struct ControlEvent *controlEvent, struct ProcessEvent *procEvent);




#endif /* CONTROLS_H_ */
