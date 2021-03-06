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
#include "adc.h"
#include <avr/io.h>
#include "../../config.h"
#include "../../settings.h"
#include "../../eeprom.h"

namespace adc
{

namespace settings
{

#define EEPROM_ADDR_REF     _eeprom_byte (ADC_EEPROM_OFFSET)
#define EEPROM_ADDR_VOLTAGE _eeprom_float(ADC_EEPROM_OFFSET + 1)

const char __opt_adcref [] PROGMEM = "ADCREF";
const char __opt_adcuref[] PROGMEM = "ADCUREF";

const ::settings::option_t __settings[] PROGMEM = {
    declare_uint8_option (__opt_adcref, EEPROM_ADDR_REF),
    declare_float_option (__opt_adcuref, EEPROM_ADDR_VOLTAGE),
};

bool initialized = false;

void init()
{
    if (initialized)
        return;
    ::settings::append_section(__settings, sizeof(__settings) / sizeof(::settings::option_t));
    initialized = true;
}

void reset()
{
    eeprom_write_byte(EEPROM_ADDR_REF, ADC_DEFAULT_REF);
    eeprom_write_float(EEPROM_ADDR_VOLTAGE, ADC_DEFAULT_REF_VOLTAGE);
}

uint8_t ref_source;
float ref_voltage;

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

void init()
{
    settings::ref_source = eeprom_read_byte(EEPROM_ADDR_REF);
    settings::ref_voltage = eeprom_read_float(EEPROM_ADDR_VOLTAGE);

    ADMUX = settings::ref_source << 6;
    ADCSRA |= _BV (ADEN) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
}

// TODO: Interrupt-based conversions

uint16_t read(uint8_t channel)
{
    ADMUX = (settings::ref_source << 6) | (channel & 0x0f);
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    ADCSRA |= _BV(ADIF);
    return ADC;
}

float value(uint8_t channel, float multiplier)
{
    return (read(channel) / 1024.0 * settings::ref_voltage) * multiplier;
}

}  // namespace adc
