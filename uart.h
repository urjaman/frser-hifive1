#pragma once

void init_uart(void);

void uart_isr(void);

uint8_t uart_recv(void);
void uart_send(uint8_t val);

#define BAUD 4000000
/* If your flashrom has custom baudrate support, you can try this */
//#define BAUD 12000000

#define RECEIVE() uart_recv()
#define SEND(n) uart_send(n)
#define UART_BUFLEN 2048
