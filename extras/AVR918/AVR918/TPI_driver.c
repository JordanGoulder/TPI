/**
 * \file TPI_driver.c
 *
 * \brief Driver routines for TPI Communication
 *
 * This file contains fucntions necessary for TPI Communications
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
#include "TPI_driver.h"


volatile uint8_t nvm_status =0X80;


/* Initialise Port pins for TPI communication */
void tpi_init(void)
{
	DDRD = (1<<TPI_RESET)|(1<<TPI_CLK)|(1<<TPI_DATA_TX);
	PORTD |=(1<<TPI_RESET)|(1<<TPI_CLK);
}

/* Send Serial KEY signaling instruction, followed by the Enable Key 
** for NVM programming 
** nvm_key : variable to hold NVM_PROGRAM_ENABLE for transfer */
void send_skey(uint64_t nvm_key)
{
	tpi_send_byte(SKEY);
	while(nvm_key)
	{
		tpi_send_byte(nvm_key & 0xFF);
		nvm_key>>=8;
	}
}
/* Read TPI Status Register, if NVM is enabled it should return 0X02 (NVMEN set),
** else will return 0X00  */
void read_tpi_status(void)
{
	tpi_send_byte(SLDCS | TPISR);
	tpi_idle(idle_clock);
 	tpi_status = tpi_receive_byte();
}

/* Write GT[2:0] bits in TPI Physical Layer Control Register, This defines the 
** IDLE time required when changing from reception more to transmission mode. 
** time : Gaurd time value to be programmed 
*/
void set_gaurd_time(uint8_t time)
{
uint8_t powof2=1;
	tpi_send_byte(SSTCS |TPIPCR);
	tpi_send_byte(time);
	/* Calculate no of idle clock cycles required based on the gaurd time set */
	for(uint8_t i=0; i<(~time & 0x07); i++) powof2 *=2;
	idle_clock = (time==GAURD_TIME_0) ? 2 : (powof2+2);
  
}
/* Read GT[2:0] bits in TPI Physical Layer Control Register, This defines the 
** IDLE time required when changing from reception more to trasmission mode. */
void read_gaurd_time(void)
{
	tpi_send_byte(SLDCS |TPIPCR);
	tpi_idle(idle_clock);
	gaurd_time= tpi_receive_byte();
	
}
/* Read TPI Identification Register, This should return 0X80 */
void read_tpi_id(void)
{
	tpi_send_byte(SLDCS |TPIIR);
	tpi_idle(140);
	tpi_id = tpi_receive_byte();

}

/* Enable TPI Communication. This includes reading TPI ID, Setting Gaurd time, 
   enable NVM programming by sending SKEY, and checking TPI Status */
void tpi_enable()
{
  	tpi_init();
	tpi_comm_init();  
	
	hold_reset_low();
	_delay_us(90);
	hold_data_high();
	tpi_receive_disable();
	tpi_transmit_enable();
	tpi_idle(50);

	read_tpi_id();
	set_gaurd_time(GAURD_TIME_128); 
	read_gaurd_time();
	send_skey(NVM_PROGRAM_ENABLE);
	while(!tpi_status) 
	{
		read_tpi_status();
	}
	
}

/* Read 3 bytes of Device ID  */
void read_device_id(void)
{
 	for(uint8_t bytes_read = 0; bytes_read <DEV_ID_SIZE;bytes_read++)
	{
		send_nvm_command(NVMCMD_NO_OPERATION);
		send_nvm_address(DEV_ID_START + bytes_read);
		dev_id[bytes_read] = read_nvm_byte();

	}
}
/* Read Lockbyte */
void read_lockbyte(void)
{
		send_nvm_command(NVMCMD_NO_OPERATION);
		send_nvm_address(NVM_LOCK_BITS_START );
		lock_byte[1] = read_nvm_byte();

}

