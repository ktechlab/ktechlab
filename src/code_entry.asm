; Variables
ok	equ	0xc
__op1	equ	0xd
__l	equ	0xe
__k	equ	0xf
__j	equ	0x10
__m	equ	0x11
__i	equ	0x12

list p=16F84
include "p16f84.inc"

; Config options
  __config _WDT_OFF

START

;#MSRC	3; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 0
;#MSRC	4; /tmp/kde-david/ktechlabpTzd1b.microbe	keypad keypad1 RB0 RB1 RB2 RB3 RB4 RB5 RB6
;#MSRC	5; /tmp/kde-david/ktechlabpTzd1b.microbe	TRISA = 28
	movlw	28
	bsf		STATUS,5
	movwf	TRISA
;#MSRC	6; /tmp/kde-david/ktechlabpTzd1b.microbe	TRISB = 240
	movlw	240
	movwf	TRISB
	bcf		STATUS,5
;#MSRC	7; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTA = 0
	clrf	PORTA
;#MSRC	8; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTB = 0
	clrf	PORTB
;#MSRC	9; /tmp/kde-david/ktechlabpTzd1b.microbe	__label_setpin__65:
;#MSRC	10; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTA.0 = low

__label_setpin__65
	bcf		PORTA,0
;#MSRC	11; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTA.1 = low
	bcf		PORTA,1
;#MSRC	12; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 1
	movlw	1
	movwf	ok
;#MSRC	13; /tmp/kde-david/ktechlabpTzd1b.microbe	if keypad1 != 1 then
;{
	call	__wait_read_keypad_keypad1
	sublw	1
	btfsc	STATUS,2
	goto	__1_case_false
;#MSRC	15; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 0
	clrf	ok
;}
;#MSRC	17; /tmp/kde-david/ktechlabpTzd1b.microbe	if keypad1 != 2 then
;{

__1_case_false
	call	__wait_read_keypad_keypad1
	sublw	2
	btfsc	STATUS,2
	goto	__3_case_false
;#MSRC	19; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 0
	clrf	ok
;}
;#MSRC	21; /tmp/kde-david/ktechlabpTzd1b.microbe	if keypad1 != 3 then
;{

__3_case_false
	call	__wait_read_keypad_keypad1
	sublw	3
	btfsc	STATUS,2
	goto	__5_case_false
;#MSRC	23; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 0
	clrf	ok
;}
;#MSRC	25; /tmp/kde-david/ktechlabpTzd1b.microbe	if keypad1 == 4 then
;{

__5_case_false
	call	__wait_read_keypad_keypad1
	sublw	4
	btfss	STATUS,2
	goto	__12_case_false
;#MSRC	27; /tmp/kde-david/ktechlabpTzd1b.microbe	if ok == 1 then
;{
	movf	ok,0
	sublw	1
	btfss	STATUS,2
	goto	__10_case_false
;#MSRC	29; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTA.0 = high
	bsf		PORTA,0
;#MSRC	30; /tmp/kde-david/ktechlabpTzd1b.microbe	__label_testpin:
;#MSRC	31; /tmp/kde-david/ktechlabpTzd1b.microbe	if PORTA.2 is high then
;{

__label_testpin
	btfss	PORTA,2
	goto	__label_testpin
;#MSRC	33; /tmp/kde-david/ktechlabpTzd1b.microbe	goto __label_setpin__65
	goto	__label_setpin__65
;#MSRC	37; /tmp/kde-david/ktechlabpTzd1b.microbe	goto __label_testpin
;}
;#MSRC	42; /tmp/kde-david/ktechlabpTzd1b.microbe	__label_setpin__73:
;#MSRC	43; /tmp/kde-david/ktechlabpTzd1b.microbe	PORTA.1 = high

__label_setpin__73
__10_case_false
	bsf		PORTA,1
;#MSRC	44; /tmp/kde-david/ktechlabpTzd1b.microbe	delay 5000
	incf	__l,1
	movlw	26
	movwf	__k
	movlw	119
	movwf	__j
	call	__delay_subroutine
;#MSRC	45; /tmp/kde-david/ktechlabpTzd1b.microbe	goto __label_setpin__65
	goto	__label_setpin__65
;}
;#MSRC	50; /tmp/kde-david/ktechlabpTzd1b.microbe	ok = 0

__12_case_false
	clrf	ok
;#MSRC	51; /tmp/kde-david/ktechlabpTzd1b.microbe	goto __label_setpin__73
	goto	__label_setpin__73
;}
;#MSRC	53; /tmp/kde-david/ktechlabpTzd1b.microbe	end

__wait_read_keypad_keypad1
	call	__read_keypad_keypad1
	movwf	__m
	sublw	255
	btfsc	STATUS,Z
	goto	__wait_read_keypad_keypad1

__wait_keypad_keypad1
	incf	__l,1
	incf	__k,1
	movlw	13
	movwf	__j
	movlw	251
	movwf	__i
	call	__delay_subroutine
	call	__read_keypad_keypad1
	sublw	255
	btfss	STATUS,Z
	goto	__wait_keypad_keypad1
	movf	__m,0
	return

__read_keypad_keypad1
	bcf		PORTB,0
	bcf		PORTB,1
	bcf		PORTB,2
	bcf		PORTB,3
	bsf		PORTB,0
	btfsc	PORTB,4
	retlw	1
	btfsc	PORTB,5
	retlw	2
	btfsc	PORTB,6
	retlw	3
	bcf		PORTB,0
	bsf		PORTB,1
	btfsc	PORTB,4
	retlw	4
	btfsc	PORTB,5
	retlw	5
	btfsc	PORTB,6
	retlw	6
	bcf		PORTB,1
	bsf		PORTB,2
	btfsc	PORTB,4
	retlw	7
	btfsc	PORTB,5
	retlw	8
	btfsc	PORTB,6
	retlw	9
	bcf		PORTB,2
	bsf		PORTB,3
	btfsc	PORTB,4
	retlw	253
	btfsc	PORTB,5
	retlw	0
	btfsc	PORTB,6
	retlw	254
	bcf		PORTB,3
	retlw	255

__delay_subroutine
	decfsz	__i,1
	goto	__delay_subroutine
	decfsz	__j,1
	goto	__delay_subroutine
	decfsz	__k,1
	goto	__delay_subroutine
	return

	END

