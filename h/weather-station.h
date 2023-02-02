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
#include <weather-station.h>

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

#endif
