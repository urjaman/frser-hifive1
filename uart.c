#include "main.h"
#include "uart.h"

#include "platform.h"

uint8_t uart_rxbuf[UART_BUFLEN];
volatile uint32_t uart_bufwo;
uint32_t uart_bufro;

void init_uart(void) {
	GPIO_REG(GPIO_IOF_SEL) &= ~IOF0_UART0_MASK;
	GPIO_REG(GPIO_IOF_EN) |= IOF0_UART0_MASK;
	UART0_REG(UART_REG_DIV) = (F_CPU / BAUD) - 1;
	UART0_REG(UART_REG_TXCTRL) = UART_TXEN;
	UART0_REG(UART_REG_RXCTRL) = UART_RXEN;
	uart_bufwo = 0;
	uart_bufro = 0;
	volatile uint32_t* plic_enables = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_ENABLE_OFFSET);
	volatile uint32_t* plic_priorities = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_PRIORITY_OFFSET);
	plic_priorities[3] = 4; /* There is space around that priority :P */
	plic_enables[0] |= (1 << 3); /* Enable UART0 ISR. */
	UART0_REG(UART_REG_IE) = UART_IP_RXWM;

}

void uart_isr(void) {
	/* slurp the FIFO into memory */
	volatile uint32_t * uart_rx = (uint32_t *)(UART0_CTRL_ADDR + UART_REG_RXFIFO);
	uint32_t wo = uart_bufwo;
	for(;;) {
		uint32_t c = *uart_rx;
		if (c & 0x80000000) break;
		uart_rxbuf[wo++] = c;
		if (wo>=UART_BUFLEN) wo = 0;
	}
	uart_bufwo = wo;
	return;
}

uint8_t uart_recv(void) {
	uint32_t ro = uart_bufro;
	while (uart_bufwo == ro) ;
	uint8_t c = uart_rxbuf[ro++];
	if (ro >= UART_BUFLEN) ro = 0;
	uart_bufro = ro;
	return c;
}

void uart_send(uint8_t c) {
	while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
	UART0_REG(UART_REG_TXFIFO) = c;
}
