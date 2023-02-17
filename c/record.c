/* record.c - data recording functions
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
#include <convert.h>
#include <sensor.h>
#include <dv-stdio.h>

/* record_sensor_start() - record a sensor's start time
 *
 * A sensor start record is of the form "Sxx".
 * Any characters after the xx are ignored
*/
void record_sensor_start(dv_u8_t id)
{
	/* Debug
	*/
	dv_printf("Sensor %02x: start record\n", id);

	dv_u8_t idx = find_sensor(id);
	if ( idx == 0xff )
	{
		idx = new_sensor(id, '?');
		dv_printf("New sensor %02x registered at %u\n", id, idx);
	}
	else
	{
		/* Start record for existing sensor
		*/
		sensors[idx].n_starts++;
		dv_printf("Existing sensor %02x  at index %u restarted %u times\n", id, idx, sensors[idx].n_starts);
	}
}

void record_temperature(dv_u8_t id, dv_u16_t current, dv_u16_t min, dv_u16_t max)
{
#if 0
	/* Debug
	*/
	dv_printf("Temperature data received: %02x: temperature %03x min %03x max %03x\n", id, current, min, max);

	dv_u8_t idx = find_sensor(id);
	if ( idx == 0xff )
	{
		idx = new_sensor(id, 'T');
	}
#else
	fixedpoint_printable_t tcur, tmax, tmin;
	fixedpoint_to_printable(current, &tcur);
	fixedpoint_to_printable(min, &tmin);
	fixedpoint_to_printable(max, &tmax);

	//dv_printf("Sensor %02x: temperature %03x min %03x max %03x\n", id, current, min, max);
	dv_printf("Sensor %02x: temperature %c%d.%04d ( %c%d.%04d .. %c%d.%04d )\n", id,
				tcur.sign, tcur.i, tcur.f, tmin.sign, tmin.i, tmin.f, tmax.sign, tmax.i, tmax.f);
#endif
}

void record_sensor_error(dv_u8_t id, dv_u8_t errorcode)
{
	/* Temporary
	*/
	dv_printf("Sensor %02x: error %u\n", id, errorcode);
}

void record_invalid_sensor_data(char *b, int e)
{
	/* Temporary
	*/
	dv_printf("Invalid sensor data: \"%s\". Error %d\n", b, e);
}
