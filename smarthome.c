#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"
#include "pin_io.h"
#include "eeprom.h"

void println(char* buf){
	tx((uint8_t*)buf);
	return;
}

//this function takes the command string and decides what to do
void cmd(char* buf){
	char* tok=strtok(buf," ");
	if(!strcmp(tok,"help")){
		println("heellpp");
		return;
	}
	if(!strcmp(tok,"on")){
		tok=strtok(NULL," ");
		fix_out_val(1023,0);
		return;
	}
	if(!strcmp(tok,"off")){
		fix_out_val(0,0);
		return;
	}
	if(!strcmp(tok,"connect")){
		create_d_con(0,0);
		return;
	}
	if(!strcmp(tok,"disconnect")){
		delete_con(0);
		return;
	}
	if(!strcmp(tok,"show")){
		
		return;
	}
	if(!strcmp(tok,"set")){
		
		return;
	}
	if(!strcmp(tok,"reset")){
		
		return;
	}
	println("no cmd found, type help");
}

//MAIN
int main(void){
	UART_init();
	
	di_init();
	an_init();
	out_init();
	
	
	tx((uint8_t*)"olaa");
	
	
	char buffer[256];
	
	
	
	while(1){
		if(rx_state()){
			rx_get((uint8_t*)buffer);
			cmd(buffer);
			for(uint8_t i=0;i<255;i++){
				buffer[i]=0;
			}
		}
		
		
		refresh_output();
		_delay_ms(10);
	}
}
