#include "../offline-shell/serial.h"
#include "sock.h"
 
#define BUFF_SIZE 256

int comm_prot(int sock){
	int tents;
	char buf1[BUFF_SIZE],buf2[BUFF_SIZE];
	int i=0;
	memset(buf1,0,BUFF_SIZE);
	
	int read_res=read(sock,buf1,BUFF_SIZE);
		
	if(!read_res){
		return 0;
	}
	
	for(int i=0;i<BUFF_SIZE;i++){
		if(buf1[i]=='\n'){
			buf1[i]=0;
			break;
		}
	}
	printf("recived from server:\n%s\n",buf1);
	
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(send_string(buf1))
			break;
		tents--;
	}
	if(!tents){
		sprintf(buf2,"comunication error");
		write(sock,buf2,strlen(buf2));
		write(sock,"\r\n",2);
		printf("sent to client:\n%s\n",buf2);
		return 1;
	}
	
	
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(get_string(buf2))
			break;
		tents--;
	}
	if(!tents){
		sprintf(buf2,"comunication error");
		write(sock,buf2,strlen(buf2));
		write(sock,"\r\n",2);
		printf("sent to client:\n%s\n",buf2);
		return 1;
	}
	
	write(sock,buf2,strlen(buf2));
	write(sock,"\r\n",2);
	printf("sent to client:\n%s\n",buf2);
		
	return 1;
}


//////////////////MAIN///////////////////////////////
int main(){

	printf("Starting client\n");
	fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("serial connection established\n");
	
	int s1=websockinit();
	if(s1<0){
		printf("con not inizialize the web socket\n");
		return -1;
	}
	while(1){
		//starting server socket
		printf("Waiting for a client...\n"); 
		int sock=sockaccept(s1);
		if(sock==-1){printf("can not create the server socket\n");return -1;}
		printf("Socket connection established\n");
		while(1){
			int a=comm_prot(sock);
			if(!a){
				printf("client disconnected\n");
				break;
			}
		}
	}
	
	return 0;
}

