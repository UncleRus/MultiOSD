#include "telemetry.h"
#include "adc_battery/adc_battery.h"
#include "../../config.h"

#ifdef TELEMETRY_MODULES_ADC_BATTERY
#	include "adc_battery/adc_battery.h"
#endif

#ifdef TELEMETRY_MODULES_UAVTALK
#	include "uavtalk/uavtalk.h"
#endif

namespace telemetry
{

namespace status
{
	uint8_t connection = CONNECTION_STATE_DISCONNECTED;
	uint16_t flight_time = 0;
	uint8_t flight_mode = FLIGHT_MODE_MANUAL;
	bool armed = false;
	uint8_t rssi = 100;
}

namespace attitude
{
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
}

namespace input
{
	float throttle = 0.0;
	float roll = 0.0;
	float pitch = 0.0;
	float yaw = 0.0;
	float collective = 0.0;
	float thrust = 0.0;
	int16_t channels [INPUT_CHANNELS];
}

namespace gps
{
	float latitude = 0.0;
	float longitude = 0.0;
	float altitude = 0.0;
	float speed = 0.0;
	float heading = 0.0;
	int8_t sattelites = 0;
	uint8_t state = GPS_STATE_NO_FIX;
}

namespace velocity
{
	float climb = 0.0;
}

namespace battery
{
	float voltage = 0.0;
	float current = 0.0;
	uint16_t consumed = 0;
}


void init ()
{
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	adc_battery::init ();
#endif
}

bool update ()
{
	bool res = false;
#ifdef TELEMETRY_MODULES_UAVTALK
	res |= uavtalk::update ();
#endif
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	res |= adc_battery::update ();
#endif
	return res;
}

}
