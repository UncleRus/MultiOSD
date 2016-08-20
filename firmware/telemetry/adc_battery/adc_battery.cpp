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
#include "adc_battery.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "../../lib/timer/timer.h"
#include "../../lib/adc/adc.h"
#include "../telemetry.h"
#include "../../config.h"
#include "../../settings.h"
#include "../../eeprom.h"

namespace telemetry
{

namespace modules
{

namespace adc_battery
{

namespace settings
{

#define EEPROM_ADDR_UPDATE_INTERVAL     _eeprom_word  (ADC_BATTERY_EEPROM_OFFSET)
#define EEPROM_ADDR_VOLTAGE1_CHANNEL    _eeprom_byte  (ADC_BATTERY_EEPROM_OFFSET + 2)
#define EEPROM_ADDR_VOLTAGE2_CHANNEL    _eeprom_byte  (ADC_BATTERY_EEPROM_OFFSET + 3)
#define EEPROM_ADDR_AMPERAGE1_CHANNEL	_eeprom_byte  (ADC_BATTERY_EEPROM_OFFSET + 4)
#define EEPROM_ADDR_AMPERAGE2_CHANNEL	_eeprom_byte  (ADC_BATTERY_EEPROM_OFFSET + 5)
#define EEPROM_ADDR_VOLTAGE1_FACTOR _eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 6)
#define EEPROM_ADDR_VOLTAGE2_FACTOR _eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 10)
#define EEPROM_ADDR_AMPERAGE1_FACTOR _eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 14)
#define EEPROM_ADDR_AMPERAGE2_FACTOR _eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 18)

const char __opt_abint  [] PROGMEM = "ABINT";
const char __opt_abu1ch [] PROGMEM = "ABU1CH";
const char __opt_abu2ch [] PROGMEM = "ABU2CH";
const char __opt_abi1ch [] PROGMEM = "ABI1CH";
const char __opt_abi2ch [] PROGMEM = "ABI2CH";
const char __opt_abu1f  [] PROGMEM = "ABU1F";
const char __opt_abu2f  [] PROGMEM = "ABU2F";
const char __opt_abi1f  [] PROGMEM = "ABI1F";
const char __opt_abi2f  [] PROGMEM = "ABI2F";

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint16_option (__opt_abint,  EEPROM_ADDR_UPDATE_INTERVAL),
	declare_uint8_option  (__opt_abu1ch, EEPROM_ADDR_VOLTAGE1_CHANNEL),
	declare_uint8_option  (__opt_abu2ch, EEPROM_ADDR_VOLTAGE2_CHANNEL),
	declare_uint8_option  (__opt_abi1ch, EEPROM_ADDR_AMPERAGE1_CHANNEL),
	declare_uint8_option  (__opt_abi2ch, EEPROM_ADDR_AMPERAGE2_CHANNEL),
	declare_float_option  (__opt_abu1f,  EEPROM_ADDR_VOLTAGE1_FACTOR),
	declare_float_option  (__opt_abu2f,  EEPROM_ADDR_VOLTAGE2_FACTOR),
	declare_float_option  (__opt_abi1f,  EEPROM_ADDR_AMPERAGE1_FACTOR),
	declare_float_option  (__opt_abi2f,  EEPROM_ADDR_AMPERAGE2_FACTOR),
};

struct settings_t
{
	uint16_t update_interval;
	uint8_t  voltage1_channel;
	uint8_t  voltage2_channel;
	uint8_t  amperage1_channel;
	uint8_t  amperage2_channel;
	float    voltage1_factor;
	float    voltage2_factor;
	float    amperage1_factor;
	float    amperage2_factor;

	settings_t ()
	{
		update_interval = eeprom_read_word (EEPROM_ADDR_UPDATE_INTERVAL);
		voltage1_channel = eeprom_read_byte (EEPROM_ADDR_VOLTAGE1_CHANNEL);
		voltage2_channel = eeprom_read_byte (EEPROM_ADDR_VOLTAGE2_CHANNEL);
		amperage1_channel = eeprom_read_byte (EEPROM_ADDR_AMPERAGE1_CHANNEL);
		amperage2_channel = eeprom_read_byte (EEPROM_ADDR_AMPERAGE2_CHANNEL);
		voltage1_factor = eeprom_read_float (EEPROM_ADDR_VOLTAGE1_FACTOR);
		voltage2_factor = eeprom_read_float (EEPROM_ADDR_VOLTAGE2_FACTOR);
		amperage1_factor = eeprom_read_float (EEPROM_ADDR_AMPERAGE1_FACTOR);
		amperage2_factor = eeprom_read_float (EEPROM_ADDR_AMPERAGE2_FACTOR);
	}
};

void init ()
{
	adc::settings::init ();
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_word (EEPROM_ADDR_UPDATE_INTERVAL, ADC_BATTERY_DEFAULT_UPDATE_INTERVAL);
	eeprom_update_byte (EEPROM_ADDR_VOLTAGE1_CHANNEL, ADC_BATTERY_DEFAULT_VOLTAGE1_CHANNEL);
	eeprom_update_byte (EEPROM_ADDR_VOLTAGE2_CHANNEL, ADC_BATTERY_DEFAULT_VOLTAGE2_CHANNEL);
	eeprom_update_byte (EEPROM_ADDR_AMPERAGE1_CHANNEL, ADC_BATTERY_DEFAULT_AMPERAGE1_CHANNEL);
	eeprom_update_byte (EEPROM_ADDR_AMPERAGE2_CHANNEL, ADC_BATTERY_DEFAULT_AMPERAGE2_CHANNEL);
	eeprom_update_float (EEPROM_ADDR_VOLTAGE1_FACTOR, ADC_BATTERY_DEFAULT_VOLTAGE1_FACTOR);
	eeprom_update_float (EEPROM_ADDR_VOLTAGE2_FACTOR, ADC_BATTERY_DEFAULT_VOLTAGE2_FACTOR);
	eeprom_update_float (EEPROM_ADDR_AMPERAGE1_FACTOR, ADC_BATTERY_DEFAULT_AMPERAGE1_FACTOR);
	eeprom_update_float (EEPROM_ADDR_AMPERAGE2_FACTOR, ADC_BATTERY_DEFAULT_AMPERAGE2_FACTOR);

	adc::settings::reset ();
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

uint32_t last_update_time = 0;
settings::settings_t s;

void init ()
{
	adc::init ();
}

bool update ()
{
	uint16_t interval = timer::ticks () - last_update_time;

	if (interval < s.update_interval) return false;

	last_update_time += interval;

	float dt = interval / 1000.0;
	float alpha = 1.0 - dt / (dt + 2.0);

	battery::battery1.set_voltage (alpha * battery::battery1.voltage + (1 - alpha) * adc::value (s.voltage1_channel, s.voltage1_factor), true);
	battery::battery2.set_voltage (alpha * battery::battery2.voltage + (1 - alpha) * adc::value (s.voltage2_channel, s.voltage2_factor), true);
	battery::battery1.set_amperage (alpha * battery::battery1.amperage + (1 - alpha) * adc::value (s.amperage1_channel, s.amperage1_factor), interval);
	battery::battery2.set_amperage (alpha * battery::battery2.amperage + (1 - alpha) * adc::value (s.amperage2_channel, s.amperage2_factor), interval);

	return true;
}


}  // namespace adc_battery

}  // namespace modules

}  // namespace telemetry
