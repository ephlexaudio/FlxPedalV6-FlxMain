
#include "GPIOClass.h"
#include "config.h"

using namespace std;


GPIOClass::GPIOClass()
{
	this->gpionum = 4; //GPIO4 is default
}

GPIOClass::GPIOClass(int/*string*/ gnum)
{
	this->gpionum = gnum;  //Instatiate GPIOClass object for GPIO pin number "gnum"
}

int GPIOClass::export_gpio()
{
	string export_str = "/sys/class/gpio/export";
	ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
	/*if (exportgpio < 0){
		cout << " OPERATION FAILED: Unable to export GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}*/

	exportgpio << this->gpionum ; //write GPIO number to export
    exportgpio.close(); //close export file
    return 0;
}

int GPIOClass::unexport_gpio()
{
	string unexport_str = "/sys/class/gpio/unexport";
	ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
	/*if (unexportgpio < 0){
		cout << " OPERATION FAILED: Unable to unexport GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}*/

	unexportgpio << this->gpionum ; //write GPIO number to unexport
    unexportgpio.close(); //close unexport file
    return 0;
}

int GPIOClass::setdir_gpio(char*/*string*/ dir)
{

	//string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/direction";
	sprintf(this->setdir_str,"/sys/class/gpio/gpio%d/direction", this->gpionum);
	//ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
	ofstream setdirgpio(this->setdir_str); // open direction file for gpio
		/*if (setdirgpio < 0){
			cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpionum <<" ."<< endl;
			return -1;
		}*/

		setdirgpio << dir; //write direction to direction file
		setdirgpio.close(); // close direction file
	    return 0;
}


int GPIOClass::setval_gpio(int/*string*/ val)
{

	//string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	sprintf(this->setval_str, "/sys/class/gpio/gpio%d/value", this->gpionum);

	//ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
	ofstream setvalgpio(this->setval_str); // open value file for gpio
		/*if (setvalgpio < 0){
			cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpionum <<" ."<< endl;
			return -1;
		}*/
		setvalgpio << val ;//write value to value file
		setvalgpio.close();// close value file
	    return 0;
}

ifstream getvalgpioValue;
int GPIOClass::getval_gpio(int/*string &*/ &val){
	//int tempVal = 0;
	int status = 0;
	//cout << "test string" << endl;
	///char test[2];
	//cout << "this->gpionum: " << this->gpionum << endl;

	//string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	sprintf(this->getval_str, "/sys/class/gpio/gpio%d/value", this->gpionum);
	//ifstream getvalgpio(getval_str.c_str());// open value file for gpio
	ifstream getvalgpio(this->getval_str);// open value file for gpio

	/*if (getvalgpio < 0){
		cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}*/

	int testVal = 0;
	getvalgpio >> testVal;
	//cout << "testVal: " << testVal << endl;//val ;  //read gpio value
	errno = 0;
	val = testVal;
	/*try
	{
		tempVal = getvalgpio.get();
	}
	catch(std::exception const &e)
	{
		fprintf(stderr,"getval_gpio error: %s\n", e.what());
	}

	if(tempVal >= 0)
	{
		val = tempVal-48;
	}
	else
	{
		fprintf(stderr,"*****************************************\n");
		if(getvalgpio.badbit) fprintf(stderr,"getvalgpio bad bit is set\n");
		if(getvalgpio.failbit) fprintf(stderr,"getvalgpio fail bit is set\n");
		if(getvalgpio.eofbit) fprintf(stderr,"getvalgpio EOF bit is set\n");
		if(getvalgpio.goodbit) fprintf(stderr,"getvalgpio good bit is set\n");
		fprintf(stderr,"getvalgpio.get failed: %d,%d, %s\n", tempVal, errno, strerror(errno));
		val = 0;
		status = 1;
	}*/
	errno = 0;
	//cout << "getvalgpio.get(): " << tempVal << endl;


	/*if(val != "0")
		val = "1";
	else
		val = "0";*/

	getvalgpio.close(); //close the value file
    return status;
}


int/*string*/ GPIOClass::get_gpionum(){

return this->gpionum;

}

