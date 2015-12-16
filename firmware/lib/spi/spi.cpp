/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * MultiOSD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "spi.h"
#include <avr/io.h>
#include "../common.h"
#include "../../config.h"

namespace spi
{

void init ()
{
	// initialize the SPI pins
	SPI_DDR = (SPI_DDR & ~_BV (SPI_MISO_BIT)) | _BV (SPI_SS_BIT) | _BV (SPI_MOSI_BIT) | _BV (SPI_SCK_BIT);

	// SPI master
	SPCR = SPI_CPOL | SPI_CPHA | SPI_CLOCK | _BV (SPE) | _BV (MSTR);
}

uint8_t transfer (uint8_t value)
{
	SPDR = value;
	loop_until_bit_is_set (SPSR, SPIF);
	return SPDR;
}

}
