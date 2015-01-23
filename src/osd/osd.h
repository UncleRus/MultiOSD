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
#ifndef OSD_OSD_H_
#define OSD_OSD_H_

#include "panel.h"
#include "screen.h"

#define OSD_SWITCH_OFF         0x00
#define OSD_SWITCH_FLIGHT_MODE 0x01
#define OSD_SWITCH_RAW_CHANNEL 0x80

namespace osd
{

void main ();
void init ();

namespace settings
{

	void reset ();

}  // namespace settings

}


#endif /* OSD_OSD_H_ */
