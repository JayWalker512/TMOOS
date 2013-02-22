#include "print.h"
#include "../console/console.h"

static void printFloat(float input, enum e_VarType type);
static void printDouble(double input, enum e_VarType type);

void
printv(const char *string, void *var, enum e_VarType type)
{
	
}

void 
printInt(long input, enum e_VarType type)
{
	//works well for unsigned ints...
	//SURELY there is a better way that requires less ram
	char myString[8] = "";
	char myDigits[8] = "";
	
	if (type == VAR_UNSIGNED)
	{
		unsigned char numDigits = 0;
		do {

		//NOTE bad habit of manipulating input variable (parameter)...
		unsigned char digit = input % 10;
		myDigits[numDigits] = '0' + digit;
		numDigits++;
		input /= 10;

		} while (input > 0);

		for (unsigned char i = 0; i < numDigits; i++)
			myString[i] = myDigits[numDigits - 1 - i];

		myString[numDigits] = '\0';
		CON_SendRAMString(myString);
	}
}

void 
printFloat(float input, enum e_VarType type)
{
	
}

void 
printDouble(double input, enum e_VarType type)
{
	
}