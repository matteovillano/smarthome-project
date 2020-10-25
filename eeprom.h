#pragma once

#define NO_DEV 0xff

void EEPROM_write(uint16_t address,uint8_t data);
uint8_t EEPROM_read(uint16_t address);
void EEPROM_seq_write(uint16_t address,uint8_t* data,uint16_t len);
void EEPROM_seq_read(uint16_t address, uint8_t* buf, uint16_t len);

//ouput
void get_out_name(char* buf,uint8_t output);
void set_out_name(char* buf,uint8_t output);
uint8_t get_out(char* buf);

//digital input
void get_di_name(char* buf,uint8_t input);
void set_di_name(char* buf,uint8_t input);
uint8_t get_di(char* buf);

//analog input
void get_ai_name(char* buf,uint8_t input);
void set_ai_name(char* buf,uint8_t input);
uint8_t get_ai(char* buf);

////
void set_default_name(void);


//connections
void set_con(uint8_t i,uint16_t status);
uint16_t get_con(uint8_t i);
