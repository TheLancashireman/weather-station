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
#include <sensor.h>

const dv_u8_t rounding_tbl[16] = {	0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 9 };

sensor_t sensors[MAX_SENSORS];
dv_u8_t n_sensors;

temperature_sensor_t temperature_sensors[MAX_TEMPERATURE_SENSORS];
dv_u8_t n_temperature_sensors;

testing_sensor_t testing_sensors[MAX_TESTING_SENSORS];
dv_u8_t n_testing_sensors;

#if 0	/* For reference only: to be removed */

/* Structure to hold information about an abstract sensor
*/
typedef struct
{
	void *data;			/* Sensor data (data type depends on sensor type */
	dv_u16_t n_starts;	/* Number of starts */
	dv_u8_t id;			/* Sensor id */
	char type;			/* Sensor type */
} sensor_t;

/* Structure to hold data from a temperature sensor
*/
typedef struct
{
	dv_i32_t sum;			/* Sum of the last n_readings values */
	dv_u16_t n_readings;	/* No. of readings received */
	dv_i16_t last_reading;	/* The last reading received */
	dv_i16_t sensor_min;	/* Lowest temperature according to the sensor */
	dv_i16_t sensor_max;	/* Highest temperature according to the sensor */
	dv_i16_t reading_min;	/* Lowest reading observed */
	dv_i16_t reading_max;	/* Highest reading observed */
} temperature_sensor_t;

/* Structure to hold data from a testing sensor
 * This is a dummy sensor type, only for testing
 * To be deleted eventually.
*/
typedef struct
{
	dv_u16_t v1;
	dv_u16_t v2;
} testing_sensor_t;

/* Recording/logging functions
*/
extern void record_sensor_start(dv_u8_t id);
extern void record_temperature(dv_u8_t id, dv_u16_t current, dv_u16_t min, dv_u16_t max);
extern void record_sensor_error(dv_u8_t id, dv_u8_t errorcode);
extern void record_invalid_sensor_data(char *b, int e);

#endif

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
		sensors[idx].n_starts = 0;
		sensors[idx].data = 0;
		n_sensors++;
		return idx;
	}
	return 0xff;
}
