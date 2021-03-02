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

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF1 (built-in red LED)
#include <stdint.h>
#include "SysTick.h"
#include "TExaS.h"
#include "../inc/tm4c123gh6pm.h"



void DisableInterrupts(void);
void EnableInterrupts(void);

#define PB543210                (*((volatile uint32_t *)0x400050FC)) // bits 5-0
#define PE210                   (*((volatile uint32_t *)0x4002401C)) // bits 2-0
#define PF321                   (*((volatile uint32_t *)0x40025038)) // bits 3-1


void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTB_DATA_R;
}
// run this version in the simulator
int main(void){volatile uint32_t delay;
  DisableInterrupts();
  //TExaS_Init(&LogicAnalyzerTask);
  PLL_Init(); // PLL on at 80 MHz
  SYSCTL_RCGC2_R |= 0x32; // Ports B,E,F
  delay = SYSCTL_RCGC2_R;

//// run this version on the board
//int main(void){volatile uint32_t delay;
//  DisableInterrupts();
//  TExaS_Init(&LogicAnalyzerTask);
//  //PLL_Init(); // PLL on at 80 MHz
//  SYSCTL_RCGC2_R |= 0x32; // Ports B,E,F
//  delay = SYSCTL_RCGC2_R;
 
  EnableInterrupts();
    
  while(1){
// output
// wait
// input
// next	
  }
}



