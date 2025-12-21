/*
 * UART.c
 *
 * Created: 25/07/2013 22:17:30
 *  Author: Renan
 */ 

#include "UART.h"

void UART_init( unsigned long BAUD ){
	unsigned long ubrr;
	
	UCSR0A = 0x02;
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);
	UCSR0C = (3<<UCSZ00);
	
	ubrr = ((F_CPU/8)/BAUD)-1;
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) (ubrr);
}

void UART_Write( unsigned char Data ){
	while( !(UCSR0A & (1<<UDRE0)) );
	UDR0 = Data;
}

unsigned char UART_Read( ){
	if( UCSR0A & (1<<RXC0)){
		return UDR0;
	}
	return 0;
}

unsigned char UART_ReadWait( ){
	while((UCSR0A & (1<<RXC0)) == 0);
	return UDR0;
}

char UART_DataIsReady( ){
	if( UCSR0A & (1<<RXC0)) return 1;
	return 0;
}

void UART_print( char * text)
{
	for(; *text != '\0'; text++)
		UART_Write(*text);
}