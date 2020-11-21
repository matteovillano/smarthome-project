#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define UARTBAUD 19200
#define UARTUBRR (F_CPU/16/UARTBAUD-1)
#define BUFFSIZE 256


void UART_init(void);		//uart init

//tx
void tx(uint8_t* buf, uint8_t len);

//rx
uint8_t rx_state(void);
void rx(uint8_t* buf);




