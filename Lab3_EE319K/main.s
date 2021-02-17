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
GPIO_PORTE_DR8R_R  EQU 0x40024508


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
TOTAL_DELAY 	DCD 6640000
TWENTY_PERCENT 	DCD 1328000
THIRTY_PERCENT 	DCD 1992000
NINETY_PERCENT 	DCD 5976000
TEN_PERCENT 	DCD 664000

SIN_ELEMENTS	DCD 100
MAX_SIN			DCD 10000


SinTable
  DCD  5000, 5308, 5614, 5918, 6219, 6514, 6804, 7086, 7361, 7626
  DCD  7880, 8123, 8354, 8572, 8776, 8964, 9137, 9294, 9434, 9556
  DCD  9660, 9746, 9813, 9861, 9890, 9900, 9890, 9861, 9813, 9746
  DCD  9660, 9556, 9434, 9294, 9137, 8964, 8776, 8572, 8354, 8123
  DCD  7880, 7626, 7361, 7086, 6804, 6514, 6219, 5918, 5614, 5308
  DCD  5000, 4692, 4386, 4082, 3781, 3486, 3196, 2914, 2639, 2374
  DCD  2120, 1877, 1646, 1428, 1224, 1036,  863,  706,  566,  444
  DCD   340,  254,  187,  139,  110,  100,  110,  139,  187,  254
  DCD   340,  444,  566,  706,  863, 1036, 1224, 1428, 1646, 1877
  DCD  2120, 2374, 2639, 2914, 3196, 3486, 3781, 4082, 4386, 4692


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

	 ; increase max output current of PE2 to 8 mA
	 LDR    R0, =GPIO_PORTE_DR8R_R
	 LDR	R1, [R0]
	 ORR	R1, #0x04
	 STR	R1, [R0]

	 ; Port F
	 LDR 	R0, =GPIO_PORTF_DEN_R
	 LDR 	R1, [R0]
	 ORR 	R1, #0x10							; Set PF4 bit to 1 to enable it
	 STR 	R1, [R0]

	 ; Enable Pull Up Resistor on PE4
	 LDR	R0, =GPIO_PORTF_PUR_R
	 LDR 	R1, [R0]
	 ORR	R1, #0x10
	 STR	R1,	[R0]

	 LDR 	current, THIRTY_PERCENT				; Start with a duty cycle of 30%
	 LDR 	R0,  =GPIO_PORTE_DATA_R				; R0 will hold the address of port E
	 LDR 	R11, =SinTable						; load address of SinTable
	 LDR 	R12, =GPIO_PORTF_DATA_R				; R12 will hold the address of port D


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

	 ; turn on LED
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

; SUBROUTINE: startBreathing
startBreathing

	PUSH	{LR, R4}

; keep track of number of array elements indexed
startBreathing_init
	 LDR	R10, SIN_ELEMENTS
	 LDR 	R11, =SinTable
	 
continueBreathing
	 ; turn on LED
	 LDR 	R1, [R0]
	 ORR 	R1, #0x04
	 STR 	R1, [R0]

	 ; high duty cycle
	 LDR	R9, [R11]
	 MOV 	varDuty, R9
	 BL 	delay

	 ; turn off LED
	 LDR 	R1, [R0]
	 BIC	R1, #0x04
	 STR 	R1, [R0]

	 ; low duty cycle
	 LDR 	R4, MAX_SIN
	 SUBS  	varDuty, R4, R9
	 BL 	delay

	 ; load new delay from SinTable
	 ADD 	R11, #4
	 LDR 	R9, [R11]
	 SUBS	R10, #1
	 BEQ	startBreathing_init

	 ; check to see if PF4 is not pressed
	 LDR 	R1, [R12]
	 AND 	R4, R1, #0x10
	 CMP	R4, #0x00
	 BEQ	next
	 POP	{LR, R4}
	 BX		LR

next
	 ; check if full breathing cycle completed
	 CMP 	R10, #0
	 BEQ	startBreathing
	 B 		continueBreathing

; SUBROUTINE: delay
delay
	 SUBS 	varDuty, varDuty, #1
	 BNE 	delay
	 BX 	LR


; SUBROUTINE: checkButtons
checkButtons

; Check if PF4 is low, if so, start breathing
breathingButton
  	 LDR 	R1, [R12]
	 AND 	R4, R1, #0x10
	 CMP	R4, #0x00
	 BNE	dutyButton
	 PUSH	{LR, R4}
	 BL		startBreathing
	 POP	{LR, R4}

; Check if PE1 is high, if so call changeDutyCycle subroutine
dutyButton
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

     ; wait until PE1 is low
wait
	 LDR 	R1, [R0]
	 AND 	R4, R1, #0x02
	 CMP 	R4, #0x02
	 BEQ 	wait

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
