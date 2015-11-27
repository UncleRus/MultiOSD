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
#ifndef LIB_ADC_ADC_H_
#define LIB_ADC_ADC_H_

#define ADC_MODULE

#include <stdint.h>

namespace adc
{

#define ADC_REF_AREF		0
#define ADC_REF_AVCC		1
#define ADC_REF_INTERNAL	3

namespace settings
{

	extern bool initialized;
	extern uint8_t ref_source;
	extern float ref_voltage;

	void init ();
	void reset ();

}  // namespace settings

void init ();
uint16_t read (uint8_t channel);
float value (uint8_t channel, float multiplier);

}

#endif /* LIB_ADC_ADC_H_ */
