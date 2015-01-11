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
#include "../../adc/adc.h"
#include "../../../config.h"
#include "../../settings/settings.h"
#include "../../timer/timer.h"
#include "../../telemetry/telemetry.h"

namespace adc_battery
{

static uint8_t _last_update_time = 0;
static bool _current_sensor = false;
static float _voltage_divider = 0.0;
static float _current_divider = 0.0;
static float _battery_low_voltage = 0.0;

void init ()
{
	_current_sensor = eeprom_read_byte (EEPROM_ADC_BATTERY_CURRENT_SENSOR);
	_voltage_divider = eeprom_read_float (EEPROM_ADC_BATTERY_VOLTAGE_DIVIDER);
	if (_current_sensor)
		_current_divider = eeprom_read_float (EEPROM_ADC_BATTERY_VOLTAGE_DIVIDER);
	_battery_low_voltage = eeprom_read_float (EEPROM_BATTERY_LOW_VOLTAGE);
}

bool update ()
{
	uint32_t ticks = timer::ticks ();
	uint16_t interval = (uint16_t) (ticks - _last_update_time);
	if (interval < ADC_BATTERY_UPDATE_INTERVAL)
		return false;
	_last_update_time = ticks;

	telemetry::battery::voltage = ADC_VALUE (adc::read (ADC_BATTERY_VOLTAGE_PIN), ADC_BATTERY_VOLTAGE_DIVIDER);
	telemetry::messages::battery_low = telemetry::battery::voltage <= _battery_low_voltage;
	if (_current_sensor)
	{
		telemetry::battery::current = ADC_VALUE (adc::read (ADC_BATTERY_CURRENT_PIN), ADC_BATTERY_CURRENT_DIVIDER);
		telemetry::battery::consumed += telemetry::battery::current * interval / 3600.0;
	}
	return true;
}

}
