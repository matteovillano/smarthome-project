#pragma once

#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define NEW_CMD 4
#define CMD_ACC 5
#define PAYLOAD 6
#define CHECKSUM 7
#define ACK 8

typedef struct{
	uint8_t header;
	uint8_t payload[4];
}paket;

void UART_init(void);		//uart init

void tx(uint8_t* buf, uint8_t len);
void rx(uint8_t* buf);
uint8_t rx_state(void);


void pak_tx(paket* p);
paket pak_rx(void);
