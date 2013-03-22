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

void printv(const char *string, void *var, enum e_VarType type);
void printInt(long input, enum e_VarType type);

#endif