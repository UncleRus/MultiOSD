#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

namespace uart0
{

#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)

#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
#	error RX buffer size is not a power of 2
#endif
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
#	error TX buffer size is not a power of 2
#endif

static volatile uint8_t _tx_buffer [UART_TX_BUFFER_SIZE];
static volatile uint8_t _rx_buffer [UART_RX_BUFFER_SIZE];
static volatile uint8_t _tx_head;
static volatile uint8_t _tx_tail;
static volatile uint8_t _rx_head;
static volatile uint8_t _rx_tail;
static volatile uint8_t last_error;

ISR (UART0_RECEIVE_INTERRUPT)
{
	uint8_t _head;
	uint8_t data;
	uint8_t usr;
	uint8_t error;

	/* read UART status register and UART data register */
	usr = UART0_STATUS;
	data = UART0_DATA;

	/* */
#if defined(AT90_UART)
	error = (usr & (_BV (FE) | _BV (DOR)));
#elif defined(ATMEGA_USART)
	error = (usr & (_BV (FE) | _BV (DOR)));
#elif defined(ATMEGA_USART0)
	error = (usr & (_BV (FE0) | _BV (DOR0)));
#elif defined (ATMEGA_UART)
	error = (usr & (_BV (FE) | _BV (DOR)));
#endif

	/* calculate buffer index */
	_head = (_rx_head + 1) & UART_RX_BUFFER_MASK;

	if (_head == _rx_tail)
		/* error: receive buffer overflow */
		error = UART_BUFFER_OVERFLOW >> 8;
	else
	{
		/* store new index */
		_rx_head = _head;
		/* store received data in buffer */
		_rx_buffer [_head] = data;
	}
	last_error |= error;
}

ISR (UART0_TRANSMIT_INTERRUPT)
{
	uint8_t _tail;

	if (_tx_head == _tx_tail)
	{
		//PORTB = 0xff;
		/* tx buffer empty, disable UDRE interrupt */
		UART0_CONTROL &= ~_BV (UART0_UDRIE);
		return;
	}
	/* calculate and store new buffer index */
	_tail = (_tx_tail + 1) & UART_TX_BUFFER_MASK;
	_tx_tail = _tail;
	/* get one byte from buffer and write it to UART */
	UART0_DATA = _tx_buffer [_tail]; /* start transmission */
}

void init (uint16_t baud_rate)
{
	_tx_head = 0;
	_tx_tail = 0;
	_rx_head = 0;
	_rx_tail = 0;

#if defined(AT90_UART)
	/* set baud rate */
	UBRR = (uint8_t) baud_rate;

	/* enable UART receiver and transmmitter and receive complete interrupt */
	UART0_CONTROL = _BV (RXCIE) | _BV (RXEN) | _BV (TXEN);

#elif defined (ATMEGA_USART)
	/* Set baud rate */
	if (baud_rate & 0x8000)
	{
		UART0_STATUS = _BV (U2X);  //Enable 2x speed
		baud_rate &= ~0x8000;
	}
	UBRRH = (uint8_t) (baud_rate >> 8);
	UBRRL = (uint8_t) baud_rate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL =	_BV (RXCIE) | _BV (RXEN) | _BV (TXEN);

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL
	UCSRC = _BV (URSEL) | (3 << UCSZ0);
#else
	UCSRC = (3 << UCSZ0);
#endif

#elif defined (ATMEGA_USART0)
	/* Set baud rate */
	if (baud_rate & 0x8000)
	{
		UART0_STATUS = _BV (U2X0);  //Enable 2x speed
		baud_rate &= ~0x8000;
	}
	UBRR0H = (uint8_t) (baud_rate >> 8);
	UBRR0L = (uint8_t) baud_rate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = _BV (RXCIE0) | _BV (RXEN0) | _BV (TXEN0);

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL0
	UCSR0C = _BV (URSEL0) | (3 << UCSZ00);
#else
	UCSR0C = (3 << UCSZ00);
#endif

#elif defined (ATMEGA_UART)
	/* set baud rate */
	if (baud_rate & 0x8000)
	{
		UART0_STATUS = _BV (U2X);  //Enable 2x speed
		baud_rate &= ~0x8000;
	}
	UBRRHI = (uint8_t) (baud_rate >> 8);
	UBRR = (uint8_t) baud_rate;

	/* Enable UART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = _BV (RXCIE) | _BV (RXEN) | _BV (TXEN);
#endif
}

uint16_t receive ()
{
	uint8_t _tail;
	uint8_t data;

	//if (!(SREG & _BV (SREG_I))) sei ();

	if (_rx_head == _rx_tail)
		return UART_NO_DATA; /* no data available */

	/* calculate /store buffer index */
	_tail = (_rx_tail + 1) & UART_RX_BUFFER_MASK;
	_rx_tail = _tail;

	/* get data from receive buffer */
	data = _rx_buffer [_tail];

	data = (last_error << 8) + data;
	last_error = 0;
	return data;
}

void send (uint8_t data)
{
	uint8_t _head;

	_head = (_tx_head + 1) & UART_TX_BUFFER_MASK;

	//if (!(SREG & _BV (SREG_I))) sei ();

	while (_head == _tx_tail)
		;/* wait for free space in buffer */

	_tx_buffer [_head] = data;
	_tx_head = _head;

	/* enable UDRE interrupt */
	UART0_CONTROL |= _BV (UART0_UDRIE);
}

void send_string (const char *s)
{
	while (*s)
		send (*s ++);
}

void send_string_p (const char *progmem_s)
{
	register char c;

	while ((c = pgm_read_byte (progmem_s ++)))
		send (c);
}

}

