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
#include "max7456.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "../common.h"
#include "../spi/spi.h"
#include "../settings/settings.h"
#include "../../config.h"


namespace max7456
{

//MAX7456 reg read addresses
#define MAX7456_REG_STAT 0x20 //0xa0 Status

//MAX7456 reg write addresses
#define MAX7456_REG_VM0   0x00
#define MAX7456_REG_VM1   0x01
#define MAX7456_REG_DMM   0x04
#define MAX7456_REG_DMAH  0x05
#define MAX7456_REG_DMAL  0x06
#define MAX7456_REG_DMDI  0x07
#define MAX7456_REG_OSDM  0x0c //not used. Is to set mix
#define MAX7456_REG_OSDBL 0x6c //black level

//MAX7456 reg write addresses to recording NVM process
#define MAX7456_REG_CMM   0x08
#define MAX7456_REG_CMAH  0x09
#define MAX7456_REG_CMAL  0x0a
#define MAX7456_REG_CMDI  0x0b

#define MAX7456_MASK_PAL 0x40 //PAL mask 01000000
#define MAX7456_MASK_NTCS 0x00 //NTSC mask 00000000 ("|" will do nothing)

#define MAX7456_PAL_COLUMNS		30
#define MAX7456_PAL_ROWS		16
#define MAX7456_PAL_HCENTER		MAX7456_PAL_COLUMNS / 2
#define MAX7456_PAL_VCENTER		MAX7456_PAL_ROWS / 2

#define MAX7456_NTSC_COLUMNS	30
#define MAX7456_NTSC_ROWS		13
#define MAX7456_NTSC_HCENTER	MAX7456_NTSC_COLUMNS / 2
#define MAX7456_NTSC_VCENTER	MAX7456_NTSC_ROWS / 2

#define _chip_select() { cbi (MAX7456_SELECT_PORT, MAX7456_SELECT_BIT); }
#define _chip_unselect() { sbi (MAX7456_SELECT_PORT, MAX7456_SELECT_BIT); }

// Write VM0[3] = 1 to enable the display of the OSD image.
// mode | autosync | vsync on | enable OSD
#define _enable_osd() { write_register (MAX7456_REG_VM0, _mask | 0x0c); }
#define _disable_osd() { write_register (MAX7456_REG_VM0, 0); }

static uint8_t _mask = 0;
static uint8_t _mode = 0;
static uint8_t _right, _bottom, _hcenter, _vcenter;
static bool _opened = false;

inline uint8_t read_register (uint8_t reg)
{
	spi::transfer (reg | 0x80);
	return spi::transfer (0xff);
}

inline void write_register (uint8_t reg, uint8_t val)
{
	spi::transfer (reg);
	spi::transfer (val);
}

inline uint8_t mode ()
{
	return _mode;
}

inline uint8_t right ()
{
	return _right;
}

inline uint8_t bottom ()
{
	return _bottom;
}

inline uint8_t hcenter ()
{
	return _hcenter;
}

inline uint8_t vcenter ()
{
	return _vcenter;
}

inline void _set_mode (uint8_t mode)
{
	_mode = mode;
	if (mode == MAX7456_MODE_NTSC)
	{
		_mask = MAX7456_MASK_NTCS;
		_right = MAX7456_NTSC_COLUMNS - 1;
		_bottom = MAX7456_NTSC_ROWS - 1;
		_hcenter = MAX7456_NTSC_HCENTER;
		_vcenter = MAX7456_NTSC_VCENTER;
		return;
	}
	_mask = MAX7456_MASK_PAL;
	_right = MAX7456_PAL_COLUMNS - 1;
	_bottom = MAX7456_PAL_ROWS - 1;
	_hcenter = MAX7456_PAL_HCENTER;
	_vcenter = MAX7456_PAL_VCENTER;
}

inline void _detect_mode ()
{
	if (bis (MAX7456_MODE_JUMPER_PORT, MAX7456_MODE_JUMPER_BIT))
	{
		// jumper is closed, ignore settings
		_set_mode (MAX7456_MODE_DEFAULT);
		return;
	}

	// read STAT and auto detect video mode PAL/NTSC
	_chip_select ();
	uint8_t stat = read_register (MAX7456_REG_STAT);
	_chip_unselect ();

	if (bis (stat, 0))
	{
		_set_mode (MAX7456_MODE_PAL);
		return;
	}
	if (bis (stat, 1))
	{
		_set_mode (MAX7456_MODE_NTSC);
		return;
	}

	// no video signal detected, try to read video mode setting
	uint8_t opt = eeprom_read_byte (EEPROM_OSD_VIDEO_MODE);
	if (opt == MAX7456_MODE_PAL || opt == MAX7456_MODE_NTSC)
	{
		_set_mode (opt);
		return;
	}

	// garbage in EEPROM, set default mode
	_set_mode (MAX7456_MODE_DEFAULT);
}

FILE stream;

int _put (char chr, FILE *s);

void init ()
{
	// Prepare pins
	cbi (MAX7456_MODE_JUMPER_DDR, MAX7456_MODE_JUMPER_BIT);
	sbi (MAX7456_SELECT_DDR, MAX7456_SELECT_BIT);
	cbi (MAX7456_VSYNC_DDR, MAX7456_VSYNC_BIT);
	sbi (MAX7456_VSYNC_PORT, MAX7456_VSYNC_BIT); // pull-up

	// Detect video mode
	_detect_mode ();

	_chip_select ();

	// Reset?
	//write_register (MAX7456_REG_VM0, MAX7456_CMD_RESET);
	//_delay_us (100);
	//while (read_register (MAX7456_REG_STAT) & 0x40)
	//	;

	/*
	Write OSDBL[4] = 0 to enable automatic OSD black
	level control. This ensures the correct OSD image
	brightness. This register contains 4 factory-preset
	bits [3:0] that must not be changed.
	*/
	write_register (MAX7456_REG_OSDBL, read_register (MAX7456_REG_OSDBL) & 0xef);

	_enable_osd ();

	// set all rows to same character brightness black/white level
	uint8_t brightness = eeprom_read_byte (EEPROM_OSD_BRIGHTNESS);
	for (uint8_t r = 0; r < 16; ++ r)
		write_register (0x10 + r, brightness);

	_chip_unselect ();

	fdev_setup_stream (&stream, _put, NULL, _FDEV_SETUP_WRITE);
}

void clear ()
{
	_chip_select ();
	write_register (MAX7456_REG_DMM, 0x04);
	_delay_us (30);
	_chip_unselect ();
}

void upload_char (uint8_t char_index, uint8_t data [])
{
	_chip_select ();
	_disable_osd ();

	// Write CMAH[7:0] = xxH to select the character (0–255) to be written
	write_register (MAX7456_REG_CMAH, char_index);

	for (uint8_t i = 0; i < 54; i ++)
	{
		// Write CMAL[7:0] = xxH to select the 4-pixel byte (0–53) in the character to be written
		write_register (MAX7456_REG_CMAL, i);
		// Write CMDI[7:0] = xxH to set the pixel values of the selected part of the character
		write_register (MAX7456_REG_CMDI, data [i]);
	}

	// Write CMM[7:0] = 1010xxxx to write to the NVM array from the shadow RAM
	write_register (MAX7456_REG_CMM, 0xa0);
	/*
	The character memory is busy for approximately 12ms during this operation.
	STAT[5] can be read to verify that the NVM writing process is complete.
	*/
	_delay_ms (12);
	while (read_register (MAX7456_REG_STAT) & 0x20)
		;

	_enable_osd ();
	_chip_unselect ();
}

inline void _set_offset (uint8_t col, uint8_t row)
{
	uint16_t offset = (row * 30 + col) & 0x1ff;
	write_register (MAX7456_REG_DMAH, offset >> 8);
	write_register (MAX7456_REG_DMAL, (uint8_t) offset);
}

void put (uint8_t col, uint8_t row, uint8_t chr, uint8_t attr)
{
	if (_opened) close ();

	_chip_select ();

	write_register (MAX7456_REG_DMM, (attr & 0x07) << 3);
	_set_offset (col, row);
	write_register (MAX7456_REG_DMDI, chr);

	_chip_unselect ();
}

static uint8_t _start_col = 0;
static uint8_t _start_row = 0;
static uint8_t _cur_attr = 0;

void open (uint8_t col, uint8_t row, uint8_t attr)
{
	if (_opened) close ();

	// remember start
	_start_col = col;
	_start_row = row;
	_cur_attr = attr;

	_chip_select ();

	_opened = true;
	// 16 bits operating mode, char attributes, autoincrement
	write_register (MAX7456_REG_DMM, ((attr & 0x07) << 3) | 0x01);
	_set_offset (col, row);
}

void open_center (uint8_t width, uint8_t height, uint8_t attr)
{
	open (_hcenter - width / 2, _vcenter - width / 2);
}

void close ()
{
	if (!_opened) return;

	// terminate autoincrement mode
	write_register (MAX7456_REG_DMDI, 0xff);
	_chip_unselect ();
}

int _put (char chr, FILE *s)
{
	if (chr == MAX7456_EOL_CHAR)
	{
		// end of line (kinda CR/LF)
		close ();
		open (_start_col, _start_row + 1, _cur_attr);
	}
	write_register (MAX7456_REG_DMDI, chr);
	return 0;
}

// 0xff terminates autoincrement mode
#define _valid_char(c) (c == 0xff ? 0x00 : c)

void puts (uint8_t col, uint8_t row, const char *s, uint8_t attr)
{
	open (col, row, attr);
	while (*s)
	{
		write_register (MAX7456_REG_DMDI, _valid_char (*s));
		s ++;
	}
	close ();
}

void puts_p (uint8_t col, uint8_t row, const char *progmem_str, uint8_t attr)
{
	open (col, row, attr);
	register char c;
	while ((c = pgm_read_byte (progmem_str ++)))
		write_register (MAX7456_REG_DMDI, _valid_char (c));
	close ();
}

}
