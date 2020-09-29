#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>

//MAIN
int main(void){
	UART_init();
	
	
	tx((uint8_t*)"olaa");
	uint8_t buf[256];
	while(1){
		if(rx_state()){
			rx_get(buf);
			tx(buf);
		}
		_delay_ms(100);
	}
}
