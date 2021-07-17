;****************************************************************************************************
;This software is provided in an “AS IS” condition,NO WARRANTIES in any form apply to this software.
; picmicrolab.com 4.10.2014
;***************************************************************************************************
;PIC16F877A Based PWM Fan Controller and Monitor.SW1 - Controls Fan speed

LIST P=PIC16F877A 
include <P16f877A.inc> 

__CONFIG _CP_OFF & _WDT_OFF & _BODEN_OFF & _PWRTE_OFF & _HS_OSC  & _LVP_OFF & _DEBUG_OFF & _CPD_OFF

 aH equ 0x40;For multiplication
 aL equ 0x41
 bH equ 0x42
 bL equ 0x43
 r4 equ 0x44
 r3 equ 0x45
 r2 equ 0x46
 r1 equ 0x47
 t1	equ 0x48

ProdH equ 0x30;For Unpacked BCD
ProdL equ 0x31
D0 equ 0x32
D1 equ 0x33
D2 equ 0x34
D3 equ 0x49	
PWMcounter equ 0x38
; helper macro
	mmac MACRO A,bit, u2,u1
	BTFSC	A,bit
	ADDWF	u2,F
	RRF	u2,F
	RRF	u1,F
	ENDM

ResultH		EQU 0X34
ResultL		EQU 0X33


org 0x00 
reset:
goto start
org 0x04 
goto		IntSRV
org		0x10
start: 	bcf STATUS, RP0 
	   	bcf STATUS, RP1 
;-----------------------------------------
	CLRF	T2CON
    BSF     T2CON,0x0
	CLRF	CCP1CON		;CCP module OFF
	CLRF	TMR2		;Clear Timer2
;-----------------------------------------	
movlw 0x89
movwf ADCON0 ;Fosc/32; channel_1; ADC on
;------------------------------------------	

		clrf PORTB
		clrf PORTC
		clrf PORTD
		clrf PORTE
		clrf 0x30
		clrf 0x31
 		clrf 0x32
		clrf 0x33
		clrf 0x34
		clrf 0x35
		clrf 0x38
	
bsf STATUS, RP0 
movlw	0x02
movwf	ADCON1; all A analog; E digital
			; format : 6 Lower bit of ADRESL =0
movlw 0xff
movwf TRISB ;RB0 - PWM Fan Sense Input ;RB1 - SW1 - Input
movwf TRISA
clrf TRISC 
clrf TRISD
clrf TRISE
bcf STATUS, RP0
bsf PORTC,0X00


        clrf            T1CON                                 
        movlw           0xD8
        movwf           TMR1H             
        movlw			0xF0
		movwf			TMR1L
        clrf            INTCON          
        bsf             STATUS,RP0
        clrf            PIE1            
        bcf  			STATUS,RP0
        clrf            PIR1            
        movlw           0x20            
        movwf           T1CON          
        bsf				INTCON,PEIE	
		bsf				INTCON,GIE 

		;bcf				INTCON,GIE 
  
		bsf				STATUS,RP0 
		bsf				PIE1,TMR1IE	
        bcf				STATUS,RP0


		bsf   		T1CON,TMR1ON
		bcf		STATUS, RP0		;Bank 0	
		call	init
		;call	PWM
looop:	

		

		;call	ADCconvert
		btfss PORTB,0x00
		goto  looop

		;movlw 0x30
		;movwf PWMcounter

		incf	0x37;Frequency Counter	

;-----------------------------------------------
btfss	PORTB,0X01; Check if SW1 was pressed
incf	PWMcounter
movlw	0x30
subwf 		PWMcounter,w
btfsc 		STATUS,Z
clrf		PWMcounter
;------------------------------------------------

looop1:	btfsc PORTB,0x00
		goto  looop1

goto looop

;------------------------------------
	
delay:					
		movlw		0x20		
		movwf		0x51
CONT1:	movlw		0xA0		
		movwf		0x52
CONT2:	decfsz		0x52,f
		goto		CONT2
		decfsz		0x51,f
		goto		CONT1
		return	
;-----------------------------------

