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

static unsigned tty1_overruns;

/* uart1 is the console (tty1). During startup, polling mode is used.
 * When the ISRs have been configured, it is switched to ringbuffer mode
*/

/* Mapping functions for polled operation
*/
dv_boolean_t uart1_putc(int c)
{
	dv_stm32_uart_putc(&dv_uart1, c);
	return 1;
}

int uart1_getc(void)
{
	return dv_stm32_uart_getc(&dv_uart1);
}

dv_boolean_t uart1_isrx(void)
{
	return dv_stm32_uart_isrx(&dv_uart1);
}

dv_boolean_t uart1_istx(void)
{
	return dv_stm32_uart_istx(&dv_uart1);
}

/* Mapping functions for console in ringbuffer mode
*/
dv_boolean_t tty1_istx(void)
{
	return !dv_rb_full(&tty1_tx_rbm);
}

dv_boolean_t tty1_putc(int c)
{
	while ( !dv_rb_u8_put(&tty1_tx_rbm, tty1_tx_rb, (dv_u8_t)c) )
	{
		/* Wait. Enable the tx interrupt in case it got disabled.
		*/
		dv_uart1.cr[0] |= DV_UART_TXEIE;
	}
	dv_uart1.cr[0] |= DV_UART_TXEIE;
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
void tty1_init(void)
{
	/* Initialise the ringbuffers
	*/
	tty1_rx_rbm.head = 0;
	tty1_rx_rbm.tail = 0;
	tty1_rx_rbm.length = 256;

	tty1_tx_rbm.head = 0;
	tty1_tx_rbm.tail = 0;
	tty1_tx_rbm.length = 256;

	/* Configure the console for interrupt-driven operation
	*/
	dv_consoledriver.putc = tty1_putc;
	dv_consoledriver.getc = tty1_getc;
	dv_consoledriver.istx = tty1_istx;
	dv_consoledriver.isrx = tty1_isrx;

	/* Enable the uart tx and rx interrupts
	*/
	dv_uart1.cr[0] |= (DV_UART_RXNEIE | DV_UART_TXEIE);

	/* Enable the uart IRQ at the NVIC
	*/
	dv_enable_irq(dv_irq_usart1);
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
			/* Character discarded. Count the overruns and inform the data gathering task
			*/
			if ( tty1_overruns < 0xffffffff )
				tty1_overruns++;
#if 0
			(void)dv_setevent(XXXXX, ev_tty1_overrun);
#endif
		}
#if 0
		if ( c == '\n' )
		{
			/* Full line received. Inform the command interpreter
			*/
			(void)dv_setevent(XXXXX,ev_tty1_rxline);
		}
#endif
	}

	while ( dv_stm32_uart_istx(&dv_uart1) )
	{
		int c = dv_rb_u8_get(&tty1_tx_rbm, tty1_tx_rb);
		if ( c < 0 )
		{
			dv_uart1.cr[0] &= ~DV_UART_TXEIE;
			break;
		}
		else
		{
			dv_stm32_uart_putc(&dv_uart1, c);
		}
	}
}
