/* gather.c - data gathering task
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

static void read_sensor_data(void);

/* main_Gather() - main function for the data gathering task
*/
void main_Gather(void)
{

	dv_printf("main_Gather() started\n");

	for (;;)
	{
		dv_eventmask_t evts;

		dv_waitevent(0xffffffff);	/* The standard wait/get/clear sequence */
		dv_getevent(Gather, &evts);
		dv_clearevent(evts);

		if ( (evts & ev_tty2_overrun) != 0 )
		{
			/* Clear the tty2 input stream
			*/
		}

		if ( (evts & ev_tty2_rxline) != 0 )
		{
			/* Process the tty2 input stream
			*/
			read_sensor_data();
		}
	}
}

/* read_sensor_data() - read data from a remote sensor
*/
static char sensor_buffer[MAX_SENSOR_STRING+1];
static dv_u8_t sb_index;
static dv_u8_t sb_accept;

static void read_sensor_data(void)
{
	int c;

	while ( (c = tty2_getc()) >= 0 )
	{
		switch ( c )
		{
		case 'S':		/* Sensor power-up */
		case 'T':		/* Temperature reading */
			sensor_buffer[0] = c;
			sb_index = 1;
			sb_accept = 1;
			break;

		case '\r':
		case '\n':
			if ( sb_accept )
			{
				sensor_buffer[sb_index] = '\0';
				process_sensor_data(sensor_buffer);
				sb_accept = 0;
			}
			break;

		default:
			if ( sb_accept )
			{
				sensor_buffer[sb_index] = c;
				sb_index++;
				if ( sb_index >= MAX_SENSOR_STRING )
				{
					sensor_buffer[MAX_SENSOR_STRING] = '\0';
					process_sensor_data(sensor_buffer);
					sb_accept = 0;
				}
			}
		}
	}
}
