/*
 * device_wrapper.c
 *
 * Created: 6/30/2026 8:58:11 AM
 *  Author: Renan
 */ 


#include "device_wrapper.h"


inline void fUSB_Delay_10ms(){
    _delay_ms(10);
}


inline void fUSB_Delay_1us(){
    _delay_us(1);
}

void fUsb_setMode_Reset(){
    fUsb_setMode_Output();
    USBPORT &= ~((1<<0)|(1<<1));	//reset
}


void fUsb_setMode_Input(){
    USBDDR&=0xFC;
    USBPORT&=0xFC;
}


void fUsb_setMode_Output(){
    USBPORT&=0xFD;
    USBPORT|=0x01;
    USBDDR|=0x03;
}
