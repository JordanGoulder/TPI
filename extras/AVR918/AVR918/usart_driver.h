#ifndef USART_DRIVER_H_INCLUDED
#define USART_DRIVER_H_INCLUDED

#include <avr/io.h>
#include "TPI_driver.h"

#define AVROSP_RXD PD0
#define AVROSP_TXD PD1

#define AVROSP_BAUD 5
#define TPI_BAUD 5

/* Prototypes */
void tpi_comm_init();
uint8_t tpi_receive_byte(void);
void  tpi_send_byte( uint8_t data );


void avrosp_comm_init(void);
uint8_t avrosp_receive_byte(void);
void  avrosp_send_byte( char data );


#endif /* USART_DRIVER_H_INCLUDED */
