#include "serial.h"

#define BUFF_SIZE 256

int comm_prot(){
	int tents;
	char buf1[BUFF_SIZE],buf2[BUFF_SIZE];
	int i=0;
	memset(buf1,0,BUFF_SIZE);
		
	printf("cmd$>");
	fgets(buf1,BUFF_SIZE,stdin);
		
	for(int i=0;i<BUFF_SIZE;i++){
		if(buf1[i]=='\n'){
			buf1[i]=0;
			break;
		}
	}
	
	if(!strcmp(buf1,"q")||!strcmp(buf1,"quit")){
		printf("exiting...");
		exit(0);
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
		if(get_string(buf2))break;
		tents--;
	}
	if(!tents)return 0;
	
	printf("recived:\n %s\n",buf2);
	return 1;
}


//////////////////MAIN///////////////////////////////
int main(){

	printf("Starting client\n");
	fd=serialport_init("/dev/ttyACM0");
	if(fd==-1){printf("can not create a connection\n");return -1;}
	printf("connection established\n");
	
	while(1){
		int a=comm_prot();
		if(!a)printf("comunication error\n");
	}
	
	return 0;
}
