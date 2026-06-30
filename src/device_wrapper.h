/*
 * device_wrapper.h
 *
 * Created: 6/30/2026 8:51:14 AM
 *  Author: Renan
 */ 


#ifndef DEVICE_WRAPPER_H_
#define DEVICE_WRAPPER_H_

#include <avr/io.h>
#include <util/delay.h>


#define USBPORT	PORTB
#define USBPIN	PINB
#define USBDDR	DDRB


void fUSB_Delay_1us();
void fUSB_Delay_10ms();
void fUsb_setMode_Reset();
void fUsb_setMode_Input();
void fUsb_setMode_Output();


#endif /* DEVICE_WRAPPER_H_ */
