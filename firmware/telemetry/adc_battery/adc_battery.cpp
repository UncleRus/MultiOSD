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

#define EEPROM_ADDR_CURRENT_SENSOR		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET)
#define EEPROM_ADDR_VOLTAGE_MULTIPLIER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_CURRENT_MULTIPLIER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 5)
#define EEPROM_ADDR_UPDATE_INTERVAL		_eeprom_word (ADC_BATTERY_EEPROM_OFFSET + 9)
#define EEPROM_ADDR_VOLTAGE_CHANNEL		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET + 11)
#define EEPROM_ADDR_CURRENT_CHANNEL		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET + 12)

const char __opt_abcs   [] PROGMEM = "ABCS";
const char __opt_abvmul [] PROGMEM = "ABVMUL";
const char __opt_abcmul [] PROGMEM = "ABCMUL";
const char __opt_abui   [] PROGMEM = "ABUI";
const char __opt_abvch  [] PROGMEM = "ABVCH";
const char __opt_abcch  [] PROGMEM = "ABCCH";

const ::settings::option_t __settings [] PROGMEM = {
	declare_bool_option   (__opt_abcs,   EEPROM_ADDR_CURRENT_SENSOR),
	declare_float_option  (__opt_abvmul, EEPROM_ADDR_VOLTAGE_MULTIPLIER),
	declare_float_option  (__opt_abcmul, EEPROM_ADDR_CURRENT_MULTIPLIER),
	declare_uint16_option (__opt_abui,   EEPROM_ADDR_UPDATE_INTERVAL),
	declare_uint8_option  (__opt_abvch,  EEPROM_ADDR_VOLTAGE_CHANNEL),
	declare_uint8_option  (__opt_abcch,  EEPROM_ADDR_CURRENT_CHANNEL),
};

void init ()
{
	adc::settings::init ();
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_CURRENT_SENSOR, ADC_BATTERY_DEFAULT_CURRENT_SENSOR);
	eeprom_update_float (EEPROM_ADDR_VOLTAGE_MULTIPLIER, ADC_BATTERY_DEFAULT_VOLTAGE_MULTIPLIER);
	eeprom_update_float (EEPROM_ADDR_CURRENT_MULTIPLIER, ADC_BATTERY_DEFAULT_CURRENT_MULTIPLIER);
	eeprom_update_word (EEPROM_ADDR_UPDATE_INTERVAL, ADC_BATTERY_DEFAULT_UPDATE_INTERVAL);
	eeprom_update_byte (EEPROM_ADDR_VOLTAGE_CHANNEL, ADC_BATTERY_DEFAULT_VOLTAGE_CHANNEL);
	eeprom_update_byte (EEPROM_ADDR_CURRENT_CHANNEL, ADC_BATTERY_DEFAULT_CURRENT_CHANNEL);

	adc::settings::reset ();
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

uint32_t last_update_time = 0;
bool current_sensor;
float voltage_multiplier;
float current_multiplier;
uint16_t update_interval;
uint8_t voltage_channel, current_channel;

void init ()
{
	adc::init ();

	current_sensor = eeprom_read_byte (EEPROM_ADDR_CURRENT_SENSOR);
	voltage_multiplier = eeprom_read_float (EEPROM_ADDR_VOLTAGE_MULTIPLIER);
	if (current_sensor)
		current_multiplier = eeprom_read_float (EEPROM_ADDR_CURRENT_MULTIPLIER);
	update_interval = eeprom_read_word (EEPROM_ADDR_UPDATE_INTERVAL);
	voltage_channel = eeprom_read_byte (EEPROM_ADDR_VOLTAGE_CHANNEL);
	current_channel = eeprom_read_byte (EEPROM_ADDR_CURRENT_CHANNEL);
}

bool update ()
{
	uint16_t interval = timer::ticks () - last_update_time;

	if (interval < update_interval) return false;

	last_update_time += interval;

	float dt = interval / 1000.0;
	float alpha = 1.0 - dt / (dt + 2.0);

	telemetry::battery::voltage = alpha * telemetry::battery::voltage + (1 - alpha) * adc::value (voltage_channel, voltage_multiplier);
	telemetry::battery::update_voltage ();

	if (current_sensor)
	{
		telemetry::battery::current = alpha * telemetry::battery::current + (1 - alpha) * adc::value (current_channel, current_multiplier);
		telemetry::battery::update_consumed (interval);
	}
	return true;
}


}  // namespace adc_battery

}  // namespace modules

}  // namespace telemetry
