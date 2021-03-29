// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"

// Bit-specific addressing
#define PF2 (*((volatile unsigned long *)0x40025010)) // PF2
#define PF4 (*((volatile unsigned long *)0x40025040))  // PF4




//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none

void IO_Init(void) { volatile uint32_t delay;
 // --UUU-- Code to initialize PF4 and PF2
  
  
}

//------------IO_HeartBeat------------
// Toggle the output state of the PF2 LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- 
  
}

//------------IO_Touch------------
// wait for release and press of the switch
// Input: none
// Output: none
// 1) wait for release; 
// 2) delay for 5ms; 
// 3) wait for press; and then 
// 4) delay for another 5ms
void IO_Touch(void) {
 // --UUU-- 
  
}  


