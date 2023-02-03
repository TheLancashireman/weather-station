/* convert.c - data conversion functions
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

/* asciihex_to_u8() - convert an ascii-hex character to binary
 *
 * Parameter *must* be [0-9A-Fa-f] otherwise result is garbage
*/
dv_u32_t asciihex_to_4bit(char c)
{
	dv_u32_t b = c;
	b -= 0x30;
	if ( b > 9 )
	{
		b -= 0x11;
		if ( b > 0x0f )
			b -= 0x20;
	}
	return b;
}

/* asciihex_to_binary() - convert ascii-hex string to binary
 *
 * Returns 0xffffffff if error
*/
dv_u32_t asciihex_to_binary(char *b, int n)
{
	dv_u32_t r = 0;

	for ( int i = 0; i < n; i++ )
	{
		if ( isxdigit(b[i]) )
			r = r * 16 + asciihex_to_4bit(b[i]);
		else
			return 0xffffffff;
	}
	return r;
}
