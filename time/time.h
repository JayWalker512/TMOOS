#ifndef TIME_H
#define TIME_H

enum CPU_FREQS {
	CPU_16MHZ = 0x00,
	CPU_8MHZ = 0x01,
	CPU_4MHZ = 0x02,
	CPU_2MHZ = 0x03,
	CPU_1MHZ = 0x04
};

char TME_Init(void);

char TME_ScaleCpu(enum CPU_FREQS freq);

unsigned char TME_GetCpuClockMhz(void);

unsigned long TME_GetAccurateMicros(void);

unsigned long TME_GetAccurateMillis(void);

void TME_DelayRealMicros(unsigned int micros);

void TME_DelayRealMillis(unsigned int millis);

#endif
