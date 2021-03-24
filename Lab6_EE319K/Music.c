// Music.c
// This program can use timer0A and timer1A ISR  
// playing your favorite song.
//
// For use with the TM4C123
// EE319K lab6 extra credit
// Program written by: put your names here
// 1/17/21

#include "Sound.h"
#include "DAC.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define C0  4778   // 523.3 Hz
#define D   4257   // 587.3 Hz
#define E   3792   // 659.3 Hz
#define F   3579   // 698.5 Hz
#define G    3189   // 784 Hz


struct note {
	uint32_t pitch;
	uint32_t volume;
	uint32_t time;
};

struct note E_q = {E, , 500};
struct note F_q = {F, , 500};
struct note G_q = {G, , 500};
struct note D_q = {D, , 500};
struct note C_q = {C0, , 500};

struct note E_dq = {E, ,750};
struct note D_e = {D, , 250};
struct note D_h = {D, , 1000};
	

void Music_Init(void){
  // write this
  // extra credit

}


// Play song, while button pushed or until end
void Music_PlaySong(void){
  // write this
  // extra credit

}

// Stop song
void Music_StopSong(void){
   // write this
  // extra credit

}

// Timer0A actually controls outputting to DAC
void Timer0A_Handler(void){
    // write this
  // extra credit

}

  // Timer1A acts as a metronome for the song(s)
void Timer1A_Handler(void){
    // write this
  // extra credit


}
#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define NVIC_EN0_INT21          0x00200000  // Interrupt 21 enable

// ***************** Timer_Init ****************
// Activate Timer0A and Timer1A interrupts to run user task periodically
// Inputs: period0 in nsec
//         period1 in msec
// Outputs: none
void Timer_Init(unsigned int period0, unsigned int period1){ uint32_t volatile delay;
   // write this
  // extra credit

}
void Timer_Stop(void){ 
    // write this
  // extra credit

}

