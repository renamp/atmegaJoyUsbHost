;---------------------------------
; UsbLow16mhz.s
; USB low speed core functions
;
; Created: 10/16/2023 11:20:22 PM
;  Author: Renan
;--------------------------------

#include <avr/io.h>

#ifdef __SFR_OFFSET
  #undef __SFR_OFFSET
  #define __SFR_OFFSET 0
#endif

#define USBPORT	PORTB
#define USBPIN	PINB
#define USBDDR	DDRB

.section .data

.section .text
.global Send_RawDiff
.global Receive_Bytes
.global Receive_Bytes_ack
.global Send_Bytes
.global Bytes2Rawdiff


;---------------------------------
; Function to Send bytes to device
; void Send_Bytes(uint8_t size, uint8_t *data);
; [25:24] len,[23:22] ptr_data,
Send_Bytes:
	rcall	Bytes2Rawdiff
	rcall	Send_RawDiff
	RET


;---------------------------------
; Function to Send Raw diff bytes to device
; [25:24] bits_len,[23:22] ptr_data,
Send_RawDiff:
	CLI
	movw	ZL, r22				; Array Pointer		
	mov		r18, r24			; bits length
	; set usb lines to output
	in		r19, USBPORT
	andi	r19, 0xFC
	ori		r19, 0x01
	out		USBPORT, r19
	in		r19, USBDDR
	ori		r19, 0x03
	out		USBDDR, r19
Send_RawDiff_L1:
	ld		r20, -Z				;2			[11] next byte
Send_RawDiff_2:
	out		USBPORT, r20		;1			[0]
	dec		r18					;1			[1]
	lpm		r19, z				;3	dummy	[4]  
	lpm		r19, z				;3	dummy	[7] 
	breq Send_RawDiff_End		;1/2		[8/9]
	lsr		r20					;1			[9]
	lsr		r20					;1			[10]
	
	out		USBPORT, r20		;1			[0]
	dec		r18					;1			[1]
	lpm		r19, z				;3	dummy	[4]
	ld		r19, z				;2	dummy	[7]
	breq	Send_RawDiff_End	;1/2		[8/9]
	lsr		r20					;1			[9]
	lsr		r20					;1			[10]

	out		USBPORT, r20		;1			[0]
	dec		r18					;1			[1]
	lpm		r19, z				;3	dummy	[4]
	ld		r19, z				;2	dummy	[6]
	breq	Send_RawDiff_End	;1/2		[7/8]
	lsr		r20					;1			[8]
	lsr		r20					;1			[9]

	out		USBPORT, r20		;1			[0]
	lpm		r19, z				;3	dummy	[3]  
	lpm		r19, z				;3	dummy	[6]
	dec		r18					;1			[7]
	brne	Send_RawDiff_L1		;2/1		[9/8]
	nop							;1			[9]
Send_RawDiff_End:
	clr		r20					;1			[10]

	out		USBPORT, r20		;1			[0]
	clr		r22					;1			[1]
	clr		r23					;1			[2]
	clr		r24					;1			[3] 
	clr		r25					;1			[4]
	lpm		r19, z				;3	dummy	[7]
	lpm		r19, z				;3	dummy	[10]
	
	clr		r18					;1			[0]
	lpm		r19, z				;3	dummy	[3]
	lpm		r19, z				;3	dummy	[6]
	lpm		r19, z				;3	dummy	[9]	
	ldi		r19, 0x01			;1			[10]
	out		USBPORT, r19		;1			[0]
	clr		r19

	SEI
	RET


;---------------------------------
; Function to Receive bytes from device
; prototype:
;	extern uint8_t Receive_Bytes_ack(uint8_t *ptr_data);
; (ptr_array[24:25])
Receive_Bytes_ack:
	ldi		r22, 0x01;	; with ack
	rjmp	Receive_Bytes_INI1
;---------------------------------
; Function to Receive bytes from device
; prototype:
;	extern uint8_t Receive_Bytes(uint8_t *ptr_data);
; (ptr_array[24:25])
Receive_Bytes:
	clr		r22			;
