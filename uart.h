#pragma once

void init_uart(void);

uint8_t uart_recv(void);
void uart_send(uint8_t val);

#define BAUD 115200

#define RECEIVE() uart_recv()
#define SEND(n) uart_send(n)
#define UART_BUFLEN 2048
