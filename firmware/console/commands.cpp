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
#include "commands.h"
#include <util/delay.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include "../config.h"
#include "../lib/uart/uart.h"
#include "console.h"
#include "../lib/max7456/max7456.h"
#include "../settings.h"
#include "../osd/panel.h"
#include "../osd/screen.h"
#include "../osd/osd.h"
#include "../telemetry/telemetry.h"

#if defined (ADC_MODULE)
#	include "../lib/adc/adc.h"
#endif

namespace console
{

const char _str_done[] PROGMEM = "Done.\r\n";

uint8_t read()
{
    while (true)
    {
        uint16_t c = CONSOLE_UART::receive();
        if (!(c & 0xff00))
            return c;
    }
}

namespace commands
{

namespace font
{

    const char command[] PROGMEM = "font";
    const char help[] PROGMEM = "Upload and download mcm-file";

    void draw()
    {
        uint8_t left = max7456::hcenter - 8;
        max7456::clear();
        for (uint8_t h = 0; h < 0x10; h++)
            for (uint8_t l = 0; l < 0x10; l++)
                max7456::put(left + l, h, (h << 4) | l);
    }

    void print_byte(uint8_t b)
    {
        for (int8_t i = 7; i >= 0; i--)
            CONSOLE_UART::send(((b >> i) & 1) ? '1' : '0');
        eol();
    }

    void download()
    {
        draw();
        CONSOLE_UART::send_string_p(PSTR("MAX7456\r\n"));
        for (uint16_t c = 0; c < 0x100; c++)
        {
            uint8_t data[54];
            max7456::download_char(c, data);
            for (uint8_t i = 0; i < 54; i++)
                print_byte(data[i]);
            for (uint8_t i = 0; i < 10; i++)
                print_byte(0);
        }
    }

    uint8_t read_byte()
    {
        uint8_t res = 0;
        for (uint8_t i = 0; i < 8; i++)
            res = (res << 1) | ((console::read() - '0') & 1);
        // \r\n
        console::read();
        console::read();
        return res;
    }

    void upload()
    {
        draw();
        CONSOLE_UART::send_string_p(PSTR("Send MCM-file\r\n"));
        for (uint8_t i = 0; i < 9; i++)
            read();
        for (uint16_t c = 0; c < 0x100; c++)
        {
            uint8_t data[54];
            for (uint8_t i = 0; i < 54; i++)
                data[i] = read_byte();
            for (uint8_t i = 0; i < 10; i++)
                read_byte();
            max7456::upload_char(c, data);
        }
        CONSOLE_UART::send_string_p(_str_done);
    }

    void exec()
    {
        const char *arg = console::argument(1);
        if (arg)
        {
            switch (*arg)
            {
                case 'u':
                case 'U':
                    upload();
                    return;
                case 'd':
                case 'D':
                    download();
            }
            return;
        }
        CONSOLE_UART::send_string_p(PSTR("Args: u - upload, d - download"));
    }

} // namespace font

namespace reset
{

    const char command[] PROGMEM = "reset";
    const char help[] PROGMEM = "Reset settings to defaults";

    void exec()
    {
        CONSOLE_UART::send_string_p(PSTR("Reset to defaults... "));
        settings::reset();
        CONSOLE_UART::send_string_p(_str_done);
    }

}  // namespace reset

namespace opt
{

    const char command[] PROGMEM = "opt";
    const char help[] PROGMEM = "Read/write OSD options";

    const char __unknown[] PROGMEM = "ERR: Unknown option";

    const char __uint[] PROGMEM = "%u";
    const char __float[] PROGMEM = "%0.4f";

    const char __t_bool[] PROGMEM = "bool";
    const char __t_byte[] PROGMEM = "byte";
    const char __t_word[] PROGMEM = "word";
    const char __t_dword[] PROGMEM = "dword";
    const char __t_float[] PROGMEM = "float";
    const char __t_str[] PROGMEM = "str";

    const char * const types[] PROGMEM = { __t_bool, __t_byte, __t_word, __t_dword, __t_float, __t_str };

    void display(const settings::option_t *option)
    {
        if (!option)
        {
            CONSOLE_UART::send_string_p(__unknown);
            return;
        }

        const char *name_p = (const char *) pgm_read_ptr(&option->name_p);
        uint8_t type = pgm_read_byte(&option->type);

        fprintf_P(&CONSOLE_UART::stream, PSTR("0x%04x: %S\t%S\t= "), pgm_read_word(&option->addr), (const char *) pgm_read_ptr(&types[type]),
                name_p);

        char buf[16];

        switch (type)
        {
            case settings::ot_bool:
                fprintf_P(&CONSOLE_UART::stream, __uint, settings::read_bool_option(option));
                break;
            case settings::ot_uint8:
                fprintf_P(&CONSOLE_UART::stream, __uint, settings::read_uint8_option(option));
                break;
            case settings::ot_uint16:
                fprintf_P(&CONSOLE_UART::stream, __uint, settings::read_uint16_option(option));
                break;
            case settings::ot_uint32:
                ltoa(settings::read_uint32_option(option), buf, 10);
                CONSOLE_UART::send_string(buf);
                break;
            case settings::ot_float:
                fprintf_P(&CONSOLE_UART::stream, __float, settings::read_float_option(option));
                break;
            case settings::ot_str:
                settings::read_str_option(option, buf);
                CONSOLE_UART::send_string(buf);
                break;
        }
    }

