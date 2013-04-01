#include "print.h"
#include "../console/console.h"
#include "../avr_common.h"
#include <string.h> //for working with strings in regular RAM

static void printFloat(float input, enum e_VarType type);
static void printDouble(double input, enum e_VarType type);

void
printv(const char *string, void *var, enum e_VarType type)
{
	/* & is restricted character here, represents variable. 
	 Make a call like:
	 printv("var is & haha!", myint, VAR_UINT16); */
	
	
}

/* XXX INCOMPLETE */
void 
printInt(long input, enum e_VarType type)
{
	char myDigits[8] = "";
	long modInput = input < 0 ? 0 - input : input;
	
	unsigned char numDigits = 0;
	do {
		//NOTE bad habit of manipulating input variable (parameter)...
		unsigned char digit = modInput % 10;
		myDigits[numDigits] = '0' + digit;
		numDigits++;
		modInput /= 10;
	} while (modInput > 0);

	char tempDigit = 0;
	if (numDigits % 2 == 0) //if number of digits is even, we need to change algorithm
	{
		char i;
		for (i = 0; i < numDigits; i++)
		{
			tempDigit = myDigits[i];
			myDigits[i] = myDigits[numDigits - 1 - i];
			myDigits[numDigits - 1 - i] = tempDigit;
			if (i + 1 == numDigits - 1 - i)
				break;
		}
	}
	else
	{
		char i;
		for (i = 0; i < numDigits; i++)
		{
			tempDigit = myDigits[i];
			myDigits[i] = myDigits[numDigits - 1 - i];
			myDigits[numDigits - 1 - i] = tempDigit;
			if (i == numDigits - 1 - i)
				break;
		}
	}

	myDigits[numDigits] = '\0';
	
	/* TODO this needs a better way to deal with unsigned/signed ints.*/
	if (type == VAR_UNSIGNED)
		CON_SendRAMString(myDigits);
	else if (type == VAR_SIGNED)
	{
		if (input < 0)
			CON_SendString(PSTR("-")); 
		
		CON_SendRAMString(myDigits);
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

void
printString(const char *string)
{
	CON_SendRAMString(&string);
}