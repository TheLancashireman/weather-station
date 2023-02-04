/* weather-station.h - weather station (base station)
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
#ifndef WEATHER_STATION_H
#define WEATHER_STATION_H	1

#include <davroska.h>

#define MAX_SENSOR_STRING		32

/* Object identifiers
*/
extern dv_id_t Init, Led, Gather;		/* Tasks */
extern dv_id_t Itty1, Itty2, Timer;		/* ISRs */
extern dv_id_t Ticker;					/* Counters */
extern dv_id_t LedAlarm;				/* Alarms */

/* Events for the Gather task
*/
#define ev_tty2_overrun		0x00000001
#define ev_tty2_rxline		0x00000002

/* Task and ISR main functions
*/
extern void main_Init(void);
extern void main_Led(void);
extern void main_Gather(void);

extern void main_Itty1(void);
extern void main_Itty2(void);
extern void main_Timer(void);

/* Callouts
*/
extern dv_u64_t af_LedAlarm(dv_id_t unused_a, dv_param_t unused_d);

/* Data conversion
*/
extern dv_u32_t asciihex_to_4bit(char c);
extern dv_u32_t asciihex_to_binary(char *b, int n);

/* tty1 functions
*/
extern void tty1_init(void);
extern dv_boolean_t tty1_isrx(void);
extern dv_boolean_t tty1_istx(void);
extern dv_boolean_t tty1_putc(int c);
extern int tty1_getc(void);
/* tty1 polled
*/
extern dv_boolean_t uart1_isrx(void);
extern dv_boolean_t uart1_istx(void);
extern dv_boolean_t uart1_putc(int c);
extern int uart1_getc(void);

/* tty2 functions
*/
extern void tty2_init(void);
extern dv_boolean_t tty2_isrx(void);
extern dv_boolean_t tty2_istx(void);
extern dv_boolean_t tty2_putc(int c);
extern int tty2_getc(void);

/* Processsing functions
*/
extern void process_temperature(dv_u8_t id, char *b);

/* Recording/logging functions
*/
extern void record_sensor_start(dv_u8_t id);
extern void record_temperature(dv_u8_t id, dv_u16_t current, dv_u16_t min, dv_u16_t max);
extern void record_sensor_error(dv_u8_t id, dv_u8_t errorcode);
extern void record_invalid_sensor_data(char *b);

#endif
