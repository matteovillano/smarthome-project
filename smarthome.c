#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"
#include "pin_io.h"
#include "eeprom.h"


//this function takes the command string and decides what to do
void cmd(char* buf,char* out_buf){
	char* tok=strtok(buf," ");
	if(!strcmp(tok,"help")){
		strcpy(out_buf,"all commands are described in instructions.pdf in the current folder");
		return;
	}
	if(!strcmp(tok,"on")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==NO_DEV){
			strcpy(out_buf,"no output found");
			return;
		}
		if(fix_out_val(1023,o))
		strcpy(out_buf,"done");
		else
		strcpy(out_buf,"error");
		return;
	}
	if(!strcmp(tok,"off")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==NO_DEV){strcpy(out_buf,"no output found");return;}
		if(fix_out_val(0,o))
		strcpy(out_buf,"done");
		else
		strcpy(out_buf,"error");
		return;
	}
	if(!strcmp(tok,"connect")){
		tok=strtok(NULL," ");
		uint8_t d=1;
		uint8_t i=get_di(tok);
		if(i==NO_DEV){i=get_ai(tok); d=0;}
		if(i==NO_DEV){strcpy(out_buf,"no input found"); return;}
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(d){
			if(create_d_con(i,o))
			strcpy(out_buf,"done");
			else
			strcpy(out_buf,"can not create a connection");
		}else{
			if(create_a_con(i,o))
			strcpy(out_buf,"done");
			else
			strcpy(out_buf,"can not create a connection");
		}
		return;
	}
	if(!strcmp(tok,"disconnect")){
		tok=strtok(NULL," ");
		uint8_t o=get_out(tok);
		if(o==NO_DEV){
			strcpy(out_buf,"no output found");
			return;
		}
		if(delete_con(o))
		strcpy(out_buf,"done");
		else
		strcpy(out_buf,"error");
		return;
	}
	
	if(!strcmp(tok,"show")){
		char b1[16];
		out_buf[0]=0;
		tok=strtok(NULL," ");
		if(!strcmp(tok,"output")){
			strcat(out_buf,"outputs:\n");
			for(uint8_t i=0;i<8;i++){
				get_out_name(b1,i);
				strcat(out_buf,b1);
				strcat(out_buf,"\n");
			}
			strcat(out_buf,"end");
			return;
		}
		if(!strcmp(tok,"digital")){
			strcat(out_buf,"digital inputs:\n");
			for(uint8_t i=0;i<8;i++){
				get_di_name(b1,i);
				strcat(out_buf,b1);
				strcat(out_buf,"\n");
			}
			strcat(out_buf,"end");
			return;
		}
		if(!strcmp(tok,"analog")){
			strcat(out_buf,"analog inputs:\n");
			for(uint8_t i=0;i<8;i++){
				get_ai_name(b1,i);
				strcat(out_buf,b1);
				strcat(out_buf,"\n");
			}
			strcat(out_buf,"end");
			return;
		}
		if(!strcmp(tok,"connection")){
			strcat(out_buf,"connetion:\n");
			for(uint8_t i=0;i<8;i++){
				uint16_t s=get_con(i);
				if(s>0){
					if(s<1024)
						strcat(out_buf,"on: ");
					else if(s&(1<<10)){
						get_di_name(b1,s&0x0f);
						strcat(out_buf,b1);
						strcat(out_buf," -> ");
					}
					else if(s&(1<<11)){
						get_ai_name(b1,s&0x0f);
						strcat(out_buf,b1);
						strcat(out_buf," -> ");
					}
					get_out_name(b1,i);
					strcat(out_buf,b1);
					strcat(out_buf,"\n");
				}
			}
			strcat(out_buf,"end");
			return;
		}
		strcpy(out_buf,"no show cmd found");
		return;
	}
	
	
	if(!strcmp(tok,"set")){
		tok=strtok(NULL," ");
		uint8_t d;
		d=get_out(tok);
		if(d!=NO_DEV){
			tok=strtok(NULL," ");
			set_out_name(tok,d);
			strcpy(out_buf,"done");
			return;
		}
		d=get_di(tok);
		if(d!=NO_DEV){
			tok=strtok(NULL," ");
			set_di_name(tok,d);
			strcpy(out_buf,"done");
			return;
		}
		d=get_ai(tok);
		if(d!=NO_DEV){
			tok=strtok(NULL," ");
			set_ai_name(tok,d);
			strcpy(out_buf,"done");
			return;
		}
		strcpy(out_buf,"no device found");
		return;
	}
	if(!strcmp(tok,"reset")){
		tok=strtok(NULL," ");
		if(!strcmp(tok,"names")){
			set_default_name();
			strcpy(out_buf,"done");
			return;
		}
		if(!strcmp(tok,"connection")){
			for(uint8_t i=0;i<8;i++){
				delete_con(i);
			}
			strcpy(out_buf,"done");
			return;
		}
		if(!strcmp(tok,"all")){
			set_default_name();
			for(uint8_t i=0;i<8;i++){
				delete_con(i);
			}
			strcpy(out_buf,"done");
			return;
		}
		strcpy(out_buf,"no reset cmd found");
		return;
	}
	strcpy(out_buf,"no cmd found, type help");
	return;
}


