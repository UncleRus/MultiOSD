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
#include "adc_rssi.h"
#include <avr/eeprom.h>
#include <math.h>
#include "../../lib/adc/adc.h"
#include "../telemetry.h"
#include "../../config.h"
#include "../../settings.h"
#include "../../eeprom.h"

namespace telemetry
{

namespace modules
{

namespace adc_rssi
{

namespace settings
{

#define EEPROM_ADDR_CHANNEL         _eeprom_byte (ADC_RSSI_EEPROM_OFFSET)
#define EEPROM_ADDR_UPDATE_INTERVAL _eeprom_word (ADC_RSSI_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_FACTOR          _eeprom_float (ADC_RSSI_EEPROM_OFFSET + 3)

const char __opt_arch  [] PROGMEM = "ARCH";
const char __opt_arint [] PROGMEM = "ARINT";
const char __opt_arf   [] PROGMEM = "ARF";

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option  (__opt_arch, EEPROM_ADDR_CHANNEL),
	declare_uint16_option (__opt_arint, EEPROM_ADDR_UPDATE_INTERVAL),
	declare_float_option  (__opt_arf, EEPROM_ADDR_FACTOR),
};

struct settings_t
{
	uint8_t channel;
	uint16_t update_interval;
	float factor;

	settings_t ()
	{
		channel = eeprom_read_byte (EEPROM_ADDR_CHANNEL);
		update_interval = eeprom_read_word (EEPROM_ADDR_UPDATE_INTERVAL);
		factor = eeprom_read_float (EEPROM_ADDR_FACTOR);
	}
};

void init ()
{
	adc::settings::init ();
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte  (EEPROM_ADDR_CHANNEL, ADC_RSSI_DEFAULT_CHANNEL);
	eeprom_update_word  (EEPROM_ADDR_UPDATE_INTERVAL, ADC_RSSI_DEFAULT_UPDATE_INTERVAL);
	eeprom_update_float (EEPROM_ADDR_FACTOR, ADC_RSSI_DEFAULT_MULTIPLIER);

	adc::settings::reset ();
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

uint32_t last_update;
settings::settings_t s;

void init ()
{
	adc::init ();
}

bool update ()
{
	uint16_t interval = telemetry::update_time - last_update;

	if (interval < s.update_interval) return false;

	last_update = telemetry::update_time;

	int16_t value = round (adc::value (s.channel, s.factor));

	if (value < 0) value = 0;
	else if (value > 100) value = 100;

	input::set_rssi (value);

	return true;
}

}  // namespace adc_rssi

}  // namespace modules

}  // namespace telemetry
