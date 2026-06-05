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

#define USB_UPDATE_TIME_TRIGGER 10

typedef struct
{
	uint8_t data[12];
} USBPacket;

typedef struct
{
    uint8_t pid;
    uint8_t addr     : 7;
    uint8_t endpoint : 4;
    uint8_t crc5     : 5;
} USBToken;

extern uint8_t usbconnected;
extern uint8_t usbdata[];
extern volatile uint8_t usbdatalen;

extern void USB_SendTokenPacket(uint8_t size, USBToken *data);
extern void USB_SendBytes(uint8_t size, uint8_t *data);
extern uint8_t USB_ReceiveBytes(uint8_t *data);
extern uint8_t USB_ReceiveBytesAck(uint8_t *data);
extern uint8_t USB_AppendCRC5(USBToken *token);

void USB_reset();
void USB_keep_alive();
void USB_Update();

void JoyUSB_init();
void joyUSB_read();

uint8_t USB_receive_data(uint8_t pos_len, uint8_t *ptrOut);
int USB_PacketOut(uint8_t size, uint8_t *data);
int USB_PacketSetup(uint8_t size, uint8_t *data);
void USB_setTokenAddr(uint8_t addr);
void USB_setTokenEndpoint(uint8_t endpoint);

#endif /* USBHOST_H_ */
