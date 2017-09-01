/**
 * \file usart_driver.c
 *
 * \brief Driver routines for USART Communications
 *
 * This file contains  USART0 & 1 fucntions necessary to be used for TPI and AVROSP communicstions
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include "usart_driver.h"


/* Initialize USART for TPI Communication */
void tpi_comm_init()
{	
	DDRD |= (1<<TPI_CLK);
	/* Set TPI Clock frequency */
	UBRR1H =  (TPI_BAUD>>8);                  
	UBRR1L =  TPI_BAUD;
	/* Synchronous USART, Frame format is ST,8,E,SP1,SP2 */
	UCSR1C =(1<<UMSEL10)|(1<< USBS1) |(1<<UPM11)|(1<<UCSZ11)|(1<<UCSZ10)|(1<<UCPOL1);

}

/* Receive one byte via TPI , return the received value */
uint8_t tpi_receive_byte( void )
{
uint8_t timeout=100,rx_data;
	tpi_transmit_disable();
	tpi_receive_enable();
	// Wait for incoming data
	while(!(UCSR1A & (1<<RXC1)))
	{
		if(!timeout--) break;
	}
	rx_data=UDR1;
	return rx_data;  
}

/*Send one byte via TPI 
** tx_data : data to be sent 
*/
void tpi_send_byte( uint8_t tx_data )
{
	tpi_receive_disable();
	tpi_transmit_enable();
	while (PIND & (1 << TPI_CLK));
	while (!(PIND & (1 << TPI_CLK)));

	while (!(UCSR1A & (1<<UDRE1)));  // Wait for empty transmit buffer    
	UCSR1A|=(1<<TXC1);            
	UDR1 = tx_data;                   // Start transmission
}

/* Initialise UART for AVROSP communication*/
void avrosp_comm_init(void)
{
	DDRD |= (1<<AVROSP_TXD);
	/* 	AVROSP baud rate */
	UBRR0H =  (AVROSP_BAUD>>8);                  
	UBRR0L =  AVROSP_BAUD;
	/* Asynchronous USART , Frame format is ST,8,SP1 */
 	UCSR0C =(1<<UCSZ01)|(1<<UCSZ00);
	/* enable receive and transmit */
 	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);  

}

/* Send one byte to AVROSP 
** tx_data : data to be sent via UART 
*/
void avrosp_send_byte(char tx_data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));  
	UCSR0A|=(1<<TXC0);            
	/* Start Transmission */
	UDR0 = tx_data;         
}

/* Receive one byte from AVROSP, return the received value */
uint8_t avrosp_receive_byte( void )
{
	volatile uint8_t rx_data;
	/* Wait for incoming data*/
	while(!(UCSR0A & (1<<RXC0)));
	rx_data=UDR0;
	return rx_data;                    
}