/////////////comm_protocol//////////////
void comm_prot(void){
	
	paket p;
	uint8_t tx_b[256];
	uint8_t rx_b[256];
	uint8_t i=0;
	uint8_t cs[4]={0,0,0,0};
	uint8_t f;
	uint32_t t;
	
	
	if(!rx_state())return;
	
	p=pak_rx();
	
	if(p.header!=CON_REQ){
		return;
	}
	
	p=new_h_pak(CON_ACC);
	pak_tx(&p);
	
	
	while(1){
		
		
		t=0;
		while(!rx_state()){
			if(t++>1600000)return;
		}
		
		
		p=pak_rx();
		
		if(p.header==PAYLOAD){
			for(uint8_t j=0;j<4;j++){
				tx_b[i+j]=p.payload[j];
				cs[j]+=p.payload[j];
			}
			i+=4;
		}else if(p.header==CHECKSUM){
			for(uint8_t j=0;j<4;j++){
				if(cs[j]!=p.payload[j]){
					p=new_h_pak(NACK);
					pak_tx(&p);
					return;
				}
			}
			p=new_h_pak(ACK);
			pak_tx(&p);
			break;
		}else{
			p=new_h_pak(CON_REF);
			pak_tx(&p);
			return;
		}
	}
	
	cmd((char*)tx_b,(char*)rx_b);
	
	for(uint8_t k=0;k<3;k++){
		i=0;
		f=1;
		for(uint8_t j=0;j<4;j++){
			cs[j]=0;
		}
		while(f){
			p=new_p_pak(PAYLOAD,rx_b+i);
			pak_tx(&p);
			for(uint8_t j=0;j<4;j++){
				cs[j]+=p.payload[j];
				if(!p.payload[j])f=0;
			}
			i+=4;
		}
		p=new_p_pak(CHECKSUM,cs);
		pak_tx(&p);
		
		t=0;
		while(!rx_state()){
			if(t++>1600000)return;
		}
		
		p=pak_rx();
		if(p.header!=NACK)
		k=4;
	}
	
	p=new_h_pak(CON_REF);
	pak_tx(&p);
	
	return;
}
///////////////////////////////



///////////MAIN////////////////////
int main(void){
	UART_init();
	
	di_init();
	an_init();
	out_init();
	/*
	for(uint8_t i=0;i<8;i++){
		delete_con(i);
	}
	*/
	restore_con();

	while(1){
		
		if(rx_state()){
			comm_prot();
		}
		
		refresh_output();
		_delay_ms(10);
		
	}
}
////////////////////////////////////
