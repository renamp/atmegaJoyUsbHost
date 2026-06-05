/*
 * UsbHost.c
 *
 * Created: 10/21/2023 10:32:36 PM
 *  Author: Renan
 */ 

#include "UsbHost.h"

volatile uint8_t usb_update_counter;
uint8_t usbconnected;
uint8_t usbdata[16];
volatile uint8_t usbdatalen;
USBToken tokenAddrEndpoint;


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


void USB_setTokenAddr(uint8_t addr){
    tokenAddrEndpoint.addr = addr;
    USB_AppendCRC5(&tokenAddrEndpoint);
}


void USB_setTokenEndpoint(uint8_t endpoint){
    tokenAddrEndpoint.endpoint = endpoint;
    USB_AppendCRC5(&tokenAddrEndpoint);
}


uint8_t USB_receive_data(uint8_t pos_len, uint8_t *ptrOut)
{
	uint8_t size = 0;
    uint8_t ptrOutlen;
	USBPacket packet;
	uint8_t remaningCount = 0;

	USB_SendTokenPacket(USB_PID_IN, &tokenAddrEndpoint);
	ptrOutlen = USB_ReceiveBytesAck(packet.data) - 3;
	if(pos_len > 5){
		remaningCount = pos_len - ptrOutlen;
		memcpy(ptrOut, packet.data + 1, ptrOutlen);
	}
	else if(ptrOutlen > 0) {
		remaningCount = packet.data[pos_len+1] - ptrOutlen;
		memcpy(ptrOut, packet.data + 1, ptrOutlen);
	}
	while (remaningCount > 0){
		USB_SendTokenPacket(USB_PID_IN, &tokenAddrEndpoint);
		size = USB_ReceiveBytesAck(packet.data) - 3;
		memcpy(ptrOut + ptrOutlen, packet.data + 1, size);
		remaningCount -= size;
		ptrOutlen += size;
	}
	return ptrOutlen;
}


int USB_SendData(uint8_t size, uint8_t *data)
{
    USB_SendBytes(size, data);
    size = USB_ReceiveBytes(data);
    if(size != 1 && data[0] != USB_PID_ACK)
        return 0;
    return 1;
}


int USB_PacketOut(uint8_t size, uint8_t *data)
{
	USB_SendTokenPacket(USB_PID_OUT, &tokenAddrEndpoint);
	return USB_SendData(size, data);
}


int USB_PacketSetup(uint8_t size, uint8_t *data)
{
    USB_SendTokenPacket(USB_PID_SETUP, &tokenAddrEndpoint);
    return USB_SendData(size, data);
}


void JoyUSB_init()
{
	USBPacket packet;
    uint8_t databuf[128];
    uint8_t databuflen;
	
	usbconnected = 0;
    USB_setTokenAddr(0x00);
    USB_setTokenEndpoint(0x00);
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x40,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);

	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;

	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	
    USB_setTokenAddr(0x05);
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(2, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x03,0x00,0x00,0xFF,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
		
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x02,0x03,0x09,0x04,0xFF,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;	
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x80,0x06,0x00,0x02,0x00,0x00,0x29,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(2, databuf);
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
	
	packet = (USBPacket) {{USB_PID_DATA0,0x00,0x09,0x01,0x00,0x00,0x00,0x00,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	
	packet = (USBPacket) {{USB_PID_DATA0,0x21,0x0A,0x00,0x00,0x00,0x00,0x00,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0, databuf);
	
	packet = (USBPacket) {{USB_PID_DATA0,0x81,0x06,0x00,0x22,0x00,0x00,0xA5,0x00}};
	if( USB_PacketSetup(9, packet.data) == 0)
		return;
	databuflen = USB_receive_data(0x65, databuf);
		packet = (USBPacket) {{USB_PID_DATA0,0x00,0x00}};
	if( USB_PacketOut(1, packet.data) == 0)
		return;
	
    USB_setTokenEndpoint(0x1);
	joyUSB_read();
	usbconnected = 1;
}


void joyUSB_read()
{
	usbdatalen = USB_receive_data(0x08, usbdata);
}
