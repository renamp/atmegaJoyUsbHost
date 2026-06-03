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
	USBPacket packet;
	uint8_t remaningCount;
	
	USB_SendTokenPacket(3, pid);
	usbdatalen = USB_ReceiveBytesAck(packet.data) - 3;
	if(pos_len > 5){
		remaningCount = pos_len - usbdatalen;
		memcpy(usbdata, packet.data + 1, usbdatalen);
	}
	else{
		remaningCount = packet.data[pos_len+1] - usbdatalen;
		usbdatalen--;
		memcpy(usbdata, packet.data + 2, usbdatalen);
	}
	while (remaningCount > 0){
		USB_SendTokenPacket(3, pid);
		size = USB_ReceiveBytesAck(packet.data) - 3;
		memcpy(usbdata + usbdatalen, packet.data + 1, size);
		remaningCount -= size;
		usbdatalen += size;
	}
	return usbdatalen;
}


int USB_SendData(uint8_t size, uint8_t *data)
{
    USB_SendBytes(size, data);
    size = USB_ReceiveBytes(data);
    if(size != 1 && data[0] != USB_PID_ACK)
        return 0;
    return 1;
}


int USB_send(uint8_t *pid, uint8_t size, uint8_t *data)
{
	USB_SendTokenPacket(3, pid);
	return USB_SendData(size, data);
}


int USB_packetSetup(uint8_t *pid, uint8_t size, uint8_t *data)
{
    USB_SendTokenPacket(3, pid);
    return USB_SendData(size, data);
}


void JoyUSB_init()
{
	USBPacket packet;
	uint8_t pidsetup[]	= {USB_PID_SETUP,0x00,0x10};
	uint8_t pidin[]	= {USB_PID_IN, 0x00, 0x10};
	uint8_t pidsend[]	= {USB_PID_OUT,0x00, 0x10};
	
	usbconnected = 0;
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x40,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	pidsetup[1] = 0x05; pidsetup[2] = 0xD0;	// change address
	pidin[1]	= 0x05; pidin[2]	= 0xD0;
	pidsend[1]	= 0x05; pidsend[2]	= 0xD0;
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(2, pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x03,0x00,0x00,0xFF,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
		
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x02,0x03,0x09,0x04,0xFF,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;	
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x29,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x09,0x01,0x00,0x00,0x00,0x00,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	packet = (USBPacket) {{USB_PID_DATA0,0x21,0x0A,0x00,0x00,0x00,0x00,0x00,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0,pidin, usbdata);
	
	packet = (USBPacket) {{USB_PID_DATA0,0x81,0x06,0x00,0x22,0x00,0x00,0xA5,0x00}};
	if( USB_send(pidsetup, 9, packet.data) == 0)
		return;
	usbdatalen = USB_receive_data(0x65, pidin, usbdata);
		packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_send(pidsend, 3, packet.data) == 0)
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