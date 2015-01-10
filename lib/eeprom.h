#ifndef LIB_EEPROM_H_
#define LIB_EEPROM_H_

/*
 * EEPROM addresses for settings
 */

#include <avr/eeprom.h>


/*
 * Video mode. 0x00 = PAL, 0x01 = NTSC
 */
#define EEPROM_OSD_VIDEO_MODE	((uint8_t *) 0x10)
/*
 * OSD brightness.
 * 	Bits 7-4: Don’t Care
 * 	Bits 3,2: Character Black Level (00 = 0%, 01 = 10%, 10 = 20%, 11 = 30%)
 * 	Bits 1,0: Character White Level (00 = 120%, 01 = 100%, 10 = 90%, 11 = 80%)
 */
#define EEPROM_OSD_BRIGHTNESS	((uint8_t *) 0x11)

/*
 * 0: current sensor disabled, > 0: enabled
 */
#define EEPROM_ADC_BATTERY_CURRENT_SENSOR ((uint8_t *) 0x12)

#define EEPROM_ADC_BATTERY_VOLTAGE_DIVIDER ((float *) 0x13)

#define EEPROM_ADC_BATTERY_CURRENT_DIVIDER ((float *) 0x17)

#define EEPROM_BATTERY_LOW_VOLTAGE ((float *) 0x1b)

#endif /* LIB_EEPROM_H_ */
