#include "cmd_line.h"

char* toks[NUMTOKS];
char out_buf[BUFFLEN];

uint8_t help(void){
	strcpy(out_buf,
	"all commands are described in presentazione.pdf in the current folder");
	return CMDSUCCESS;
}

uint8_t on(void){
	uint8_t o=get_out(toks[1]);
	if(o==NO_DEV){
		strcpy(out_buf,"no output found");
		return CMDFAIL;
	}
	if(fix_out_val(MAXVAL,o)){
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	else
		strcpy(out_buf,"error");
	return CMDFAIL;
}

uint8_t off(void){
	uint8_t o=get_out(toks[1]);
	if(o==NO_DEV){strcpy(out_buf,"no output found");return CMDFAIL;}
	if(fix_out_val(0,o)){
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	else
		strcpy(out_buf,"error");
	return CMDFAIL;
}

uint8_t connect(void){
	uint8_t d=1;
	uint8_t i=get_di(toks[1]);
	if(i==NO_DEV){i=get_ai(toks[1]); d=0;}
	if(i==NO_DEV){strcpy(out_buf,"no input found"); return CMDFAIL;}
	uint8_t o=get_out(toks[2]);
	if(d){
		if(create_d_con(i,o)){
			strcpy(out_buf,"done");
			return CMDSUCCESS;
		}
		else{
			strcpy(out_buf,"can not create a connection");
			return CMDFAIL;
		}
	}else{
		if(create_a_con(i,o)){
			strcpy(out_buf,"done");
			return CMDSUCCESS;
		}
		else{
			strcpy(out_buf,"can not create a connection");
			return CMDFAIL;
		}
	}
	return CMDFAIL;
}

uint8_t disconnect(void){
	uint8_t o=get_out(toks[1]);
	if(o==NO_DEV){
		strcpy(out_buf,"no output found");
		return CMDFAIL;
	}
	if(delete_con(o)){
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}else
		strcpy(out_buf,"error");
	return CMDFAIL;
}

uint8_t show(void){
	char b1[16];
	if(!strcmp(toks[1],"output")){
		strcat(out_buf,"outputs:\n");
		for(uint8_t i=0;i<8;i++){
			get_out_name(b1,i);
			strcat(out_buf,b1);
			strcat(out_buf,"\n");
		}
		strcat(out_buf,"end");
		return CMDSUCCESS;
	}else if(!strcmp(toks[1],"digital")){
		strcat(out_buf,"digital inputs:\n");
		for(uint8_t i=0;i<8;i++){
			get_di_name(b1,i);
			strcat(out_buf,b1);
			strcat(out_buf,"\n");
		}
		strcat(out_buf,"end");
		return CMDSUCCESS;
	}else if(!strcmp(toks[1],"analog")){
		strcat(out_buf,"analog inputs:\n");
		for(uint8_t i=0;i<8;i++){
			get_ai_name(b1,i);
			strcat(out_buf,b1);
			strcat(out_buf,"\n");
		}
		strcat(out_buf,"end");
		return CMDSUCCESS;
	}else if(!strcmp(toks[1],"connection")){
		strcat(out_buf,"connetion:\n");
		for(uint8_t i=0;i<8;i++){
			uint16_t s=get_con(i);
			if(s>0){
				if(s<1024)
					strcat(out_buf,"on: ");
				else if(s&(1<<10)){
					get_di_name(b1,s&0x0f);
					strcat(out_buf,b1);
					strcat(out_buf," -> ");
				}else if(s&(1<<11)){
					get_ai_name(b1,s&0x0f);
					strcat(out_buf,b1);
					strcat(out_buf," -> ");
				}
				get_out_name(b1,i);
				strcat(out_buf,b1);
				strcat(out_buf,"\n");
			}
		}
		strcat(out_buf,"end");
		return CMDSUCCESS;
	}else
		strcpy(out_buf,"no show cmd found");
	return CMDFAIL;
}

uint8_t set(void){
	uint8_t d;
	d=get_out(toks[1]);
	if(d!=NO_DEV){
		set_out_name(toks[2],d);
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	d=get_di(toks[1]);
	if(d!=NO_DEV){
		set_di_name(toks[2],d);
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	d=get_ai(toks[1]);
	if(d!=NO_DEV){
		set_ai_name(toks[2],d);
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	strcpy(out_buf,"no device found");
	return CMDFAIL;
}

uint8_t reset(void){
	if(!strcmp(toks[1],"name")){
		set_default_name();
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}else if(!strcmp(toks[1],"connection")){
		for(uint8_t i=0;i<8;i++){
			delete_con(i);
		}
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}else if(!strcmp(toks[1],"all")){
		set_default_name();
		for(uint8_t i=0;i<8;i++){
			delete_con(i);
		}
		strcpy(out_buf,"done");
		return CMDSUCCESS;
	}
	strcpy(out_buf,"no reset cmd found");
	return CMDFAIL;
}

uint8_t select_cmd(void){
	
	if(!strcmp(toks[0],"help")){
		help();	
	}
	else if(!strcmp(toks[0],"on")){
		on();
	}
	else if(!strcmp(toks[0],"off")){
		off();
	}
	else if(!strcmp(toks[0],"connect")){
		connect();
	}
	else if(!strcmp(toks[0],"disconnect")){
		disconnect();
	}
	else if(!strcmp(toks[0],"show")){
		show();
	}
	else if(!strcmp(toks[0],"set")){
		set();
	}
	else if(!strcmp(toks[0],"reset")){
		reset();
	}else return 0;
	
	return 1;
}


char* cmd(char* buf){
	
	memset(toks,0,sizeof(char*)*NUMTOKS);
	memset(out_buf,0,BUFFLEN);
	
	toks[0]=strtok(buf," ");
	for(uint8_t i=1;i<NUMTOKS;i++){
		toks[i]=strtok(NULL," ");
		if(toks[i]==NULL)
			break;
	}
	
	if(select_cmd()==CMDFAIL)
		strcpy(out_buf,"no cmd found, type help");
	
	return out_buf;
}

