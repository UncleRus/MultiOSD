/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * MultiOSD is free software: you can redistribute it and/or modify
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
#include "uavtalk.h"
#include <avr/pgmspace.h>
#include "../../lib/uart/uart.h"
#include "../../lib/timer/timer.h"
#include "../telemetry.h"
#include "lib/common.h"
#include "../../settings.h"
#include "../../eeprom.h"

namespace telemetry
{

namespace modules
{

namespace uavtalk
{

namespace settings
{

#define EEPROM_ADDR_RELEASE            _eeprom_byte (UAVTALK_EEPROM_OFFSET)
#define EEPROM_ADDR_BAUDRATE           _eeprom_byte (UAVTALK_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_INTERNAL_HOME_CALC _eeprom_byte (UAVTALK_EEPROM_OFFSET + 2)
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
#define EEPROM_ADDR_RSSI_THRESHOLD     _eeprom_byte (UAVTALK_EEPROM_OFFSET + 3)
#endif

const char __opt_utrel [] PROGMEM = "UTREL";
const char __opt_utbr  [] PROGMEM = "UTBR";
const char __opt_utihc [] PROGMEM = "UTIHC";
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
const char __opt_utrlt [] PROGMEM = "UTRLT";
#endif

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option (__opt_utrel, EEPROM_ADDR_RELEASE),
	declare_uint8_option (__opt_utbr,  EEPROM_ADDR_BAUDRATE),
	declare_uint8_option (__opt_utihc, EEPROM_ADDR_INTERNAL_HOME_CALC),
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	declare_uint8_option (__opt_utrlt, EEPROM_ADDR_RSSI_THRESHOLD),
#endif
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_RELEASE, UAVTALK_DEFAULT_RELEASE);
	eeprom_update_byte (EEPROM_ADDR_BAUDRATE, UAVTALK_DEFAULT_BAUDRATE);
	eeprom_update_byte (EEPROM_ADDR_INTERNAL_HOME_CALC, UAVTALK_DEFAULT_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	eeprom_update_byte (EEPROM_ADDR_RSSI_THRESHOLD, UAVTALK_DEFAULT_RSSI_THRESHOLD);
#endif
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

bool update ()
{
	bool updated = false;

	// handle all received messages
	while (uavtalk::receive ())
	{
		uint32_t _ticks = timer::ticks ();
		updated |= handle ();
		if (status::connection == CONNECTION_STATE_DISCONNECTED)
		{
			status::connection = CONNECTION_STATE_ESTABLISHING;
			connection_timeout = _ticks + UAVTALK_CONNECTION_TIMEOUT;
		}
		if (_ticks >= telemetry_request_timeout && status::connection == CONNECTION_STATE_CONNECTED)
		{
			// time to send GCSTelemetryStats to FC
			update_connection ();
			telemetry_request_timeout = _ticks + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL;
		}
	}

	if (ticks >= connection_timeout && status::connection == CONNECTION_STATE_CONNECTED)
	{
		status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{
	release_idx = eeprom_read_byte (EEPROM_ADDR_RELEASE);
	internal_home_calc = eeprom_read_byte (EEPROM_ADDR_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	rssi_low_threshold = eeprom_read_byte (EEPROM_ADDR_RSSI_THRESHOLD);
#endif
	set_release ();
	TELEMETRY_UART::init (uart_utils::get_baudrate (eeprom_read_byte (EEPROM_ADDR_BAUDRATE), UAVTALK_DEFAULT_BAUDRATE));
}

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry
