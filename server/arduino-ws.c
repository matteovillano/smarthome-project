#include "serial.h"
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
	
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(send_string(buf1))
			break;
		tents--;
	}
	if(!tents)return 0;
	
	tents=NUMRETRASMTENTS;
	while(tents>0){
		if(get_string(buf2))
			break;
		tents--;
	}
	if(!tents)return 0;
	
	write(sock,buf2,strlen(buf2));
	write(sock,"\r\n",2);
	printf("sended to client: %s\n",buf2);
		
	return 1;
}


//////////////////MAIN///////////////////////////////
int main(){

	printf("Starting client\n");
	fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("connection established\n");
	
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

/*
//////////////////MAIN///////////////////////////////
int main(){
	char buff1[BUFF_SIZE],buff2[BUFF_SIZE];
	bzero(buff1,BUFF_SIZE);
	bzero(buff2,BUFF_SIZE);
	//starting connection with arduino
	printf("Starting arduino web server...\n");
	int fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("connection with arduino established\n");
	
	int s1=websockinit();
	if(s1<0){return -1;}
	
	while(1){
	//starting server socket
	printf("Waiting for a client...\n"); 
	int sock=sockaccept(s1);
	if(sock==-1){printf("can not create the server socket\n");return -1;}
	printf("Socket connection established\n");
	
	
	while(1){
		
		//receving string from client
		int read_res=read(sock,buff1,BUFF_SIZE);
		
		if(!read_res){
			printf("Client disconnected...\n");
			break;
		}
	
		printf("recived from client: %s",buff1);
		
		//comunicating with arduino
		int a=comm_prot(fd,buff1,buff2);
		if(a<0)strcpy("comunication error",buff2);
		
		//sending string to client
		write(sock,buff2,strlen(buff2));
		write(sock,"\r\n",2);
		printf("sended to client: %s\n",buff2);
		
		//buffer cleaning
		bzero(buff1,BUFF_SIZE);
		bzero(buff2,BUFF_SIZE);
	}
	}
	return 0;
}
*/
