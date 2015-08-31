/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef UAVTALK_POSITIONSTATE_H
#define UAVTALK_POSITIONSTATE_H

#define UAVTALK_POSITIONSTATE_OBJID 0x4AFDB658
#define UAVTALK_POSITIONSTATE_NUMBYTES sizeof (PositionStateData)

struct PositionStateDataPacked
{
    float North;
    float East;
    float Down;
};

#endif // UAVTALK_POSITIONSTATE_H
