/*
 * This program is free software: you can redistribute it and/or modify
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
#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <avr/eeprom.h>

#define EEPROM_SIZE 0x400
#define EEPROM_HEADER 0x5552

#define MAX7456_EEPROM_OFFSET        0x10
#define ADC_BATTERY_EEPROM_OFFSET    0x12
#define UAVTALK_EEPROM_OFFSET        0x20
#define OSD_EEPROM_OFFSET            0x70
#define OSD_SCREENS_EEPROM_OFFSET    (OSD_EEPROM_OFFSET + 0x10)

#define _eeprom_byte(x) ((uint8_t *) (x))
#define _eeprom_word(x) ((uint32_t *) (x))
#define _eeprom_float(x) ((float *) (x))

namespace settings
{

void init ();
void reset ();

}

#endif /* SETTINGS_H_ */
