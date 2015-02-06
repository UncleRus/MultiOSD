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

static uint8_t _ref;
static float _ref_voltage;

void init ()
{
	_ref = eeprom_read_byte (ADC_EEPROM_REF) << 6;
	_ref_voltage = eeprom_read_float (ADC_EEPROM_REF_VOLTAGE);

	ADMUX = _ref;
	ADCSRA |= _BV (ADEN) | _BV (ADPS0) | _BV (ADPS1) | _BV (ADPS2);
}

// TODO: Interrupt-based conversions

uint16_t read (uint8_t channel)
{
	ADMUX = _ref | (channel & 0x0f);
	ADCSRA |= _BV (ADSC);
	loop_until_bit_is_set (ADCSRA, ADSC);
	ADCSRA |= _BV (ADIF);
	return ADC;
}

float value (uint8_t channel, float divider)
{
	return read (channel) / (1024.0 / _ref_voltage) * divider;
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
