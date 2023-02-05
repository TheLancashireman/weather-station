/* convert.h - data conversion functions
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
#ifndef CONVERT_H
#define CONVERT_H	1

#include <davroska.h>

/* Structure to hold a fixed point value in a ready-to-print form (without using float)
 *
 * Use the format specifier "%c%d.%04d" to print.
*/
typedef struct
{
	dv_u16_t i;		/* Integer part */
	dv_u16_t f;		/* Fractional part */
	char sign;		/* Sign: either '+' or '-' */
} fixedpoint_printable_t;

/* Data conversion
*/
extern dv_u32_t asciihex_to_4bit(char c);
extern dv_u32_t asciihex_to_binary(char *b, int n);
extern void fixedpoint_to_printable(dv_u16_t fpval, fixedpoint_printable_t *out);

/* sign_extend_12() - sign-extend a 12-bit signed value to 16 bits
*/
static inline dv_i16_t sign_extend_12(dv_u16_t val)
{
	if ( val >= 0x800 )
		val |= 0xf000;
	return (dv_i16_t)val;
}

#endif
