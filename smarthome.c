#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "pin_io.h"
#include "eeprom.h"

//MAIN
int main(void){
	UART_init();
	
	di_init();
	an_init();
	out_init();
	
	
	tx((uint8_t*)"olaa");
	
	
	
	while(1){
		
		
		_delay_ms(1000);
	}
}
