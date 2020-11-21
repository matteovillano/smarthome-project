#include "uart.h"

///////////////VARS///////////////
volatile uint8_t tx_buf[BUFFSIZE];
volatile uint8_t tx_idx=0; 
volatile uint8_t tx_complete=0;

volatile uint8_t rx_buf[BUFFSIZE];
volatile uint8_t rx_idx;
volatile uint8_t rx_complete;


////////INTERRUPT HANDLER////////////////
//rx
ISR(USART0_RX_vect){
	uint8_t b=UDR0;
	rx_buf[rx_idx]=b;
	rx_idx++;
	if(rx_buf[rx_idx-1]=='\n'&&rx_buf[rx_idx-2]=='\r'){
		rx_complete=1;
		UCSR0B&=~(1<<RXCIE0);
	}
}

//tx
ISR(USART0_UDRE_vect){
	uint8_t b=tx_buf[tx_idx];
	UDR0=b;
	tx_idx++;
	if(tx_buf[tx_idx-1]=='\n'&&tx_buf[tx_idx-2]=='\r'){
		tx_complete=1;
		UCSR0B&=~(1<<UDRIE0);
	}
}
//////////////////


//////////////TX//////////////////////
void tx(uint8_t* buf, uint8_t len){
	for(uint8_t i=0;i<len;i++){
		tx_buf[i]=buf[i];
	}
	
	tx_buf[len]='\r';
	tx_buf[len+1]='\n';
	
	tx_idx=0;
	tx_complete=0;
	
	sei();
	UCSR0B|=(1<<UDRIE0);
	while(!tx_complete);
	return;
}

///////////////////////////////////////



////////////RX///////////////////
uint8_t rx_state(void){
	return rx_complete;
}

void rx(uint8_t* buf){
	if(!rx_complete)return;
	uint8_t i=0;
	while(1){
		buf[i]=rx_buf[i];
		i++;
		if(rx_buf[i]=='\r'&&rx_buf[i+1]=='\n')
		break;
	}
	
	rx_complete=0;
	rx_idx=0;
	UCSR0B|=(1<<RXCIE0);
	return;
}

///////////////////////////////



////////uart inizialization//////////////
void UART_init(void){
	cli();
	UBRR0H = (uint8_t)(UARTUBRR>>8); /*UBRR*/
	UBRR0L = (uint8_t)UARTUBRR;

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  
	
	sei();
	
	rx_idx=0;
	rx_complete=0;
	tx_idx=0; 
	tx_complete=0;
	return;
}
////////////////////////////////




