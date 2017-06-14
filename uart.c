#include "main.h"
#include "uart.h"

#include "platform.h"

void init_uart(void) {
	GPIO_REG(GPIO_IOF_SEL) &= ~IOF0_UART0_MASK;
	GPIO_REG(GPIO_IOF_EN) |= IOF0_UART0_MASK;
	UART0_REG(UART_REG_DIV) = (F_CPU / BAUD) - 1;
	UART0_REG(UART_REG_TXCTRL) |= UART_TXEN;
	UART0_REG(UART_REG_RXCTRL) = UART_RXEN;
}

uint8_t uart_recv(void) {
	volatile uint32_t * uart_rx = (uint32_t *)(UART0_CTRL_ADDR + UART_REG_RXFIFO);
	uint32_t c;
	do {
		c = *uart_rx;
	} while (c & 0x80000000);
	return c;
}

void uart_send(uint8_t c) {
	while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
	UART0_REG(UART_REG_TXFIFO) = c;
}
