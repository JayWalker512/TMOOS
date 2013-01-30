#include "string.h"

/* Returns the length of a character array (string). */
unsigned int strlen(const char *str)
{
	unsigned int i = 0;
	while(*(str + i) != NULL)
	{
		i++;
	}
	return i;
}

/* Concatenates (appends) one string onto another. */
char *strcat(char *dst, const char *src)
{
	unsigned int start = 0;
	while(*(dst + start) != NULL)
	{
		start++;
	}
	
	unsigned int i = 0;
	while(*(src + i) != NULL)
	{
		*(dst + start + i) = *(src + i);
	}
	
	return dst; //standard behavior
}

/* Copies one string into another buffer. */
void strcpy(char *dst, const char *src)
{
	unsigned int i = 0;
	while(*(src + i) != NULL)
	{
		*(dst + i) = *(src + i);
	}
}

int strcmp(const char *s1, const char *s2)
{
	unsigned int i = 0;
	while(*(s1 + i) == *(s2 + i))
	{
		i++;
		if (*(s1 + i) == NULL && *(s2 + i) == NULL)
			return 0; //they are equal!
	}
	
	return *(s1 + i) - *(s2 + i);
}

/* Takes an integer and returns it as an ASCII string in *str. */
char *itoa(const int value, char *str)
{
	//TODO UNFINISHED
	//value = 0; str = 0; //silence "unused" warning for now
	return 0;
}
