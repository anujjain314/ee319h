// Key.c
// This software configures the off-board piano keys
// Lab 6 requires a minimum of 3 keys, but you could have more
// Runs on TM4C123
// Program written by: Anuj Jain and George Koussa
// Date Created: 3/6/17 
// Last Modified: 1/17/21  
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// **************Key_Init*********************
// Initialize piano key inputs on PE3-0
// Input: none 
// Output: none
void Key_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x10;
	volatile int a = 4;
	a++;
	GPIO_PORTE_DIR_R |= 0x0F;
	GPIO_PORTE_DIR_R |= 0x0F;
}
// **************Key_In*********************
// Input from piano key inputs on PA5-2 PB3-0 or PE3-0
// Input: none 
// Output: 0 to 15 depending on keys
//   0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
uint32_t Key_In(void){ 
	return (GPIO_PORTE_DATA_R & 0x0F);
}

