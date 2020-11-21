#include "comm_prot.h"


///////////paket creation///////////////
paket new_h_pak(uint8_t header){
	paket p;
	p.header=header;
	memset(p.payload,0,PAYLOAD_SIZE);
	return p;
}

paket new_p_pak(uint8_t header, uint8_t* payload){
	paket p;
	p.header=header;
	memcpy(p.payload,payload,PAYLOAD_SIZE);
	return p;
}

//////////////////paket transmission/////////////
paket pak_rx(void){
	paket p;
	uint8_t *pa=(uint8_t*)&p;
	uint8_t buf[BUFFSIZE];
	
	memset(pa,0,sizeof(paket));
	uint32_t timeout=10000000;
	while(!rx_state()){
		timeout--;
		if(!timeout)return p;
	}
	rx(buf);
	for(uint8_t i=0;i<sizeof(paket);i++)
	pa[i]=buf[i];
	
	return p;
}


void pak_tx(paket* p){
	tx((uint8_t*)p,sizeof(paket));
	return;
}


/////////////comm_protocol//////////////

void comm_prot(void){
	paket p;
	uint8_t tx_b[256];
	uint8_t* rx_b;
	uint8_t i=0;
	uint8_t cs[4]={0,0,0,0};
	uint8_t f;
	uint32_t t;
	
	if(!rx_state())return;
	
	p=pak_rx();
	
	if(p.header!=CON_REQ)return;
	
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
	
	rx_b=(uint8_t*)cmd((char*)tx_b);
	
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
	return;
}
