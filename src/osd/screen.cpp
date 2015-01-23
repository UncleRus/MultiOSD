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

#include "screen.h"
#include "../settings.h"
#include "../config.h"
#include "../lib/max7456/max7456.h"
#include "panel.h"

namespace osd
{

namespace screen
{

struct _panel_pos_t
{
	uint8_t panel;
	uint8_t x, y;
};

static _panel_pos_t _panels [OSD_SCREEN_PANELS];
static uint8_t _count = 0;
static uint8_t _screen = 0;

void load (uint8_t num)
{
	if (num >= OSD_SCREENS) num = OSD_DEFAULT_SCREEN;

	_screen = num;

	uint8_t *offset = (uint8_t *) (OSD_SCREENS_EEPROM_OFFSET + num * sizeof (_panel_pos_t) * OSD_SCREEN_PANELS);

	uint8_t i = 0;
	for (; i < OSD_SCREEN_PANELS; i ++)
	{
		_panels [i].panel = eeprom_read_byte (offset);
		if (_panels [i].panel >= OSD_PANELS_COUNT)
			break;
		_panels [i].x = eeprom_read_byte (offset + 1);
		_panels [i].y = eeprom_read_byte (offset + 2);
		offset += sizeof (_panel_pos_t);
	}
	_count = i;

	max7456::clear ();
}

void draw ()
{
	max7456::wait_vsync ();
	max7456::clear ();
	for (uint8_t i = 0; i < _count; i ++)
		osd::panel::draw (_panels [i].panel, _panels [i].x, _panels [i].y);
}

namespace settings
{

void reset ()
{
	// TODO: default screens config
}

}  // namespace settings

}  // namespace screen

}  // namespace osd

