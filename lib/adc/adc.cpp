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
