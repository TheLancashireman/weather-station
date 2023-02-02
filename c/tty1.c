/* tty1.c - uart1/tty1 handling for weather-station
 *
 * Copyright David Haworth
 *
 * This file is part of weather-station.
 *
 * weather-station is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * weather-station is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with weather-station.  If not, see <http://www.gnu.org/licenses/>.
*/
#define DV_ASM	0
#include <dv-config.h>
#include <weather-station.h>
#include <ws-blue-pill.h>
#include <dv-stm32-uart.h>
#include <dv-ringbuf.h>

static dv_rbm_t tty1_rx_rbm;
static dv_u8_t tty1_rx_rb[256];

static dv_rbm_t tty1_tx_rbm;
static dv_u8_t tty1_tx_rb[256];

/* uart1 is the console (tty1). During startup, polling mode is used.
 * When the ISRs have been configured, it is switched to ringbuffer mode
*/

/* Mapping functions for polled operation
*/
int uart1_putc(int c)
{
	dv_stm32_uart_putc(&dv_uart1, c);
	return 1;
}

int uart1_getc(void)
{
	return dv_stm32_uart_getc(&dv_uart1);
}

int uart1_isrx(void)
{
	return dv_stm32_uart_isrx(&dv_uart1);
}

int uart1_istx(void)
{
	return dv_stm32_uart_istx(&dv_uart1);
}

/* uart1_polled_init() - initialise uart1/console for polled operation
*/
void uart1_polled_init(void)
{
	(void)dv_stm32_uart_init(&dv_uart1, 115200, "8N1");
	dv_consoledriver.putc = uart1_putc;
	dv_consoledriver.getc = uart1_getc;
	dv_consoledriver.istx = uart1_istx;
	dv_consoledriver.isrx = uart1_isrx;
}

/* Mapping functions for console in ringbuffer mode
*/
dv_boolean_t tty1_istx(void)
{
	return !dv_rb_full(&tty1_tx_rbm);
}

dv_boolean_t tty1_putc(int c)
{
	while ( !dv_rb_u8_put(&tty1_tx_rbm, tty1_rx_rb, (dv_u8_t)c) )
	{
		/* Wait */
	}
	return 1;
}

dv_boolean_t tty1_isrx(void)
{
	return !dv_rb_empty(&tty1_rx_rbm);
}

int tty1_getc(void)
{
	return dv_rb_u8_get(&tty1_rx_rbm, tty1_rx_rb);
}

/* tty1_init() - initialise uart1 for buffer handling
 *
 * uart1 has already been initialised for the console, but the console tx is operating in polled mode
 * and the console rx isn't much used.
 *
 * This function initializes the buffer handling for both tx and rx and switches the console pointers
*/
void tty1_Init(void)
{
	tty1_rx_rbm.head = 0;
	tty1_rx_rbm.tail = 0;
	tty1_rx_rbm.length = 256;

	tty1_tx_rbm.head = 0;
	tty1_tx_rbm.tail = 0;
	tty1_tx_rbm.length = 256;
}

/* main_Itty1() - body of ISR to handle uart1 interrupt
*/
void main_Itty1(void)
{
	while ( dv_stm32_uart_isrx(&dv_uart1) )
	{
		dv_u8_t c = (dv_u8_t)dv_stm32_uart_getc(&dv_uart1);
		if ( !dv_rb_u8_put(&tty1_rx_rbm, tty1_rx_rb, c) )
		{
			/* Character discarded. Report overrun error?
			*/
		}
	}

	while ( dv_stm32_uart_istx(&dv_uart1) )
	{
		int c = dv_rb_u8_get(&tty1_tx_rbm, tty1_tx_rb);
		if ( c < 0 )
		{
			break;
		}
		else
		{
			dv_stm32_uart_putc(&dv_uart1, c);
		}
	}
}
