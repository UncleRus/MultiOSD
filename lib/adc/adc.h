#ifndef LIB_ADC_ADC_H_
#define LIB_ADC_ADC_H_

#include <stdint.h>

namespace adc
{

#define ADC_REF_AREF		0
#define ADC_REF_AVCC		1
#define ADC_REF_INTERNAL	3

void init ();
uint16_t read (uint8_t channel);

}

#endif /* LIB_ADC_ADC_H_ */
