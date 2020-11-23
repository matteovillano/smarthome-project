#pragma once

#include "uart.h"
#include "cmd_line.h"
////////////
#include "pin_io.h"

#define TIMEOUT 10000000
#define NUMRETRASMTENTS 3
#define PAYLOAD_SIZE 4


#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define PAYLOAD 4
#define CHECKSUM 5
#define ACK 6
#define NACK 7
#define CON_END 8


typedef struct{
	uint8_t header;
	uint8_t payload[PAYLOAD_SIZE];
}paket;

paket new_h_pak(uint8_t header);
paket new_p_pak(uint8_t header, uint8_t* payload);

void pak_tx(paket* p);
paket pak_rx(void);


uint8_t get_string(char* s);
uint8_t send_string(char* s);

void comm_prot(void);
