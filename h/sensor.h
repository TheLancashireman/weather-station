/* sensor.h - sensor records
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
#ifndef SENSOR_H
#define SENSOR_H	1

#include <davroska.h>

#define MAX_SENSOR_STRING		32
#define MAX_SENSORS				10

/* Structure to hold data from a testing sensor
 * This is a dummy sensor type, only for testing
 * To be deleted eventually.
*/
typedef struct
{
	dv_u16_t v1;
	dv_u16_t v2;
} testing_sensor_t;

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

/* Structure to hold information about an abstract sensor
*/
typedef struct
{
	dv_u16_t n_starts;	/* Number of starts */
	dv_u8_t id;			/* Sensor id */
	char type;			/* Sensor type */
	union {
		temperature_sensor_t temp;
		testing_sensor_t testing;
	} data;
} sensor_t;

extern sensor_t sensors[MAX_SENSORS];
extern dv_u8_t n_sensors;

/* Processsing functions
*/
extern void process_temperature(dv_u8_t id, char *b);

/* Recording/logging functions
*/
extern void record_sensor_start(dv_u8_t id);
extern void record_temperature(dv_u8_t id, dv_u16_t current, dv_u16_t min, dv_u16_t max);
extern void record_sensor_error(dv_u8_t id, dv_u8_t errorcode);
extern void record_invalid_sensor_data(char *b, int e);
extern dv_u8_t find_sensor(dv_u8_t id);
extern dv_u8_t new_sensor(dv_u8_t id, char type);
extern dv_u8_t get_sensor(dv_u8_t id, char type);

#endif
