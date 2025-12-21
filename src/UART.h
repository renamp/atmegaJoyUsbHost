/*
 * UART.h
 *
 * Created: 25/07/2013 22:15:55
 *  Author: Renan
 */ 

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#ifdef UCSRA
	#define UCSR0A	UCSRA
	#define UCSR0B	UCSRB
	#define UCSR0C	UCSRC
	#define TXEN0	TXEN
	#define RXEN0	RXEN
	#define UBRR0H	UBRRH
	#define UBRR0L	UBRRL
	#define UDR0	UDR
	
	#define UCSZ00	UCSZ0
	#define UCSZ01	UCSZ1
	#define RXC0	RXC
	#define TXC0	TXC
	#define UDRE0	UDRE
#endif	// #ifdef UCSRA

void UART_init( unsigned long BAUD);
void UART_Write( unsigned char Data );
void UART_print( char * text);
char UART_DataIsReady( );
unsigned char UART_Read( );
unsigned char UART_ReadWait( );

#endif /* UART_H_ */