Receive_Bytes_INI1:
	CLI					; disable interrupt
	push	r22			; 0 dont call ACK
	push	r25			; array pointer
	push	r24			; 
	; set usb lines input
	in		r18, USBDDR
	andi	r18, 0xFC
	out		USBDDR, r18
	in		r18, USBPORT
	andi	r18, 0xFC
	out		USBPORT, r18
	;
	in		ZL, SPL		; stack index to Z
	in		ZH, SPH		; stack index to Z
	ld		r18, Z+		; dummy to correct Z index
	clr		r18			; bit counter
	ldi		r19, 0x02	; bit adder
	ldi		r20, 0x02	; Idle state (d- d+)
	ldi		r22, 0xff	; temp 
	ldi		r23, 0x00	; temp
Rcv_Bytes_LS0: ; sync byte
	sbis	USBPIN, 1				;1/2		[0] ---------
	rjmp	Rcv_Bytes_LS0			;2
	sbic	USBPIN, 0				;1/2		[2]
	rjmp	Rcv_Bytes_LS0			;2
	lpm		r24, z					;3	dummy	[6]  
	ld		r24, z					;2	dummy	[8]  
	sbis	USBPIN, 0					;1/2		[9] ---------
	rjmp	Rcv_Bytes_LS1			;2
Rcv_Bytes_LS1:				
	lpm		r24, z					;3 dummy	[12/14]
	out		PORTD, r23				;1 dummy(0)	[13/15]

	in		r21, USBPIN				;1			[0] ---------
	andi	r21, 0x03				;1			[1]
	eor		r20, r21				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[6]	
	lpm		r24, z					;3	dummy	[9]
	out		PORTD, r22				;1 dummy(1)	[10]

	in		r20, USBPIN				;1			[0] ---------
	andi	r20, 0x03				;1			[1]
	eor		r21, r20				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[7]	
	lpm		r24, z					;3	dummy	[10]
	out		PORTD, r23				;1 dummy(0)	[4]

	in		r21, USBPIN				;1			[0] ---------
	andi	r21, 0x03				;1			[1]
	eor		r20, r21				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[7]	
	ld		r24, z					;2	dummy	[9]
	out		PORTD, r22				;1 dummy(1)	[4]

	in		r20, USBPIN				;1			[0] ---------
	andi	r20, 0x03				;1			[1]
	eor		r21, r20				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[7]	
	lpm		r24, z					;3	dummy	[10]
	out		PORTD, r23				;1 dummy(0)	[4]

	in		r21, USBPIN				;1			[0] ---------
	andi	r21, 0x03				;1			[1]
	eor		r20, r21				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[7]	
	lpm		r24, z					;3	dummy	[10]
	out		PORTD, r22				;1 dummy(1)	[4]

	in		r20, USBPIN				;1			[0] ---------
	andi	r20, 0x03				;1			[1]
	eor		r21, r20				;1			[2]
	breq	Rcv_Bytes_SyncErro		;1/2		[3]
	lpm		r24, z					;3	dummy	[7]	
	ld		r24, z					;2	dummy	[9]
	out		PORTD, r23				;1 dummy(0)	[4]

	in		r21, USBPIN				;1			[0] ---------
	andi	r21, 0x03				;1			[1]
	eor		r20, r21				;1			[2]
	brne	Rcv_Bytes_SyncErro		;1/2		[3/4]
	out		PORTD, r22				;1 dummy(1)	[4]
	out		PORTD, r23				;1 dummy(0)	[5]
	lpm		r24, z					;3	dummy	[8]	
	rjmp	Rcv_Bytes_L1			;2			[10]
Rcv_Bytes_SyncErro: ; sync failed
	ldi		r22, 0x10
	out		SPDR, r22
	SEI
	RET
