#pragma once

#include <stdio.h>    
#include <unistd.h>   
#include <fcntl.h>    
#include <termios.h>   
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_WAIT 2*CLOCKS_PER_SEC

#define PAYLOAD_SIZE 4
#define NUMRETRASMTENTS 3

#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define PAYLOAD 4
#define CHECKSUM 5
#define ACK 6
#define NACK 7


typedef struct{
	char header;
	char payload[PAYLOAD_SIZE];
}pak;


int fd;

void delay(int ms);

void pak_print(pak p);
pak new_h_pak(char header);
pak new_p_pak(char header,char* payload);
int serialport_init(const char* serialport);
int pak_tx(pak p);
pak pak_rx();
int send_string(char* s);
int get_string(char* s);
