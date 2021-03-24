// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: George Koussa & Anuj Jain
// Date Created: 3/6/17 
// Last Modified: 3/24/21
// Lab number: 6
// Hardware connections
// PB0-PB4 : 4-bit DAC

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"

void Sound_Off(void);

static uint8_t index = 0;		// current location within wave array
static uint8_t volume = 15;	// max value of 15, the volume of the sound to be played
const unsigned short wave[32] = {
  8,9,11,12,13,14,14,15,15,15,14,
  14,13,12,11,9,8,7,5,4,3,2,
  2,1,1,1,2,2,3,4,5,7};

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init();
	Sound_Off();
	index = 0;
	volume = 15;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;	// set Systick priority to 2
}

// **************Sound_Start*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Start called again, or Sound_Off is called
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Start(uint32_t period){
	if(period > 0){
		NVIC_ST_RELOAD_R = period - 1;	// set reload to period
		NVIC_ST_CTRL_R = 0x00000007;		// enable systick and systick interupts
	} else {
		Sound_Off();										// turn sound off if period is zero
	}
}

// **************Sound_Voice*********************
// Change voice
// EE319K optional
// Input: voice specifies which waveform to play
//           Pointer to wave table
// Output: none
void Sound_Voice(const uint8_t *voice){
  // optional
}
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
	index = 0;											// reset waveform index
	DAC_Out(0);											// output a default value of 0 to the DAC
  NVIC_ST_CTRL_R = 0x00000005;		// disable systick interrupts
}


// **************Sound_SetVolume*********************
// Sets Volume to a new Value
// Inputs: new value for volume
// Output: none
void Sound_SetVolume(uint8_t vol){
	volume = vol;
}
// **************Sound_GetVoice*********************
// Read the current voice
// EE319K optional
// Input: 
// Output: voice specifies which waveform to play
//           Pointer to current wavetable
const uint8_t *Sound_GetVoice(void){
 // write this
 // optional
  return 0;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
// outputs values from the waveform to the DAC
void SysTick_Handler(void){
		uint8_t output = wave[index];		//get output from wave array
		output = (output*volume)/15;		//scale output depending on volume
		DAC_Out(output);								//output to DAC
		index = 0x1F&(index+1);					//go to next index (wraps around when reaches final index)
}
