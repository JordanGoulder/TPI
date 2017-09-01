/**
 * \file protocol_avr911.c
 *
 * \brief AVROSP protcol implementation
 *
 * This file provides compatibility to AVROSP protocol described in application note AVR911, 
 * decodes the AVROSP commands and calls the appropriate TPI functions
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

#include "protocol_avr911.h"
#include "RC_Calib.h"
#include "device.h"


/* Temporary global variables used in protocol */
volatile uint16_t address,flash_data,flash_address;
uint8_t high_byte,low_byte;
uint8_t prog_cmd[4];
uint8_t k=0,autoincrement=0;

/* Decipher the Universal Command for AVR ISP sent by AVROSP(AVR911) 
** 0x38 0x00 0xXX 0X00 - Read OSCCAL
** 0xac 0xe0 0x00 data - Write Lockbyte
** 0x58 0x00 0x00 0x00 - Read Lockbyte
** 0xac 0xa0 0x00 data - Write Fuse Low
** 0xac 0xa8 0x00 data - Write Fuse High
** 0x50 0x00 0x00 0x00 - Read Fuse Low
** 0x58 0x08 0x00 0x00 - Read Fuse High 
*/

void universal_command(void)
{
	if(prog_cmd[0]==0x38)
	{
		read_calib_byte0();
		avrosp_send_byte(calib_byte_0);
	}
	else
	{
		if(prog_cmd[0]==0xac)
		{
			switch(prog_cmd[1]){
			case 0xe0:
				program_lockbyte(prog_cmd[3]);
				avrosp_send_byte(0);//dummy write for return code
				break;
			case 0xa0:
				erase_config_byte0();
				program_config_byte0(prog_cmd[3]);
				avrosp_send_byte(0);//dummy write for return code
				break;
			case 0xa8:
				avrosp_send_byte(0);//dummy write for return code
				break;
			}
		}
		else
		{
			if(prog_cmd[0]==0x50)
			{
				read_config_byte0();
				avrosp_send_byte(config_byte_0);
			}
			else
			{
				switch(prog_cmd[1]){
				case 0x00:
					read_lockbyte();
					avrosp_send_byte(lock_byte[1]);
					break;
				case 0x08:
					avrosp_send_byte(0XFF);//dummy write for high fuse byte
					break;
				}
			}
		}
	}
}

/**
 * \brief This is the protocol handler for the AVR911 protocol
 *
 * This function is called from the main application each time
 * there is a new command form the host.
 */
void avrosp_protocol(uint8_t data)
{
	/* Temporary variables used in protocol */
	uint8_t dummy_read;
  uint8_t osccal_value;

	switch (data) {
		/* Start Communication, disregard read data for compatibility*/
		case 'T' :
			dummy_read = avrosp_receive_byte();
			avrosp_send_byte('\r');
			break;
		/* Enable Programming Mode */
		case 'P' :
			tpi_enable();
			avrosp_send_byte('\r');
			break;
		/* Disable Programming Mode */
		case 'L' :
			tpi_disable();
			avrosp_send_byte('\r');
			break;
		/* 	Return 'Y' indicating support for autoincrement of address */
		case 'a' :
			avrosp_send_byte('Y');
			autoincrement=1;
			break;
		/* Set NVM Address */
		case 'A' :
			address = ((uint16_t)avrosp_receive_byte() << 8) ;
			address |= avrosp_receive_byte();
			avrosp_send_byte('\r');
			break;
		/* Get Low byte of data to be written */
		case 'c' :
			flash_data = avrosp_receive_byte();
			avrosp_send_byte('\r');
			break;
		/* Get high byte of data to be written and initiate word write */
		case 'C' :
			flash_data |= ((uint16_t)avrosp_receive_byte() << 8) & 0xFF00;
			if(autoincrement) 
			{
				flash_address = NVM_PROG_MEM_START + address;
				autoincrement=0;
			}
     program_code_word(flash_address,flash_data);
			flash_address+=2; 
			avrosp_send_byte('\r');
			break;
		/* Finshed writing one Page, send return code */
		case 'm' :			
			avrosp_send_byte('\r');
			break;
		/* Read one Word from NVM and return data to AVROSP */
		case 'R' :
			high_byte = read_code_byte(NVM_PROG_MEM_START | (address +1));
			avrosp_send_byte(high_byte);
			low_byte = read_code_byte(NVM_PROG_MEM_START| address);
			avrosp_send_byte(low_byte);
			address+=2;
			break;
		/* Initiate chip erase */
		case 'e' : 
			chip_erase();
			avrosp_send_byte('\r');
			break;	
		/* Start of 4-byte Universal Command*/
		case '.' :
			for(uint8_t i=0;i<4;i++)
			 {
			 prog_cmd[i] = avrosp_receive_byte();
			 }
			universal_command();
			avrosp_send_byte('\r');
			break;
		/* High fuse byte , provided for compatibility with AVROSP */
		case 'N' :
			avrosp_send_byte(0XFF);
			break;
		/* Return programmer identifier "AVR ISP" in this case*/
		case 'S' :
			avrosp_send_byte('A');  
			avrosp_send_byte('V');  
			avrosp_send_byte('R');
			avrosp_send_byte(' ');
			avrosp_send_byte('I');
			avrosp_send_byte('S');
			avrosp_send_byte('P');
			break;
		/* Get device ID */
		case 's' :
			read_device_id();
			avrosp_send_byte(dev_id[2]);
			avrosp_send_byte(dev_id[1]);
			avrosp_send_byte(dev_id[0]);
			break;
    /* Internal RC calibration */
    case 'Y' :
      osccal_value = RC_Calibrate();
      avrosp_send_byte(osccal_value);
      //flash_data = (uint16_t)osccal_value;
      //tpi_enable();
      //chip_erase();
      //flash_address = (OSCCAL_FLASH_ADDR+NVM_PROG_MEM_START);//(uint16_t)flash_end;
      //program_code_word(flash_address,flash_data);
      //tpi_disable();
      break;
		/* ignore if ESC */
		case 0x1b:
			break;
		default :
			avrosp_send_byte('\r');
			break;
	}
}
