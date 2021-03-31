; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; SSD1306_OutChar   outputs a single 8-bit ASCII character
; SSD1306_OutString outputs a null-terminated string 

    IMPORT   SSD1306_OutChar
    IMPORT   SSD1306_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
    PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
		
	MACRO
	MOD $Rd, $Rn1, $Rn2
	UDIV $Rd, $Rn1, $Rn2
	MUL $Rd, $Rd, $Rn2
	SUB $Rd, $Rn1, $Rd
	MEND

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
digit EQU 0						; binding, digit = rightmost digit of input
LCD_OutDec
     PUSH {R0, LR}
	 SUB SP, SP, #4			    ; allocation
	 
	 MOV R2, #10				; recursive case 
	 MOD R1, R0, R2				; digit = R0 % 10
	 STRB R1, [SP, #digit] 
	 UDIV R0, R2				; input = input / 10
	 
	 CMP R0, #0					; base case - input is zero
	 BEQ OutDec_Done
	 
	 BL LCD_OutDec				; call OutDec with new input
	 
OutDec_Done	 
	 LDRB R0, [SP, #digit]		; after reaching base case, convert digits to ascii characters and print
	 ADD R0, #0x30
	 BL SSD1306_OutChar
 
	 ADD SP, SP, #4;	  		; deallocation
     POP {R0, LR}
	 BX LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
dec0 EQU 0					; binding
int  EQU 4
dec1 EQU 8
LCD_OutFix
	PUSH {R11, LR}
	SUB SP, SP, #12			; allocation

	LDR R1, =1000			; output *.** if R0 > 999
	CMP R0, R1
	BHS outStars
	
	MOV R1, #10				; dec0 = input%10, ones digit
	MOD R2 ,R0, R1
	STR R2, [SP, #dec0]
	UDIV R0, R0, R1
	
	MOD R2, R0, R1		    ; dec1 = (input/10)%10, tens digit
	STR R2, [SP, #dec1]	
	UDIV R2, R0, R1
	
	STR R2, [SP, #int]		; int = inpt/100, hundreds digit
	
	LDR R0, [SP, #int]	    ; print int, hundreds digit
	ADD R0, #0x30
	BL SSD1306_OutChar
	
	MOV R0, #0x2E			; print "."
	BL SSD1306_OutChar
	
	LDR R0, [SP, #dec1]	    ; print dec1, tens digit
	ADD R0, #0x30
	BL SSD1306_OutChar
	
	LDR R0, [SP, #dec0]		; print dec2, ones digit
	ADD R0, #0x30
	BL SSD1306_OutChar
	
	B doneOutFix
	
outStars				   ; output *.**
	MOV R0, #0x2A
	BL SSD1306_OutChar
	MOV R0, #0x2E
	BL SSD1306_OutChar
	MOV R0, #0x2A
	BL SSD1306_OutChar
	MOV R0, #0x2A
	BL SSD1306_OutChar
	
doneOutFix
	ADD SP, SP, #12		   ; deallocation
	POP {R11, LR}
	
    BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN          ; make sure the end of this section is aligned
     END            ; end of file