    void list()
    {
        for (uint8_t s = 0; s < settings::sections_count; s++)
        {
            const settings::option_t *opts = settings::sections[s].options;
            for (uint8_t i = 0; i < settings::sections[s].size; i++)
            {
                display(opts + i);
                console::eol();
            }
        }
    }

    void get()
    {
        const char *name = console::argument(2);

        if (!name)
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <name>"));
            return;
        }

        display(settings::get_option(name));
    }

    void set()
    {
        const char *name_arg = console::argument(2);
        const char *value = console::argument(3);

        if (!name_arg || !value)
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <name> <value>"));
            return;
        }

        char name[SETTINGS_MAX_NAME_LEN];
        console::read_argument(name_arg, name);

        const settings::option_t *option = settings::get_option(name);

        if (!option)
        {
            CONSOLE_UART::send_string_p(__unknown);
            return;
        }

        uint8_t type = pgm_read_byte(&option->type);

        switch (type)
        {
            case settings::ot_bool:
                settings::write_bool_option(name, atoi(value));
                break;
            case settings::ot_uint8:
                settings::write_uint8_option(name, atoi(value));
                break;
            case settings::ot_uint16:
                settings::write_uint16_option(name, atol(value));
                break;
            case settings::ot_uint32:
                settings::write_uint32_option(name, atol(value));
                break;
            case settings::ot_float:
                settings::write_float_option(name, atof(value));
                break;
            case settings::ot_str:
                settings::write_str_option(name, value);
                break;
        }

        display(option);
    }

    void exec()
    {
        const char *arg = console::argument(1);
        if (arg)
        {
            switch (*arg)
            {
                case 'l':
                case 'L':
                    list();
                    return;
                case 'g':
                case 'G':
                    get();
                    return;
                case 's':
                case 'S':
                    set();
                    return;
            }
        }
        CONSOLE_UART::send_string_p(PSTR("Args: l - list, g - get, s - set"));
    }

}  // namespace opt

namespace screen
{

    const char command[] PROGMEM = "scr";
    const char help[] PROGMEM = "View/edit screens layout";

    const char screens_opt[] PROGMEM = "SCREENS";
    uint8_t screens_count;

    void __attribute__ ((noinline)) print_err_p(const char *msg)
    {
        CONSOLE_UART::send_string_p(PSTR("ERR: "));
        CONSOLE_UART::send_string_p(msg);
    }

    bool check_screen(uint8_t num)
    {
        if (num >= screens_count)
        {
            print_err_p(PSTR("Bad screen #"));
            return false;
        }
        return true;
    }

    bool check_panel(uint8_t x, uint8_t y, uint8_t panel)
    {
        if (x >= MAX7456_PAL_COLUMNS || y >= MAX7456_PAL_ROWS)
        {
            print_err_p(PSTR("Bad coordinates"));
            return false;
        }

        if (panel >= osd::panel::count)
        {
            print_err_p(PSTR("Bad panel id"));
            return false;
        }

        return true;
    }

    bool check_panel_idx(uint8_t scr, uint8_t i)
    {
        uint8_t *offset = osd::screen::eeprom_offset(scr) + sizeof(osd::screen::panel_pos_t) * i;
        if (i >= OSD_SCREEN_PANELS || eeprom_read_byte(offset) >= osd::panel::count)
        {
            print_err_p(PSTR("Bad panel #"));
            return false;
        }
        return true;
    }

    void print(uint8_t num)
    {
        if (!check_screen(num))
            return;

        fprintf_P(&CONSOLE_UART::stream, PSTR("Screen %u" CONSOLE_EOL), num);
        CONSOLE_UART::send_string_p(PSTR("#\tx\ty\tpanel\tpanel name" CONSOLE_EOL "--\t--\t--\t--\t--" CONSOLE_EOL));

        uint8_t *offset = osd::screen::eeprom_offset(num);

        for (uint8_t i = 0; i < OSD_SCREEN_PANELS; i++, offset += sizeof(osd::screen::panel_pos_t))
        {
            uint8_t panel = eeprom_read_byte(offset);
            if (panel >= osd::panel::count)
                break;
            fprintf_P(&CONSOLE_UART::stream, PSTR("%u\t%u\t%u\t%u\t%S" CONSOLE_EOL), i, eeprom_read_byte(offset + 1),
                    eeprom_read_byte(offset + 2), panel, osd::panel::name_p(panel));
        }
        console::eol();
    }

