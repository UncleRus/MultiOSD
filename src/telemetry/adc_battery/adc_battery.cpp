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
#define ADC_BATTERY_EEPROM_CURRENT_SENSOR	_eeprom_byte (ADC_BATTERY_EEPROM_OFFSET)
#define ADC_BATTERY_EEPROM_VOLTAGE_DIVIDER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 1)
#define ADC_BATTERY_EEPROM_CURRENT_DIVIDER	_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 5)
#define ADC_BATTERY_EEPROM_LOW_VOLTAGE		_eeprom_float (ADC_BATTERY_EEPROM_OFFSET + 9)

namespace adc_battery
{

static uint8_t _last_update_time = 0;
static bool _current_sensor = false;
static float _voltage_divider = 0.0;
static float _current_divider = 0.0;
static float _battery_low_voltage = 0.0;

void init ()
{
	_current_sensor = eeprom_read_byte (ADC_BATTERY_EEPROM_CURRENT_SENSOR);
	_voltage_divider = eeprom_read_float (ADC_BATTERY_EEPROM_VOLTAGE_DIVIDER);
	if (_current_sensor)
		_current_divider = eeprom_read_float (ADC_BATTERY_EEPROM_CURRENT_DIVIDER);
	_battery_low_voltage = eeprom_read_float (ADC_BATTERY_EEPROM_LOW_VOLTAGE);
}

bool update ()
{
	uint32_t ticks = timer::ticks ();
	uint32_t interval = ticks - _last_update_time;

	if (interval < ADC_BATTERY_UPDATE_INTERVAL) return false;

	_last_update_time = ticks;

	telemetry::battery::voltage = ADC_VALUE (adc::read (ADC_BATTERY_VOLTAGE_CHANNEL), _voltage_divider);
	telemetry::messages::battery_low = telemetry::battery::voltage <= _battery_low_voltage;
	if (_current_sensor)
	{
		telemetry::battery::current = ADC_VALUE (adc::read (ADC_BATTERY_CURRENT_CHANNEL), _current_divider);
		// FIXME: incorrect consumed energy calc
		telemetry::battery::consumed += (telemetry::battery::current * ((float) interval / 3600.0));
	}
	return true;
}


namespace settings
{

void reset ()
{
	eeprom_write_byte (ADC_BATTERY_EEPROM_CURRENT_SENSOR, ADC_BATTERY_DEFAULT_CURRENT_SENSOR);
	eeprom_write_float (ADC_BATTERY_EEPROM_VOLTAGE_DIVIDER, ADC_BATTERY_DEFAULT_VOLTAGE_DIVIDER);
	eeprom_write_float (ADC_BATTERY_EEPROM_CURRENT_DIVIDER, ADC_BATTERY_DEFAULT_CURRENT_DIVIDER);
	eeprom_write_float (ADC_BATTERY_EEPROM_LOW_VOLTAGE, ADC_BATTERY_DEFAULT_LOW_VOLTAGE);
}

}

}