/* Write Lockbyte 
** lockbyte : lock byte to be set*/
void program_lockbyte(uint8_t lockbyte)
{
	send_nvm_address(NVM_LOCK_BITS_START );
	send_nvm_command(NVMCMD_MEMORY_WRITE);
	write_nvm_word(0xFF00 | lockbyte);
	while(is_nvm_busy());
}

/* Read Configuration byte ( Fuse byte) */
void read_config_byte0(void)
{
	send_nvm_command((uint8_t)NVMCMD_NO_OPERATION);
	send_nvm_address(CONF_BITS_START);
	config_byte_0 =	read_nvm_byte();

}
/* Erase Configuration Section */
void erase_config_byte0(void)
{
	send_nvm_address(CONF_BITS_START);
	send_nvm_command((uint8_t)NVMCMD_SECTION_ERASE);
	write_nvm_word(0xFFFF);
	while(is_nvm_busy());
}
/* Program Configruation byte (Fuse byte) 
** config_byte : Fuse value to be programmed
*/
void program_config_byte0(uint8_t config_byte)
{
uint8_t i=0;
	do{
		send_nvm_address(CONF_BITS_START +(i*2));
		send_nvm_command((uint8_t)NVMCMD_MEMORY_WRITE);
		write_nvm_word(0XFF00 | config_byte);
		i++;
		if(words_to_write -i) tpi_idle(12);
	}while(i<words_to_write);
	while(is_nvm_busy());

}

/* Read Calibration byte0 (OSCCAL value from production) */
void read_calib_byte0(void)
{
 	send_nvm_command((uint8_t)NVMCMD_NO_OPERATION);
	send_nvm_address(CALIB_BITS_START);
 	calib_byte_0 =	read_nvm_byte();

}

/* Write one word to program memory 
** nvm_address : Memory address to be written to
** code_word : Data to be written
*/
void program_code_word(uint16_t nvm_address,uint16_t code_word)
{
	send_nvm_address(nvm_address);
	send_nvm_command(NVMCMD_MEMORY_WRITE);
	write_nvm_word(code_word);
	if(words_to_write > 1) tpi_idle(12);
	while(is_nvm_busy());
}

/* Write one word to program memory 
** nvm_address : Memory address to be read from
*/
uint8_t read_code_byte(uint16_t nvm_address)
{	
	volatile uint8_t nvm_byte;
	send_nvm_command((uint8_t)NVMCMD_NO_OPERATION);
	send_nvm_address(nvm_address);
	nvm_byte =	read_nvm_byte();
	return nvm_byte;
}

/* Erase Code Section, this is not same as Chip Erase. */
void erase_code_section(void)
{
	send_nvm_address(NVM_PROG_MEM_START);
	send_nvm_command(NVMCMD_SECTION_ERASE);
	write_nvm_word(0XFFFF);
	while(is_nvm_busy());
}

/* Chip Erase */
void chip_erase(void)
{
	send_nvm_address(NVM_PROG_MEM_START);
	send_nvm_command(NVMCMD_CHIP_ERASE);
	write_nvm_word(0xFFFF);
	while(is_nvm_busy());
}

/* Check if NVM is busy by reading NVMCSR */
bool is_nvm_busy(void)
{
	tpi_send_byte(SIN |((NVMCSR & 0XF0)<<1) | (NVMCSR & 0X0F));
	tpi_idle(idle_clock);
	nvm_status = tpi_receive_byte();
	if(nvm_status == 0x80) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* Disable TPI */
void tpi_disable()
{
	PORTD |=(1<<TPI_RESET);//Release RESET
	UCSR1B &=(~(1<<RXEN1));
	UCSR1B &=(~(1<<TXEN1));
}	

/* Idle for the choosen gaurdtime */
void tpi_idle(uint8_t gaurd_time)
{
	for(uint8_t i=0;i<gaurd_time;i++)
	{ 
		while (PIND & (1 << TPI_CLK));
		while (!(PIND & (1 << TPI_CLK)));
	}
}
