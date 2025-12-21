/*
 * atmegaJoyUsbHost.c
 *
 * Created: 12/20/2025 11:29:42 PM
 *  Author: Renan
 */ 


#include <avr/io.h>
#include "UART.h"


void setup(void)
{
	// UART initialization
	UART_init(115200);
	
	UART_print("OK\n");
}


int main(void)
{
	setup();
	
    while(1)
    {
        UART_print(".");
    }
}