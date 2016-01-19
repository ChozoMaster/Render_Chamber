#include "parameters.h"

Parameter* Parameter::instance = 0;
Parameter* Parameter::getInstance()
{
	if (instance == 0)
		instance = new Parameter();
	return instance;
}





	
	






