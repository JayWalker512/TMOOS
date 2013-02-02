#ifndef DISK_H
#define DISK_H

char DSK_Init(void);

unsigned char (*DSK_ReadByte)(const unsigned int *address);

unsigned int (*DSK_ReadWord)(const unsigned int *address);

unsigned long (*DSK_ReadDoubleWord)(const unsigned int *address);

#endif
