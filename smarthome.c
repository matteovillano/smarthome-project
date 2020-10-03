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
		uint8_t o=get_out(tok);
		if(o==0xff){println("no output found");return;}
		if(fix_out_val(1023,o))
		println("done");
		else
		println("error");
		return;
	}
	if(!strcmp(tok,"off")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==0xff){println("no output found");return;}
		if(fix_out_val(0,o))
		println("done");
		else
		println("error");
		return;
	}
	if(!strcmp(tok,"connect")){
		tok=strtok(NULL," ");
		uint8_t d=1;
		uint8_t i=get_di(tok);
		if(i==0xff){i=get_ai(tok); d=0;}
		if(i==0xff){println("no input found"); return;}
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(d){
			if(create_d_con(i,o))
			println("done");
			else
			println("error");
		}else{
			if(create_a_con(i,o))
			println("done");
			else
			println("error");
		}
		return;
	}
	if(!strcmp(tok,"disconnect")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(delete_con(o))
		println("done");
		else
		println("error");
		return;
	}
	if(!strcmp(tok,"show")){
		
		return;
	}
	if(!strcmp(tok,"set")){
		
		return;
	}
	if(!strcmp(tok,"reset")){
		tok=strtok(NULL," ");
		if(!strcmp(tok,"names")){
			set_default_name();
			println("done");
		}
		if(!strcmp(tok,"connections")){
			for(uint8_t i=0;i<8;i++){
				delete_con(i);
			}
			println("done");
		}
		if(!strcmp(tok,"all")){
			set_default_name();
			for(uint8_t i=0;i<8;i++){
				delete_con(i);
			}
			println("done");
		}
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
			//println("sono in rx");
			rx_get((uint8_t*)buffer);
			cmd(buffer);
			for(uint8_t i=0;i<255;i++){
				buffer[i]=0;
			}
		}
		//*/
		
		
		refresh_output();
		_delay_ms(10);
	}
}
