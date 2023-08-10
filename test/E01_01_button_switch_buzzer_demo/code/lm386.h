#ifndef _LM386_H
#define _LM386_H

#include "zf_common_headfile.h"

#define lm386_ADCchannel ADC2_CH14_C4   //ADC2_CH6_A6
#define lm386_resolution ADC_12BIT

void lm386_init(void);
uint16 lm386_convert(void);
uint16 lm386_convert2(void);
void Serial_Readinit();
uint8 Recv();
#endif