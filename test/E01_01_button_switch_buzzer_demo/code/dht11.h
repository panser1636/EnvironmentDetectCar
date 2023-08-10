#ifndef __DHT11_H
#define	__DHT11_H

#include "zf_common_headfile.h"

#define DATA D8  //B4

typedef struct
{
	uint8_t  humi_int;		
	uint8_t  humi_deci;	 	
	uint8_t  temp_int;	 	
	uint8_t  temp_deci;	 
	uint8_t  check_sum;	
		                 
}DHT11_Data_TypeDef;


void DHT11_GPIO_Config(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
static uint8_t Read_Byte(void);

#endif







