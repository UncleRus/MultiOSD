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
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "../lib/pgmspace.h"

namespace console
{

namespace commands
{

	struct command_t
	{
		typedef void (* proc_t) ();

		const char *cmd_p;
		const char *help_p;
		proc_t exec;
	};

	// console commands collection
	extern const command_t values [] PROGMEM;
	extern const uint8_t count;

	// execute command
	bool exec (const char *cmd, uint8_t size);

	inline const char *get_cmd_p (uint8_t n)
	{
		return (const char *) pgm_read_ptr (&values [n].cmd_p);
	}

	inline const char *get_help_p (uint8_t n)
	{
		return (const char *) pgm_read_ptr (&values [n].help_p);
	}

}  // namespace commands

void init ();
void process (const char *cmd);

}

#endif /* COMMANDS_H_ */