IntSRV:	
	bcf   		T1CON,TMR1ON
	bcf         PIR1,TMR1IF
	
	movlw       0xD8
	movwf		TMR1H
	movlw		0xF0
	movwf		TMR1L
	incf		0x35


	movlw 		0x7D 
	subwf 		0x35,w
	btfss 		STATUS,Z
	goto next
	
;------------------------------------------------
	clrf 0x35;1Hz Output
	movlw 		0x01
	xorwf		PORTC,f
	btfss		PORTC,0X00

	goto next

	movf		0x37,w
	clrf		0x37
	;movwf		PORTD
	movwf		aL
	clrf		aH
	call ShowFrequency	
	call	PWM

next:
	
	bsf   		T1CON,TMR1ON
	retfie	

return
;--------------------------------------------
;  LCD
;wait	goto	wait
;
;subroutine to initialize LCD
;
init	movlw	0x30
		movwf	0x20
		call 	lcdc
		call	del_41

		movlw	0x30
		movwf	0x20
		call 	lcdc
		call	del_01

		movlw	0x30
		movwf	0x20
		call 	lcdc
		call	mdel

		movlw	0x01		; display clear
		movwf	0x20
		call 	lcdc
		call	mdel

		movlw	0x06		; ID=1,S=0 increment,no  shift 000001 ID S
		movwf	0x20
		call 	lcdc
		call	mdel

		movlw	0x0c		; D=1,C=B=0 set display ,no cursor, no blinking
		movwf	0x20
		call 	lcdc
		call	mdel

		movlw	0x38		; dl=1 ( 8 bits interface,n=12 lines,f=05x8 dots)
		movwf	0x20
		call 	lcdc
		call	mdel
		return
;
;subroutine to write command to LCD
;

lcdc	movlw	0x00		; E=0,RS=0 
		movwf	PORTE
		movf	0x20,w
		movwf	PORTD
		movlw	0x01		; E=1,RS=0
		movwf	PORTE
        call	sdel
		movlw	0x00		; E=0,RS=0
		movwf	PORTE
		return
;
;subroutine to write data to LCD
;
lcdd	movlw		0x02		; E=0, RS=1
		movwf		PORTE
		movf		0x20,w
		movwf		PORTD
        movlw		0x03		; E=1, rs=1  
		movwf		PORTE
		call		sdel
		movlw		0x02		; E=0, rs=1  
		movwf		PORTE
		return
;----------------------------------------------------------

del_41	movlw		0xcd
		movwf		0x23
loop6	movlw		0x20
		movwf		0x22
loop7	decfsz		0x22,1
		goto		loop7
		decfsz		0x23,1
		goto 		loop6 
		return


del_01	movlw		0x20
		movwf		0x22
loop8	decfsz		0x22,1
		goto		loop8
		return


sdel	movlw		0x19		; movlw = 1 cycle
		movwf		0x23		; movwf	= 1 cycle
loop2	movlw		0xfa
		movwf		0x22
loop1	decfsz		0x22,1		; decfsz= 12 cycle
		goto		loop1		; goto	= 2 cycles
		decfsz		0x23,1
		goto 		loop2 
		return


mdel	movlw		0x0a
		movwf		0x24
loop5	movlw		0x19
		movwf		0x23
loop4	movlw		0xfa
		movwf		0x22
loop3	decfsz		0x22,1
		goto		loop3
		decfsz		0x23,1
		goto 		loop4 
		decfsz		0x24,1
		goto		loop5
		return

