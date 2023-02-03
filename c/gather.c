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
#include <dv-ctype.h>
#include <dv-stdio.h>

static void read_sensor_data(void);
static void process_sensor_data(char *b);

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

/* process_sensor_data() - process a line of sensor data
*/
static void process_sensor_data(char *b)
{
	dv_u8_t id;

	if ( isxdigit(b[1]) && isxdigit(b[2]) )
	{
		id = asciihex_to_binary(&b[1], 2);

		switch ( b[0] )
		{
		case 'S':
			record_sensor_start(id);
			break;

		case 'T':
			process_temperature(id, b);
			break;

		default:
			/* Unreachable if read_sensor_data() is working properly
			*/
			break;
		}
	}
	else
	{
		record_invalid_sensor_data(b);
	}
}

/* process_temperature() - process a temperature record
 *
 * A temperature record is of the form "Txx ccc mmm MMM".
 *                                      012345678901234
 * ccc is the current temperature. Special case: **n is an error code.
 * mmm is the lowest temperature since the last restart
 * MMM is the highest temperature since the last restart
 * Any characters after the MMM are ignored
*/
void process_temperature(dv_u8_t id, char *b)
{
	dv_u32_t v;
	dv_u16_t current, max, min;

	if ( b[3] == ' ' )
	{
		v = asciihex_to_binary(&b[6], 3);
		if ( v == 0xffffffff )
		{
			if ( (b[4] == '*') && (b[5] == '*') )
				record_sensor_error(id, asciihex_to_4bit(b[6]));
			else
				record_invalid_sensor_data(b);
			return;
		}

		current = v;

		if ( b[7] != ' ' )
		{
			record_invalid_sensor_data(b);
			return;
		}

		v = asciihex_to_binary(&b[8], 3);
		if ( v == 0xffffffff )
		{
			record_invalid_sensor_data(b);
			return;
		}

		min = v;

		if ( b[11] != ' ' )
		{
			record_invalid_sensor_data(b);
			return;
		}

		v = asciihex_to_binary(&b[12], 3);
		if ( v == 0xffffffff )
		{
			record_invalid_sensor_data(b);
			return;
		}

		max = v;

		record_temperature(id, current, min, max);
	}
}
