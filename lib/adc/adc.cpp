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
#include "adc.h"
#include <avr/io.h>
#include "../../config.h"

namespace adc
{

#define _ADC_REF (ADC_REF << 6)

void init ()
{
	ADMUX = _ADC_REF;
	ADCSRA |= _BV (ADEN);
}

// TODO: Interrupt-based conversions

uint16_t read (uint8_t channel)
{
	ADMUX = _ADC_REF | (channel & 0x0f);
	ADCSRA |= _BV (ADSC);
	loop_until_bit_is_set (ADCSRA, ADSC);
	return ADC;
}

}
