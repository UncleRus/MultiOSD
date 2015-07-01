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
#include "adc_rssi.h"
#include <avr/eeprom.h>
#include <math.h>
#include "../../lib/adc/adc.h"
#include "../telemetry.h"
#include "../../settings.h"
#include "../../config.h"

#define ADC_RSSI_EEPROM_CHANNEL			_eeprom_byte (ADC_RSSI_EEPROM_OFFSET)
#define ADC_RSSI_EEPROM_UPDATE_INTERVAL	_eeprom_word (ADC_RSSI_EEPROM_OFFSET + 1)
#define ADC_RSSI_EEPROM_LOW_THRESHOLD	_eeprom_byte (ADC_RSSI_EEPROM_OFFSET + 3)
#define ADC_RSSI_EEPROM_MULTIPLIER		_eeprom_float (ADC_RSSI_EEPROM_OFFSET + 4)


namespace telemetry
{

namespace modules
{

namespace adc_rssi
{

void reset ()
{
	eeprom_update_byte (ADC_RSSI_EEPROM_CHANNEL, ADC_RSSI_DEFAULT_CHANNEL);
	eeprom_update_word (ADC_RSSI_EEPROM_UPDATE_INTERVAL, ADC_RSSI_DEFAULT_UPDATE_INTERVAL);
	eeprom_update_byte (ADC_RSSI_EEPROM_LOW_THRESHOLD, ADC_RSSI_DEFAULT_LOW_THRESHOLD);
	eeprom_update_float (ADC_RSSI_EEPROM_MULTIPLIER, ADC_RSSI_DEFAULT_MULTIPLIER);
}

uint8_t channel;
uint16_t update_interval;
uint8_t low_threshold;
uint32_t last_update;
float multiplier;

void init ()
{
	channel = eeprom_read_byte (ADC_RSSI_EEPROM_CHANNEL);
	update_interval = eeprom_read_word (ADC_RSSI_EEPROM_UPDATE_INTERVAL);
	low_threshold = eeprom_read_byte (ADC_RSSI_EEPROM_LOW_THRESHOLD);
	multiplier = eeprom_read_float (ADC_RSSI_EEPROM_MULTIPLIER);
}

bool update ()
{
	uint16_t interval = telemetry::ticks - last_update;

	if (interval < update_interval) return false;

	last_update = telemetry::ticks;

	int16_t value = round (adc::value (channel, multiplier));
	if (value < 0) value = 0;
	else if (value > 100) value = 100;
	telemetry::input::rssi = value;

	telemetry::messages::rssi_low = telemetry::input::rssi < low_threshold;

	return true;
}

}  // namespace adc_rssi

}  // namespace modules

}  // namespace telemetry
