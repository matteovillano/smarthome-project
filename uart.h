#pragma once

void UART_init(void);		//uart init

void rx_get(uint8_t* buf);	//copy the rx in buf
uint8_t rx_state(void);		//return 1 in the rx is complete or 0 otherwise

void tx(uint8_t* buf);		//tx
