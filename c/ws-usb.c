/* ws-usb.c - weather station USB interface
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
 * Copied and adapted from tinyusb/examples/device/cdc_dual_port.
 * Original license follows...
*/
#define DV_ASM	0
#include <dv-config.h>
#include <davroska.h>
#include <ws-blue-pill.h>
#include <weather-station.h>
#include <tusb.h>
#include <dv-stdio.h>

