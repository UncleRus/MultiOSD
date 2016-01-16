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
#define EEPROM_ADDR_LOW_THRESHOLD   _eeprom_byte (ADC_RSSI_EEPROM_OFFSET + 3)
#define EEPROM_ADDR_MULTIPLIER      _eeprom_float (ADC_RSSI_EEPROM_OFFSET + 4)

const char __opt_arch [] PROGMEM = "ARCH";
const char __opt_arui [] PROGMEM = "ARUI";
const char __opt_arlt [] PROGMEM = "ARLT";
const char __opt_armul [] PROGMEM = "ARMUL";

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option  (__opt_arch, EEPROM_ADDR_CHANNEL),
	declare_uint16_option (__opt_arui, EEPROM_ADDR_UPDATE_INTERVAL),
	declare_uint8_option  (__opt_arlt, EEPROM_ADDR_LOW_THRESHOLD),
	declare_float_option  (__opt_armul, EEPROM_ADDR_MULTIPLIER),
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
	eeprom_update_byte  (EEPROM_ADDR_LOW_THRESHOLD, ADC_RSSI_DEFAULT_LOW_THRESHOLD);
	eeprom_update_float (EEPROM_ADDR_MULTIPLIER, ADC_RSSI_DEFAULT_MULTIPLIER);

	adc::settings::reset ();
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

uint8_t channel;
uint16_t update_interval;
uint8_t low_threshold;
uint32_t last_update;
float multiplier;

void init ()
{
	adc::init ();

	channel = eeprom_read_byte (EEPROM_ADDR_CHANNEL);
	update_interval = eeprom_read_word (EEPROM_ADDR_UPDATE_INTERVAL);
	low_threshold = eeprom_read_byte (EEPROM_ADDR_LOW_THRESHOLD);
	multiplier = eeprom_read_float (EEPROM_ADDR_MULTIPLIER);
}

bool update ()
{
	uint16_t interval = telemetry::ticks - last_update;

	if (interval < update_interval) return false;

	last_update = telemetry::ticks;

	int16_t value = round (adc::value (channel, multiplier));
	if (value < 0) value = 0;
	else if (value > 100) value = 100;
	input::rssi = value;

	messages::rssi_low = input::rssi < low_threshold;

	return true;
}

}  // namespace adc_rssi

}  // namespace modules

}  // namespace telemetry
