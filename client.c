#include <stdio.h>    
#include <unistd.h>   
#include <fcntl.h>    
#include <termios.h>   
#include <time.h>


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
	now=then=clock();
	while((now-then)<pause)
	now=clock();
	return;
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

pak pak_creation(char header,char* payload){
	pak p;
	p.header=header;
	for(int i=0;i<4;i++){
		p.payload[i]=payload[i];
	}
	return p;
}


int pak_tx(pak p){
	char b[7];
	char* pa=(char*)&p;
	for(int i=0;i<5;i++){
		b[i]=pa[i];
	}
	b[5]='\r';
	b[6]='\n';
	return write(fd,b,7);
	
}

pak pak_rx(){
	pak p;
	char buf[32];
	char read_buf;
	int i=0;
	int read_res=0;
	while(1){
		read_res=read(fd,&read_buf,1);
		if(read_res){
			buf[i]=read_buf;
			i++;
		}
		if(i>=7&&(buf[i-1]==10||buf[i-1]==13)&&(buf[i-2]==10||buf[i-2]==13))break;
	}
	p.header=buf[i-7];
	for(int j=0;j<4;j++){
		p.payload[j]=buf[i-6+j];
	}
	return p;
}


void pak_print(pak p){
	printf("<h:%d p:%d %d %d %d>\n",p.header,p.payload[0],p.payload[1],p.payload[2],p.payload[3]);
	return;
}


int main(){

	printf("Starting client\n");
	
	fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("connection established\n");
	
	while(1){
		char tx[256]="";
		printf("cmd$>");
		fgets(tx,256,stdin);
		for(int i=0;i<256;i++){
			if(tx[i]=='\n'){
				tx[i]=0;
				break;
			}
		}
		pak p;
		p=pak_creation(tx[0],tx+1);
		
		printf("sending: ");
		pak_print(p);
		pak_tx(p);
		
		
		p=pak_rx();
		printf("recived: ");
		pak_print(p);
		
		
		
	}

	return 0;
}
