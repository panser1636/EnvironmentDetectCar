#ifndef _UART_ESP32_H
#define _UART_ESP32_H

#include "zf_common_headfile.h"

#define myUartChannel UART_6
#define myBaud 115200
#define myTXPin UART6_TX_C6
#define myRxPin UART6_RX_C7

void uart_esp32_init(void);
uint8 uart_esp32_read(void);

#endif
