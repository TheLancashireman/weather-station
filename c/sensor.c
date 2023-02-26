/* sensor.c - sensor handling
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
#include <weather-station.h>
#include <sensor.h>
#include <convert.h>

sensor_t sensors[MAX_SENSORS];
dv_u8_t n_sensors;

/* get_sensor() - find a sensor, create one if needed and possible
 *
*/
dv_u8_t get_sensor(dv_u8_t id, char type)
{
	dv_u8_t idx = find_sensor(id);
	if ( idx < MAX_SENSORS )
	{
		if ( sensors[idx].type != type )
		{
			if ( sensors[idx].type == '?' )
				sensors[idx].type = type;
			else
			if ( type != '?' )
			{
				dv_printf("Sensor type mismatch : id = %02x found %c  looking for %c\n", id, sensors[idx].type, type);
				idx = 0xff;
			}
		}
	}
	else
	{
		idx = new_sensor(id, type);
		if ( idx == 0xff )
			dv_printf("Sensor table overflow : id = %c%02x\n", type, id);
		else
		if ( logging )
			dv_printf("New sensor %c%02x registered at %u\n", type, id, idx);
	}

	return idx;
}

/* find_sensor() - find a sensor in the sensors array
 *
 * Returns 0xff if not found
*/
dv_u8_t find_sensor(dv_u8_t id)
{
	dv_u8_t idx = 0;

	do {
		if ( sensors[idx].type != '\0' )
		{
			/* Sensor structure is occupied by something. Does it match?
			*/
			if ( sensors[idx].id == id )
			{
				/* Matching id found. Return it!
				*/
				return idx;
			}
		}
		idx++;
	} while ( idx < MAX_SENSORS );

	return 0xff;
}

/* new_sensor() - add a new a sensor to the sensors array
 *
 * Returns new sensor index, 0xff if no room or other error
 * Condition on entry: sensor does not already exist
*/
dv_u8_t new_sensor(dv_u8_t id, char type)
{
	if ( n_sensors < MAX_SENSORS )
	{
		dv_u8_t idx = n_sensors;
		sensors[idx].id = id;
		sensors[idx].type = type;
		n_sensors++;
		return idx;
	}
	return 0xff;
}

/* print_all_sensors() - print the sensor information
*/
void print_all_sensors(void)
{
	for ( int i = 0; i < n_sensors; i++ )
	{
		print_sensor(i);
	}
}

/* print_sensor() - print information for one sensor
*/
void print_sensor(int s)
{
	dv_printf("Sensor %c%02x: %d starts, ", sensors[s].type, sensors[s].id, sensors[s].n_starts);
	switch ( sensors[s].type )
	{
	case 'T':	print_temperature_sensor(s);			break;	
	case 'X':	print_testing_sensor(s);				break;	
	case '?':	dv_printf("unspecified sensor\n");		break;	
	default:	dv_printf("unknown sensor\n");			break;	
	}
}

/* print_temperature_sensor() - print information for a temperature sensor
*/
void print_temperature_sensor(int s)
{
	sensor_t *sx = &sensors[s];
	fixedpoint_rounded_printable_t pval;

	fixedpoint_to_rounded_printable(sx->data.temp.last_reading, &pval);
	dv_printf("t = %c%d.%c, ", pval.sign, pval.i, pval.f);

	fixedpoint_to_rounded_printable(sx->data.temp.sensor_min, &pval);
	dv_printf("(%c%d.%c .. ", pval.sign, pval.i, pval.f);

	fixedpoint_to_rounded_printable(sx->data.temp.sensor_max, &pval);
	dv_printf("%c%d.%c) avg ", pval.sign, pval.i, pval.f);

	if ( sx->data.temp.n_readings <= 0 )
		dv_printf("--- (%d) ", sx->data.temp.n_readings);
	else
	{
		dv_u16_t av = (sx->data.temp.sum + sx->data.temp.n_readings/2) / sx->data.temp.n_readings;
		fixedpoint_to_rounded_printable(av, &pval);
		dv_printf("%c%d.%c (%u) ",  pval.sign, pval.i, pval.f, sx->data.temp.n_readings);
	}

	fixedpoint_to_rounded_printable(sx->data.temp.reading_min, &pval);
	dv_printf("(%c%d.%c .. ", pval.sign, pval.i, pval.f);

	fixedpoint_to_rounded_printable(sx->data.temp.reading_max, &pval);
	dv_printf("%c%d.%c)\n", pval.sign, pval.i, pval.f);
}

/* print_testing_sensor() - print information for a testing sensor
*/
void print_testing_sensor(int s)
{
	dv_printf("Testing sensor %d ... %04x %04x\n", sensors[s].data.testing.v1, sensors[s].data.testing.v2);
}
