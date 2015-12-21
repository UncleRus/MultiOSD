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
#ifndef CONFIG_H_
#define CONFIG_H_

#define _VER(h,l) (((h) << 8) | l)

#define VERSION _VER (0, 12) // current version is 0.12

// eclipse build conf
#ifdef ECLIPSE_BUILD
#	include "defaults.h"
#endif

/*
 * Telemetry modules config
 */
#if !defined (TELEMETRY_MODULES_UAVTALK) \
    	&& !defined (TELEMETRY_MODULES_MAVLINK) \
    	&& !defined (TELEMETRY_MODULES_UBX)
	#define TELEMETRY_MODULES_MAVLINK                // ArduPilot/ArduCopter telemetry module by default
#endif

#if defined (TELEMETRY_MODULES_ADC_BATTERY) || defined (TELEMETRY_MODULES_ADC_BATTERY)
	#define ADC_MODULE
#endif

/*
 * Telemetry config
 */
#define TELEMETRY_UART uart0
#define TELEMETRY_DEFAULT_BATTERY_MAX_CELL_VOLTAGE 4.2    // Maximal cell voltage, 4.2 for LiPo
#define TELEMETRY_DEFAULT_BATTERY_NOM_CELL_VOLTAGE 3.7    // Nominal cell voltage, 3.7 for LiPo
#define TELEMETRY_DEFAULT_BATTERY_MIN_CELL_VOLTAGE 3.2    // Minimal cell voltage (dead battery), 3.2 for LiPo
#define TELEMETRY_DEFAULT_BATTERY_LOW_CELL_VOLTAGE 3.5    // Warning threshold
#define TELEMETRY_DEFAULT_CALLSIGN "DRONE"                // Default callsign

/*
 * UAVTalk config
 */
#define UAVTALK_DEFAULT_BAUDRATE UART_BR_57600
#define UAVTALK_DEFAULT_RELEASE UAVTALK_RELEASE_LP150900
#define UAVTALK_CONNECTION_TIMEOUT 6000                   // ms
#define UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL 500     // ms
#define UAVTALK_DEFAULT_INTERNAL_HOME_CALC 1              // Home distance/direction calculation: 0 - flight controller (REVO only), 1 - MultiOSD by GPS
#define UAVTALK_DEFAULT_RSSI_THRESHOLD 10                 // RSSI low threshold (TauLabs only), %

/*
 * MAVLink config
 */
#define MAVLINK_DEFAULT_BAUDRATE UART_BR_57600
#define MAVLINK_SYSID 'R'                                 // MAVLink system ID
#define MAVLINK_COMPID 1                                  // MAVLink component ID
#define MAVLINK_CONNECTION_TIMEOUT 2000                   // ms
#define MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL 1             // Internal battery level calculation (may be better)
#define MAVLINK_DEFAULT_RSSI_LOW_THRESHOLD 10             // RSSI low threshold, %
#define MAVLINK_DEFAULT_EMULATE_RSSI 0                    // Emulate RSSI by input channels
#define MAVLINK_DEFAULT_EMULATE_RSSI_CHANNEL 2            // Input channel number for RSSI emulation 0 - roll, 1 - pitch, 2 - throttle, ...
#define MAVLINK_DEFAULT_EMULATE_RSSI_THRESHOLD 920        // Input channel threshod for RSSI emulation

/*
 * UBX config
 */
#define UBX_DEFAULT_BAUDRATE UART_BR_19200                // values higher than 57600 is not recommended
#define UBX_DEFAULT_TIMEOUT 1000                          // ms
#define UBX_DEFAULT_AUTOCONF 0                            // automatically configure GPS-module after connect

/*
 * OSD config
 */
