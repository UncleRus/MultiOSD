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
#ifndef UAVTALK_OP150202_MAGSTATE_H
#define UAVTALK_OP150202_MAGSTATE_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace op150202
{

#define UAVTALK_OP150202_MAGSTATE_OBJID 0x9FFEAA0C

enum MagStateSource {
	MAGSTATE_SOURCE_INVALID = 0,
	MAGSTATE_SOURCE_ONBOARD = 1,
	MAGSTATE_SOURCE_AUX     = 2
};

struct MagState
{
	float x;
	float y;
	float z;
	MagStateSource Source;
};

}  // namespace op150202

UT_NAMESPACE_CLOSE

#endif // UAVTALK_OP150202_MAGSTATE_H
