
#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H


#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <cstring>
#include <string.h>

/* GPIO Class
 * Purpose: Each object instatiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */


class GPIOClass
{
private:
	int gpionum;//string gpionum;
	char setdir_str[50];
	char setval_str[50];
	char getval_str[50];
public:
	GPIOClass();
	GPIOClass(int/*string*/ gnum);
	int export_gpio();
	int unexport_gpio();
    int setdir_gpio(char*/*string*/ dir);
    int setval_gpio(int/*string*/ val);
    int getval_gpio(int/*string &*/ &val);
    int/*string*/ get_gpionum();
};

#endif
