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
#ifndef OSD_SYMBOLS_H_
#define OSD_SYMBOLS_H_

#define OSD_SYMBOL_SATELLITE "\x10\x11"
#define OSD_SYMBOL_CROSSHAIR "\x13\x14\x15"
#define OSD_SYMBOL_LOGO "\xbd\xbe\xbf"

#define OSD_SYMBOL_GROUNDSPEED "\x80"
#define OSD_SYMBOL_KMH "\x81"
#define OSD_SYMBOL_MAH "\x82"
#define OSD_SYMBOL_LAT "\x83"
#define OSD_SYMBOL_LON "\x84"
#define OSD_SYMBOL_ALT "\x85"
#define OSD_SYMBOL_THROTTLE "\x87"
#define OSD_SYMBOL_AIRSPEED "\x88"
#define OSD_SYMBOL_DEGREES "\x89"
#define OSD_SYMBOL_CELSIUM "\x8a"
#define OSD_SYMBOL_KM "\x8b"
#define OSD_SYMBOL_MS "\x8c"
#define OSD_SYMBOL_M "\x8d"
#define OSD_SYMBOL_V "\x8e"
#define OSD_SYMBOL_A "\x8f"
#define OSD_SYMBOL_PITCH "\xb1"
#define OSD_SYMBOL_ROLL "\xb2"
#define OSD_SYMBOL_TIME "\xb3"

#define OSD_SYMBOL_HORIZON_RULER_LEFT "\xb8"
#define OSD_SYMBOL_HORIZON_RULER_LEFT_CENTER "\xc8"
#define OSD_SYMBOL_HORIZON_RULER_RIGHT "\xb9"
#define OSD_SYMBOL_HORIZON_RULER_RIGHT_CENTER "\xc9"

#define OSD_SYMBOL_BAT1_CURRENT "\xfa"
#define OSD_SYMBOL_BAT1_CONSUMED "\xfb"
#define OSD_SYMBOL_BAT2_CURRENT "\xec"
#define OSD_SYMBOL_BAT2_CONSUMED "\xed"

#define OSD_SYMBOL_TEMPERATURE "\xfd"

#define OSD_SYMBOL_NO_FLIGHT_MODE "\x09\x09\x09\x09"
#define OSD_SYMBOL_NO_HOME_DISTANCE "\x09\x09\x09\x8d"

#define OSD_SYMBOL_RSSI0 "\xe5\xe8\xe8"
#define OSD_SYMBOL_RSSI1 "\xe2\xe8\xe8"
#define OSD_SYMBOL_RSSI2 "\xe2\xe6\xe8"
#define OSD_SYMBOL_RSSI3 "\xe2\xe3\xe8"
#define OSD_SYMBOL_RSSI4 "\xe2\xe3\xe7"
#define OSD_SYMBOL_RSSI5 "\xe2\xe3\xe4"

#define OSD_CHAR_2D 0x01
#define OSD_CHAR_3D 0x02

#define OSD_CHAR_FALL1 0x03
#define OSD_CHAR_FALL2 0x04
#define OSD_CHAR_FALL3 0x05
#define OSD_CHAR_CLIMB1 0x06
#define OSD_CHAR_CLIMB2 0x07
#define OSD_CHAR_CLIMB3 0x08

#define OSD_CHAR_HORIZON_TOP  0x0e
#define OSD_CHAR_HORIZON_LINE 0x16

#define OSD_CHAR_HOME 0x12

#define OSD_CHAR_ARROWS 0x90

#define OSD_CHAR_RSSI_FLAG 0xb4

#define OSD_CHAR_ARMED_INV 0xe0
#define OSD_CHAR_CONNECTION_INV 0xe1

#define OSD_CHAR_BATTERY1 0xee
#define OSD_CHAR_BATTERY2 0xf4

#define OSD_CHAR_RECT_THIN_LT 0xd0
#define OSD_CHAR_RECT_THIN_CT 0xd1
#define OSD_CHAR_RECT_THIN_RT 0xd2
#define OSD_CHAR_RECT_THIN_LC 0xd3
#define OSD_CHAR_RECT_THIN_LB 0xd4
#define OSD_CHAR_RECT_THIN_CB 0xd5
#define OSD_CHAR_RECT_THIN_RB 0xd6
#define OSD_CHAR_RECT_THIN_RC 0xd7

#define OSD_CHAR_RECT_FILL_LT 0xd0
#define OSD_CHAR_RECT_FILL_CT 0xd8
#define OSD_CHAR_RECT_FILL_RT 0xd2
#define OSD_CHAR_RECT_FILL_LC 0xd9
#define OSD_CHAR_RECT_FILL_LB 0xd4
#define OSD_CHAR_RECT_FILL_CB 0xda
#define OSD_CHAR_RECT_FILL_RB 0xd6
#define OSD_CHAR_RECT_FILL_RC 0xdb

#define OSD_CHAR_COMPASS_DEFAULT 0xc6
#define OSD_CHAR_COMPASS_INTERNAL 0xc7
#define OSD_CHAR_COMPASS_GPS 0xb6
#define OSD_CHAR_COMPASS_EXTERNAL 0xb7

#define OSD_CHAR_COMPASS_N 0xc2
#define OSD_CHAR_COMPASS_E 0xc4
#define OSD_CHAR_COMPASS_S 0xc3
#define OSD_CHAR_COMPASS_W 0xc5
#define OSD_CHAR_COMPASS_LINE 0xc0
#define OSD_CHAR_COMPASS_TICK 0xc1

#endif /* OSD_SYMBOLS_H_ */
