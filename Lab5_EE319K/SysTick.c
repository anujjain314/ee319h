// SysTick.h
// Runs on TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait. 
// Daniel Valvano
// January 17, 2021
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;						// Clear Enable Bit
	NVIC_ST_RELOAD_R = 0x00FFFFFF;	// Set Reload value to max 
	NVIC_ST_CURRENT_R = 0;				// Clear Current 
	NVIC_ST_CTRL_R = 0x05;				// Set clock mode, disable interupts, set enable
}


// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(uint32_t delay){
  NVIC_ST_RELOAD_R = delay - 1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000) == 0){}	// wait until count it set
}


// 10000us equals 10ms
void SysTick_Wait10ms(uint32_t delay){
	for(uint32_t i = 0; i < delay; i++){
		SysTick_Wait(800000);				// 800000 cycles * 12.5 ns/cycle = 10 ms
	}
}

