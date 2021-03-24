// Music.c
// This program can use timer0A and timer1A ISR  
// playing your favorite song.
//
// For use with the TM4C123
// EE319K lab6 extra credit
// Program written by: George Koussa & Anuj Jain
// 3/24/21

#include "Sound.h"
#include "DAC.h"
#include "Music.h" 
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define msToClockCycle(t) (t*80000) 

void Timer_Init(void);
void Timer_Stop(void);
void Timer0A_Handler(void);


static uint16_t length = 0;	// length of currentSong (0 means there are no notes left to play)
static Note* currentSong;		// pointer to the array of notes that is currently being processed (or has just been processed)
uint8_t playingMusic = 0;		// 0 is music is not playing, 1 if music is playing

// initialized the interrupt timer and the sound module, run once
void Music_Init(void){
	Sound_Init();
	Timer_Init();
	length = 0;
}

// plays the next note in the currentSong and updates the length
void playNextNote(){
		Sound_SetVolume(currentSong->volume);	// set volume
		Sound_Start(currentSong->pitch);			// set pitch
		currentSong = currentSong + 1;				// go to the next note
		length--;															// update length
}

// Play song, passed as a pointer to an array of notes along with its length
void Music_PlaySong(Note* song, uint16_t len){
	if(len > 0){
		playingMusic = 1;											// music is starting to be played
		length = len;
		currentSong = song;
		Timer0A_Handler();										// play first note
		TIMER0_CTL_R = 0x00000001; 						// enable timer0A
	} else {
		Music_StopSong();											// dont play anything if there are no notes to play
	}
}

// Stop song
void Music_StopSong(void){
	Timer_Stop();														// stop the timer
	length = 0;		
	Sound_SetVolume(15);										// set volume to default value
	Sound_Off();														// turn off sound
	playingMusic = 0;												// music is not longer being played
}

// Timer0A acts as a metronome for the song(s)
void Timer0A_Handler(void){
	TIMER0_ICR_R = 0x00000001;  // acknowledge
	if(length > 0){							// more notes to play
		TIMER0_TAILR_R = msToClockCycle(currentSong->time) - 1;		// set timer to the time that the note should be played for
		playNextNote();																						// play the note
	} else {
		Music_StopSong();																					// no more notes to play
	}
}


// NOT USED
// Timer1A actually controls outputting to DAC
void Timer1A_Handler(void){
    // write this
  // extra credit


}
#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define NVIC_EN0_INT21          0x00200000  // Interrupt 21 enable

// ***************** Timer_Init ****************
// Initilize Timer0A interrupt to run user task periodically, does not enable the timer
// Outputs: none
void Timer_Init(void){ uint32_t volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x01;                        // activate timer0
  delay = SYSCTL_RCGCTIMER_R;
  TIMER0_CTL_R = 0x00000000;                         // disable timer0A
  TIMER0_CFG_R = 0x00000000;                         // 32-bit mode
  TIMER0_TAMR_R = 0x00000002;                        // periodic mode
  TIMER0_TAPR_R = 0;                                 // 12.5 ns
  TIMER0_ICR_R = 0x00000001;   											 // clear timeout flag
  TIMER0_IMR_R = 0x00000001;   											 // arm timeout
  NVIC_PRI5_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000; // priority 4
  NVIC_EN0_R = NVIC_EN0_INT19;          						 // enable IRQ 19
}

// disables the timer
void Timer_Stop(void){ 
  TIMER0_CTL_R = 0x00000000;                         // disable timer0A
}

