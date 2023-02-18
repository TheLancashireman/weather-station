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
#include <dv-string.h>

#define MAXCOMMAND	15

static void process_command(char *cmd);

/* main_Command() - main function for the data gathering task
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
					process_command(cmdbuf);
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
static void process_command(char *cmd)
{
	dv_printf("%s : ", cmd);

	if ( dv_strcmp(cmd, "log") == 0 )
	{
		dv_printf("logging enabled\n");
		logging = 1;
	}
	else if ( dv_strcmp(cmd, "nolog") == 0 )
	{
		dv_printf("logging disabled\n");
		logging = 0;
	}
	else
	{
		dv_printf("wot?\n");
	}
}
