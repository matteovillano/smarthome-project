#include "serial-comm.h"

#define MAX_WAIT 2*CLOCKS_PER_SEC

///////////////////////////////////////
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
///////////////////////////


void pak_print(pak p){
	printf("<h:%d p:%d %d %d %d>\n",p.header,p.payload[0],p.payload[1],p.payload[2],p.payload[3]);
	return;
}

pak new_h_pak(char header){
	pak p;
	p.header=header;
	for(int i=0;i<4;i++){
		p.payload[i]=0;
	}
	return p;
}

pak new_p_pak(char header,char* payload){
	pak p;
	p.header=header;
	for(int i=0;i<4;i++){
		p.payload[i]=payload[i];
	}
	return p;
}


int input_clean(int fd){
	char a[256];
	return read(fd,a,256);
}

int pak_tx(int fd,pak p){
	char b[7];
	char* pa=(char*)&p;
	for(int i=0;i<5;i++){
		b[i]=pa[i];
	}
	b[5]='\r';
	b[6]='\n';
	//printf("sending: ");
	//pak_print(p);
	return write(fd,b,7);
	
}

pak pak_rx(int fd){
	pak p;
	char buf[16];
	char read_buf;
	int i=0;
	int read_res=0;
	clock_t now,then;
	then=clock();
	now=then;
	while(1){
		read_res=read(fd,&read_buf,1);
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
	p=new_p_pak(buf[i-7],buf+i-6);
	//printf("recived: ");
	//pak_print(p);
	return p;
}


int comm_prot(int fd,char* tx,char* rx){
	pak p;
	int idx=0;
	int k;
	char cs[4]={0,0,0,0};
	int f=1;
	
	if(!strlen(tx))return 0;
	
	for(int i=0;i<256;i++){
		if(tx[i]=='\n'){
			tx[i]=0;
			break;
		}
	}

	input_clean(fd);
	
	
	for(k=0;k<3;k++){
		p=new_h_pak(CON_REQ);
		pak_tx(fd,p);
		p=pak_rx(fd);
		if(p.header!=CON_ACC)
			return -1;
			
		while(f){
			p=new_p_pak(PAYLOAD,tx+idx);
			pak_tx(fd,p);
			for(int i=0;i<4;i++){
				cs[i]+=p.payload[i];
				if(!p.payload[i])
					f=0;
			}
			idx+=4;
		}
			
		p=new_p_pak(CHECKSUM,cs);
		pak_tx(fd,p);
			
			
		p=pak_rx(fd);
		if(p.header==ACK)
			break;
		else if(p.header==NACK){
			continue;
		}else return -1;
	}
	if(k>=3)return -1;
	
	f=1;
	for(k=0;k<3&&f;k++){
		for(int i=0;i<4;i++){
			cs[i]=0;
		}
			
		idx=0;
		while(1){
			p=pak_rx(fd);
			if(p.header==CON_REF)
				return -1;
			if(p.header==PAYLOAD){
				for(int j=0;j<4;j++){
					rx[idx+j]=p.payload[j];
					cs[j]+=p.payload[j];
				}
				idx+=4;
			}else if(p.header==CHECKSUM){
				for(int j=0;j<4;j++){
					if(cs[j]!=p.payload[j]){
						p=new_h_pak(NACK);
						pak_tx(fd,p);
						continue;
					}
				}
				p=new_h_pak(ACK);
				pak_tx(fd,p);
				f=0;
				break;
			}else
				return -1;
		}
	}
	if(k>=3)return -1;

		
	p=pak_rx(fd);
	if(p.header==CON_REF)
	return 0;
	else
	return -1;
}
////////////////////////////////
