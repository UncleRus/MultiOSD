#ifndef LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_
#define LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_

namespace adc_battery
{

#define REF_VOLTAGE_AREF		0
#define REF_VOLTAGE_AVCC		1
#define REF_VOLTAGE_INTERNAL	3

void init ();
bool update ();

}

#endif /* LIB_TELEMETRY_ADC_BATTERY_ADC_BATTERY_H_ */
