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
// west sensor connected to PA3
// south sensor connected to PA2

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
	uint32_t outputTraffic;
	uint32_t outputWalk;
	State_t *next[8];
}; 


#define start 			&FSM[0]
#define walk 				&FSM[1]
#define walkWarn1 	&FSM[2]
#define walkOff1 		&FSM[3]
#define walkWarn2 	&FSM[4]
#define walkOff2 		&FSM[5]
#define walkWarn3 	&FSM[6]
#define southG			&FSM[7]
#define southY			&FSM[8]
#define southR			&FSM[9]
#define westG				&FSM[10]
#define westY				&FSM[11]
#define westR				&FSM[12]


State_t FSM[13] = {
	// start
	{100, 0x24, 0x02, {start, westG, southG, southG, walk, walk, walk, walk}},
	// walk
	{100, 0x24, 0x0E, {walkWarn1, walkWarn1, walkWarn1, walkWarn1, walk, walkWarn1, walkWarn1, walkWarn1}},
	// walkWarn1
	{25, 0x24, 0x02, {walkOff1, walkOff1 , walkOff1 , walkOff1, walkOff1, walkOff1, walkOff1, walkOff1}},
	// walkOff1
	{25, 0x24, 0x00, {walkWarn2, walkWarn2, walkWarn2, walkWarn2, walkWarn2, walkWarn2, walkWarn2, walkWarn2}},
	// walkWarn2
	{25, 0x24, 0x02, {walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2, walkOff2}},
	// walkOff2
	{25, 0x24, 0x00, {walkWarn3, walkWarn3, walkWarn3, walkWarn3, walkWarn3, walkWarn3, walkWarn3, walkWarn3}},
	// walkWarn3
	{100, 0x24, 0x02, {start, westG, southG, southG, walk, westG, southG, southG}},
	// southG
	{100, 0x0C, 0x02, {southY, southY, southG, southY, southY, southY, southY, southY}},
	// southY
	{100, 0x14, 0x02, {southR, southR, southR, southR, southR, southR, southR, southR}},
	// southR
	{100, 0x24, 0x02, {start, westG, southG, westG, walk, westG, walk, westG}},
	// westG
	{100, 0x21, 0x02, {westY, westG, westY, westY, westY, westY, westY, westY}},
	// westY
	{100, 0x22, 0x02, {westR, westR, westR, westR, westR, westR, westR, westR}},
	// westR
	{100, 0x24, 0x02, {start, westG, southG, southG, walk, walk, walk, walk}}
};

void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTE_DATA_R;
}

void GPIO_Init(){ volatile uint32_t delay;
		SYSCTL_RCGC2_R |= 0x31;  		// LM3S legacy clock register, turn on port A,E,F Clock
		delay = SYSCTL_RCGC2_R;
	
		GPIO_PORTA_DEN_R |= 0x1C;		// Enable Pins A2-A4, E0-E5
		GPIO_PORTE_DEN_R |= 0x3F;
		GPIO_PORTF_DEN_R |= 0x0E;
		
		GPIO_PORTA_DIR_R &= ~0x1C;	// make A2-A4 inputs
		GPIO_PORTE_DIR_R |= 0x3F;		// make E0-E5 outputs
		GPIO_PORTF_DIR_R |= 0x0E;		// make F1-F3 outputs
	
		GPIO_PORTA_PDR_R |= 0x1C;		// activate internal PDR for switch inputs
}

int main(void){
	// Initialization
	GPIO_Init();
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask);
  //PLL_Init();     // PLL on at 80 MHz
  SysTick_Init();   // Initialize SysTick for software waits
// **************************************************
// weird old bug in the traffic simulator
// run next two lines on real board to turn on F E B clocks
//  SYSCTL_RCGCGPIO_R |= 0x32;  // real clock register 
//  while((SYSCTL_PRGPIO_R&0x32)!=0x32){};
// run next two lines on simulator to turn on F E B clocks
// ************************************************** 
  EnableInterrupts();
	
	// FMS ENGINE	
	State_t* currentState = start;
	
  while(1){
		PE543210 = currentState -> outputTraffic;	
		PF321 = currentState -> outputWalk;
		SysTick_Wait10ms(currentState -> wait);
		uint8_t input = PA432 >> 2;
		currentState = currentState -> next[input];
  }
}

