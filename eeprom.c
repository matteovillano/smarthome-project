#include "eeprom.h"

//EEPROM handling
void EEPROM_write(uint16_t address,uint8_t data){
	while(EECR&(1<<EEPE));
	EEAR=address;
	EEDR=data;
	
	EECR|=(1<<EEMPE);
	EECR|=(1<<EEPE);
	return;
}

uint8_t EEPROM_read(uint16_t address){
	while(EECR&(1<<EEPE));
	EEAR=address;
	EECR|=(1<<EERE);
	return EEDR;
}


void EEPROM_seq_write(uint16_t address,uint8_t* data,uint16_t len){
	cli();
	for(uint16_t i=0;i<len;i++){
		EEPROM_write(address+i,data[i]);
	}
	sei();
	return;
}

void EEPROM_seq_read(uint16_t address, uint8_t* buf, uint16_t len){
	cli();
	for(uint16_t i=0;i<len;i++){
		buf[i]=EEPROM_read(address+i);
	}
	sei();
	return;
}



//ouput
void get_out_name(char* buf,uint8_t output){
	uint8_t b1[NAMELEN];
	EEPROM_seq_read(NAMES_OFFSET+(NAMELEN*output),b1,NAMELEN);
	uint8_t i=0;
	while(b1[i]){
		buf[i]=b1[i];
		i++;
	}
	buf[i]=0;
	return;
}
void set_out_name(char* buf,uint8_t output){
	uint8_t b1[NAMELEN];
	uint8_t i=0;
	while(buf[i]){
		b1[i]=buf[i];
		i++;
	}
	while(i<NAMELEN){
		b1[i++]=0;
	}
	EEPROM_seq_write(NAMES_OFFSET+(NAMELEN*output),b1,NAMELEN);
	return;
}
uint8_t get_out(char* buf){
	char b[NAMELEN];
	for(uint8_t i=0;i<8;i++){
		get_out_name(b,i);
		if(!strcmp(buf,b))return i;
	}
	return NO_DEV;
}

//digital input
void get_di_name(char* buf,uint8_t input){
	uint8_t b1[NAMELEN];
	EEPROM_seq_read(NAMES_OFFSET+(NAMELEN*(8+input)),b1,NAMELEN);
	uint8_t i=0;
	while(b1[i]){
		buf[i]=b1[i];
		i++;
	}
	buf[i]=0;
	return;
}
void set_di_name(char* buf,uint8_t input){
	uint8_t b1[NAMELEN];
	uint8_t i=0;
	while(buf[i]){
		b1[i]=buf[i];
		i++;
	}
	while(i<NAMELEN){
		b1[i++]=0;
	}
	EEPROM_seq_write(NAMES_OFFSET+(NAMELEN*(8+input)),b1,NAMELEN);
	return;
}
uint8_t get_di(char* buf){
	char b[NAMELEN];
	for(uint8_t i=0;i<8;i++){
		get_di_name(b,i);
		if(!strcmp(buf,b))return i;
	}
	return NO_DEV;
}

//analog input
void get_ai_name(char* buf,uint8_t input){
	uint8_t b1[NAMELEN];
	EEPROM_seq_read(NAMES_OFFSET+(NAMELEN*(16+input)),b1,NAMELEN);
	uint8_t i=0;
	while(b1[i]){
		buf[i]=b1[i];
		i++;
	}
	buf[i]=0;
	return;
}
void set_ai_name(char* buf,uint8_t input){
	uint8_t b1[NAMELEN];
	uint8_t i=0;
	while(buf[i]){
		b1[i]=buf[i];
		i++;
	}
	while(i<NAMELEN){
		b1[i++]=0;
	}
	EEPROM_seq_write(NAMES_OFFSET+(NAMELEN*(16+input)),b1,NAMELEN);
	return;
}
uint8_t get_ai(char* buf){
	char b[NAMELEN];
	for(uint8_t i=0;i<8;i++){
		get_ai_name(b,i);
		if(!strcmp(buf,b))return i;
	}
	return NO_DEV;
}

////
void set_default_name(void){
	for(uint8_t i=0;i<8;i++){
		char a[NAMELEN];
		sprintf(a,"led%d",i);
		set_out_name(a,i);
		sprintf(a,"dig%d",i);
		set_di_name(a,i);
		sprintf(a,"an%d",i);
		set_ai_name(a,i);
	
	}
	
}

////connections handling
void set_con(uint8_t output,uint16_t status){
	uint8_t a[2];
	a[0]=(uint8_t)(status>>8);
	a[1]=(uint8_t)(status);
	EEPROM_seq_write(CON_OFFSET+(output*2),a,2);
	return;
}

uint16_t get_con(uint8_t i){
	uint8_t a[2];
	uint16_t s;
	a[0]=EEPROM_read(CON_OFFSET+(i*2));
	a[1]=EEPROM_read(CON_OFFSET+(i*2)+1);
	s=(uint16_t)a[0]<<8;
	s+=(uint16_t)a[1];
	return s;
}