#define OSD_DEFAULT_SCREENS 8                             // each screen will consume (OSD_SCREEN_PANELS * 3) bytes in EEPROM
#define OSD_DEFAULT_SCREEN_PANELS 24                      // (OSD_SCREEN_PANELS * 3) bytes in SRAM
#define OSD_DEFAULT_CHANNEL_MIN 1000                      // us
#define OSD_DEFAULT_CHANNEL_MAX 2000                      // us
#define OSD_EEPROM_SWITCH_DEFAULT OSD_SWITCH_RAW_CHANNEL  // Screens switch: OSD_SWITCH_OFF (0) - Single screen,
                                                          //                 OSD_SWITCH_FLIGHT_MODE (1) - By "Flight mode" switch
                                                          //                 OSD_SWITCH_RAW_CHANNEL (2) - By input channel
#define OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT 6           // Input channel to switch screens

/*
 * UART config
 */
#define UART_STDIO                                        // we need fprintf
#define UART_RX_BUFFER_SIZE 128                           // I like big buffers
#define UART_TX_BUFFER_SIZE 64

/*
 * Console config
 */
#define CONSOLE_UART uart0
#define CONSOLE_MAX_CMD_LENGTH 32
#define CONSOLE_EOL "\r\n"
#define CONSOLE_PROMPT "osd# "
#define CONSOLE_BAUDRATE UART_BR_57600

/*
 * Boot config
 */
#define BOOT_CONFIG_CODE "config"
#define BOOT_CONFIG_WAIT_TIME 8000

/*
 * ADC config
 */
#define ADC_DEFAULT_REF	ADC_REF_INTERNAL                  // ADC reference voltage source: ADC_REF_AREF (0) / ADC_REF_AVCC (1) / ADC_REF_INTERNAL (2)
#define ADC_DEFAULT_REF_VOLTAGE 1.1                       // ADC reference voltage value

/*
 * ADC battery
 */
#define ADC_BATTERY_DEFAULT_UPDATE_INTERVAL 200           // ms
#define ADC_BATTERY_DEFAULT_VOLTAGE_CHANNEL 2             // ADC2 25 pin
#define ADC_BATTERY_DEFAULT_CURRENT_CHANNEL 1             // ADC1 24 pin
#define ADC_BATTERY_DEFAULT_CURRENT_SENSOR 1              // ADC current sensor enabled
#define ADC_BATTERY_DEFAULT_VOLTAGE_MULTIPLIER 1.0
#define ADC_BATTERY_DEFAULT_CURRENT_MULTIPLIER 1.0

/*
 * ADC RSSI
 */
#define ADC_RSSI_DEFAULT_CHANNEL 3                        // ADC3 26 pin
#define ADC_RSSI_DEFAULT_UPDATE_INTERVAL 200              // ms
#define ADC_RSSI_DEFAULT_LOW_THRESHOLD 10                 // RSSI low threshold, %
#define ADC_RSSI_DEFAULT_MULTIPLIER 1.0


/*
 * SPI config
 */
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_SS_BIT PB2
#define SPI_MOSI_BIT PB3
#define SPI_MISO_BIT PB4
#define SPI_SCK_BIT PB5
#define SPI_CPOL 0                                        // Clock polarity: low when idle
#define SPI_CPHA 0                                        // Clock phase: leading edge
#define SPI_CLOCK 0                                       // Fosc / 4

/*
 * MAX7456 config
 */
#define MAX7456_DEFAULT_MODE MAX7456_MODE_PAL             // default video mode, if jumper closed
#define MAX7456_DEFAULT_BRIGHTNESS 0x00                   // 120% white, 0% black

// ~CS (Chip Select) pin
#define MAX7456_SELECT_PORT PORTD
#define MAX7456_SELECT_DDR DDRD
#define MAX7456_SELECT_BIT PD6

// VSYNC port
#define MAX7456_VSYNC_PORT PORTD
#define MAX7456_VSYNC_PIN PIND
#define MAX7456_VSYNC_DDR DDRD
#define MAX7456_VSYNC_BIT PD2

// Video mode pin. High = default, low = autodetect
#define MAX7456_MODE_JUMPER_PORT PORTD
#define MAX7456_MODE_JUMPER_DDR DDRD
#define MAX7456_MODE_JUMPER_BIT PD3


#endif /* CONFIG_H_ */
