// Music.h
// This program contains timer0A timer1A ISR for 
// playing your favorite Song.
//
// For use with the TM4C123
// EE319K lab6 
// 3/24/21

// Description: 
// This file contains the interrupt delays for notes, the samples
// for the sine wave used to make sound, and the data structure
// containing notes for your favorite.
//

#include <stdint.h>

#ifndef MUSIC_H
#define MUSIC_H

// 1 if the Timer interrupt is playing music, 0 if no music is playing
extern uint8_t playingMusic;

// a single note, with a volume from 0 - 15, a pitch, and a time for how long the note is played
typedef const struct Note {
	uint16_t pitch;
	uint8_t volume;
	uint32_t time;
} Note;

// initialized the interrupt timer and the sound module
void Music_Init(void);

// Plays your favorite song
// inputs: a pointer to an array of notes, the length of that array
void Music_PlaySong(Note* song, uint16_t len);

// Stops song
void Music_StopSong(void);

#endif

