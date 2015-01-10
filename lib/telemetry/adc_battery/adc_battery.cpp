#include "adc_battery.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include "../../adc/adc.h"
#include "../../../config.h"
#include "../../eeprom.h"
#include "../../timer/timer.h"
#include "../../telemetry/telemetry.h"

namespace adc_battery
{

#define _ADC_VALUE(v, divider) (v / (1024.0 / ADC_BATTERY_REF_VOLTAGE) * (float) divider)

static volatile uint8_t _last_update_time = 0;
static bool _current_sensor = false;
static float _voltage_divider = 0.0;
static float _current_divider = 0.0;

void init ()
{
	_current_sensor = (bool) eeprom_read_byte (EEPROM_ADC_BATTERY_CURRENT_SENSOR);
	_voltage_divider = eeprom_read_float (EEPROM_ADC_BATTERY_VOLTAGE_DIVIDER);
	if (_current_sensor)
		_current_divider = eeprom_read_float (EEPROM_ADC_BATTERY_VOLTAGE_DIVIDER);
}

bool update ()
{
	uint32_t ticks = timer::ticks ();
	uint16_t interval = (uint16_t) (ticks - _last_update_time);
	if (interval < ADC_BATTERY_UPDATE_INTERVAL)
		return false;
	_last_update_time = ticks;

	telemetry::battery::voltage = _ADC_VALUE (adc::read (ADC_BATTERY_VOLTAGE_PIN), ADC_BATTERY_VOLTAGE_DIVIDER);
	if (_current_sensor)
	{
		telemetry::battery::current = _ADC_VALUE (adc::read (ADC_BATTERY_CURRENT_PIN), ADC_BATTERY_CURRENT_DIVIDER);
		telemetry::battery::consumed += telemetry::battery::current * interval / 3600.0;
	}
	return true;
}

}
