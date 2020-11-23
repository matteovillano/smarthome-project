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
	uint32_t timeout=TIMEOUT;
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

//////////////////////////

uint8_t get_string(char* s){
	
	paket p;
	uint8_t idx=0;
	uint8_t cs[PAYLOAD_SIZE];
	for(uint8_t i=0;i<PAYLOAD_SIZE;i++){
		cs[i]=0;
	}
	
	p=pak_rx();
	if(p.header!=CON_REQ)
		return 0;
	
	p=new_h_pak(CON_ACC);
	pak_tx(&p);
	
	while(1){
		p=pak_rx();
		if(p.header==PAYLOAD){
			for(uint8_t i=0;i<PAYLOAD_SIZE;i++){
				s[idx++]=p.payload[i];
				cs[i]+=p.payload[i];
			}
		}else if(p.header==CHECKSUM){
			for(uint8_t i=0;i<PAYLOAD_SIZE;i++){
				if(cs[i]!=p.payload[i]){
					p=new_h_pak(NACK);
					return 0;
				}
			}
			break;
		}else{
			return 0;
		}
	}
	p=new_h_pak(ACK);
	pak_tx(&p);
	
	return 1;
}

uint8_t send_string(char* s){
	
	paket p;
	uint8_t f=1;
	uint8_t idx=0;
	uint8_t cs[PAYLOAD_SIZE];
	for(uint8_t i=0;i<PAYLOAD_SIZE;i++){
		cs[i]=0;
	}
	
	p=new_h_pak(CON_REQ);
	pak_tx(&p);
	
	p=pak_rx();
	if(p.header!=CON_ACC)
		return 0;
	
	while(f){
		p=new_p_pak(PAYLOAD,(uint8_t*)s+idx*PAYLOAD_SIZE);
		idx++;
		for(uint8_t i=0;i<PAYLOAD_SIZE;i++){
			if(!p.payload[i])
				f=0;
			if(!f)
				p.payload[i]=0;
			cs[i]+=p.payload[i];
		}
		pak_tx(&p);
	}
	p=new_p_pak(CHECKSUM,cs);
	pak_tx(&p);
	
	p=pak_rx();
	if(p.header==ACK)
	return 1;
	
	return 0;
}

/////////////comm_protocol//////////////

void comm_prot(void){
	uint8_t tents;
	char* r;
	char buff[256];
	for(uint16_t i=0;i<256;i++)
		buff[i]=0;
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(get_string(buff))
			break;
		tents--;
	}
	if(!tents)return;
	r=cmd(buff);
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(send_string(r))
			break;
		tents--;
	}
	if(!tents)return;
			
}
