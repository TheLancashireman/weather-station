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

#define MAXCOMMAND	15

extern void process_command(const dv_uartdriver_t *f, char *cmd);
static void read_rfm_config(void);

/* main_Command() - main function for the command task
*/
void main_Command(void)
{

	dv_printf("main_Command() started\n");

	for (;;)
	{
		dv_eventmask_t evts;

		dv_waitevent(0xffffffff);	/* The standard wait/get/clear sequence */
		dv_getevent(Command, &evts);
		dv_clearevent(evts);

		if ( (evts & ev_tty_overrun) != 0 )
		{
			int c;
			/* Clear the tty1 input stream
			*/
			while ( (c = tty1_getc()) >= 0 )
			{
			}
		}

		if ( (evts & ev_tty_rxline) != 0 )
		{
			/* Process the tty1 input stream
			*/
			int c;
			int n = 0;
			char cmdbuf[MAXCOMMAND+1];

			while ( (c = tty1_getc()) >= 0 )
			{
				if ( ( c == '\r') || ( c == '\n' ) )
				{
					cmdbuf[n] = '\0';
					process_command(&dv_consoledriver, cmdbuf);
					break;
				}
				else
				if ( n < MAXCOMMAND )
				{
					cmdbuf[n] = c;
					n++;
				}
			}
		}
	}
}

/* process_command() - interpret and act on an incoming command
*/
void process_command(const dv_uartdriver_t *f, char *cmd)
{
	dv_fprintf(f, "%s :\n", cmd);

	if ( dv_strcmp(cmd, "log") == 0 )
	{
		dv_fprintf(f, "logging enabled\n");
		logging = 1;
	}
	else if ( dv_strcmp(cmd, "nolog") == 0 )
	{
		dv_fprintf(f, "logging disabled\n");
		logging = 0;
	}
	else if ( dv_strcmp(cmd, "dump") == 0 )
	{
		print_all_sensors();
	}
	else if ( dv_strcmp(cmd, "r") == 0 )
	{
		read_rfm_config();
	}
	else if ( dv_strcmp(cmd, "D") == 0 )
	{
		extern int rfm64_debug;
		rfm64_debug = !rfm64_debug;
		dv_fprintf(f, "rfm64 debug %sabled\n", rfm64_debug ? "en" : "dis");
	}
	else
	{
		dv_fprintf(f, "wot?\n");
	}
}

/* TEMPORARY
*/
static void read_rfm_config(void)
{
	dv_boolean_t save_lg = logging;
	logging = 0;

	dv_printf("reading RFM64 configuration\n");

	for ( dv_u8_t i = 0; i < 32; i++ )
	{
		dv_u8_t rval;
		int e = rfm64_read_cfgr(i, &rval);
		if ( e == 0 )
			dv_printf("rfm64 register %02d = 0x%02x\n", i, rval);
		else
			dv_printf("rfm64_read_cfgr(&d, ...) returned %d\n", i, e);
	}
	tty1_flush();

	logging = save_lg;
}
