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
#ifndef OSD_PANEL_H_
#define OSD_PANEL_H_

#include <stdint.h>
#include <avr/pgmspace.h>

#define OSD_PANEL_ALT                 0
#define OSD_PANEL_CLIMB               1
#define OSD_PANEL_FLIGHT_MODE         2
#define OSD_PANEL_ARMING_STATE        3
#define OSD_PANEL_CONNECTION_STATE    4
#define OSD_PANEL_FLIGHT_TIME         5
#define OSD_PANEL_ROLL                6
#define OSD_PANEL_PITCH               7
#define OSD_PANEL_GPS_STATE           8
#define OSD_PANEL_GPS_LAT             9
#define OSD_PANEL_GPS_LON             10
#define OSD_PANEL_HORIZON             11
#define OSD_PANEL_THROTTLE            12
#define OSD_PANEL_GROUND_SPEED        13
#define OSD_PANEL_BATTERY_VOLTAGE     14
#define OSD_PANEL_BATTERY_CURRENT     15
#define OSD_PANEL_BATTERY_CONSUMED    16

#define OSD_PANELS_COUNT (OSD_PANEL_BATTERY_CONSUMED + 1)

namespace osd
{

namespace panel
{
	typedef void (* panel_draw_t) (uint8_t x, uint8_t y);

	extern const panel_draw_t panels [] PROGMEM;
	extern const char * const panel_names [] PROGMEM;

	void draw (uint8_t panel, uint8_t x, uint8_t y);

}  // namespace panels

namespace draw
{

	extern bool flash;

	void rect (uint8_t l, uint8_t t, uint8_t w, uint8_t h, bool filled = false, uint8_t attr = 0);

}  // namespace draw

}  // namespace osd

#endif /* OSD_PANEL_H_ */