;-------------------------------------------------------------------------------------------------------------
MULT_16x16:
;MULT_16x16_FASTEST MACRO aH,aL, bH,bL, r4,r3,r2,r1, t1

	; 16x8 multiply  aH:aL * bL -> r3:r4:r1
	CLRF	r3
	CLRF	r1
	CLRC
	MOVFW	bL
	mmac	aL,0, r3,r1
	mmac	aL,1, r3,r1
	mmac	aL,2, r3,r1
	mmac	aL,3, r3,r1
	mmac	aL,4, r3,r1
	mmac	aL,5, r3,r1
	mmac	aL,6, r3,r1
	mmac	aL,7, r3,r1
	CLRF	r4
	; carry already clear from last RRF of mmac above
	; bL still in W
	mmac	aH,0, r3,r4
	mmac	aH,1, r3,r4
	mmac	aH,2, r3,r4
	mmac	aH,3, r3,r4
	mmac	aH,4, r3,r4
	mmac	aH,5, r3,r4
	mmac	aH,6, r3,r4
	mmac	aH,7, r3,r4
	
	; 16x8 multiply  aH:aL * bH -> r4:t1:r2
	;  middle byte from previous 16x8 multiplication starts in r4
	;  but ends in r2
	CLRF	r2
	MOVFW	bH
	; carry already clear from last RRF of mmac above
	mmac	aL,0, r4,r2
	mmac	aL,1, r4,r2
	mmac	aL,2, r4,r2
	mmac	aL,3, r4,r2
	mmac	aL,4, r4,r2
	mmac	aL,5, r4,r2
	mmac	aL,6, r4,r2
	mmac	aL,7, r4,r2
	; W still holds bH
	CLRF	t1	; bH,bL, or aL may be used for t1
	; carry already clear from last RRF of mmac above
	mmac	aH,0, r4,t1
	mmac	aH,1, r4,t1
	mmac	aH,2, r4,t1
	mmac	aH,3, r4,t1
	mmac	aH,4, r4,t1
	mmac	aH,5, r4,t1
	mmac	aH,6, r4,t1
	mmac	aH,7, r4,t1
	
	; add middle byte aH:aL * bH to upper byte of aH:aL * bL
	MOVFW	t1
	ADDWF	r3,F
	SKPNC
	 INCF	r4,F

return
;-------------------------------------------------------
 ;  Peter Hemsley's 12-bit Bin_to_BCD (ProdH:ProdL binary input
 ;  and D3:D0 unpacked BCD output)
 ;
 Bin2Dec
         movf    ProdH,W         ; 16-bit binary hi                
         iorlw   0xF0            ; w=H2-16                         
         movwf   D1              ; D1=H2-16                        
         addwf   D1,f            ; D1=H2*2-32                      
         addwf   D1,f            ; D1=H2*3-48                      
         movwf   D2              ; D2=H2-16                        
         addlw   -D'5'           ; w=H2-21                         
         addwf   D2,f            ; D2=H2*2-37 Done!                
         addlw   D'41'           ; w=H2+20                         
         movwf   D0              ; D0=H2+20                        
         swapf   ProdL,W         ; 16-bit binary lo                
         iorlw   0xF0            ; w=H1-16                         
         addwf   D1,f            ; D1=H2*3+H1-64                   
         addwf   D0,f            ; D0=H2+H1+4, C=1                 
         rlf     D0,f            ; D0=(H2+H1)*2+9, C=0             
         comf    D0,f            ; D0=-(H2+H1)*2-10                
         rlf     D0,f            ; D0=-(H2+H1)*4-20                
         movf    ProdL,W         ;                                 
         andlw   0x0F            ; w=H0                            
         addwf   D0,f            ; D0=H0-(H2+H1)*4-20 Done!        
         rlf     D1,f            ; C=0, D1=H2*6+H1*2-128 Done!     
         movlw   D'5'            ;                                 
         movwf   D3              ;                                 
         movlw   D'10'           ;                                 
 mod0    addwf   D0,f            ; D(X)=D(X)mod10                  
         decf    D1,f            ; D(X+1)=D(X+1)+D(X)div10         
         skpc                    ;                                
         goto    mod0            ;                                 
 mod1    addwf   D1,f            ;                                 
         decf    D2,f            ;                                 
         skpc                    ;                                 
         goto    mod1            ;                                 
 mod2    addwf   D2,f            ;                                
         decf    D3,f            ;                                 
         skpc                    ;                                 
         goto    mod2            ;                                 
         return                  ;                                 
  
ShowFrequency:
;------------------------------------------------------------------
		movlw 	0x0f
		movwf	bL
		clrf	bH
