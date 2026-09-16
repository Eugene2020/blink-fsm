#pragma once
#include <stdint.h>
#include <stdbool.h>

void uart_setup(void);

bool uart_getc(uint8_t *out);
void uart_puts(const char *s);
void uart_putc(char c);

void usart1_isr(void);