    void dump()
    {
        const char *idx = console::argument(2);
        if (idx)
        {
            print(atoi(idx));
            return;
        }

        for (uint8_t i = 0; i < screens_count; i++)
            print(i);
    }

    void append()
    {
        const char *idx = console::argument(2);
        const char *sx = console::argument(3);
        const char *sy = console::argument(4);
        const char *sp = console::argument(5);
        if (!(idx && sx && sy && sp))
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <screen> <x> <y> <panel>"));
            return;
        }
        uint8_t scr = atoi(idx);
        if (!check_screen(scr))
            return;
        uint8_t x = atoi(sx);
        uint8_t y = atoi(sy);
        uint8_t p = atoi(sp);
        if (!check_panel(x, y, p))
            return;

        uint8_t *offset = osd::screen::eeprom_offset(scr);
        uint8_t i = 0;
        for (; i < OSD_SCREEN_PANELS; i++, offset += sizeof(osd::screen::panel_pos_t))
        {
            uint8_t panel = eeprom_read_byte(offset);
            if (panel >= osd::panel::count)
            {
                eeprom_update_byte(offset, p);
                eeprom_update_byte(offset + 1, x);
                eeprom_update_byte(offset + 2, y);
                break;
            }
        }
        if (i == OSD_SCREEN_PANELS)
        {
            print_err_p(PSTR("Too much panels"));
            return;
        }
        print(scr);
    }

    void edit()
    {
        const char *idx = console::argument(2);
        const char *si = console::argument(3);
        const char *sx = console::argument(4);
        const char *sy = console::argument(5);
        const char *sp = console::argument(6);
        if (!(idx && si && sx && sy && sp))
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <screen> <#> <x> <y> <panel>"));
            return;
        }
        uint8_t scr = atoi(idx);
        if (!check_screen(scr))
            return;
        uint8_t i = atoi(si);
        if (!check_panel_idx(scr, i))
            return;
        uint8_t x = atoi(sx);
        uint8_t y = atoi(sy);
        uint8_t p = atoi(sp);
        if (!check_panel(x, y, p))
            return;

        uint8_t *offset = osd::screen::eeprom_offset(scr, i);
        eeprom_update_byte(offset, p);
        eeprom_update_byte(offset + 1, x);
        eeprom_update_byte(offset + 2, y);
        print(scr);
    }

    void remove()
    {
        const char *idx = console::argument(2);
        const char *si = console::argument(3);
        if (!(idx && si))
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <screen> <#>"));
            return;
        }
        uint8_t scr = atoi(idx);
        if (!check_screen(scr))
            return;
        uint8_t i = atoi(si);
        if (!check_panel_idx(scr, i))
            return;

        i++;
        uint8_t *offset = osd::screen::eeprom_offset(scr, i);
        if (i == OSD_SCREEN_PANELS || eeprom_read_byte(offset) >= osd::panel::count)
        {
            // last panel
            uint8_t *prev = osd::screen::eeprom_offset(scr, i - 1);
            eeprom_update_byte(prev, 0xff);
            eeprom_update_byte(prev + 1, 0xff);
            eeprom_update_byte(prev + 2, 0xff);
        }
        else
        {
            for (; i < OSD_SCREEN_PANELS; i++, offset += sizeof(osd::screen::panel_pos_t))
            {
                uint8_t *prev = osd::screen::eeprom_offset(scr, i - 1);
                eeprom_update_byte(prev, eeprom_read_byte(offset));
                eeprom_update_byte(prev + 1, eeprom_read_byte(offset + 1));
                eeprom_update_byte(prev + 2, eeprom_read_byte(offset + 2));
            }
            eeprom_update_byte(offset, 0xff);
            eeprom_update_byte(offset + 1, 0xff);
            eeprom_update_byte(offset + 2, 0xff);
        }
        print(scr);
    }

    void clear()
    {
        const char *idx = console::argument(2);
        if (!idx)
        {
            CONSOLE_UART::send_string_p(PSTR("Args: <screen>"));
            return;
        }
        uint8_t scr = atoi(idx);
        if (!check_screen(scr))
            return;

        uint8_t *offset = osd::screen::eeprom_offset(scr);
        for (uint8_t i = 0; i < OSD_SCREEN_PANELS; i++, offset += sizeof(osd::screen::panel_pos_t))
        {
            eeprom_update_byte(offset, 0xff);
            eeprom_update_byte(offset + 1, 0xff);
            eeprom_update_byte(offset + 2, 0xff);
        }
        print(scr);
    }

    void exec()
    {
        screens_count = osd::screens_enabled();

        const char *arg = console::argument(1);
        if (arg)
        {
            switch (*arg)
            {
                case 'd':
                case 'D':
                    dump();
                    return;
                case 'a':
                case 'A':
                    append();
                    return;
                case 'e':
                case 'E':
                    edit();
                    return;
                case 'r':
                case 'R':
                    remove();
                    return;
                case 'c':
                case 'C':
                    clear();
                    return;
            }
        }
        CONSOLE_UART::send_string_p(PSTR("Args: d - dump screens, a - append panel, e - edit panel, r - remove panel, c - clear screen"));
    }

}  // namespace screen

