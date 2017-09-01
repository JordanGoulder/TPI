/**
 * \file protocol_avr911.c
 *
 * \brief RC Oscillator Calibration implementation
 *
 * This file provides the routine for the internal 8MHz RC oscillator
 * calibration
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

#include "RC_Calib.h"
#include "TPI_driver.h"

/* Function for RC oscillator calibration */
uint8_t RC_Calibrate(void)
{
	/* Configure the I/O pins */
	uint8_t Temp_DDRD = DDRD;
	uint8_t Temp_PORTD = PORTD;
	uint8_t osccal_value = 0, temp;
  
  /* Disable USART1 */
  UCSR1B &= ~((1<<TXEN1)|(1<<RXEN1));
	
	/* Configure PD2 & PD3 pins as input to sense calibration status */
	DDRD &= ~((1<<TPI_DATA_TX)|(1<<TPI_DATA_RX));
  
  /* Enable pull-up on PD2 & PD3 */
  PORTD |= (1<<TPI_DATA_TX)|(1<<TPI_DATA_RX);
	
	/* Configure PD4 pin as output to provide calibration clock */
	DDRD |= (1<<TPI_CLK);
	
	/* Enable pull-up on PD2 & PD3 */
	PORTD |= ((1<<PORTD2)|(1<<PORTD3));
		
	/* Configure Timer 1 to produce 32kHz signal */
	OCR1A = 0xAD;
	TCCR1A = (1<<COM1B0);
	TCCR1B = (1<<WGM12)|(1<<CS10);
	
	while((PIND & (1<<PIND2))!=0);
	
	TCCR1B = 0x00;
	TCCR1A = 0x00;
  
	PORTD |= (1<<PORTD4);
	_delay_ms(1);
	
  for(uint8_t i=0; i<8 ;i++)
  {
    PORTD &= ~(1<<PORTD4);
    _delay_us(100);
    PORTD |= (1<<PORTD4);
    temp = (PIND<<5) & 0x80;
    osccal_value |= (temp>>i);
  }
	
  /* Enable USART1 */
  UCSR1B |= ((1<<TXEN1)|(1<<RXEN1));
  
	DDRD = Temp_DDRD;
	PORTD = Temp_PORTD;
	
	return(osccal_value);
}