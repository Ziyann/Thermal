#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_setup(void);
void uart_enable_rx_pullup(void);
void uart_sendint(int32_t);
void uart_sendchar(char);
void uart_sendstring(char*);
char uart_getchar(void);

#endif /* UART_H_ */