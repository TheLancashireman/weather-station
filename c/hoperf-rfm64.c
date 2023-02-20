/* hoperf-rfm64.c - Hope RF RFM64 433 MHz interface module
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
#include <weather-station.h>
#include <ws-blue-pill.h>
#include <hoperf-rfm64.h>
#include <dv-stm32-spi.h>
#include <dv-stm32-gpio.h>
#include <dv-stm32-rcc.h>

int rfm64_debug = 0;

/* rfm64_read_cfgr() - read a single configuration register
*/
int rfm64_read_cfgr(dv_u8_t adr, dv_u8_t *out)
{
	dv_spi_t *spi = dv_stm32_get_spi(RFM64_SPI);
	volatile dv_u16_t rxdata;
	if ( spi == DV_NULL )
		return 1;

	if ( dv_takemutex(SpiMutex) != dv_e_ok )
		return 2;

	/* Initialise SPI for the RFM64 config slave
	*/
	dv_stm32_spi_init(RFM64_SPI, RFM64_MAXBAUD_CFG, RFM64_STM32_SPI_MODE);

	/* DEBUG */
	if ( rfm64_debug )
		dv_printf("SPI1: cr1 0x%04x cr2 0x%04x sr 0x%04x\n", dv_spi1.cr[0], dv_spi1.cr[1], dv_spi1.sr);

	/* Set NSS_CONFIG output low to select the slave
	*/
	dv_stm32_gpio_pinset(RFM64_CONFIG_PORT, RFM64_CONFIG_PIN, 0);

	rxdata = dv_stm32_spi_read_dr(spi);			// Read the data register to discard any leftover incoming byte
	dv_stm32_spi_put(spi, RFM64_CMD(adr, 0));	// Send the command
	dv_stm32_spi_waittx(spi);					// Wait until the command has gone
	rxdata = dv_stm32_spi_get(spi);				// Discard the byte received while sending the command
	dv_stm32_spi_put(spi, 0);					// Send 0 to clock the data into the rx buffer
	rxdata = dv_stm32_spi_get(spi);				// Read the data

	/* Set NSS_CONFIG output high to deselect the slave
	*/
	dv_stm32_gpio_pinset(RFM64_CONFIG_PORT, RFM64_CONFIG_PIN, 1);

	dv_dropmutex(SpiMutex);

	*out = rxdata;

	return 0;
}
