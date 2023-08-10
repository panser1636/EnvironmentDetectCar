#include "uart_esp32.h"

static  UART_Type   *uart_index[UART_NUM] = {UART1, UART2, UART3, UART4, UART5, UART6, UART7};

void uart_esp32_init(void)
{
	uart_init(myUartChannel, myBaud, myTXPin, myRxPin);
	uart_rx_interrupt(myUartChannel, 1);
}

uint8 uart_esp32_read(void)
{
	return uart_read_byte(myUartChannel);
}
