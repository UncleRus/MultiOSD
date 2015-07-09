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

static uint32_t last_update_time = 0;
static bool current_sensor;
static float voltage_multiplier;
static float current_multiplier;
static uint16_t update_interval;
static uint8_t voltage_channel, current_channel;

void init ()
{
	adc::init ();

	current_sensor = eeprom_read_byte (ADC_BATTERY_EEPROM_CURRENT_SENSOR);
	voltage_multiplier = eeprom_read_float (ADC_BATTERY_EEPROM_VOLTAGE_MULTIPLIER);
	if (current_sensor)
		current_multiplier = eeprom_read_float (ADC_BATTERY_EEPROM_CURRENT_MULTIPLIER);
	update_interval = eeprom_read_word (ADC_BATTERY_EEPROM_UPDATE_INTERVAL);
	voltage_channel = eeprom_read_byte (ADC_BATTERY_EEPROM_VOLTAGE_CHANNEL);
	current_channel = eeprom_read_byte (ADC_BATTERY_EEPROM_CURRENT_CHANNEL);
}

bool update ()
{
	uint16_t interval = telemetry::ticks - last_update_time;

	if (interval < update_interval) return false;

	last_update_time = telemetry::ticks;

	telemetry::battery::voltage = adc::value (voltage_channel, voltage_multiplier);
	telemetry::battery::update_voltage ();

	if (current_sensor)
	{
		telemetry::battery::current = adc::value (current_channel, current_multiplier);
		telemetry::battery::update_consumed ();
	}
	return true;
}


}  // namespace adc_battery

}  // namespace modules

}  // namespace telemetry
