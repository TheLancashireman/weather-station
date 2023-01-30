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
 *
 *
 *
*/
#define DV_ASM	0
#include <dv-config.h>
#include <davroska.h>
#include <dv-stdio.h>
#include <dv-string.h>

#include <ws-blue-pill.h>
#include <weather-station.h>

/* Object identifiers
*/
dv_id_t Init, Led;							/* Tasks */
dv_id_t Uart1, Timer;						/* ISRs */
											/* Mutexes */
dv_id_t Ticker;								/* Counters */
											/* Alarms */

/* main_Init() - start the ball rolling
*/
void main_Init(void)
{
	dv_printf("main_Init() reached\n");
}

/* main_Led() - task body function for the Led task
*/
void main_Led(void)
{
}

/* main_Uart1() - body of ISR to handle uart1 rx interrupt
*/
void main_Uart1(void)
{
}

/* main_Timer() - body of ISR to handle interval timer interrupt
*/
void main_Timer(void)
{
	hw_ClearTimer();

	dv_statustype_t ee = dv_advancecounter(Ticker, 1);
	if ( ee != dv_e_ok )
		dv_shutdown(ee);
}

/* callout_addtasks() - configure the tasks
*/
void callout_addtasks(dv_id_t mode)
{
	Init = dv_addtask("Init", &main_Init, 4, 1);
	dv_printf("Init : %d\n", Init);
	Led = dv_addextendedtask("Led", &main_Led, 1, 2048);
	dv_printf("Led : %d\n", Led);
}

/* callout_addisrs() - configure the isrs
*/
void callout_addisrs(dv_id_t mode)
{
	Uart1 = dv_addisr("Uart1", &main_Uart1, hw_Uart1InterruptId, 5);
	Timer = dv_addisr("Timer", &main_Timer, hw_TimerInterruptId, 6);
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
}

/* callout_autostart() - start the objects that need to be running after dv_startos()
*/
void callout_autostart(dv_id_t mode)
{
	dv_activatetask(Init);
	dv_activatetask(Led);

	/* Enable interrupts from the UART1
	*/
	hw_EnableUart1RxInterrupt();
	dv_enable_irq(hw_Uart1InterruptId);

	hw_InitialiseMillisecondTicker();
	dv_enable_irq(hw_TimerInterruptId);
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

	for (;;) {} /* Temporary */
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
#ifndef panic_info
#define panic_info()	do { } while (0)
#endif
void callout_panic(dv_panic_t p, dv_sid_t sid, char *fault)
{
	dv_printf("Panic %d in %d : %s\n", p, sid, fault);
	panic_info();
}

/* main() - well, it's main, innit?
*/
int main(int argc, char **argv)
{
	dv_printf("weather-station starting ...\n");

	dv_startos(0);

	return 0;
}
