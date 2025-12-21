/*
 * UsbHost.c
 *
 * Created: 10/21/2023 10:32:36 PM
 *  Author: Renan
 */ 

#include "UsbHost.h"


void USB_reset()
{
	USB_OUT;						// output (idle)
	USBPORT &= ~((1<<0)|(1<<1));	//reset
	_delay_ms(10);
	USBPORT |= (1<<0);				//idle
}


void USB_keep_alive()
{
	USBPORT&=0xFC; USBDDR|=0x03;	//reset
	_delay_us(1);
	USBDDR&=0xFC; USBPORT&=0xFC;	//idle
}


void USB_Update()
{
	if(--usb_update_counter == 0)
	{
		usb_update_counter = USB_UPDATE_TIME_TRIGGER;
		joyUSB_read();
	}
	else{
		USB_keep_alive();
	}
}


uint8_t USB_receive_data(uint8_t pos_len, uint8_t *pid, uint8_t *outdata)
{
	uint8_t size = 0;
	USBData buffdata;
	uint8_t remaningCount;
	
	Send_Bytes(3, pid);
	usbdatalen = Receive_Bytes_ack(buffdata.data) - 3;
	if(pos_len > 5){
		remaningCount = pos_len - usbdatalen;
		memcpy(usbdata, buffdata.data + 1, usbdatalen);
	}
	else{
		remaningCount = buffdata.data[pos_len+1] - usbdatalen;
		usbdatalen--;
		memcpy(usbdata, buffdata.data + 2, usbdatalen);
	}
	while (remaningCount > 0){
		Send_Bytes(3, pid);
		size = Receive_Bytes_ack(buffdata.data) - 3;
		memcpy(usbdata + usbdatalen, buffdata.data + 1, size);
		remaningCount -= size;
		usbdatalen += size;
	}
	return usbdatalen;
}


int USB_send(uint8_t *pidsetup, uint8_t size, uint8_t *data)
{
	Send_Bytes(3, pidsetup);
	Send_Bytes(size, data);
	size = Receive_Bytes(data);
	if(size != 1 && data[0] != USB_PID_ACK)
		return 0;
	return 1;
}


void JoyUSB_init()
{
	USBData buffdata;
	uint8_t pidsetup[]	= {USB_PID_SETUP,0x00,0x10};
	uint8_t pidin[]	= {USB_PID_IN, 0x00, 0x10};
	uint8_t pidsend[]	= {USB_PID_OUT,0x00, 0x10};
	
	usbconnected = 0;
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x40,0x00,0xDD,0x94}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0xEA,0xA1}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	pidsetup[1] = 0x05; pidsetup[2] = 0xD0;	// change address
	pidin[1]	= 0x05; pidin[2]	= 0xD0;
	pidsend[1]	= 0x05; pidsend[2]	= 0xD0;
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,0xE0,0xF4}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00,0xE9,0xA4}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(2, pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x03,0x00,0x00,0xFF,0x00,0xD4,0x64}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
		
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x02,0x03,0x09,0x04,0xFF,0x00,0x97,0xDB}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;	
	
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00,0xAE,0x04}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	buffdata = (USBData) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x29,0x00,0xB7,0xC4}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	buffdata = (USBData) {{USB_PID_DATA0,0x00,0x09,0x01,0x00,0x00,0x00,0x00,0x00,0x27,0x25}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	buffdata = (USBData) {{USB_PID_DATA0,0x21,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0xD6,0x20}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	buffdata = (USBData) {{USB_PID_DATA0,0x81,0x06,0x00,0x22,0x00,0x00,0xA5,0x00,0x93,0x0F}};
	if( USB_send(pidsetup, 11, buffdata.data) == 0)
		return;
	usbdatalen = USB_receive_data(0x65, pidin, usbdata);
		buffdata = (USBData) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, buffdata.data) == 0)
		return;
	
	// change address
	pidin[1]	= 0x85; pidin[2]	= 0x60;
	USB_receive_data(0x08,pidin, usbdata);
	usbconnected = 1;
}


void joyUSB_read()
{
	uint8_t pidin[]	= {USB_PID_IN, 0x85, 0x60};
	usbdatalen = USB_receive_data(0x08,pidin, usbdata);
}