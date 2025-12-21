/*
 * atmegaJoyUsbHost.c
 *
 * Created: 12/20/2025 11:29:42 PM
 *  Author: Renan
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "UART.h"
#include "UsbHost.h"


void printdata_asc(uint8_t len, uint8_t *data)
{
	char valuestr[10];
	sprintf(valuestr, "len:%d\n",len);
	UART_print(valuestr);
	for(int i=0; i<len; i++)
	{
		sprintf(valuestr, "0x%X, ",data[i]);
		UART_print(valuestr);
	}
	UART_print("\n");
}


void setup(void)
{
	// UART initialization
	UART_init(115200);
	UART_print("init..\n");
	
	USB_reset();
	JoyUSB_init();
	
	UART_print("OK\n");
}


int main(void)
{
	setup();
	
    while(1)
    {
        if(usbconnected)
		{
			joyUSB_read();
			printdata_asc(usbdatalen, usbdata);
			_delay_ms(300);
		}
    }
}