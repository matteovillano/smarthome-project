#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"
#include "pin_io.h"
#include "eeprom.h"

void println(char* string){
	
}

//this function takes the command string and decides what to do
void cmd(char* buf){
	char* tok=strtok(buf," ");
	if(!strcmp(tok,"help")){
		println("all commands are described in instructions.pdf in the current folder");
		return;
	}
	if(!strcmp(tok,"on")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==0xff){
			println("no output found");
			return;
		}
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
			println("can not create a connection");
		}else{
			if(create_a_con(i,o))
			println("done");
			else
			println("can not create a connection");
		}
		return;
	}
	if(!strcmp(tok,"disconnect")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==0xff){
			println("no output found");
			return;
		}
		if(delete_con(o))
		println("done");
		else
		println("error");
		return;
	}
	if(!strcmp(tok,"show")){
		char b1[16];
		tok=strtok(NULL," ");
		if(!strcmp(tok,"names")){
			println("outputs:");
			for(uint8_t i=0;i<8;i++){
				get_out_name(b1,i);
				println(b1);
			}
			println("digila inputs:");
			for(uint8_t i=0;i<8;i++){
				get_di_name(b1,i);
				println(b1);
			}
			println("analog inputs:");
			for(uint8_t i=0;i<8;i++){
				get_ai_name(b1,i);
				println(b1);
			}
			println("end");
			return;
		}
		if(!strcmp(tok,"output")){
			println("outputs:");
			for(uint8_t i=0;i<8;i++){
				get_out_name(b1,i);
				println(b1);
			}
			println("end");
			return;
		}
		if(!strcmp(tok,"digital")){
			println("digila inputs:");
			for(uint8_t i=0;i<8;i++){
				get_di_name(b1,i);
				println(b1);
			}
			println("end");
			return;
		}
		if(!strcmp(tok,"analog")){
			println("analog inputs:");
			for(uint8_t i=0;i<8;i++){
				get_ai_name(b1,i);
				println(b1);
			}
			println("end");
			return;
		}
		println("no show cmd found");
		return;
	}
	if(!strcmp(tok,"set")){
		tok=strtok(NULL," ");
		uint8_t d;
		d=get_out(tok);
		if(d!=0xff){
			tok=strtok(NULL," ");
			set_out_name(tok,d);
			println("done");
			return;
		}
		d=get_di(tok);
		if(d!=0xff){
			tok=strtok(NULL," ");
			set_di_name(tok,d);
			println("done");
			return;
		}
		d=get_ai(tok);
		if(d!=0xff){
			tok=strtok(NULL," ");
			set_ai_name(tok,d);
			println("done");
			return;
		}
		println("no device found");
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
		println("no reset cmd found");
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

	
	paket p;
	

	while(1){
		
		if(rx_state()){
			p=pak_rx();
			pak_tx(&p);
		}
			
		refresh_output();
		_delay_ms(10);
		
	}
}
