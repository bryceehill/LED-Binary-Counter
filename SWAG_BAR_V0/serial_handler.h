/*
 * serial_handler.h
 *
 *  Created on: Mar 3, 2016
 *      Author: BHill
 */

#ifndef REELS_SERIAL_HANDLER_H_
#define REELS_SERIAL_HANDLER_H_
#define uart_max 50
extern unsigned char tx_data_str[uart_max], rx_data_str[uart_max],rx_flag ,dec_str[7],eos_flag;
extern char dec_char[6];
void uart_init(int);
void uart_write_string(int,int);
char uart_get_char(int);
void uart_set_char(char,int);
void conv_hex_dec(int);
void unsigned_conv_hex_dec(int);
int conv_dec_hex (void);
//void uart_write_fast_string(int, int);


#endif /* REELS_SERIAL_HANDLER_H_ */
