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
#ifndef CONFIG_H_
#define CONFIG_H_

#define VERSION 4

/*
 * Telemetry modules config
 */
//#define TELEMETRY_MODULES_UAVTALK					// main telemetry module
#define TELEMETRY_MODULES_MAVLINK					// main telemetry module
//#define TELEMETRY_MODULES_ADC_BATTERY				// read battery stats from adc

/*
 * Telemetry config
 */
#define BATTERY_MAX_CELL_VOLTAGE 4.2	// Maximal cell voltage, 4.2 for LiPo
#define BATTERY_NOM_CELL_VOLTAGE 3.7	// Nominal cell voltage, 3.7 for LiPo
#define BATTERY_MIN_CELL_VOLTAGE 3.2	// Minimal cell voltage (dead battery), 3.2 for LiPo
#define BATTERY_LOW_CELL_VOLTAGE 3.5	// Warning threshold

/*
 * UAVTalk config
 */
#define UAVTALK_UART uart0
#define UAVTALK_VERSION 0x20
#ifndef UAVTALK_VERSION_RELEASE
#	define UAVTALK_VERSION_RELEASE 150202
#endif
#define UAVTALK_DEFAULT_BOARD UAVTALK_BOARD_CC3D		// default board
#define UAVTALK_CONNECTION_TIMEOUT 10000				// ms
#define UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL 500	// ms
// Home distance/direction calculation: 0 - flight controller (REVO only), 1 - min_raw_osd by GPS
// Overrided by board setting
#define UAVTALK_DEFAULT_INTERNAL_HOME_CALC 0

/*
 * MAVLink config
 */
#define MAVLINK_UART uart0
#define MAVLINK_CONNECTION_TIMEOUT 5000					// ms
#define MAVLINK_DEFAULT_BOARD MAVLINK_BOARD_APM			// default board
#define MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL 0			// Internal battery level calculation

/*
 * OSD config
 */
#define OSD_MAX_SCREENS 8			// each screen will consume (OSD_SCREEN_PANELS * 3) bytes in EEPROM
#define OSD_SCREEN_PANELS 24		// (OSD_SCREEN_PANELS * 3) bytes in SRAM
#define OSD_CHANNEL_MIN 1000		// us
#define OSD_CHANNEL_MAX 2000		// us
//#define OSD_DRAW_INTERVAL_PAL  40	// 1000ms / 25fps
//#define OSD_DRAW_INTERVAL_NTSC 33	// 1000ms / 29.97fps
// when to switch between screens
//#define OSD_EEPROM_SWITCH_DEFAULT OSD_SWITCH_FLIGHT_MODE // flight mode switch
// or
///*
#define OSD_EEPROM_SWITCH_DEFAULT OSD_SWITCH_RAW_CHANNEL // raw channel value
#define OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT 6 // it's ACCESSORY1 on my Tx
//*/
// or
/*
#define OSD_EEPROM_SWITCH_DEFAULT OSD_SWITCH_OFF // single screen mode
*/
//#define UAVTALK_FAKE_GPS	// debug


/*
 * UART config
 */
#define UART_STDIO					// we need fprintf
#define UART_BAUD_RATE 57600
#define UART_RX_BUFFER_SIZE 128		// I like big buffers
#define UART_TX_BUFFER_SIZE 64

/*
 * Console config
 */
#define CONSOLE_UART uart0
#define CONSOLE_MAX_CMD_LENGTH 32
#define CONSOLE_EOL "\r\n"
#define CONSOLE_PROMPT "osd# "

/*
 * Boot config
 */
#define BOOT_CONFIG_CODE "config"
#define BOOT_CONFIG_WAIT_TIME 5000

/*
 * ADC config
 */
#define ADC_DEFAULT_REF	ADC_REF_INTERNAL	// Internal source as voltage reference
#define ADC_DEFAULT_REF_VOLTAGE 1.1			// 1.1V

/*
 * ADC battery
 */
#define ADC_BATTERY_DEFAULT_UPDATE_INTERVAL 200		// ms

#define ADC_BATTERY_DEFAULT_VOLTAGE_CHANNEL 6		// ADC6 19 pin
#define ADC_BATTERY_DEFAULT_CURRENT_CHANNEL 7		// ADC7 22 pin

#define ADC_BATTERY_DEFAULT_CURRENT_SENSOR 1		// ADC current sensor enabled
#define ADC_BATTERY_DEFAULT_VOLTAGE_MULTIPLIER 10.98
#define ADC_BATTERY_DEFAULT_CURRENT_MULTIPLIER 18.1818

/*
 * SPI config
 */
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_SS_BIT PB2
#define SPI_MOSI_BIT PB3
#define SPI_MISO_BIT PB4
#define SPI_SCK_BIT PB5
#define SPI_CPOL 0		// Clock polarity: low when idle
#define SPI_CPHA 0		// Clock phase: leading edge
#define SPI_CLOCK 0		// Fosc / 4

/*
 * MAX7456 config
 */
#define MAX7456_DEFAULT_MODE MAX7456_MODE_PAL 	// default video mode, if jumper closed
#define MAX7456_DEFAULT_BRIGHTNESS 0x00			// 120% white, 0% black
#define MAX7456_EOL_CHAR '\n' 					// Special next row char

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
