#ifndef STRING_H
#define STRING_H

#define NULL 0

/* Returns the length of a character array (string). */
unsigned int strlen(const char *str);

/* Concatenates (appends) one string onto another. */
char *strcat(char *dst, const char *src);

/* Copies one string into another buffer. */
void strcpy(char *dst, const char *src);

/* Compares 2 strings. 0 if equal, difference if not. */
int strcmp(const char *s1, const char *s2);

/* Takes an integer and returns it as an ASCII string. */
char *itoa(const int value, char *str);

#endif
