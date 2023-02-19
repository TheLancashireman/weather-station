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
#if 1
#include <hoperf-rfm64.h>
dv_u8_t led_rfm64_reg = 32;		// Temporary
#endif

/* main_Led() - task body function for the Led task
*/
void main_Led(void)
{
	dv_stm32_gpio_pinset('c', LED_PIN, ledstate);	/* Inverse! */
	ledstate = !ledstate;
#if 1
	if ( led_rfm64_reg < 32 )
	{
		dv_u8_t rval;
		int e = rfm64_read_cfgr(led_rfm64_reg, &rval);
		if ( e == 0 )
			dv_printf("Led: rfm64 register %02u = 0x%02x\n", led_rfm64_reg, rval);
		else
			dv_printf("Led: rfm64_read_cfgr(&u, ...) returned %d\n", led_rfm64_reg, e);
		led_rfm64_reg++;
		dv_setalarm_rel(Ticker, LedAlarm, 1000);
		return;
	}
#endif

	dv_setalarm_rel(Ticker, LedAlarm, ledstate ? 20 : 4980);
}

/* af_LedAlarm() - alarm function to activate the Led task
*/
dv_u64_t af_LedAlarm(dv_id_t unused_a, dv_param_t unused_d)
{
    dv_activatetask(Led);
    return 0;				/* Single-shot */
}
