#ifndef LIB_SETTINGS_SETTINGS_H_
#define LIB_SETTINGS_SETTINGS_H_

/*
 * EEPROM addresses for settings
 */

#include <avr/eeprom.h>

#define EEPROM_HEADER 0x5552

#define _eeprom_byte(x) ((uint8_t *) (x))
#define _eeprom_word(x) ((uint32_t *) (x))
#define _eeprom_float(x) ((float *) (x))

namespace settings
{

void init ();
void reset ();

}

#endif /* LIB_SETTINGS_SETTINGS_H_ */
