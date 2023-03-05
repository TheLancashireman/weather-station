/* hoperf-rfm64.h - header file for the Hope RF RFM64 433 MHz interface module
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
#ifndef HOPERF_RFM64_H
#define HOPERF_RFM64_H	1

/* According to the datasheet (RFM64-V1.2.pdf) the data is latched on the rising edge of the
 * clock signal, MSB first.
 * The module requires the SPI clock signal to be low before communication starts. After a select pin
 * (either NSS_CONFIG or NSS_DATA) goes low, the first edge on the SCLK pin is a rising edge that latches
 * the first data bit into the shift register. After that, subsequent bits are shifted in on rising edges
 * of SCLK. It seems that the STM32 modes CPOL=0, CPHA=0 fulfil this requirement.
 *
 * Config write consists of two bytes:
 *	Byte 1:   7  6  5  4  3  2  1  0     Byte 2:  7  6  5  4  3  2  1  0
 *  Tx        S  R  A4 A3 A2 A1 A0 S              D7 D6 D5 D4 D3 D2 D1 D0
 *  Rx        x  x  x  x  x  x  x  x              x  x  x  x  x  x  x  x
 *       
 *
 * Config read consists of two bytes:
 *	Byte 1:   7  6  5  4  3  2  1  0     Byte 2:  7  6  5  4  3  2  1  0
 *  Tx        S  R  A4 A3 A2 A1 A0 S              x  x  x  x  x  x  x  x
 *  Rx        x  x  x  x  x  x  x  x              D7 D6 D5 D4 D3 D2 D1 D0
 *
 *  S  = start/stop bits (always 0)
 *  R  = read (0)
 *  W  = write (1)
 *  An = register address
 *  Dn = data
 *  x  = don't care
*/
#define RFM64_CMD_W					0x00
#define RFM64_CMD_R					0x40
#define RFM64_CMD(adr, w)			( (((dv_u8_t)(adr)) << 1) | ((w) ? RFM64_CMD_W : RFM64_CMD_R) )

/* The maximum SPI clock speed. Data is slower than config
*/
#define RFM64_MAXBAUD_CFG			6000000
#define RFM64_MAXBAUD_DATA			1000000


/* Bit fields in the config registers
 *
 * RFM64_xx_PARAM_NAME, where xx is the register index (decimal) and PARAM_NAME is the name of the parameter
*/

/* Main configuration - MCParam
*/
#define RFM64_00_MODULE_MODE		0xe0
#define RFM64_00_FREQ_BAND			0x1c
#define RFM64_00_SUB_BAND			0x03
#define RFM64_01_DATA_MODE			0xc0
#define RFM64_01_FSK_OOK_CTRL		0x30
#define RFM64_01_OOK_THRESH_TYPE	0x0c
#define RFM64_01_IF_GAIN			0x03
#define RFM64_02_FREQ_DIV			0xff
#define RFM64_03_BR_C				0xff
#define RFM64_04_BR_D				0xff
#define RFM64_05_PA_RAMP			0x30
#define RFM64_05_LOW_POWER_RX		0x20
#define RFM64_05_TRIM_BAND			0x06
#define RFM64_05_RF_FREQUENCY		0x01
#define RFM64_06_R1					0xff
#define RFM64_07_P1					0xff
#define RFM64_08_S1					0xff
#define RFM64_09_R2					0xff
#define RFM64_10_P2					0xff
#define RFM64_11_S2					0xff
#define RFM64_12_RES				0x07	/* Reserved. This looks like a doc. error */

