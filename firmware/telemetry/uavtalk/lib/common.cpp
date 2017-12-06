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
#include "../lib/common.h"

#include "op150202/release.h"
#include "op150502/release.h"
#include "lp150900/release.h"
#include "tl20151123/release.h"
#include "lp1609rc1/release.h"
#include "dr201702131/release.h"
#include "dr20170717/release.h"

UT_NAMESPACE_OPEN

bool internal_home_calc;
#if !defined(TELEMETRY_MODULES_I2C_BARO)
bool baro_enabled = false;
#endif
#if !defined(TELEMETRY_MODULES_I2C_COMPASS)
bool mag_enabled = false;
bool use_attituide_heading = true;
#endif
uint8_t release_idx = 0;
release_t release;
#if !defined(TELEMETRY_MODULES_ADC_RSSI)
bool emulate_rssi = false;
#endif
uint8_t header_len;
uint8_t rssi_low_threshold = 0;
uint32_t telemetry_request_timeout = 0;
uint32_t connection_timeout = 0;

message_t buffer;

const uint8_t crc_table[256] PROGMEM =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

uint8_t __attribute__((noinline)) get_crc(uint8_t b)
{
    return pgm_read_byte(&crc_table[b]);
}

void send(const header_t &head, uint8_t *data, uint8_t size)
{
    uint8_t crc = 0;
    uint8_t *offset = (uint8_t *)&head;
    for (uint8_t i = 0; i < header_len; i ++, offset ++)
    {
        crc = get_crc(crc ^ *offset);
        TELEMETRY_UART::send(*offset);
    }
    for (uint8_t i = 0; i < size; i ++)
    {
        uint8_t value = data ? *(data + i) : 0;
        crc = get_crc(crc ^ value);
        TELEMETRY_UART::send(value);
    }
    TELEMETRY_UART::send(crc);
}

void request_object(uint32_t obj_id)
{
    header_t h(_UT_TYPE_OBJ_REQ, header_len, obj_id);
    send(h, NULL, 0);
}

enum parser_state_t
{
    PS_SYNC     = 0,
    PS_MSG_TYPE = 1,
    PS_LENGTH   = 2,
    PS_OBJID    = 3,
    PS_INSTID   = 4,
    PS_DATA     = 5,
    PS_CRC      = 6,
    PS_READY    = 7,
};

parser_state_t parser_state = PS_SYNC;
uint8_t parser_crc = 0;
// current byte in every state
uint8_t parser_step = 0;

#define _update_crc(b)      do { parser_crc = get_crc (parser_crc ^ b); } while (0)
#define _receive_byte(v, b) do { v |= ((uint32_t) b) << (parser_step << 3); parser_step ++; } while (0)

bool parse (uint8_t b)
{
    switch (parser_state)
    {
        case PS_SYNC:
            if (b != UAVTALK_SYNC) return false;
            parser_crc = get_crc(b);
            buffer.head.sync = b;
            buffer.head.length = 0;
            buffer.head.objid = 0;
            buffer.head.instid = 0;
            parser_state = PS_MSG_TYPE;
            break;
        case PS_MSG_TYPE:
            if ((b & 0xf8) != UAVTALK_VERSION)
            {
                parser_state = PS_SYNC;
                return false;
            }
            _update_crc(b);
            buffer.head.msg_type = b;
            parser_step = 0;
            parser_state = PS_LENGTH;
            break;
        case PS_LENGTH:
            _receive_byte(buffer.head.length, b);
            _update_crc(b);
            if (parser_step == 2)
            {
                if (buffer.head.length < header_len || buffer.head.length > 0xff + (uint16_t)header_len)
                {
                    parser_state = PS_SYNC;
                    return false;
                }
                parser_state = PS_OBJID;
                parser_step = 0;
            }
            break;
        case PS_OBJID:
            _receive_byte(buffer.head.objid, b);
            _update_crc(b);
            if (parser_step == 4)
            {
                if (release.instid_required) parser_state = PS_INSTID;
                else parser_state = buffer.head.length == header_len ? PS_CRC : PS_DATA;
                parser_step = 0;
            }
            break;
        case PS_INSTID:
            _receive_byte(buffer.head.instid, b);
            _update_crc(b);
            if (parser_step == 2)
            {
                parser_state = buffer.head.length == header_len ? PS_CRC : PS_DATA;
                parser_step = 0;
            }
            break;
        case PS_DATA:
            _update_crc(b);
            buffer.data[parser_step] = b;
            parser_step ++;
            if (parser_step >= buffer.head.length - header_len) parser_state = PS_CRC;
            break;
        case PS_CRC:
        case PS_READY: // supress warning
            buffer.crc = b;
            parser_state = PS_READY;
            break;
    }

    if (parser_state == PS_READY)
    {
        parser_state = PS_SYNC;
        bool res = buffer.crc == parser_crc;
        if (res && buffer.head.msg_type == _UT_TYPE_OBJ_ACK)
        {
            header_t head;
            head.objid = buffer.head.objid;
            head.msg_type = _UT_TYPE_ACK;
            send(head);
        }
        return res;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////

const release_t releases [] PROGMEM = {
    { op150202::instid_required,    op150202::flightstatus_objid,    op150202::handlers,    op150202::fm::names,    op150202::fm::size },
    { op150502::instid_required,    op150502::flightstatus_objid,    op150502::handlers,    op150502::fm::names,    op150502::fm::size },
    { lp150900::instid_required,    lp150900::flightstatus_objid,    lp150900::handlers,    op150502::fm::names,    op150502::fm::size },
    { tl20151123::instid_required,  tl20151123::flightstatus_objid,  tl20151123::handlers,  tl20151123::fm::names,  tl20151123::fm::size },
    { lp1609rc1::instid_required,   lp1609rc1::flightstatus_objid,   lp1609rc1::handlers,   lp1609rc1::fm::names,   op150502::fm::size },
    { dr201702131::instid_required, dr201702131::flightstatus_objid, dr201702131::handlers, dr201702131::fm::names, dr201702131::fm::size },
    { dr20170717::instid_required,  dr20170717::flightstatus_objid,  dr20170717::handlers,  dr20170717::fm::names,  dr20170717::fm::size },
};

const char *get_fm_name_p(uint8_t fm)
{
    return fm < release.fm_count ? (const char *)pgm_read_ptr(&release.fm_names[fm]) : NULL;
}

bool handle()
{
    const obj_handler_t *handler = release.handlers;
    while (true)
    {
        uint32_t objid = pgm_read_dword(&handler->objid);
        if (objid == buffer.head.objid)
        {
            ((obj_handler_t::callable_t)pgm_read_ptr(&handler->handler))();
            return true;
        }
        if (!objid) break;
        handler ++;
    }
    return false;
}

void set_release()
{
    if (release_idx >= sizeof(releases) / sizeof(release_t))
        release_idx = UAVTALK_DEFAULT_RELEASE;

    memcpy_P(&release, &releases[release_idx], sizeof(release_t));
    header_len = release.instid_required
        ? sizeof(header_t)
        : sizeof(header_t) - sizeof(uint16_t);
}

void update_connection()
{
    buffer.head.objid = 0;
    handle();
}

UT_NAMESPACE_CLOSE
