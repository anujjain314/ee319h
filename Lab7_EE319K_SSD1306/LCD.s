; LCD.s
; Student names: Anuj Jain and George Koussa
; Last modification date: 3/29/2021

; Runs on TM4C123
; Use I2C3 to send data to SSD1306 128 by 64 pixel oLED

; As part of Lab 7, students need to implement I2C_Send2

      EXPORT   I2C_Send2
      PRESERVE8
      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      ALIGN
I2C3_MSA_R  EQU 0x40023000
I2C3_MCS_R  EQU 0x40023004
I2C3_MDR_R  EQU 0x40023008
; sends two bytes to specified slave
; Input: R0  7-bit slave address
;        R1  first 8-bit data to be written.
;        R2  second 8-bit data to be written.
; Output: 0 if successful, nonzero (error code) if error
; Assumes: I2C3 and port D have already been initialized and enabled
I2C_Send2
;; --UUU-- 
; 1) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
	LDR R3, =I2C3_MCS_R
	LDRB R3, [R3]
	AND R3, #0x01
	CMP R3, #0
	BNE I2C_Send2
	
; 2) write slave address to I2C3_MSA_R, 
;     MSA bits7-1 is slave address
;     MSA bit 0 is 0 for send data
	LSL R0, #1
	LDR R3, =I2C3_MSA_R
	STRB R0, [R3]
	
; 3) write first data to I2C3_MDR_R
	LDR R3, =I2C3_MDR_R
	STRB R1, [R3]
	
; 4) write 0x03 to I2C3_MCS_R,  send no stop, generate start, enable
; add 4 NOPs to wait for I2C to start transmitting
	MOV R1, #0x03
	LDR R3, =I2C3_MCS_R
	STR R1, [R3]
	
	NOP
	NOP
	NOP
	NOP

wait
; 5) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
	LDR R3, =I2C3_MCS_R
	LDRB R1, [R3]
	AND R1, #0x01
	CMP R1, #0
	BNE wait
	
; 6) check for errors, if any bits 3,2,1 I2C3_MCS_R is high 
;    a) if error set I2C3_MCS_R to 0x04 to send stop 
;    b) if error return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits

	LSR R1, #1
	AND R1, #07
	CMP R1, #0
	BEQ continue
	
	MOV R0, R1
	MOV R3, #0x04
	LDR R1, =I2C3_MCS_R
	STRB R3, [R1]
	BX LR
	
continue	

; 7) write second data to I2C3_MDR_R
	LDR R3, =I2C3_MDR_R
	STRB R2, [R3]
	
; 8) write 0x05 to I2C3_MCS_R, send stop, no start, enable
	MOV R1, #0x05
	LDR R3, =I2C3_MCS_R
	STR R1, [R3]

; add 4 NOPs to wait for I2C to start transmitting
	NOP
	NOP
	NOP
	NOP

; 9) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
wait2
	LDR R3, =I2C3_MCS_R
	LDRB R1, [R3]
	AND R1, #0x01
	CMP R1, #0
	BNE wait2
	
; 10) return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits
;     will be 0 is no error
    LSR R1, #1
	AND R1, #07
	CMP R1, #0
	BEQ continue2
	
	MOV R0, R1
	MOV R3, #0x04
	LDR R1, =I2C3_MCS_R
	STRB R3, [R1]

continue2
	MOV R0, #0

    BX  LR                          ;   return


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
 