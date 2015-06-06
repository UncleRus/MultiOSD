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
#include "adc_battery.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "../../lib/adc/adc.h"
#include "../../lib/timer/timer.h"
#include "../telemetry.h"
#include "../../settings.h"
#include "../../config.h"

// eeprom addresses
#define ADC_BATTERY_EEPROM_CURRENT_SENSOR		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET)
#define ADC_BATTERY_EEPROM_VOLTAGE_MULTIPLIER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 1)
#define ADC_BATTERY_EEPROM_CURRENT_MULTIPLIER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 5)
#define ADC_BATTERY_EEPROM_UPDATE_INTERVAL		_eeprom_word (ADC_BATTERY_EEPROM_OFFSET + 9)
#define ADC_BATTERY_EEPROM_VOLTAGE_CHANNEL		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET + 11)
#define ADC_BATTERY_EEPROM_CURRENT_CHANNEL		_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET + 12)

namespace telemetry
{

namespace modules
{

namespace adc_battery
{

void reset ()
{
	eeprom_update_byte (ADC_BATTERY_EEPROM_CURRENT_SENSOR, ADC_BATTERY_DEFAULT_CURRENT_SENSOR);
	eeprom_update_float (ADC_BATTERY_EEPROM_VOLTAGE_MULTIPLIER, ADC_BATTERY_DEFAULT_VOLTAGE_MULTIPLIER);
	eeprom_update_float (ADC_BATTERY_EEPROM_CURRENT_MULTIPLIER, ADC_BATTERY_DEFAULT_CURRENT_MULTIPLIER);
	eeprom_update_word (ADC_BATTERY_EEPROM_UPDATE_INTERVAL, ADC_BATTERY_DEFAULT_UPDATE_INTERVAL);
	eeprom_update_byte (ADC_BATTERY_EEPROM_VOLTAGE_CHANNEL, ADC_BATTERY_DEFAULT_VOLTAGE_CHANNEL);
	eeprom_update_byte (ADC_BATTERY_EEPROM_CURRENT_CHANNEL, ADC_BATTERY_DEFAULT_CURRENT_CHANNEL);
}

static uint32_t _last_update_time = 0;
static bool _current_sensor;
static float _voltage_multiplier;
static float _current_multiplier;
static uint16_t _interval;
static uint8_t _voltage_channel, _current_channel;

void init ()
{
	_current_sensor = eeprom_read_byte (ADC_BATTERY_EEPROM_CURRENT_SENSOR);
	_voltage_multiplier = eeprom_read_float (ADC_BATTERY_EEPROM_VOLTAGE_MULTIPLIER);
	if (_current_sensor)
		_current_multiplier = eeprom_read_float (ADC_BATTERY_EEPROM_CURRENT_MULTIPLIER);
	_interval = eeprom_read_word (ADC_BATTERY_EEPROM_UPDATE_INTERVAL);
	_voltage_channel = eeprom_read_byte (ADC_BATTERY_EEPROM_VOLTAGE_CHANNEL);
	_current_channel = eeprom_read_byte (ADC_BATTERY_EEPROM_CURRENT_CHANNEL);
}

bool update ()
{
	uint32_t ticks = timer::ticks ();
	uint16_t interval = ticks - _last_update_time;

	if (interval < _interval) return false;

	_last_update_time = ticks;

	telemetry::battery::voltage = adc::value (_voltage_channel, _voltage_multiplier);
	telemetry::battery::update_voltage ();

	if (_current_sensor)
	{
		telemetry::battery::current = adc::value (_current_channel, _current_multiplier);
		telemetry::battery::update_consumed ();
	}
	return true;
}


}  // namespace adc_battery

}  // namespace modules

}  // namespace telemetry
