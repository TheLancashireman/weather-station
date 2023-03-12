/* command.c - command interpreter task
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
#include <sensor.h>
#include <convert.h>
#include <dv-string.h>
#include <dv-string.h>
#include <hoperf-rfm64.h>
#include <tusb.h>

#define MAXCOMMAND	15


char cmdbuf[MAXCOMMAND+1];
int pos;

dv_boolean_t cdc0_putc(int c);
int cdc0_getc(void);
dv_boolean_t cdc0_isrx(void);
dv_boolean_t cdc0_istx(void);

const dv_uartdriver_t cdc0_drv =
{
	.putc = cdc0_putc,
	.getc = cdc0_getc,
	.istx = cdc0_istx,
	.isrx = cdc0_isrx
};

/* main_Cdc0Rx() - main function for the data gathering task
*/
void main_Cdc0Rx(void)
{
	dv_u8_t buf[65];	/* Allows for a newline if CR is last in buffer */
	int nchars;
	int i, i0;
	char ch;

	i0 = 0;
	while ( tud_cdc_n_available(0) )
	{
		nchars = tud_cdc_n_read(0, buf, sizeof(buf));
		for ( i = 0; i < nchars; i++ )
		{
			ch = buf[i];
			if ( ch == '\r' )
			{
				cmdbuf[pos] = '\0';
				tud_cdc_n_write(0, &buf[i0], i - i0 + 1);
				ch = '\n';
				tud_cdc_n_write(0, &ch, 1);
				tud_cdc_n_write_flush(0);
				i0 = i+1;
				pos = 0;
				process_command(&cdc0_drv, cmdbuf);
			}
			else
			{
				if ( pos < MAXCOMMAND )
				{
					cmdbuf[pos] = ch;
					pos++;
				}
			}
		}
		if ( nchars > i0 )
		{
			tud_cdc_n_write(0, &buf[i0], nchars - i0);
			tud_cdc_n_write_flush(0);
		}
	}
}

/* tud_cdc_rx_cb() - called when there's data on a CDC interface
*/
void tud_cdc_rx_cb(dv_u8_t itf)
{
	if ( itf == 0 )
		dv_activatetask(Cdc0Rx);
#if 0
	else if ( itf == 1 )
		dv_activatetask(UsbRead1);
#endif
}

/* Driver functions
*/
dv_boolean_t cdc0_putc(int c)
{
	tud_cdc_n_write(0, &c, 1);
	return 1;
}

int cdc0_getc(void)
{
	char ch;
	if ( tud_cdc_n_read(0, &ch, 1) == 1 )
		return ch;
	return -1;
}

dv_boolean_t cdc0_isrx(void)
{
	return (tud_cdc_n_available(0) != 0);
}

dv_boolean_t cdc0_istx(void)
{
	return 1;
}
