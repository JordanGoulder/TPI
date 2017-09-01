#ifndef TPI_DRIVER_H_INCLUDED
#define TPI_DRIVER_H_INCLUDED

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include <math.h>
#include "NVM_driver.h"
#include "usart_driver.h"
#include "device.h"

/* TPI PINS Configuration */

#define TPI_CLK PD4
#define TPI_RESET PD5
#define TPI_DATA_TX PD3
#define TPI_DATA_RX PD2

volatile uint8_t idle_clock;


/* NVM Read outs */
volatile uint8_t dev_id[DEV_ID_SIZE];
volatile uint8_t lock_byte[NVM_LOCK_BITS_SIZE];
volatile uint8_t config_byte_0;
volatile uint8_t calib_byte_0;
volatile uint8_t tpi_id;
volatile uint8_t tpi_status;
volatile uint8_t gaurd_time;

static inline void hold_reset_high(void)
{
	PORTD |=(1<<TPI_RESET);
}
static inline void hold_reset_low(void)
{
	PORTD &=~(1<<TPI_RESET);
}
static inline void hold_clock_high(void)
{
	PORTD|=(1<<TPI_CLK);
}
static inline void hold_data_high(void)
{
	PORTD |=(1<<TPI_DATA_TX);
}

/* Enable UART receiver  */
inline void tpi_receive_enable(void)
{	
	UCSR1B |= (1<<RXEN1);
}
/* Disable TPI receiver */
inline void tpi_receive_disable(void)
{
	UCSR1B &=(~(1<<RXEN1));
}
/* Enable TPI transmitter */
inline void tpi_transmit_enable(void)
{
	UCSR1B |= (1<<TXEN1);
}
/* Disable TPI receiver */
inline void tpi_transmit_disable(void)
{
	UCSR1B &=(~(1<<TXEN1));
}
	
/* Prototypes */

void read_tpi_status(void);
void send_skey(uint64_t nvm_key);
void set_gaurd_time(uint8_t time);
void read_gaurd_time(void);
void read_tpi_id(void);
void tpi_enable(void);
void read_device_id(void);
void read_lockbyte(void);
void program_lockbyte(uint8_t);
void read_config_byte0(void);
void program_config_byte0(uint8_t);
void erase_config_byte0(void);
void read_calib_byte0(void);
void program_code_word(uint16_t,uint16_t);
uint8_t read_code_byte(uint16_t);
void erase_code_section(void);
void chip_erase(void);
bool is_nvm_busy(void);
void tpi_disable(void);
void tpi_init(void);
void tpi_idle(uint8_t);

#endif /* TPI_DRIVER_H_INCLUDED*/
