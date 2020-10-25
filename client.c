#include <stdio.h>    
#include <unistd.h>   
#include <fcntl.h>    
#include <termios.h>   
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WAIT 2*CLOCKS_PER_SEC
#define CON_REQ 1
#define CON_ACC	2
#define CON_REF 3
#define PAYLOAD 4
#define CHECKSUM 5
#define ACK 6
#define NACK 7


typedef struct{
	char header;
	char payload[4];
}pak;


int fd;

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

//this function initialize the serial comm (i found it on web and modified it)
int serialport_init(const char* serialport){
    
    struct termios toptions;
    int fd= open(serialport, O_RDWR | O_NONBLOCK );
    
    if (fd == -1) return -1;
    
    if (tcgetattr(fd, &toptions) < 0) return -1;
    
    speed_t brate=B19200;
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

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
    
    tcsetattr(fd, TCSANOW, &toptions);
    if( tcsetattr(fd, TCSAFLUSH, &toptions) < 0) return -1;
	
    return fd;
}

int input_clean(){
	char a[256];
	return read(fd,a,256);
}

int pak_tx(pak p){
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

pak pak_rx(){
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


int comm_prot(){
	pak p;
	int idx=0;
	int k;
	char cs[4]={0,0,0,0};
	char tx[256]="";
	char rx[256]="";
	int f=1;
		
	printf("cmd$>");
	fgets(tx,256,stdin);
		
	for(int i=0;i<256;i++){
		if(tx[i]=='\n'){
			tx[i]=0;
			break;
		}
	}

	input_clean();
	
	if(!strcmp(tx,"q")||!strcmp(tx,"quit")){
		printf("bye :)\n");
		exit(0);
	}
	
	for(k=0;k<3;k++){
		p=new_h_pak(CON_REQ);
		pak_tx(p);
		p=pak_rx();
		if(p.header!=CON_ACC)
			return -1;
			
			
		while(f){
			p=new_p_pak(PAYLOAD,tx+idx);
			pak_tx(p);
			for(int i=0;i<4;i++){
				cs[i]+=p.payload[i];
				if(!p.payload[i])
					f=0;
			}
			idx+=4;
		}
			
		p=new_p_pak(CHECKSUM,cs);
		pak_tx(p);
			
			
		p=pak_rx();
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
			p=pak_rx();
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
						pak_tx(p);
						continue;
					}
				}
				p=new_h_pak(ACK);
				pak_tx(p);
				f=0;
				break;
			}else
				return -1;
		}
	}
	if(k>=3)return -1;

	printf("%s\n",rx);
		
	p=pak_rx();
	if(p.header==CON_REF)
	return 1;
	else
	return -1;
}


//////////////////MAIN///////////////////////////////
int main(){

	printf("Starting client\n");
	fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("connection established\n");
	
	while(1){
		int a=comm_prot();
		if(a<0)printf("comunication error\n");
	}
	
	return 0;
}
/////////////////////////////////////////////////
