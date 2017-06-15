/*	Copyright (C) 2017 Urja Rannikko <urjaman@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "uart.h"
#include "leds.h"

#include "platform.h"

static uint8_t uart_rxbuf[UART_BUFLEN];
static volatile uint32_t uart_bufwo;
static uint32_t uart_bufro;

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
	while (uart_bufwo == ro) set_led(LED_GREEN);
	clear_led(LED_GREEN);
	uint8_t c = uart_rxbuf[ro++];
	if (ro >= UART_BUFLEN) ro = 0;
	uart_bufro = ro;
	return c;
}

void uart_send(uint8_t c) {
	while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
	UART0_REG(UART_REG_TXFIFO) = c;
}
