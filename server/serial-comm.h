#pragma once

#include <stdio.h>    
#include <unistd.h>   
#include <fcntl.h>    
#include <termios.h>   
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define PAYLOAD 4
#define CHECKSUM 5
#define ACK 6
#define NACK 7


///////////////communication with arduino
typedef struct{
	char header;
	char payload[4];
}pak;



void pak_print(pak p);
pak new_h_pak(char header);
pak new_p_pak(char header,char* payload);
int serialport_init(const char* serialport);
int input_clean(int fd);
int pak_tx(int fd,pak p);
pak pak_rx(int fd);
int comm_prot(int fd,char* tx,char* rx);
