/* tty2.c - uart2/tty2 handling for weather-station
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

static dv_rbm_t tty2_rx_rbm;
static dv_u8_t tty2_rx_rb[256];

static dv_rbm_t tty2_tx_rbm;
static dv_u8_t tty2_tx_rb[256];

static unsigned tty2_overruns;

/* Mapping functions for tty2 in ringbuffer mode
*/
dv_boolean_t tty2_istx(void)
{
	return !dv_rb_full(&tty2_tx_rbm);
}

dv_boolean_t tty2_putc(int c)
{
	while ( !dv_rb_u8_put(&tty2_tx_rbm, tty2_tx_rb, (dv_u8_t)c) )
	{
		/* Wait. Enable the tx interrupt in case it got disabled.
		*/
		dv_uart2.cr[0] |= DV_UART_TXEIE;
	}
	dv_uart2.cr[0] |= DV_UART_TXEIE;
	return 1;
}

dv_boolean_t tty2_isrx(void)
{
	return !dv_rb_empty(&tty2_rx_rbm);
}

int tty2_getc(void)
{
	return dv_rb_u8_get(&tty2_rx_rbm, tty2_rx_rb);
}

/* tty2_init() - initialise uart2 for buffer handling
*/
void tty2_init(void)
{
#if 0
	tty2driver.putc = tty2_putc;
	tty2driver.getc = tty2_getc;
	tty2driver.istx = tty2_istx;
	tty2driver.isrx = tty2_isrx;
#endif

	/* Initialise the ring buffers
	*/
	tty2_rx_rbm.head = 0;
	tty2_rx_rbm.tail = 0;
	tty2_rx_rbm.length = 256;

	tty2_tx_rbm.head = 0;
	tty2_tx_rbm.tail = 0;
	tty2_tx_rbm.length = 256;

	/* Enable the uart rx interrupt. The tx interrupt gets enabled when needed.
	*/
	dv_uart2.cr[0] |= DV_UART_RXNEIE;

	/* Enable uart2 IRQ at the NVIC
	*/
	dv_enable_irq(dv_irq_usart2);
}

/* main_Itty2() - body of ISR to handle uart2 interrupt
*/
void main_Itty2(void)
{
	while ( dv_stm32_uart_isrx(&dv_uart2) )
	{
		char c = (char)dv_stm32_uart_getc(&dv_uart2);
		if ( !dv_rb_u8_put(&tty2_rx_rbm, tty2_rx_rb, c) )
		{
			/* Character discarded. Count the overruns and inform the data gathering task
			*/
			if ( tty2_overruns < 0xffffffff )
				tty2_overruns++;
			(void)dv_setevent(Gather, ev_tty_overrun);
		}
		if ( c == '\n' )
		{
			/* Full line received. Inform the data gathering task
			*/
			(void)dv_setevent(Gather, ev_tty_rxline);
		}
	}

	while ( dv_stm32_uart_istx(&dv_uart2) )
	{
		int c = dv_rb_u8_get(&tty2_tx_rbm, tty2_tx_rb);
		if ( c < 0 )
		{
			dv_uart2.cr[0] &= ~DV_UART_TXEIE;
			break;
		}
		else
		{
			dv_stm32_uart_putc(&dv_uart2, c);
		}
	}
}
