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
#ifndef TELEMETRY_MAVLINK_SETUP_H_
#define TELEMETRY_MAVLINK_SETUP_H_

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#define MAVLINK_COMM_NUM_BUFFERS 1

#define comm_send_ch(chan, b) { MAVLINK_UART::send (b); }

#include "lib/mavlink_types.h"

const mavlink_system_t mavlink_system = { MAVLINK_SYSID, MAVLINK_COMPID };

#define MAVLINK_EXTERNAL_RX_BUFFER 1
#define m_mavlink_message
namespace telemetry { namespace modules { namespace mavlink { mavlink_message_t message; } } }
mavlink_message_t *m_mavlink_buffer = &telemetry::modules::mavlink::message;

#include "lib/ardupilotmega/mavlink.h"

#endif /* TELEMETRY_MAVLINK_SETUP_H_ */
