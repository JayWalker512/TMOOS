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
	char myDigits[8] = "";
	
	if (type == VAR_UNSIGNED) //maybe dif function for signed/unsigned?
	{
		unsigned char numDigits = 0;
		do {
			//NOTE bad habit of manipulating input variable (parameter)...
			unsigned char digit = input % 10;
			myDigits[numDigits] = '0' + digit;
			numDigits++;
			input /= 10;
		} while (input > 0);

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