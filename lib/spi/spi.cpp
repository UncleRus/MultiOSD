#include "spi.h"
#include <avr/io.h>
#include "../common.h"
#include "../../config.h"

namespace spi
{

void init ()
{
	// initialize the SPI pins
	sbi (SPI_DDR, SPI_SS_BIT);
	sbi (SPI_DDR, SPI_MOSI_BIT);
	cbi (SPI_DDR, SPI_MISO_BIT);
	sbi (SPI_DDR, SPI_SCK_BIT);

	// SPI master
	SPCR = SPI_CPOL | SPI_CPHA | SPI_CLOCK | _BV (SPE) | _BV (MSTR);
}

uint8_t transfer (uint8_t value)
{
	SPDR = value;
	//if (delay > 0) delay_ms (delay);

	while (!bis (SPSR, SPIF))
	  ;

	return SPDR;
}

}
