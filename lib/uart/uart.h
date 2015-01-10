/*************************************************************************
Title: Interrupt UART library with receive/transmit circular buffers
Author: Peter Fleury <pfleury@gmx.ch> http://jump.to/fleury, adapted by UncleRus
File: $Id: uart.c,v 1.6.2.1 2007/07/01 11:14:38 peter Exp $

DESCRIPTION:
	An interrupt is generated when the UART has finished transmitting or
	receiving a byte. The interrupt handling routines use circular buffers
	for buffering received and transmitted data.

	The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE variables define
	the buffer size in bytes. Note that these variables must be a
	power of 2.

USAGE:
	Refer to the header file uart.h for a description of the routines.

NOTES:
	Based on original library by Peter Fluery, Tim Sharpe, Nicholas Zambetti.
	Based on Atmel Application Note AVR306

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*************************************************************************/

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include "../../config.h"

#ifndef UART_RX_BUFFER_SIZE
#	define UART_RX_BUFFER_SIZE 64
#endif

#ifndef UART_TX_BUFFER_SIZE
#	define UART_TX_BUFFER_SIZE 64
#endif

#define UART_BAUD_SELECT(baud_rate)  (((F_CPU) + 8UL * (baud_rate)) / (16UL * (baud_rate)) - 1UL)
#define UART_BAUD_SELECT_DOUBLE_SPEED(baud_rate) ( ((((F_CPU) + 4UL * (baud_rate)) / (8UL * (baud_rate)) - 1UL)) | 0x8000)

#if ((UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE) >= (RAMEND - 0x60))
#	error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

#define UART_FRAME_ERROR      0x1000              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0800              /* Overrun condition by UART   */
#define UART_PARITY_ERROR     0x0400              /* Parity Error by UART        */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */

#include "platform.h"

namespace uart0
{

void init (uint16_t baud_rate);
uint16_t receive ();
void send (uint8_t data);
void send_int (int data, int radix = 10);
void send_string (const char *s);
void send_string_p (const char *progmem_s);
#define send_string_P(__s) send_string_p (PSTR (__s))

}

namespace uart1
{

#ifdef ATMEGA_USART1
void init (unsigned int baud_rate);
uint16_t receive ();
void send (unsigned char data);
void send_int (int data, int radix = 10);
void send_string (const char *s);
void send_string_p (const char *s);
#define send_string_P(__s) send_string_p (PSTR (__s))
#endif

}
#endif /* UART_H_ */
