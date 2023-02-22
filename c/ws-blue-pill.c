/* ws-blue-pill.c - STM32/Bliue Pill startup functions for weather-station
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
#include <davroska.h>
#include <dv-stdio.h>
#include <dv-string.h>

#include <dv-stm32-rcc.h>
#include <dv-nvic.h>
#include <dv-stm32-uart.h>
#include <dv-stm32-gpio.h>

#include <davroska-inline.h>

#include <weather-station.h>
#include <ws-blue-pill.h>

#define DV_SPSEL		0x02

extern unsigned dv_start_data, dv_end_data, dv_start_bss, dv_end_bss, dv_idata;
extern unsigned dv_pstacktop;
extern unsigned dv_stacktop;

extern int main(int argc, char **argv);
extern void dv_switchToPsp(unsigned *psp, unsigned *msp, unsigned control, void (*fp)(void));

#if USE_USB
void dv_stm32_usb_init(void);
#endif

/* dv_init_data() - initialise variables
 *
 * Initialises all variables from the flash image (.data) or to zero (.bss)
*/
void dv_init_data(void)
{
	unsigned *s = &dv_idata;
	unsigned *d = &dv_start_data;

	while ( d < &dv_end_data )
	{
		*d++ = *s++;
	}

	d = &dv_start_bss;
	while ( d < &dv_end_bss )
	{
		*d++ = 0x00;
	}
}

/* dv_reset2() - call main() after switching to thread stack
*/
void dv_reset2(void)
{
	(void)main(0, DV_NULL);
}

/* dv_reset() - entry point from the reset vector
 *
 * SP has been initialised, but that's all
*/
void dv_reset(void)
{
	/* Initialise the PLL: 72 MHz
	*/
	dv_rcc_init();

#if USE_USB
	/* Initialize the USB controller and its clocks
	*/
	dv_stm32_usb_init();
#endif

	/* Initialise variables
	*/
	dv_init_data();

	/* Initialise the exception priorities
	*/
	dv_ctxm_scr.shpr[0] = 0x0;			/* UsageFault, BusFault, MemManage and [reserved]  all at highest priority */
	dv_ctxm_scr.shpr[1] = 0x0;			/* SVC and [reserved x 3] all at highest priority */
	dv_ctxm_scr.shpr[2] = 0xffff0000;	/* SysTick/PendSV at lowest priority, Debug and [reserved] at highest */

	/* Initialise the interrupt controller
	*/
	dv_nvic_init();

	/* Initialise uart1 and connect it to the stdio functions (polled mode)
	 * Done here so that printf() is available during startup.
	*/
	dv_stm32_gpio_pinmode(UART1_TX_PORT, UART1_TX_PIN, DV_GPIO_ALT_PP_50);
	dv_stm32_gpio_pinmode(UART1_RX_PORT, UART1_RX_PIN, DV_GPIO_IN_PUD);
	(void)dv_stm32_uart_init(1, 115200, "8N1");
	console_polled();

	/* Initialise uart2 for tty2
	*/
	dv_stm32_gpio_pinmode(UART2_TX_PORT, UART2_TX_PIN, DV_GPIO_ALT_PP_50);
	dv_stm32_gpio_pinmode(UART2_RX_PORT, UART2_RX_PIN, DV_GPIO_IN_PUD);
	(void)dv_stm32_uart_init(2, 9600, "8N1");

	/* Initialise GPIO C for the on-board LED
	 *
	 * Pin c13 (LED_PIN) output, open-drain, 50 MHz, output to 1 (turn off)
	*/
	dv_stm32_gpio_pinmode(LED_PORT, LED_PIN, DV_GPIO_OUT_OD_50);
	dv_stm32_gpio_pinset(LED_PORT, LED_PIN, 1);

	/* Initialise pins for spi1. SPI initialisation depends on the device and takes
	 * place just before use.
	 * SPI used as master only.
	 * Chip select outputs are under software control using several output pins, so NSS1 not controlled by SPI.
	*/
	dv_rcc.apb2en |= DV_RCC_SPI1;
	dv_stm32_gpio_pinmode(SCK1_PORT, SCK1_PIN, DV_GPIO_ALT_PP_50);		/* SCK1 */
	dv_stm32_gpio_pinmode(MISO1_PORT, MISO1_PIN, DV_GPIO_IN_PUD);		/* MISO1 */
	dv_stm32_gpio_pinset(MISO1_PORT, MISO1_PIN, 1); 					/* Pull up on MISO1 */
	dv_stm32_gpio_pinmode(MOSI1_PORT, MOSI1_PIN, DV_GPIO_ALT_PP_50);	/* MOSI1 */

	/* SPI slave select pins. Set iniitial state to 1
	*/
	dv_stm32_gpio_pinmode(RFM64_CONFIG_PORT, RFM64_CONFIG_PIN, DV_GPIO_OUT_OD_50);
	dv_stm32_gpio_pinset(RFM64_CONFIG_PORT, RFM64_CONFIG_PIN, 1);

	/* It would be possible to pass main() as the function pointer here,
	 * but for the time being we'll use an intermediate function so that we can find out
	 * what's going on if needed. Also, the trampoline ensures that the parameters to
	 * main() are at least defined.
	*/
	dv_switchToPsp(&dv_pstacktop, &dv_stacktop, (dv_get_control() | DV_SPSEL), &dv_reset2);
}

