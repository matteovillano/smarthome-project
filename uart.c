#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define UARTBAUD 19200
#define UARTUBRR (F_CPU/16/UARTBAUD-1)
#define BUFSIZE 256


//vars
volatile uint8_t rx_buf[BUFSIZE];
volatile uint8_t tx_buf[BUFSIZE];
volatile uint8_t rx_idx;
volatile uint8_t tx_idx;
volatile uint8_t tx_complete;
volatile uint8_t rx_complete;


//interrupts handler
ISR(USART0_RX_vect){
	if(rx_idx>=BUFSIZE)rx_idx=255;
	uint8_t c=UDR0;
	if(c=='\r'||c=='\n'){
		rx_buf[rx_idx]=0;
		rx_complete=1;
	}else rx_buf[rx_idx]=c;
	rx_idx++;
}


ISR(USART0_UDRE_vect){
	uint8_t c=tx_buf[tx_idx];
	UDR0=c;
	tx_idx++;
	if(c=='\n'||c=='\r'||tx_idx>=BUFSIZE){	
		tx_complete=1;
		UCSR0B&=~(1<<UDRIE0);	
	}
}


//uart inizialization
void UART_init(void){
	cli();
	UBRR0H = (uint8_t)(UARTUBRR>>8);
	UBRR0L = (uint8_t)UARTUBRR;

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  
	
	rx_idx=0;
	tx_idx=0;
	tx_complete=0;
	rx_complete=0;
	sei();
	return;
}


//RX
void rx_get(uint8_t* buf){
	for(uint8_t i=0;i<rx_idx;i++){
		buf[i]=rx_buf[i];
		rx_buf[i]=0;
	}
	rx_idx=0;
	rx_complete=0;
	return;
}

uint8_t rx_state(void){
	return rx_complete;
}


//TX
void tx(uint8_t* buf){
	for(uint8_t i=0;1;i++){
		if(buf[i]==0||i==BUFSIZE-1){
			tx_buf[i]='\n';
			break;
		}else tx_buf[i]=buf[i];
	}
	
	sei();
	UCSR0B|=(1<<UDRIE0);
	while(!tx_complete);
	
	for(uint8_t j=0;j<tx_idx;j++)
	tx_buf[j]=0;
	
	tx_idx=0;
	tx_complete=0;
	
	return;
}

