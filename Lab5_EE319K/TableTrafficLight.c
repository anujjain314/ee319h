// TableTrafficLight.c solution to EE319K Lab 5, spring 2021
// Runs on TM4C123
// Moore finite state machine to operate a traffic light.  
// Anuj Jain, George Koussa
// March 10, 2021

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

// red south light connected to PE5
// yellow south light connected to PE4
// green south light connected to PE3
// red west light connected to PE2
// yellow west light connected to PE1
// green west light connected to PE0

// walk sensor connected to PA4
// south sensor connected to PA3
// west sensor connected to PA2

// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF1 (built-in red LED)
#include <stdint.h>
#include "SysTick.h"
#include "TExaS.h"
#include "../inc/tm4c123gh6pm.h"



void DisableInterrupts(void);
void EnableInterrupts(void);

#define PE543210                (*((volatile uint32_t *)0x400240FC)) // bits 5-0
#define PA432                   (*((volatile uint32_t *)0x40004070)) // bits 2-4
#define PF321                   (*((volatile uint32_t *)0x40025038)) // bits 3-1

typedef const struct State State_t;
struct State {
	uint32_t wait;
	uint32_t output;
	State_t *next[8];
}; 


void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTB_DATA_R;		// do i need to activate port b clock?
}

void GPIO_Init(){ volatile uint32_t delay;
		SYSCTL_RCGC2_R |= 0x11;  		// LM3S legacy clock register, turn on port A,E Clock
		delay = SYSCTL_RCGC2_R;
	
		GPIO_PORTA_DEN_R |= 0x1C;		// Enable Pins A2-A4, E0-E5
		GPIO_PORTE_DEN_R |= 0x3F;
		
		GPIO_PORTA_DIR_R &= ~0x1C;	// make A2-A4 inputs
		GPIO_PORTE_DIR_R |= 0x3F;		// make E0-E5 outputs
	
		GPIO_PORTA_PDR_R |= 0x1C;		// activate internal PDR for switch inputs
}

int main(void){
	GPIO_Init();
	
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask);
  // PLL_Init();     // PLL on at 80 MHz
  SysTick_Init();   // Initialize SysTick for software waits
// **************************************************
// weird old bug in the traffic simulator
// run next two lines on real board to turn on F E B clocks
//  SYSCTL_RCGCGPIO_R |= 0x32;  // real clock register 
//  while((SYSCTL_PRGPIO_R&0x32)!=0x32){};
// run next two lines on simulator to turn on F E B clocks
// **************************************************
 
  EnableInterrupts();
	
	//FMS ENGINE
	
	State_t* currentState; // need to initialize
    
  while(1){
		PE543210 = currentState -> output;	
		SysTick_Wait10ms(currentState -> wait);
		currentState = currentState -> next[PA432];
  }
}