Rcv_Bytes_L1:
	out		PORTD, r22			;1 dummy(1)	[0]
	in		r20, USBPIN			;1			[1]
	andi	r20, 0x03			;1			[2]
	breq	Rcv_Bytes_EOP		;1/2		[3/4]
	swap	r20					;1			[4]
	mov		r21, r20			;1			[5]
	inc		r18					;1			[6]
	lpm		r24, z				;3	dummy	[9]
	
	out		PORTD, r23			;1 dummy(0)	[0]
	in		r20, USBPIN			;1			[1]
	andi	r20, 0x03			;1			[2]
	breq	Rcv_Bytes_EOP		;1/2		[3/4]
	lsl		r20					;1			[4]
	lsl		r20					;1			[5]
	or		r21, r20			;1			[6]
	inc		r18					;1			[7]
	lpm		r24, z				;3	dummy	[10]
	
	out		PORTD, r22			;1 dummy(1)	[0]
	in		r20, USBPIN			;1			[1]
	andi	r20, 0x03			;1			[2]
	breq	Rcv_Bytes_EOP		;1/2		[3/4]
	or		r21, r20			;1			[4]
	st		-Z, r21				;2			[6]
	inc		r18					;1			[7]
	nop							;1	dummy	[8]
	rjmp	Rcv_Bytes_L1		;2			[10]
Rcv_Bytes_EOP:					;			[4]		// 00 lines
	st		-Z, r21				;2			[6]
	lpm		r24, z				;3	dummy	[9]
	nop							;1			[10]

	nop							;1			[0]
	in		r20, USBPIN			;1			[1]
	andi	r20, 0x03			;1			[2]
	brne	Rcv_Bytes_EOP_Err	;1/2		[3/4]
	lpm		r24, z				;3	dummy	[6]
	lpm		r24, z				;3	dummy	[9]
	nop							;1			[10]

	nop							;1			[0]
	in		r20, USBPIN			;1			[1]		// should be 10 lines
	andi	r20, 0x03			;1			[2]
	ldi		r21, 0x01			;1			[3]
	eor		r20, r21			;1			[4]
	brne	Rcv_Bytes_EOP_Err	;1/2		[5/6]
	
	mov		r24, r18		; raw diff bits length
	clr		r25
	pop		r22				; ptr_array result
	pop		r23
	pop		r18				; 
	in		r20, SPL
	in		r21, SPH
	st		-Z, r21			; store previous stack
	st		-Z, r20	
	sbiw	ZL, 1			
	out		SPH, ZH
	out		SPL, ZL
	movw	XL, r20
	sbiw	XL, 2			; number of pop-1
	movw	r20, XL			; ptr_diff raw
	push	r23
	push	r22
	andi	r18, 0x01		; Return ACK
	brne	Rcv_Bytes_DoAck
Rcv_Bytes_NoAck:
	rcall	Raw_diff2bytes
	pop		r22			; restore after call
	pop		r23			; restore after call
	pop		ZL			; restore after call
	pop		ZH			; restore after call
	out		SPH, ZH
	out		SPL, ZL	

	sbi		PORTD, 3
	cbi		PORTD, 3
	SEI
	RET
Rcv_Bytes_EOP_Err:
	clr		r25;				;1			[x]
	clr		r24;				;1			[x]
	SEI
	RET
Rcv_Bytes_DoAck:
	push	r25			; store before call
	push	r24			; store before call
	push	r23			; store before call
	push	r22			; store before call
	push	r21			; store before call
	in		r22, SPL	; before last push
	in		r23, SPH	; for ptr_diff
	push	r20			; store before call
	ldi		r18, 0x66	; raw diff sync
	push	r18
	ldi		r18, 0xA6	; raw diff sync
	push	r18
	ldi		r18, 0x65	; raw diff ack
	push	r18
	ldi		r18, 0xA9	; raw diff ack
	push	r18
	ldi		r24, 16		; length diff bits
	rcall	Send_RawDiff
	pop		r18			; return stack
	pop		r18			; return stack
	pop		r18			; return stack
	pop		r18			; return stack
	pop		r20			; restore after call
	pop		r21			; restore after call
	pop		r22			; restore after call
	pop		r23			; restore after call
	pop		r24			; restore after call
	pop		r25			; restore after call
	rjmp Rcv_Bytes_NoAck


