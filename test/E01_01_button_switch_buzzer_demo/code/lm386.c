#include "lm386.h"

#define recvpin UART5_RX_D2
#define senpin UART5_TX_C12

void lm386_init(void)
{
	adc_init(lm386_ADCchannel, lm386_resolution);
}

// 返回分贝计算结果
uint16 lm386_convert(void) //C4 version
{	uint16 sample;
	uint16 factor;
	sample=adc_mean_filter_convert(lm386_ADCchannel, 2048);//remember to zfadc change sample time
	factor=sample>28?(sample>200?(sample>400?32:44):sample>80?46:36):31;
	return 0.085*sample+factor;
}

uint16 lm386_convert2(void) //car version
{	uint16 sample;
	uint16 factor;
	sample=adc_mean_filter_convert(lm386_ADCchannel, 6);//remember to zfadc change sample time
	factor=sample>28?(sample>200?(sample>400?32:44):sample>80?46:36):31;
	return 0.03*(sample-2210);
}



void Serial_Readinit(){
	uart_init(UART_5, 115200, senpin, recvpin);
	uart_rx_interrupt(UART_5, 1);
}

uint8 Recv(){

   return uart_read_byte(UART_5);
}









