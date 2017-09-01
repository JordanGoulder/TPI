/**
 * \file device.h
 *
 * \brief Device configuration
 *
 * This file initialises the configuration of the device intendend to be programmed by TPI. Supports ATtiny4/5/9/10/20/40.
 *
 * Copyright (C) 20011 Atmel Corporation. All rights reserved.
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
#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED

   
/* TPI KEY */

#define NVM_PROGRAM_ENABLE 0x1289AB45CDD888FF

/* TPI Instruction Set */

#define SLD 		0b00100000
#define SLD_POSTINC 0b00100100
#define SST 		0b01100000
#define SST_POSTINC 0b01100100
#define SSTPR 		0b01101000
#define SIN 		0b00010000
#define SOUT 		0b10010000
#define SLDCS 		0b10000000
#define SSTCS 		0b11000000
#define SKEY 		0b11100000

/* TPI Control and Status Registers */

#define TPISR    0b00000000
#define TPIPCR   0b00000010
#define TPIIR	 0b00001111

/* NVM Commands */
#define NVMCMD_NO_OPERATION		0x00
#define NVMCMD_CHIP_ERASE 		0x10
#define NVMCMD_SECTION_ERASE 	0x14
#define NVMCMD_MEMORY_WRITE		0x1D

#if defined ATTINY10
	#define words_to_write 	1
#elif defined ATTINY20
  #define words_to_write	2
#elif defined ATTINY40
	#define words_to_write	4
#endif 

/* Memory space definitions */
#define IO_SPACE_START	0x0000
#define IO_SPACE_END 	0x003F	
#define SRAM_DATA_MEMORY_START	0x0040 
#define SRAM_DATA_MEMORY_END	0x005F	
#define NVM_LOCK_BITS_START	0x3F00
#define NVM_LOCK_BITS_SIZE 2
#define CONF_BITS_START	0x3F40
#define CONF_BITS_SIZE 2
#define CALIB_BITS_START	0x3F80 
#define CALIB_BITS_SIZE	2
#define DEV_ID_START	0x3FC0
#define DEV_ID_SIZE	3 
#define NVM_PROG_MEM_START	0x4000
 
#if defined ATTINY10
	#define NVM_PROG_MEM_END	0x43FF
#elif defined ATTINY20
	#define NVM_PROG_MEM_END	0x47FF
#elif defined ATTINY40
	#define NVM_PROG_MEM_END	0x4FFF
#endif 
#define FLASH_PAGE_SIZE 16

#define GAURD_TIME_128 0
#define GAURD_TIME_64 1
#define GAURD_TIME_32 2
#define GAURD_TIME_16 3
#define GAURD_TIME_8 4
#define GAURD_TIME_4 5
#define GAURD_TIME_2 6
#define GAURD_TIME_0 7
#endif /* DEVICE_H_INCLUDED*/
