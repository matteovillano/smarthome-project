#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>
#include <pin_io.h>

//MAIN
int main(void){
	UART_init();
	
	di_init();
	an_init();
	out_init();
	
	
	tx((uint8_t*)"olaa");
	//uint8_t buf[256];
	uint8_t i=0;
	while(1){
		if(i==0)i=1;
		else i=0;
		set_out(i,7);
		_delay_ms(1000);
	}
}
