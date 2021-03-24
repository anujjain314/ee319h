// Key.h
// This software configures the off-board piano keys
// Lab 6 requires a minimum of 3 keys, but you could have more
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/21 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#ifndef KEY_H
#define KEY_H
#include <stdint.h>
// Header files contain the prototypes for public functions
// this file explains what the module does

// **************Key_Init*********************
// Initialize piano key inputs, called once to initialize the digital ports
// Input: none 
// Output: none
void Key_Init(void);

// **************Key_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 15
//   0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
//   bit n is set if key n is pressed
uint32_t Key_In(void);

#endif

