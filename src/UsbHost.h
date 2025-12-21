/*
 * UsbHost.h
 *
 * Created: 10/21/2023 10:31:28 PM
 *  Author: Renan
 */ 


#ifndef USBHOST_H_
#define USBHOST_H_

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#if F_CPU == 16000000
	#define USBHOSTCOREFILE "UsbLow16mhz.s"
#endif

#define USBPORT	PORTB
#define USBPIN	PINB
#define USBDDR	DDRB

#define USB_IN	USBDDR&=0xFC; USBPORT&=0xFC
#define USB_OUT USBPORT&=0xFD; USBPORT|=0x01; USBDDR|=0x03

#define USB_PID_SETUP	0x2D
#define USB_PID_DATA0	0xC3
#define USB_PID_DATA1	0x4B
#define USB_PID_IN		0x69
#define USB_PID_OUT		0xE1
#define USB_PID_ACK		0xD2

typedef struct
{
	uint8_t data[24];
} USBData;

uint8_t usbconnected;
uint8_t usbdata[24];
uint8_t usbdatalen;

extern void Send_Bytes(uint8_t size, uint8_t *data);
extern uint8_t Receive_Bytes(uint8_t *data);
extern uint8_t Receive_Bytes_ack(uint8_t *data);

void USB_reset();
void JoyUSB_init();
void joyUSB_read();

uint8_t USB_receive_data(uint8_t pos_len, uint8_t *pid, uint8_t *outdata);
int USB_send(uint8_t *pidsetup, uint8_t size, uint8_t *data);

#endif /* USBHOST_H_ */