#pragma once

//DIGITAL INPUT
void di_init(void);
uint8_t get_di_status(void);

//ADC
void an_init(void);
uint16_t read_adc(uint8_t pin);

//OUTPUT
void out_init(void);
void set_out_val(uint16_t val,uint8_t pin);
void set_out(uint8_t on_off,uint8_t pin);


/////////////////
uint8_t fix_out_val(uint16_t val,uint8_t output);
uint8_t create_d_con(uint8_t input,uint8_t output);
uint8_t create_a_con(uint8_t input,uint8_t output);
uint8_t delete_con(uint8_t output);
void refresh_output(void);
void restore_con(void);
