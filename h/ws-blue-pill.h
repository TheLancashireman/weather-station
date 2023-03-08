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
/*	Blue Pill pinout and use in this project
 *
 *                                 USB
 *                        PB12 |         | GND
 *                        PB13 |         | GND
 *                        PB14 |         | 3V3
 *                         PA8 |         | NRST
 *        TX1              PA9 |         | PB10
 *        RX1             PA10 |         | PB1
 *                        PA11 |         | PB0
 *                        PA12 |         | PA7   MOSI
 *                        PA15 |         | PA6   MISO
 *                         PB3 |         | PA5   SCK
 *                         PB4 |         | PA4   NSS_CONFIG  (RFM64)
 *                         PB5 |         | PA3   RX2
 *                         PB6 |         | PA2   TX2
 *                         PB7 |         | PA1   NSS_DATA    (RFM64)
 *                         PB8 |         | PA0   POR         (RFM64)
 *                         PB9 |         | PC15
 *                          5V |         | PC14
 *                         GND |         | PC13  On board LED
 *                         3V3 |         | VBAT
 *                                 DBG
*/
#ifndef WS_BLUE_PILL_H
#define WS_BLUE_PILL_H	1

#include <dv-stm32-gpio.h>
#include <dv-stm32-uart.h>
#include <dv-stm32-timx.h>
#include <dv-nvic.h>
#include <dv-stm32-interrupts.h>
#include <dv-stm32-gpio.h>

/* On-board LED
*/
#define LED_PORT		'c'	
#define LED_PIN			13

/* UART1 - not remapped
*/
#define UART1_TX_PORT	'a'
#define UART1_TX_PIN	9
#define UART1_RX_PORT	'a'
#define UART1_RX_PIN	10

/* UART2 - not remapped
*/
#define UART2_TX_PORT	'a'
#define UART2_TX_PIN	2
#define UART2_RX_PORT	'a'
#define UART2_RX_PIN	3

/* SPI1 - not remapped
*/
#define SCK1_PORT		'a'
#define SCK1_PIN		5
#define MISO1_PORT		'a'
#define MISO1_PIN		6
#define MOSI1_PORT		'a'
#define MOSI1_PIN		7

/* SPI slave selects
*/
#define RFM64_NSS_CONFIG_PORT	'a'
#define RFM64_NSS_CONFIG_PIN	4
#define RFM64_NSS_DATA_PORT		'a'
#define RFM64_NSS_DATA_PIN		1
#define RFM64_POR_PORT			'a'
#define RFM64_POR_PIN			0

extern void init_millisecond_ticker(void);

#define USE_USB	1

#if USE_USB

#define	EnableUsbIrqs() \
	do {									\
		dv_enable_irq(dv_irq_usb_lp_can_rx0);	\
		dv_enable_irq(dv_irq_usb_hp_can_tx);	\
		dv_enable_irq(dv_irq_usbwakeup);	\
	} while (0)

#endif

#endif
