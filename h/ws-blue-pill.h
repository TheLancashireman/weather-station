/* ws-blue-pill.h - configuration to allow the project to run on a Blue Pill
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
 *
 *
 *
*/
#ifndef WS_BLUE_PILL_H
#define WS_BLUE_PILL_H	1

#include <dv-stm32-gpio.h>
#include <dv-stm32-uart.h>
#include <dv-stm32-timx.h>
#include <dv-nvic.h>
#include <dv-stm32-interrupts.h>
#include <dv-stm32-gpio.h>

#define LED_PIN	13	/* On-board LED */

extern void init_millisecond_ticker(void);

#define USE_USB	0	/* USB stuff not needed (yet) */

#if USE_USB

#define hw_UsbInterruptId1		dv_irq_usb_lp_can_rx0
#define hw_UsbInterruptId2		dv_irq_usb_hp_can_tx
#define hw_UsbInterruptId3		dv_irq_usbwakeup

/* ISR bodies defined as macros to avoid having to include tinyusb header here.
*/
#define hw_UsbIsr1()	tud_int_handler(0)
#define hw_UsbIsr2()	tud_int_handler(0)
#define hw_UsbIsr3()	tud_int_handler(0)

#define	hw_EnableUsbIrqs() \
	do {									\
		dv_enable_irq(hw_UsbInterruptId1);	\
		dv_enable_irq(hw_UsbInterruptId2);	\
		dv_enable_irq(hw_UsbInterruptId3);	\
	} while (0)

#endif

#endif
