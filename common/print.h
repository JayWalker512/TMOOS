#ifndef PRINT_H
#define PRINT_H

enum e_VarType {
	VAR_INT8 = 0,
	VAR_UINT8,
	VAR_INT16,
	VAR_UINT16,
	VAR_INT32,
	VAR_UINT32,
	VAR_FLOAT,
	VAR_UFLOAT,
	VAR_DOUBLE,
	VAR_UDOUBLE,
	VAR_SIGNED, //these only for use with printInt, printDouble, etc. generic funcs
	VAR_UNSIGNED,
};


/* Prints formatted text to the terminal connected via serial. Works just
 like normal printf for the most part, with some different format chars.
 %c = char, %s = RAM string, %z = PSTR string, %d = any integer. This
 implementation doesn't support printing floats (yet). */
void serialPrintf(const char *fmt, ...);



void printv(const char *string, void *var, enum e_VarType type);
void printInt(long input, enum e_VarType type);
void printString(const char *string);

#endif