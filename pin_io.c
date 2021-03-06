#include "pin_io.h"
#include "eeprom.h"

//DIGITAL INPUT

volatile uint8_t di_status=0;

//digital input inizialization pin:53-50,10-13
void di_init(void){
	DDRB=0x00;
	PORTB=0xff;
	PCICR|=(1<<PCIE0);
	PCMSK0=0xff;
}

ISR(PCINT0_vect){
	di_status=~PINB;
}

uint8_t get_di_status(void){
	return di_status;
}

//ADC

//adc input init pins:A0-A7
void an_init(void){
	ADMUX=(1<<REFS0);
	ADCSRA=(1<<ADEN)|0x07;
}

//adc input reading
uint16_t read_adc(uint8_t pin){
	ADMUX&=0xe0;
	ADMUX|=(0x07&pin);
	
	ADCSRA|=(1<<ADSC);
	while(ADCSRA&(1<<ADSC));
	return ADC;
}

//OUTPUT

//output init 46-44,2,5-8
void out_init(void){
	//setting output pins on ports
	DDRE|=(1<<3)|(1<<4);
	DDRH|=(1<<3)|(1<<4)|(1<<5);
	DDRL|=(1<<3)|(1<<4)|(1<<5);
	
	//setting timer control registers
	TCCR3A=0xa3;
	TCCR3B=0x01;
	TCCR4A=0xab;
	TCCR4B=0x01;
	TCCR5A=0xab;
	TCCR5B=0x01;
  
	//setting all outputs off 
	OCR5A=0;
	OCR5B=0;
	OCR5C=0;
	OCR3B=0;
	OCR3A=0;
	OCR4A=0;
	OCR4B=0;
	OCR4C=0;
}

//output value setting
void set_out_val(uint16_t val,uint8_t pin){
	switch(pin){
		case 0: OCR5A=val; break;
		case 1: OCR5B=val; break;
		case 2: OCR5C=val; break;
		case 3: OCR3B=val; break;
		case 4: OCR3A=val; break;
		case 5: OCR4A=val; break;
		case 6: OCR4B=val; break;
		case 7: OCR4C=val; break;
	}
	return;
}

//output on/off setting
void set_out(uint8_t on_off,uint8_t pin){
	uint16_t v=0;
	if(on_off)v=MAXVAL;
	set_out_val(v,pin);
	return;
}


//////////////////////////////////
//these functions are called by the main to create connections between
//input and output and to do other stuff

uint16_t status[8];

uint8_t fix_out_val(uint16_t val,uint8_t output){
	if(status[output]>MAXVAL)return 0;
	status[output]=val;
	set_con(output,status[output]);
	return 1;
}

uint8_t create_d_con(uint8_t input,uint8_t output){
	if(status[output])return 0;
	status[output]=DIG_CON_FLAG;
	status[output]|=input;
	set_con(output,status[output]);
	return 1;
}

uint8_t create_a_con(uint8_t input,uint8_t output){
	if(status[output])return 0;
	status[output]=AN_CON_FLAG;
	status[output]|=input;
	set_con(output,status[output]);
	return 1;
}

uint8_t delete_con(uint8_t output){
	if(output<8){
		status[output]=0;
		set_con(output,0);
		return 1;
	}else return 0;
}

void refresh_output(void){
	uint16_t val;
	for(uint8_t i=0;i<8;i++){
		val=0;
		if(status[i]<=MAXVAL)
			val=status[i];
		else if(status[i]&DIG_CON_FLAG){
			if(di_status&(1<<(status[i]&0x0f)))
				val=MAXVAL;
			else val=0;
		}
		else if(status[i]&AN_CON_FLAG)
			val=read_adc(status[i]&0x0f);
		set_out_val(val,i);
	}
	return;
}

void restore_con(void){
	for(uint8_t i=0;i<8;i++){
		status[i]=get_con(i);
	}
	return;
}










