# Weather station

This is the firmware for a weather station. The design uses various remote sensors communicating
over serial lines, RF etc. to gather information.

The plan is to log the data to flash memory (SD card, etc.) and dump it to a PC for offline
analysis.

Possible sensors:

* Temperature
* Humidity
* Wind speed
* Rainfall
* Radiation
* ... depending on what I can find.

## Hardware

The base station uses a "Blue Pill" STM32 board - chosen because it has three UARTs.

The remote sensors will use various AVR devices - mostly ATtiny.

The remote sensors are in separate repositories.

## Additional software

The base station runs under my Davroska operating system.

## License

GPL v3 or later.  See the LICENSE file
