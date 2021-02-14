;****************** main.s ***************
; Program written by: Valvano, solution
; Date Created: 2/4/2017
; Last Modified: 1/17/2021
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE2 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608


       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2
		   
;global variables go here
varDuty RN 2
current  RN 3

       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
	
       EXPORT  Start
		   
; CONSTANTS
TOTAL_DELAY 	DCD 10000000
TWENTY_PERCENT 	DCD 2000000
THIRTY_PERCENT 	DCD 3000000
NINETY_PERCENT 	DCD 9000000
TEN_PERCENT 	DCD 1000000

Start
     ; TExaS_Init sets bus clock at 80 MHz
     BL  TExaS_Init
	 ; voltmeter, scope on PD3
	 ; Initialization goes here
	 LDR 	R0, =SYSCTL_RCGCGPIO_R					
	 LDR 	R1, [R0]								
	 ORR 	R1, #0x30							; Set bit for Port E & F clock to 1
	 STR 	R1, [R0]								
	 
	 ; Wait for the clock to stabilize
	 NOP
	 NOP
	 
	 ; Define I/O (DIR)
	 ; Port E
	 LDR 	R0, =GPIO_PORTE_DIR_R
	 LDR 	R1, [R0]
	 AND 	R1, #0x00							; Set PE1 bit to 0 for input
	 ORR 	R1, #0x04							; Set PE2 bit to 1 for output
	 STR 	R1, [R0]
	 
	 ; Port F
	 LDR 	R0, = GPIO_PORTF_DIR_R
	 LDR 	R1, [R0]
	 AND 	R1, #0x00							; Set PF4 bit to 0 for input
	 STR 	R1, [R0]
	 
	 ; Digitally enable pins (DEN)
	 ; Port E
	 LDR 	R0, =GPIO_PORTE_DEN_R
	 LDR 	R1, [R0]
	 ORR 	R1, #0x06							; Set PE1, PE2 bits to 1 to enable them
	 STR 	R1, [R0]
	 
	 ; Port F
	 LDR 	R0, =GPIO_PORTF_DEN_R
	 LDR 	R1, [R0]
	 ORR 	R1, #0x10							; Set PF4 bit to 1 to enable it
	 STR 	R1, [R0]
	 
	 ; trial
;	 ; Port E
;	 LDR R0, =GPIO_PORTE_AFSEL_R
;	 LDR R1, [R0]
;	 AND R1, #0x00
;	 STR R1, [R0]
	 												
	 LDR 	current, THIRTY_PERCENT				; Start with a duty cycle of 30%
	 LDR 	R0, =GPIO_PORTE_DATA_R				; R0 will hold the address of port E
	 
	 ; TExaS voltmeter, scope runs on interrupts
	 CPSIE  I    								
		
	 B 		loop
	
	
loop  
; main engine goes here
	 BL		controlLED
	 BL		checkButtons
	 B  	loop


; SUBROUTINE: controlLED
controlLED

	 PUSH	{LR, R4}
	 
	 ; turn off LED
	 LDR 	R1, [R0]
	 ORR 	R1, #0x04
	 STR 	R1, [R0]
	 
	 ; high duty cycle
	 MOV 	varDuty, current
	 BL 	delay
	 
	 ; turn off LED
	 LDR 	R1, [R0]
	 BIC 	R1, #0x04
	 STR 	R1, [R0]
	 
	 ; low duty cycle
	 LDR 	R4, TOTAL_DELAY
	 SUBS  	varDuty, R4, current
	 BL 	delay
	 
	 POP	{LR, R4}
	 BX		LR
	 
	 
; SUBROUTINE: delay	
delay
	 SUBS 	varDuty, varDuty, #1
	 BNE 	delay
	 BX 	LR


; SUBROUTINE: checkButtons
checkButtons
	 
	 ; Check if PE1 is high, if so call changeDutyCycle subroutine
	 LDR 	R1, [R0]
	 AND 	R4, R1, #0x02
	 CMP 	R4, #0x02
	 BNE 	continue
	 PUSH	{LR, R4}
	 BL		changeDutyCycle
	 POP	{LR, R4}
	 
continue 
	 BX		LR
	 
	 
; SUBROUTINE: changeDutyCycle
changeDutyCycle

wait
	 ; wait until PE1 is low
;	 PUSH	{LR, R12}
	 LDR 	R1, [R0]
	 AND 	R4, R1, #0x02
	 CMP 	R4, #0x02
	 BEQ	wait
;	 BL 	controlLED
;	 POP	{R12, LR}

	 ; check current duty cycle
	 LDR 	R4, NINETY_PERCENT
	 CMP 	current, R4
	 BEQ 	resetValue
	 
	 ; increase duty cycle by 20% if current is not 90%
	 LDR 	R4, TWENTY_PERCENT
	 ADD 	current, R4
	 BX		LR
	 
	 ; set duty cycle to 10% if current is 90%
resetValue
	 LDR 	current, TEN_PERCENT 
	 BX		LR

	 ALIGN      ; make sure the end of this section is aligned
	 END        ; end of file

