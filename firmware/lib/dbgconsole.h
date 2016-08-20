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
#ifndef DBGCONSOLE_H_
#define DBGCONSOLE_H_

#include <stdio.h>
#include <stdint.h>


#if !defined(DEBUG_CONSOLE_TX_DDR)
	#define DEBUG_CONSOLE_TX_DDR DDRD
#endif
#if !defined(DEBUG_CONSOLE_TX_PORT)
	#define DEBUG_CONSOLE_TX_PORT PORTD
#endif
#if !defined(DEBUG_CONSOLE_TX_BIT)
	#define DEBUG_CONSOLE_TX_BIT 5
#endif
#if !defined(DEBUG_CONSOLE_BAUDRATE)
	#define DEBUG_CONSOLE_BAUDRATE 9600
#endif
#if !defined(DEBUG_CONSOLE_PRESCALER)
	#define DEBUG_CONSOLE_PRESCALER 64
#endif
#if !defined(DEBUG_CONSOLE_TX_BUF_SIZE)
	#define DEBUG_CONSOLE_TX_BUF_SIZE 128
#endif

namespace dbgconsole
{

extern FILE stream;

void init ();
void send (uint8_t b);
void send_string (const char *s);
void send_string_p (const char *progmem_s);

}  // namespace dbgconsole

#define dbg_printf_p(...) fprintf_P (&dbgconsole::stream, __VA_ARGS__)

#endif /* DBGCONSOLE_H_ */