///////////////////////////////////////////////////////////////////////////////

namespace uart1
{

#if defined (ATMEGA_USART1)
static volatile uint8_t _tx_buffer [UART_TX_BUFFER_SIZE];
static volatile uint8_t _rx_buffer [UART_RX_BUFFER_SIZE];
static volatile uint8_t _tx_head;
static volatile uint8_t _tx_tail;
static volatile uint8_t _rx_head;
static volatile uint8_t _rx_tail;
static volatile uint8_t last_error;


ISR (UART1_RECEIVE_INTERRUPT)
{
	uint8_t _head;
	uint8_t data;
	uint8_t usr;
	uint8_t error;

	/* read UART status register and UART data register */
	usr = UART1_STATUS;
	data = UART1_DATA;

	/* */
	error = (usr & (_BV (FE1) | _BV (DOR1)));

	/* calculate buffer index */
	_head = (_rx_head + 1) & UART_RX_BUFFER_MASK;

	if (_head == _rx_tail)
	{
		/* error: receive buffer overflow */
		error = UART_BUFFER_OVERFLOW >> 8;
	}
	else
	{
		/* store new index */
		_rx_head = _head;
		/* store received data in buffer */
		_rx_buffer [_head] = data;
	}
	last_error |= error;
}

ISR (UART1_TRANSMIT_INTERRUPT)
{
	uint8_t _tail;

	if (_tx_head == _tx_tail)
	{
		/* tx buffer empty, disable UDRE interrupt */
		UART1_CONTROL &= ~_BV (UART1_UDRIE);
		return;
	}
	/* calculate and store new buffer index */
	_tail = (_tx_tail + 1) & UART_TX_BUFFER_MASK;
	_tx_tail = _tail;
	/* get one byte from buffer and write it to UART */
	UART1_DATA = _tx_buffer [_tail]; /* start transmission */
}

void init (uint16_t baud_rate)
{
	_tx_head = 0;
	_tx_tail = 0;
	_rx_head = 0;
	_rx_tail = 0;

	/* Set baud rate */
	if (baud_rate & 0x8000)
	{
		UART1_STATUS = _BV (U2X1);  //Enable 2x speed
		baud_rate &= ~0x8000;
	}
	UBRR1H = (uint8_t) (baud_rate >> 8);
	UBRR1L = (uint8_t) baud_rate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART1_CONTROL = _BV (RXCIE1) | _BV (RXEN1) | _BV (TXEN1);

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL1
	UCSR1C = (1<<URSEL1)|(3<<UCSZ10);
#else
	UCSR1C = (3 << UCSZ10);
#endif
}

uint16_t receive ()
{
	uint8_t _tail;
	uint8_t data;

	if (_rx_head == _rx_tail)
		return UART_NO_DATA; /* no data available */

	/* calculate /store buffer index */
	_tail = (_rx_tail + 1) & UART_RX_BUFFER_MASK;
	_rx_tail = _tail;

	/* get data from receive buffer */
	data = _rx_buffer [_tail];

	data = (last_error << 8) + data;
	last_error = 0;
	return data;

}

void send (uint8_t data)
{
	uint8_t _head;

	_head = (_tx_head + 1) & UART_TX_BUFFER_MASK;

	while (_head == _tx_tail)
		;/* wait for free space in buffer */

	_tx_buffer [_head] = data;
	_tx_head = _head;

	/* enable UDRE interrupt */
	UART1_CONTROL |= _BV (UART1_UDRIE);
}

void send_string (const char *s)
{
	while (*s)
		send (*s ++);
}

void send_string_p (const char *progmem_s)
{
	register char c;

	while ((c = pgm_read_byte (progmem_s++)))
		send (c);
}

#endif

}
