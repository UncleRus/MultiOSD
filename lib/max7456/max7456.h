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
#ifndef LIB_MAX7456_MAX7456_H_
#define LIB_MAX7456_MAX7456_H_

#include <stdint.h>
#include <stdio.h>

#define MAX7456_MODE_PAL 0
#define MAX7456_MODE_NTSC 1

#define MAX7456_ATTR_INVERT 1
#define MAX7456_ATTR_BLINK 	2
#define MAX7456_ATTR_LBC 	4

namespace max7456
{

void init ();
void clear ();
void upload_char (uint8_t char_index, uint8_t data []);
void put (uint8_t col, uint8_t row, uint8_t chr, uint8_t attr = 0);
void puts (uint8_t col, uint8_t row, const char *s, uint8_t attr = 0);
void puts_p (uint8_t col, uint8_t row, const char *progmem_str, uint8_t attr = 0);

void open (uint8_t col, uint8_t row, uint8_t attr = 0);
void close ();

extern FILE stream;

};

#endif /* LIB_MAX7456_MAX7456_H_ */
