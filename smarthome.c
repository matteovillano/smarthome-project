#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"
#include "comm_prot.h"
#include "pin_io.h"
#include "eeprom.h"
#include "cmd_line.h"


///////////MAIN////////////////////
int main(void){
	//UART inizialization
	UART_init();
	//in-out device inizialization
	di_init();
	an_init();
	out_init();
	
	//restoring connections from EEPROM
	restore_con();
	
	
	while(1){

		if(rx_state()){
			comm_prot();
		}
		
		refresh_output();
		_delay_ms(10);
		
	}
}
