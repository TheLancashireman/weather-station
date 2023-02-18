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
#include <hoperf-rfm64.h>
#include <dv-stm32-spi.h>

/* rfm64_read_cfgr() - read a single configuration register
*/
int rfm64_read_cfgr(dv_u8_t adr, dv_u8_t *out)
{
	dv_spi_t *spi = dv_stm32_get_spi(RFM64_SPI);
	if ( spi == DV_NULL )
		return 1;

	if ( dv_takemutex(SpiMutex) != dv_e_ok )
		return 2;

	/* ToDo: set NSS_CONFIG output low */

	dv_stm32_spi_init(RFM64_SPI, RFM64_MAXBAUD_CFG, RFM64_STM32_SPI_MODE);
	dv_stm32_spi_put(spi, RFM64_CMD(adr, RFM64_CMD_R));
	dv_stm32_spi_put(spi, 0);
	*out = dv_stm32_spi_get(spi);
	dv_stm32_spi_disable(RFM64_SPI);

	/* ToDo: set NSS_CONFIG output high */

	dv_dropmutex(SpiMutex);

	return 0;
}

#if 0
	for ( int i = 0; i < 32; i++ )
	{
		dv_u8_t rval;
		int e = rfm64_read_cfgr(i, &rval);
		if ( e == 0 )
			dv_printf("rfm64 regiser %02d = 0x%02x\n", i, rval);
		else
			dv_printf("rfm64_read_cfgr(&d, ...) returned %d\n", i, e);
	}
#endif
