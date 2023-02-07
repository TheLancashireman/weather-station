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

/* Structure to hold information about an abstract sensor
*/
typedef struct
{
	void *data;			/* Sensor data (data type depends on sensor type */
	dv_u16_t n_starts;	/* Number of starts */
	dv_u8_t id;			/* Sensor id */
	char type;			/* Sensor type */
} sensor_t;

sensor_t sensors[MAX_SENSORS];
dv_u8_t n_sensors;

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

#endif
