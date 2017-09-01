/**
 * \file NVM_driver.c
 *
 * \brief Driver routines for NVM operations
 *
 * This file contains fucntions necessary for NVM read and write operations
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
#include "NVM_driver.h"


/* Send address of the NVM location to be accessed via TPI */
void send_nvm_address(volatile uint16_t nvm_address)
{
	tpi_send_byte(SSTPR);//Address Low Byte
	tpi_send_byte(((uint8_t)nvm_address & 0x00FF));
	tpi_send_byte(SSTPR | 0X01);//Address High Byte
	tpi_send_byte((uint8_t)(nvm_address >>8));
}

/* Send NVM Command to be exectued via TPI */
void send_nvm_command(uint8_t nvm_command)
{
	tpi_send_byte(SOUT| ((NVMCMD & 0XF0)<<1) | (NVMCMD & 0X0F));
	tpi_send_byte(nvm_command);
}

/* Read one byte from NVM via TPI */
uint8_t read_nvm_byte(void)
{
uint8_t nvm_byte;
	tpi_send_byte(SLD_POSTINC);
	tpi_idle(idle_clock);
	nvm_byte = tpi_receive_byte();
	PORTD |=(1<<TPI_CLK)|(1<<TPI_DATA_TX);
	return nvm_byte;
}

/* Write one byte to NVM  via TPI */
void write_nvm_word(volatile uint16_t nvm_byte)
{
	tpi_send_byte(SST_POSTINC);
	tpi_send_byte(nvm_byte & 0X00FF);
	tpi_send_byte(SST_POSTINC);
	tpi_send_byte(nvm_byte >> 8);
 
}

