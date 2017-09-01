#ifndef NVM_DRIVER_H_INCLUDED
#define NVM_DRIVER_H_INCLUDED

#include <avr/io.h>
#include "TPI_driver.h"
#include "usart_driver.h"

/* define NVM Registers */ 
#define NVMCSR 0X32
#define NVMCMD 0X33

/* Prototypes */
void send_nvm_address(uint16_t);
void send_nvm_command(uint8_t);
uint8_t read_nvm_byte(void);
void write_nvm_word(uint16_t);


#endif /* NVM_DRIVER_H_INCLUDED*/
