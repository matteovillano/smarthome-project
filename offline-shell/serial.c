#include "serial.h"
#define DEBUGPRINT 0

void delay(int ms){
	long pause;
	clock_t now,then;
	clock_t st=clock();
	pause=ms*(CLOCKS_PER_SEC/1000);
	then=clock();
	now=then;
	while((now-then)<pause)
	now=clock();
	return;
}

void pak_print(pak p){
	printf("<h:%d ",p.header);
	for(int i=0;i<PAYLOAD_SIZE;i++){
		printf("%d ",p.payload[i]);
	}
	printf(">\n");
	return;
}

pak new_h_pak(char header){
	pak p;
	p.header=header;
	for(int i=0;i<PAYLOAD_SIZE;i++){
		p.payload[i]=0;
	}
	return p;
}

pak new_p_pak(char header,char* payload){
	pak p;
	p.header=header;
	for(int i=0;i<PAYLOAD_SIZE;i++){
		p.payload[i]=payload[i];
	}
	return p;
}


int serialport_init(const char* serialport){
    
	struct termios toptions;
    int fd= open(serialport, O_RDWR | O_NONBLOCK );
    if (fd == -1){
		printf("error from file open! errno: %d\n",errno);
		return -1;
	}
	memset(&toptions,0,sizeof(struct termios));
    if (tcgetattr(fd, &toptions) < 0){
		printf("error from tcgetattr! errno: %d\n",errno);
		return -1;
	}
    
    speed_t brate=B19200;
    cfsetispeed(&toptions, brate);	//setting input speed
    cfsetospeed(&toptions, brate);	//setting output speed
	
	//setting flags
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); 

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    toptions.c_oflag &= ~OPOST; 
	
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 0;
   
    
    if(tcsetattr(fd, TCSANOW, &toptions)< 0){
		 printf("error from tcgetattr! errno: %d\n",errno);
		 return -1;
	}
	
    return fd;
}


int pak_tx(pak p){
	char b[sizeof(pak)+2];
	char* pa=(char*)&p;
	int write_res;
	for(int i=0;i<sizeof(pak);i++){
		b[i]=pa[i];
	}
	b[sizeof(pak)]='\r';
	b[sizeof(pak)+1]='\n';
	if(DEBUGPRINT){
		printf("sending: ");
		pak_print(p);
	}
	write_res=write(fd,b,7);
	if(write_res<0){
			printf("serial communication error...\n");
			exit(0);
		}
	return 1;
}

pak pak_rx(){
	pak p;
	char buf[3*sizeof(pak)];
	char read_buf;
	int i=0;
	int read_res=0;
	clock_t now,then;
	then=clock();
	now=then;
	while(1){
		read_res=read(fd,&read_buf,1);
		if(read_res<0){
			printf("serial communication error...\n");
			exit(0);
		}
		if(read_res){
			buf[i]=read_buf;
			i++;
		}
		now=clock();
		if(now-then>MAX_WAIT){
			p=new_h_pak(CON_REF);
			return p;
		}
		if(i>=7&&(buf[i-1]==10||buf[i-1]==13)&&(buf[i-2]==10||buf[i-2]==13))
			break;
	}
	p=new_p_pak(buf[i-sizeof(pak)-2],buf+i-PAYLOAD_SIZE-2);
	if(DEBUGPRINT){
		printf("recived: ");
		pak_print(p);
	}
	return p;
}




int send_string(char* s){
	
	pak p;
	int idx=0,f=1;
	char cs[PAYLOAD_SIZE];
	memset(cs,0,PAYLOAD_SIZE);
	
	p=new_h_pak(CON_REQ);
	pak_tx(p);
	
	p=pak_rx();
	if(p.header!=CON_ACC)
		return 0;
	
	while(f){
		p=new_p_pak(PAYLOAD,s+idx*PAYLOAD_SIZE);
		idx++;
		for(int i=0;i<PAYLOAD_SIZE;i++){
			if(!p.payload[i])
				f=0;
			if(!f)
				p.payload[i]=0;
			cs[i]+=p.payload[i];
		}
		pak_tx(p);	
	}
	p=new_p_pak(CHECKSUM,cs);
	pak_tx(p);
	
	p=pak_rx();
	if(p.header==ACK)
		return 1;
	
	return 0;
	
}

int get_string(char* s){
	
	pak p;
	int idx=0;
	char cs[PAYLOAD_SIZE];
	memset(cs,0,PAYLOAD_SIZE);
	
	p=pak_rx();
	if(p.header!=CON_REQ){
		return 0;
	}
	
	p=new_h_pak(CON_ACC);
	pak_tx(p);
	
	while(1){
		p=pak_rx();
		if(p.header==PAYLOAD){
			for(int i=0;i<PAYLOAD_SIZE;i++){
				s[idx++]=p.payload[i];
				cs[i]+=p.payload[i];
			}
		}else if(p.header==CHECKSUM){
			for(int i=0;i<PAYLOAD_SIZE;i++){
				if(cs[i]!=p.payload[i]){
					p=new_h_pak(NACK);
					pak_tx(p);
					return 0;
				}
			}
			break;
		}else{
			return 0;
		}
	}
	
	p=new_h_pak(ACK);
	pak_tx(p);
	
	return 1;
}
