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

		if ( (evts & ev_tty1_overrun) != 0 )
		{
			/* Clear the tty1 input stream
			*/
		}

		if ( (evts & ev_tty1_rxline) != 0 )
		{
			/* Process the tty1 input stream
			*/
		}
	}
}