;--------------------------------------------------
; function to convert raw differential
; bits to data byte
; ([25:24] lenbits,[23:22] ptr_data, [21:20] ptr_diff(up))
Raw_diff2bytes:
	push	r17				;
	push	r16
	ldi		r18, 0x06		; count for bitstuff
	clr		r19				; count data bytes
	movw	XL, r22			; ptr_data
	movw	ZL, r20			; ptr_diff
	clr		r20				; temp data
	ldi		r23, 0x20		; previous diff (bits 5,4)
	;		r24,			; len diff bits
	ldi		r25, 0x08		; count data bits
	ldi		r17, 0x03		; diff couple/byte
	clt						; clear T
Raw_diff2BytesI0:
	ld		r21, -Z
Raw_diff2BytesI1:
	mov		r22, r21
	andi	r22, 0x30		;
	eor		r23, r22		; if same result 0
	mov		r23, r22
	brts	Raw_diff2BytesS1; skip bit stuff
	brne	Raw_diff2BytesL1; branch if change
	lsr		r20
	dec		r18
	brne	Raw_diff2BytesL2; branch if not bitstaff
	set						; set T (skip next bit)
	ldi		r18, 0x06
Raw_diff2BytesL2:		
	sbr		r20, 0x80		;
	rjmp	Raw_diff2BytesL3
Raw_diff2BytesS1:
	clt						; clear T
	rjmp Raw_diff2BytesL4
Raw_diff2BytesL1:
	lsr		r20
	ldi		r18, 0x06
Raw_diff2BytesL3:			; check number of bits
	dec		r25				; dec count data bits
	brne	Raw_diff2BytesL4; branch not zero
	ldi		r25, 0x08
	st		X+, r20			; store data
	inc		r19
	clr		r20
Raw_diff2BytesL4:			; check diff len
	dec		r24				; len diff bits
	breq	Raw_diff2BytesEND
	lsl		r21
	lsl		r21
	dec		r17				; counter diff couples
	brne	Raw_diff2BytesI1
	ldi		r17, 0x03
	rjmp	Raw_diff2BytesI0
Raw_diff2BytesEND:
	pop		r16
	pop		r17
	clr		r25
	mov		r24, r19		; len data bytes
	RET


;---------------------------------------------------
; function to convert data bytes to raw differential
; ([25:24] len,[23:22] ptr_data, [21:20] ptr_debugOut)
Bytes2Rawdiff:
	movw	ZL, r22			; ptr_data
	in		r20, SPL
	in		r21, SPH
	movw	XL, r20			; ptr_debugOut
	ldi		r19, 0x66		; load sync(Low nibble)
	st		-X, r19
	ldi		r19, 0xA6		; load sync(High nibble)
	st		-X, r19	
	mov		r18, r24		; in data length
	ldi		r19, 5			; count 1s(start with 1)
	clr		r20				; current diff byte
	ldi		r21, 0x03		; diff bits mask
	ldi		r22, 0xAA		; previous diff state
	ldi		r24, 0x08		; out len diff bits
	clr		r25				; temp
	clt						; clear T (bitstaff)
Bytes2Rawdiff_L0:
	ld		r23, Z+			; load byte from data in
	ldi		r25, 0x08		; count bits in byte
	mov		r1, r25
Bytes2Rawdiff_L1:
	lsr		r23				; rotate to carry
	brcs	Bytes2Rawdiff_L3; branch if bit is 1
Bytes2Rawdiff_L2:
	ser		r25				; set 0xff
	eor		r22, r25		; change diff
	ldi		r19, 0x07		; reset bitstuff
Bytes2Rawdiff_L3:
	inc		r24				; inc len diff bits
	mov		r25, r22
	and		r25, r21		; mask bits
	or		r20, r25		; store diff
	lsl		r21				; shift mask
	lsl		r21				; shift mask
	brne	Bytes2Rawdiff_L4; jump if dont need store
	st		-X, r20			; store diff
	clr		r20				; reset current diff
	ldi		r21, 0x03		; restore mask
Bytes2Rawdiff_L4:
	dec		r19
	breq	Bytes2Rawdiff_L2; add bitstuff (0)
	dec		r1				; bits of byte
	brne	Bytes2Rawdiff_L1
	dec		r18				; bytes in array in
	brne	Bytes2Rawdiff_L0
	clr		r25
	;		r24				; len diff bits
	in		r22, SPL		; ptr array diff
	in		r23, SPH
	clr		r1				; must clear before return
	RET

.end