/* main-Led.c - weather station LED blinker
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
#include <ws-blue-pill.h>
#include <weather-station.h>

static dv_boolean_t ledstate;

/* main_Led() - task body function for the Led task
*/
void main_Led(void)
{
	dv_stm32_gpio_pinset('c', LED_PIN, ledstate);	/* Inverse! */
	ledstate = !ledstate;

	dv_setalarm_rel(Ticker, LedAlarm, ledstate ? 20 : 4980);
}

/* af_LedAlarm() - alarm function to activate the Led task
*/
dv_u64_t af_LedAlarm(dv_id_t unused_a, dv_param_t unused_d)
{
    dv_activatetask(Led);
    return 0;				/* Single-shot */
}
