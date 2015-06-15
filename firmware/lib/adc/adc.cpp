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
#include "adc.h"
#include <avr/io.h>
#include "../../config.h"
#include "../../settings.h"

#define ADC_EEPROM_REF     _eeprom_byte (ADC_EEPROM_OFFSET)
#define ADC_EEPROM_REF_VOLTAGE _eeprom_float (ADC_EEPROM_OFFSET + 1)

namespace adc
{

uint8_t s_ref;
float s_ref_voltage;

void init ()
{
	s_ref = eeprom_read_byte (ADC_EEPROM_REF) << 6;
	s_ref_voltage = eeprom_read_float (ADC_EEPROM_REF_VOLTAGE);

	ADMUX = s_ref;
	ADCSRA |= _BV (ADEN) | _BV (ADPS0) | _BV (ADPS1) | _BV (ADPS2);
}

// TODO: Interrupt-based conversions, filters

uint16_t read (uint8_t channel)
{
	ADMUX = s_ref | (channel & 0x0f);
	ADCSRA |= _BV (ADSC);
	loop_until_bit_is_clear (ADCSRA, ADSC);
	ADCSRA |= _BV (ADIF);
	return ADC;
}

float value (uint8_t channel, float divider)
{
	return (read (channel) / 1024.0 * s_ref_voltage) * divider;
}

namespace settings
{

	void reset ()
	{
		eeprom_update_byte (ADC_EEPROM_REF, ADC_DEFAULT_REF);
		eeprom_update_float (ADC_EEPROM_REF_VOLTAGE, ADC_DEFAULT_REF_VOLTAGE);
	}

}  // namespace settings

}  // namespace adc
