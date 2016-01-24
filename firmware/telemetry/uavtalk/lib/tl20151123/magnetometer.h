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
#ifndef UAVTALK_TL20151123_MAGNETOMETER_H
#define UAVTALK_TL20151123_MAGNETOMETER_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123_MAGNETOMETER_OBJID 0x813B55DE

struct Magnetometer
{
	float x;
	float y;
	float z;
};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif // UAVTALK_TL20151123_MAGNETOMETER_H
