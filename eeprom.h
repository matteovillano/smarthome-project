#pragma once

void EEPROM_write(uint16_t address,uint8_t data);
uint8_t EEPROM_read(uint16_t address);
void EEPROM_seq_write(uint16_t address,uint8_t* data,uint8_t len);
void EEPROM_seq_read(uint16_t address,uint8_t* buf,uint8_t len);

