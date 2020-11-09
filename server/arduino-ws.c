#include "serial-comm.h"
#include "sock.h"
 
#define BUFF_SIZE 256

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
