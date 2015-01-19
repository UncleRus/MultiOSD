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
#ifndef MTWI_H_
#define MTWI_H_

#include <stdint.h>

#define MTWI_ERR_TIMEOUT 1
#define MTWI_ERR_BUS_FAIL 2
#define MTWI_ERR_BUSY 3
#define MTWI_ERR_BUFFER_OVERFLOW 4
#define MTWI_ERR_NOT_ACKED 5
#define MTWI_ERR_INVALID_COMMAND 6

namespace mtwi
{

namespace utils
{
	uint8_t hex2nibble (char hex);
}

extern volatile uint8_t error;
const uint8_t *exec (const char *cmd);
const uint8_t *exec_p (const char *progmem_cmd);
bool exists (uint8_t addr);

}

#endif /* MTWI_H_ */