;--------------------------------------------------------------------
		;movlw 	0x25
		;movwf	aL
		;clrf	bH
;--------------------------------------------------------------------
		call	MULT_16x16
		movf	r1,w
		;movlw 0xD2
		movwf	ProdL

		;movlw 0x04
		movf	r2,w
		movwf	ProdH

		call	Bin2Dec
;----------------------------*------------------------------	
		movlw	0x80	;0xC4			 ;PLACE for the data on the LCD
		movwf	0x20
		call 	lcdc
		call	mdel
;------------------------------------------------------------------------
		movlw	'P'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'W'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'M'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	' '
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'F'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'a'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'n'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	' '
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'C'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'o'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'n'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	't'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'r'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'o'
		movwf	0x20
		call 	lcdd
		call	mdel
		movlw	'l'
		movwf	0x20
		call 	lcdd
		call	mdel
		;movlw	'l'
		;movwf	0x20
		;call 	lcdd
		;call	mdel
		;movlw	'e'
		;movwf	0x20
		;call 	lcdd
		;call	mdel
		;movlw	'r'
		;movwf	0x20
		;call 	lcdd
		;call	mdel
;----------------------------------------------------------	
		movlw	0xC0			 ;PLACE for the data on the LCD
		movwf	0x20
		call 	lcdc
		call	mdel
;--------------------------------------------------------------
		movlw	'R'
		movwf	0x20
		call 	lcdd
		call	mdel

		movlw	'P'
		movwf	0x20
		call 	lcdd
		call	mdel

		movlw	'M'
		movwf	0x20
		call 	lcdd
		call	mdel

		movlw	'-'
		movwf	0x20
		call 	lcdd
		call	mdel
;------------------------------------------ CHAR (the data )
		movf	D3,f
		btfss	STATUS,Z
		goto	Digit3
		movlw	' ' ;Do not show leading Zero
		movwf	0x20
		call 	lcdd
		call	mdel
		goto	Digit2
Digit3:	movlw	0x30
		addwf	D3,w ;LSB
		movwf	0x20
		call 	lcdd
		call	mdel
;----------------------------------------------------------
Digit2:	movlw	0x30
		addwf	D2,w
		movwf	0x20
		call 	lcdd
		call	mdel
;----------------------------------------------------------
		movlw	0x30
		addwf	D1,w
		movwf	0x20
		call 	lcdd
		call	mdel
;----------------------------------------------------------
		movlw	0x30
		addwf	D0,w;MSB
		movwf	0x20
		call 	lcdd
		call	mdel
;----------------------------------------------------------


return
;------------------------------------------
PWM:

	;MOVLW	0x01;0x03   ;0x30 - Duty 97%; 0x01 - Duty < 2%
	movf	PWMcounter,w
	MOVWF	CCPR1L		;Duty Cycle  
	;CLRF	INTCON		;Disable interrupts, clear T0IF
	BSF	STATUS,RP0		;Bank1
	MOVLW	0x30;0x09	;PWM period is 10*4Tosc
	MOVWF	PR2
	BCF	TRISC,0x2		;Make pin Output
	;CLRF	PIE1		;Disable peripheral interrupts
	BCF	STATUS,RP0		;Bank0
	CLRF	PIR1
	MOVLW	0x0C		;PWM mode,2 LSB bits of Duty Cycle=10
	MOVWF	CCP1CON		;Duty Cycle=1010 =10Tosc
	BSF	T2CON,TMR2ON	;Timer2 starts to increment
	

return

;------------------------------------------
ADCconvert:

bcf STATUS,RP0 ;-------> Bank 0

call d_20 ;Delay TACQ
loop: bsf ADCON0,GO ;start conversion
waitc: btfsc ADCON0,GO ;wait end of conversion
goto waitc
call d_4

movf ADRESH,W
;movwf 0x23
movwf PORTC	

d_20: movlw 0x20
movwf 0x22
loop11: decfsz 0x22,1
goto loop11
return
d_4: movlw 0x06
movwf 0x22
loop22: decfsz 0x22,1
goto loop22
return

;--------------------------------------------------
end