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
#include "../base.h"

namespace osd
{

typedef void (* panel_draw_t) (uint8_t x, uint8_t y);

extern const panel_draw_t panels [] PROGMEM;
extern const char * const panel_names [] PROGMEM;
void draw_panel (uint8_t panel, uint8_t x, uint8_t y);

namespace draw
{

	extern bool flash;

	void rect (uint8_t l, uint8_t t, uint8_t w, uint8_t h, bool inv = false);

}  // namespace draw

}  // namespace osd

#endif /* OSD_PANEL_H_ */