namespace info
{

    const char command[] PROGMEM = "info";
    const char help[] PROGMEM = "Firmware version, modules, panels";

    void exec()
    {
        fprintf_P(&CONSOLE_UART::stream, PSTR("VERSION: %u.%u\r\n"), VERSION >> 8, VERSION);
        CONSOLE_UART::send_string_p(PSTR("MODULES: "));
        for (uint8_t i = 0; i < telemetry::modules::count; i++)
        {
            CONSOLE_UART::send_string_p(telemetry::modules::name_p(i));
            CONSOLE_UART::send(' ');
        }
        console::eol();
        CONSOLE_UART::send_string_p(PSTR("PANELS:\r\n"));
        for (uint8_t i = 0; i < osd::panel::count; i++)
        {
            fprintf_P(&CONSOLE_UART::stream, PSTR("  %u\t"), i);
            CONSOLE_UART::send_string_p(osd::panel::name_p(i));
            console::eol();
        }
    }

}  // namespace info

namespace help
{

    const char command[] PROGMEM = "help";
    const char help[] PROGMEM = "Commands list";

    void exec()
    {
        for (uint8_t i = 0; i < commands::count; i++)
        {
            CONSOLE_UART::send_string_p(get_cmd_p(i));
            CONSOLE_UART::send_string_p(PSTR(" - "));
            CONSOLE_UART::send_string_p(get_help_p(i));
            console::eol();
        }
    }

}  // namespace help

namespace exit
{

    const char command[] PROGMEM = "exit";
    const char help[] PROGMEM = "Exit console";

    void exec()
    {
        console::stop();
    }

}  // namespace exit

namespace reboot
{

    const char command[] PROGMEM = "reboot";
    const char help[] PROGMEM = "Reboot OSD";

    void exec()
    {
        wdt_enable(WDTO_250MS);
        while (true)
            ;
    }

}  // namespace reboot

#if defined (ADC_MODULE)
namespace adc
{

    const char command[] PROGMEM = "adc";
    const char help[] PROGMEM = "Read ADC values";

    void print(uint8_t ch)
    {
        if (ch > 0x0f)
        {
            CONSOLE_UART::send_string_p(PSTR("ERR: Bad ADC channel"));
            return;
        }

        uint16_t raw = ::adc::read(ch);
        float value = raw / 1024.0 * ::adc::settings::ref_voltage;

        fprintf_P(&CONSOLE_UART::stream, PSTR("ADC %u\t%.4f (%u)" CONSOLE_EOL), ch, value, raw);
    }

    void exec()
    {
        ::adc::init();

        const char *sch = console::argument(1);
        if (sch)
        {
            print(atoi(sch));
            return;
        }

        for (uint8_t i = 0; i < 0x10; i++)
            print(i);
    }

}  // namespace adc
#endif

#define declare_cmd(NS) { NS :: command, NS :: help, NS :: exec }

const command_t values[] PROGMEM = {
    declare_cmd (help),
    declare_cmd (info),
    declare_cmd (opt),
    declare_cmd (screen),
    declare_cmd (reset),
    declare_cmd (font),
#if defined (ADC_MODULE)
    declare_cmd (adc),
#endif
    declare_cmd (exit),
    declare_cmd (reboot),
};

const uint8_t count = sizeof(values) / sizeof(command_t);

bool exec(const char *cmd, uint8_t size)
{
    for (uint8_t i = 0; i < count; i++)
        if (!strncasecmp_P(cmd, (const char *)pgm_read_ptr(&values[i].cmd_p), size))
        {
            ((command_t::proc_t)pgm_read_ptr(&values[i].exec))();
            return true;
        }
    return false;
}

}  // namespace commands

void process(const char *cmd)
{
    const char *command = console::argument(0);
    uint8_t size = 0;
    while (command[size] && command[size] != ' ')
        size++;

    if (!commands::exec(command, size))
        CONSOLE_UART::send_string_p(PSTR("Bad command"));
}

}
