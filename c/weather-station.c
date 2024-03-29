/* weather-station.c - weather station (base station)
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
#include <davroska.h>
#include <dv-stdio.h>
#include <dv-string.h>

#include <ws-blue-pill.h>
#include <weather-station.h>

#include <dv-stm32-spi.h>
#include <dv-tusb-device.h>


/* Object identifiers
*/
dv_id_t Init, Led, Gather, Command;			/* Tasks */
dv_id_t Cdc0Rx;								/* Tasks */
dv_id_t Itty1, Itty2, Timer;				/* ISRs */
dv_id_t Ticker;								/* Counters */
dv_id_t LedAlarm;							/* Alarms */
dv_id_t SpiMutex, TtyMutex;					/* Mutexes */

/* Global state variables
*/
dv_boolean_t logging;

// TEMPORARY
unsigned DBG_var;

/* main_Init() - start the ball rolling
*/
void main_Init(void)
{
	/* This is the last message with the console in polled mode.
	*/
	dv_printf("main_Init() reached\n");

	/* Initialise vars
	*/
	logging = 1;

	/* Switch uart1 to tty (interrupt driven) mode. Cannot do this earlier because
	 * interrupts are disabled and there are no ISRs.
	*/
	tty1_init();

	callout_tusbd_activate();
}

/* main_Timer() - body of ISR to handle interval timer interrupt
*/
void main_Timer(void)
{
	dv_tim2.ccr[3] += 1000;			/* No in-the-past check */
	dv_tim2.sr &= ~DV_TIM_CC4IF;	/* Clear the match flag */

	dv_statustype_t ee = dv_advancecounter(Ticker, 1);
	if ( ee != dv_e_ok )
		dv_shutdown(ee);
}

/* callout_addtasks() - configure the tasks
*/
void callout_addtasks(dv_id_t mode)
{
	Init = dv_addtask("Init", &main_Init, 4, 1);
	Led  = dv_addtask("Led",  &main_Led,  3, 1);
	Cdc0Rx = dv_addtask("Cdc0Rx", &main_Cdc0Rx, 1, 2);

	Gather  = dv_addextendedtask("Gather",  &main_Gather,  2, 512);
	Command = dv_addextendedtask("Command", &main_Command, 1, 512);

	callout_tusbd_addtasks();
}

/* callout_addisrs() - configure the isrs
*/
void callout_addisrs(dv_id_t mode)
{
	Itty1 = dv_addisr("Itty1",  &main_Itty1,  dv_irq_usart1, 6);
	Itty2 = dv_addisr("Itty2",  &main_Itty2,  dv_irq_usart2, 6);
	Timer = dv_addisr("Timer",  &main_Timer,  dv_irq_tim2,   7);

	callout_tusbd_addisrs();
}

/* callout_addgroups() - configure the executable groups
*/
void callout_addgroups(dv_id_t mode)
{
}

/* callout_addmutexes() - configure the mutexes
*/
void callout_addmutexes(dv_id_t mode)
{
	SpiMutex = dv_addmutex("SpiMutex", 1);
	dv_addmutexuser(SpiMutex, Led);
	dv_addmutexuser(SpiMutex, Command);

	TtyMutex = dv_addmutex("TtyMutex", 1);
	dv_addmutexuser(TtyMutex, Init);
	dv_addmutexuser(TtyMutex, Led);
	dv_addmutexuser(TtyMutex, Gather);
	dv_addmutexuser(TtyMutex, Command);

	callout_tusbd_addmutexes();
}

/* callout_addcounters() - configure the counters
*/
void callout_addcounters(dv_id_t mode)
{
	Ticker = dv_addcounter("Ticker");
}

/* callout_addalarms() - configure the alarms
*/
void callout_addalarms(dv_id_t mode)
{
	LedAlarm = dv_addalarm("LedAlarm", &af_LedAlarm, 0);

	callout_tusbd_addalarms();
}

/* callout_autostart() - start the objects that need to be running after dv_startos()
*/
void callout_autostart(dv_id_t mode)
{
	dv_activatetask(Init);
	dv_activatetask(Led);
	dv_activatetask(Gather);
	dv_activatetask(Command);

	/* Set the 1 ms tick interrupt running
	*/
	init_millisecond_ticker();
	dv_enable_irq(dv_irq_tim2);

	/* Initialise uart2 for receiving sensor data
	*/
	tty2_init();

	/* Enable USB interrupts
	*/
	dv_tusbd_enableirqs();
}

/* callout_reporterror() - called if an error is detected
*/
dv_statustype_t callout_reporterror(dv_sid_t sid, dv_statustype_t e, dv_qty_t nparam, dv_param_t *param)
{
	dv_printf("callout_reporterror(%d, %d, %d, ...) called.\n", sid, e, nparam);
	for (int i = 0; i < nparam; i++ )
	{
		/* Only print the lower 32 bits of the parameters
		*/
		dv_printf("    param[%d] = %d (0x%08x)\n", i, (dv_u32_t)param[i], (dv_u32_t)param[i]);
	}

	return e;
}

/* callout_shutdown() - called on shutdown
*/
void callout_shutdown(dv_statustype_t e)
{
	dv_printf("callout_shutdown: %d\n", e);
}

/* callout_idle() - called in idle loop
*/
void callout_idle(void)
{
	dv_printf("Idle loop reached\n");
	for (;;) { }
}

/* callout_panic() - called from dv_panic
*/
void callout_panic(dv_panic_t p, dv_sid_t sid, char *fault)
{
	console_polled();
	dv_printf("Panic %d in %d : %s\n", p, sid, fault);
}

/* main() - well, it's main, innit?
*/
int main(int argc, char **argv)
{
	dv_printf("weather-station starting ...\n");

	dv_startos(0);

	return 0;
}