/* init_millisecond_ticker() - initialise timer2 as a 1 ms ticker.
 *
 * The timer frequency is 1 MHz (CPU freqency / 72)
 * The timer generates a regular interrupt every millisecond.
*/
void init_millisecond_ticker(void)
{
	/* Set the counter running with a 1 us tick
	*/
	dv_stm32_timx_init(2, 72);

	/* Enable CC4 interrupt
	*/
	dv_tim2.dier |= DV_TIM_CC4IE;

	/* Set the compare mode:
	 *	- CC4S = 0	--> compare mode
	 *	- OC4CE = 0	--> ETRF has no effect
	 *	- OC4M = 0	--> compare has no effect on output
	 *	- OC4PE = 0	--> CCR4 is not buffered, can be written any time
	 *	- OC4FE = 0	--> normal trigger effect
	*/
	dv_tim2.ccmr[1] = (dv_tim2.ccmr[1] & 0xff) | ( (0) << 8 );

	/* Set the compare register: initial delay of 1 ms
	*/
	dv_tim2.ccr[3] = dv_tim2.cnt + 1000;

	/* Enable the comparison operation
	 *	- clear the match flag
	 *	- enable event generation
	 *	- enable the compare function
	*/
	dv_tim2.sr &= ~DV_TIM_CC4IF;
	dv_tim2.egr |= DV_TIM_CC4G;
	dv_tim2.ccer |= DV_TIM_CCxE << 12;
}

void dv_panic_return_from_switchcall_function(void)
{
	console_polled();
	dv_panic(dv_panic_ReturnFromLongjmp, dv_sid_scheduler, "Oops! The task wrapper returned");
}

/* Below this line are stub functions to satisfy the vector addresses
*/
void dv_nmi(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! An NMI occurred");
}

void dv_hardfault(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A hardfault trap occurred");
}

void dv_memfault(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A memfault trap occurred");
}

void dv_busfault(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A busfault trap occurred");
}

void dv_usagefault(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A usage fault occurred");
}

void dv_unknowntrap(void)
{
	console_polled();
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! An undocumented trap occurred");
}

#if USE_USB
/* dv_stm32_usb_init() - initialise the usb controller
 *
 * See also dv_stm32_usb_connect()
 *
 * Initialisation sequence (from sect. 23.4.2 of STM32F10x reference manual):
 *  - provide clock signals to USB controller (rcc?)
 *  - de-assert reset signal (rcc?)
 *  - turn on analog circuitry (de-assert PDWN)
 *  - wait Tstartup (see data sheet)
 *  - de-assert controller reset (FRES)
 *  - clear spurious interrupts (ISTR)
 *  - initialise the packet buffer table (probably before removing PDWN?)
 *
 * From data sheet:
 *  - Tstartup is Max 1 us !!!  So only have to wait 1us :-)
 *  - HSE aand PLL mustr be enabled, USBCLK = 48 MHz
 *  - USB pins USB_DP and USB_DM are  PA12 and PA11 resp. Automatically configured when USB enabled.
*/
void dv_stm32_usb_init(void)
{
	/* Assert the reset signal
	*/
	dv_rcc.apb1rst |= DV_RCC_USB;

	/* Ensure a 1.5 prescaler to divide the 72 MHz down to 48 MHz for the USB port
	*/
	dv_rcc.cfg &= ~DV_RCC_USBPRE;

	/* Enable the clock to the USB peripheral
	*/
	dv_rcc.apb1en |= DV_RCC_USB;

	/* Deassert the reset signal
	*/
	dv_rcc.apb1rst &= ~DV_RCC_USB;
}
#endif
