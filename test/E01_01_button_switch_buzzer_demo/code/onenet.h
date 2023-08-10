#ifndef __ONENET_H
#define __ONENET_H
#include <string.h>
#include <stdio.h>
#include "zf_common_headfile.h"
#include <stdint.h>

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

#define ONENET_UART    ( UART_5 )
#define ONENET_RX      ( UART5_TX_A4 )                                    // ONENET RX?????????
#define ONENET_TX      ( UART5_RX_A5 )                                    // ONENET TX????

void ESP01S_Init(void);
void ESP01S_Clear(void);
//void ESP01S_SendData(unsigned char *data, unsigned short len);
//unsigned char *ESP01S_GetIPD(unsigned short timeOut);
void Esp_PUB(uint8 temp,double latitude,double longitude,uint8 wet,uint16 noise);
void ONENET_UART_RxCpltCallback();
#endif