/* Interrupt configuration - IRQParam
*/
#define RFM64_12_FIFO_SIZE			0xc0
#define RFM64_12_FIFO_THRESH		0x3f
#define RFM64_13_RX_STBY_IRQ_0		0xc0
#define RFM64_13_RX_STBY_IRQ_1		0x30
#define RFM64_13_TX_START_IRQ_0		0x08
#define RFM64_13_TX_IRQ_1			0x04
#define RFM64_13_FIFO_FULL			0x02
#define RFM64_13_FIFO_NOT_EMPTY		0x01
#define RFM64_14_FIFO_FILL			0x80
#define RFM64_14_FIFO_FILL_METHOD	0x40
#define RFM64_14_TX_DONE			0x20
#define RFM64_14_FIFO_OVERRUN_CLEAR	0x10
#define RFM64_14_RES				0x08	/* Weird; see description */
#define RFM64_14_RSSI_IRQ			0x04
#define RFM64_14_PLL_LOCKED			0x02
#define RFM64_14_PLL_LOCK_ENABLE	0x01
#define RFM64_15_RSSI_IRQ_THRESH	0xff

/* Receiver configuration - RXParam
*/
#define RFM64_16_PASSIVE_FILT		0xf0
#define RFM64_16_BUTTER_FILT		0x0f
#define RFM64_17_POLYPFILT_CENTRE	0xf0
#define RFM64_17_RES				0x0f
#define RFM64_18_POLYPFILT_ON		0x80
#define RFM64_18_BITSYNC_OFF		0x40
#define RFM64_18_SYNC_ON			0x20
#define RFM64_18_SYNC_SIZE			0x18
#define RFM64_18_SYNC_TOL			0x06
#define RFM64_18_RES				0x01
#define RFM64_19_OOK_THRESH			0xff
#define RFM64_20_RSSI_VAL			0xff	/* READ ONLY */
#define RFM64_21_OOK_THRESH_STEP	0xe0
#define RFM64_21_OOK_THRESH_DEC_P	0x1c
#define RFM64_21_OOK_AVG_THRESH_CUT	0x03

/* Sync word configuration - SYNCParam
*/
#define RFM64_22_SYNC_VALUE_31_24	0xff
#define RFM64_23_SYNC_VALUE_23_16	0xff
#define RFM64_24_SYNC_VALUE_15_8	0xff
#define RFM64_25_SYNC_VALUE_7_0		0xff

/* Transmitter configuration - TXParam
*/
#define RFM64_26_INTERP_FILT		0xf0
#define RFM64_26_P_OUT				0x0e
#define RFM64_26_TX_ZERO_IF			0x01

/* Oscillator configuration - OSCParam
*/
#define RFM64_27_CLKOUT_ON			0x80
#define RFM64_27_CLKOUT_FREQ		0x7c
#define RFM64_27_RES				0x03

/* Packet handling configuration - PKTParam
*/
#define RFM64_28_MANCHESTER_ON		0x80
#define RFM64_28_PAYLOAD_LENGTH		0x7f
#define RFM64_29_NODE_ADRS			0xff
#define RFM64_30_PKT_FORMAT			0x80
#define RFM64_30_PREAMBLE_SIZE		0x60
#define RFM64_30_WHITENING_ON		0x10
#define RFM64_30_CRC_ON				0x08
#define RFM64_30_ADRS_FILT			0x06
#define RFM64_30_CRC_STATUS			0x01
#define RFM64_31_CRC_AUTOCLR		0x80
#define RFM64_31_FIFO_STBY_ACCESS	0x40
#define RFM64_31_RES				0x3f

/* SPI modes for STM32
 *
 * CR1: DV_SPI_BIDIMODE = DV_SPI_BIDIOE = DV_SPI_CRCEN = DV_SPI_CRNEXT = 0
 * CR1: DV_SPI_DFF = 0 (8-bit)
 * CR1: DV_SPI_RXONLY = 0
 * CR1: DV_SPI_SSM = 1 
 * CR1: DV_SPI_SSI = 1
 * CR1: DV_SPI_LSBFIRST = 0 (MSB first)
 * CR1: DV_SPI_MSTR = 1
 * CR1: DV_SPI_CPOL = DV_SPI_CPHA = 0
 *
 * CR2: 0 (all interrupts disabled)
*/
#define RFM64_STM32_SPI_MODE		(DV_SPI_SSM | DV_SPI_SSI | DV_SPI_MSTR)

/* API functions
*/
extern int rfm64_read_cfgr(dv_u8_t adr, dv_u8_t *out);

#endif
