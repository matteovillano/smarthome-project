#include <avr/io.h>
#include <avr/interrupt.h>

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
