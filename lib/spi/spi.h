#ifndef LIB_SPI_SPI_H_
#define LIB_SPI_SPI_H_

#include <stdint.h>

namespace spi
{

void init ();
uint8_t transfer (uint8_t value);

}


#endif /* LIB_SPI_SPI_H_ */
