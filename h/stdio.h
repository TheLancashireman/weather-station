/* stdio.h -  compatibility file for tinyusb
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
 *
 *
 *
*/
#ifndef stdio_h
#define stdio_h

#include <dv-config.h>
#include <davroska.h>
#include <dv-stdio.h>

/* Avoid a warning in midi_device.c and elsewhere.
 * It isn't clear where the pointer definition of NULL comes from
*/
#ifdef NULL
#undef NULL
#endif
#define NULL	0

#endif
