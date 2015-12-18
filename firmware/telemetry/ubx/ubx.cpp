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
#include "ubx.h"
#include "ublox.h"
#include "../../config.h"
#include "../../settings.h"
#include "../../eeprom.h"
#include "../../lib/uart/uart.h"
#include "../telemetry.h"
#include "../../lib/max7456/max7456.h"

namespace telemetry
{

namespace modules
{

namespace ubx
{

namespace settings
{

#define EEPROM_ADDR_BAUDRATE  _eeprom_byte (UBX_EEPROM_OFFSET)
#define EEPROM_ADDR_TIMEOUT   _eeprom_word (UBX_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_AUTOCONF  _eeprom_byte (UBX_EEPROM_OFFSET + 3)

const char __opt_ubxbr [] PROGMEM = "UBXBR";
const char __opt_ubxto [] PROGMEM = "UBXTO";
const char __opt_ubxac [] PROGMEM = "UBXAC";

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option (__opt_ubxbr, EEPROM_ADDR_BAUDRATE),
	declare_uint16_option (__opt_ubxto, EEPROM_ADDR_TIMEOUT),
	declare_bool_option (__opt_ubxac, EEPROM_ADDR_AUTOCONF),
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_BAUDRATE, UBX_DEFAULT_BAUDRATE);
	eeprom_update_word (EEPROM_ADDR_TIMEOUT, UBX_DEFAULT_TIMEOUT);
	eeprom_update_byte (EEPROM_ADDR_AUTOCONF, UBX_DEFAULT_AUTOCONF);
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

enum parser_state_t
{
	ps_sync1 = 0,
	ps_sync2,
	ps_class,
	ps_id,
	ps_len1,
	ps_len2,
	ps_payload,
	ps_crc_a,
	ps_crc_b
};

parser_state_t state = ps_sync1;
packet_t buf;
uint16_t payload_count = 0;

void send_message (uint8_t cls, uint8_t id, void *data, uint16_t size)
{
	header_t h (cls, id, size);
	crc_t crc;

	TELEMETRY_UART::send (UBX_SYNC1);
	TELEMETRY_UART::send (UBX_SYNC2);

	for (uint8_t i = 0; i < sizeof (header_t); i ++)
	{
		uint8_t b = ((uint8_t *) &h) [i];
		crc.update (b);
		TELEMETRY_UART::send (b);
	}

	for (uint16_t i = 0; i < h.len; i ++)
	{
		uint8_t b = ((uint8_t *) data) [i];
		crc.update (b);
		TELEMETRY_UART::send (b);
	}

	TELEMETRY_UART::send (crc.a);
	TELEMETRY_UART::send (crc.b);
}

bool parse (uint8_t b)
{
	switch (state)
	{
		case ps_sync1:
			if (b != UBX_SYNC1) return false;
			state = ps_sync2;
			break;
		case ps_sync2:
			state = b == UBX_SYNC2 ? ps_class : ps_sync1;
			break;
		case ps_class:
			buf.header.cls = b;
			buf.crc.reset ();
			buf.crc.update (b);
			state = ps_id;
			break;
		case ps_id:
			buf.header.id = b;
			buf.crc.update (b);
			state = ps_len1;
			break;
		case ps_len1:
			buf.header.len = b;
			buf.crc.update (b);
			state = ps_len2;
			break;
		case ps_len2:
			buf.header.len |= b << 8;
			if (buf.header.len > sizeof (buf.payload))
			{
				// overflow
				state = ps_sync1;
				break;
			}
			payload_count = 0;
			buf.crc.update (b);
			state = ps_payload;
			break;
		case ps_payload:
			if (payload_count < buf.header.len)
			{
				buf.payload.data [++ payload_count] = b;
				buf.crc.update (b);
			}
			if (payload_count == buf.header.len)
				state = ps_crc_a;
			break;
		case ps_crc_a:
			state = buf.crc.a == b ? ps_crc_b : ps_sync1;
			break;
		case ps_crc_b:
			state = ps_sync1;
			return buf.crc.b == b;
	}
	return false;
}

bool receive ()
{
	uint16_t err = 0;
	do
	{
		uint16_t raw = TELEMETRY_UART::receive ();
		err = raw & 0xff00;
		if (!err && parse (raw)) return true;
	}
	while (!err);
	return false;
}

uint16_t timeout;
uint32_t connection_timeout;
bool autoconf;

void init ()
{
	TELEMETRY_UART::init (uart_utils::get_baudrate (eeprom_read_byte (EEPROM_ADDR_BAUDRATE), UBX_DEFAULT_BAUDRATE));
	timeout = eeprom_read_word (EEPROM_ADDR_TIMEOUT);
	autoconf = eeprom_read_byte (EEPROM_ADDR_AUTOCONF);
}

bool update ()
{
	bool updated = false;

	while (receive ())
	{
		connection_timeout = telemetry::ticks + timeout;
		if (!autoconf) telemetry::status::connection = CONNECTION_STATE_CONNECTED;
		// TODO: autoconf
		//if (telemetry::status::connection != CONNECTION_STATE_CONNECTED)
		//	telemetry::status::connection = CONNECTION_STATE_ESTABLISHING;
		switch (_WORD (buf.header.cls, buf.header.id))
		{
			case _WORD (UBX_CLASS_ACK, UBX_ID_ACK_NAK):
			case _WORD (UBX_CLASS_ACK, UBX_ID_ACK_ACK):
				// TODO: autoconf
				break;
			case _WORD (UBX_CLASS_NAV, UBX_ID_NAV_POSLLH):
				if (telemetry::gps::state > GPS_STATE_FIXING)
				{
					telemetry::gps::altitude = buf.payload.nav_posllh.alt / 1000.0;
#if !defined (TELEMETRY_MODULES_I2C_BARO)
					telemetry::stable::altitude = telemetry::gps::altitude;
#endif
					telemetry::gps::latitude = buf.payload.nav_posllh.lat / 1000000.0;
					telemetry::gps::longitude = buf.payload.nav_posllh.lat / 1000000.0;
					telemetry::home::update ();
					updated = true;
				}
				break;
			case _WORD (UBX_CLASS_NAV, UBX_ID_NAV_SOL):
				telemetry::gps::satellites = buf.payload.nav_sol.num_sv;
				if (buf.payload.nav_sol.flags & UBX_STATUS_FLAGS_GPSFIX_OK)
				{
					switch (buf.payload.nav_sol.fix_type)
					{
						case f_2d:
							telemetry::gps::state = GPS_STATE_2D;
							telemetry::status::armed = true;
							break;
						case f_3d:
							telemetry::gps::state = GPS_STATE_3D;
							if (telemetry::home::state == HOME_STATE_NO_FIX)
								telemetry::home::fix ();
							telemetry::status::armed = true;
							break;
						default:
							telemetry::gps::state = GPS_STATE_FIXING;
					}
				}
				else telemetry::gps::state = GPS_STATE_FIXING;
				updated = true;
				break;
			case _WORD (UBX_CLASS_NAV, UBX_ID_NAV_VELNED):
				telemetry::stable::groundspeed = telemetry::gps::speed = buf.payload.nav_velned.speed / 100.0;
				telemetry::gps::climb = -(buf.payload.nav_velned.vel_down / 100.0);
#if !defined (TELEMETRY_MODULES_I2C_BARO)
				telemetry::stable::climb = telemetry::gps::climb;
#endif
				telemetry::gps::heading = buf.payload.nav_velned.heading * 1.0e-5f;
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
				telemetry::stable::heading = telemetry::gps::heading;
#endif
				updated = true;
				break;
			case _WORD (UBX_CLASS_NAV, UBX_ID_NAV_DOP):
				telemetry::gps::hdop = buf.payload.nav_dop.hdop / 100.0;
				telemetry::gps::vdop = buf.payload.nav_dop.vdop / 100.0;
				telemetry::gps::pdop = buf.payload.nav_dop.pdop / 100.0;
				updated = true;
				break;
			case _WORD (UBX_CLASS_NAV, UBX_ID_NAV_STATUS):
				telemetry::status::flight_time = buf.payload.nav_status.uptime / 1000;
				updated = true;
				break;
		}
	}

	if (telemetry::ticks >= connection_timeout)
	{
		// disconnect, but don't reset the home pos
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		telemetry::gps::state = GPS_STATE_NO_FIX;
		updated = true;
	}

	return updated;
}

}  // namespace ubx

}  // namespace modules

}  // namespace telemetry

