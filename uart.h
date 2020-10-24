#pragma once

#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define PAYLOAD 4
#define CHECKSUM 5
#define ACK 6
#define NACK 7

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

paket new_h_pak(uint8_t header);
paket new_p_pak(uint8_t header, uint8_t* payload);
