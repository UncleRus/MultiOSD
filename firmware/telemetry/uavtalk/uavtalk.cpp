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
#define EEPROM_ADDR_EMULATE_RSSI       _eeprom_byte (UAVTALK_EEPROM_OFFSET + 3)

const char __opt_utrel [] PROGMEM = "UTREL";
const char __opt_utbr  [] PROGMEM = "UTBR";
const char __opt_utihc [] PROGMEM = "UTIHC";
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
const char __opt_uter  [] PROGMEM = "UTER";
#endif

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option (__opt_utrel, EEPROM_ADDR_RELEASE),
	declare_uint8_option (__opt_utbr,  EEPROM_ADDR_BAUDRATE),
	declare_uint8_option (__opt_utihc, EEPROM_ADDR_INTERNAL_HOME_CALC),
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
    declare_bool_option  (__opt_uter,  EEPROM_ADDR_EMULATE_RSSI),
#endif
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_RELEASE, UAVTALK_DEFAULT_RELEASE);
	eeprom_update_byte (EEPROM_ADDR_BAUDRATE, UAVTALK_DEFAULT_BITRATE);
	eeprom_update_byte (EEPROM_ADDR_INTERNAL_HOME_CALC, UAVTALK_DEFAULT_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
    eeprom_update_byte (EEPROM_ADDR_EMULATE_RSSI, UAVTALK_DEFAULT_EMULATE_RSSI);
#endif
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

bool update ()
{
	bool updated = false;

	// handle all received messages
	while (receive (parse))
	{
		uint32_t _ticks = timer::ticks ();
		updated |= handle ();
		if (status::connection == status::DISCONNECTED)
		{
			status::connection = status::ESTABLISHING;
			connection_timeout = _ticks + UAVTALK_CONNECTION_TIMEOUT;
		}
		if (_ticks >= telemetry_request_timeout && status::connection == status::CONNECTED)
		{
			// time to send GCSTelemetryStats to FC
			update_connection ();
			telemetry_request_timeout = _ticks + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL;
		}
	}

	if (update_time >= connection_timeout && status::connection == status::CONNECTED)
	{
		status::connection = status::DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{
	release_idx = eeprom_read_byte (EEPROM_ADDR_RELEASE);
	internal_home_calc = eeprom_read_byte (EEPROM_ADDR_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
    emulate_rssi = eeprom_read_byte (EEPROM_ADDR_EMULATE_RSSI);
#endif
	set_release ();
	TELEMETRY_UART::init (uart_utils::get_bitrate (eeprom_read_byte (EEPROM_ADDR_BAUDRATE), UAVTALK_DEFAULT_BITRATE));
}

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry
