#ifndef DISK_H
#define DISK_H

char DSK_Init(void);

unsigned char (*DSK_ReadByte)(const unsigned int *address);

unsigned int (*DSK_ReadWord)(const unsigned int *address);

unsigned long (*DSK_ReadDoubleWord)(const unsigned int *address);

void (*DSK_WriteByte)(unsigned char *address, unsigned char value);

void (*DSK_WriteWord)(unsigned char *address, unsigned int value);

void (*DSK_WriteDoubleWord)(unsigned char *address, unsigned long value);

#endif
