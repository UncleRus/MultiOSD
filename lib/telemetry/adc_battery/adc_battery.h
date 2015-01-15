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
#ifndef LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_
#define LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_

#define ADC_BATTERY_EEPROM_OFFSET 0x12

namespace adc_battery
{

void init ();
bool update ();

namespace settings
{
	void reset ();
}

}

#endif /* LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_ */
