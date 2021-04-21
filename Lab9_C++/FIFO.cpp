// FIFO.cpp
// Runs on any CortexM
// Provide functions that initialize a FIFO, put data in, get data out,
// and return the current size.  The file includes a transmit FIFO
// using index implementation and a receive FIFO using pointer
// implementation.  Other index or pointer implementation FIFOs can be
// created using the macros supplied at the end of the file.
// Daniel Valvano
// 1/11/2021

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to the Arm Cortex M3",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2011
   Programs 3.7, 3.8., 3.9 and 3.10 in Section 3.7

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
#include <stdint.h>

#include "FIFO.h"
#include "SSD1306.h"
#include "print.h"

// A class named Queue that defines a FIFO
Queue::Queue(){
  // Constructor - 
  // We are assuming that for an empty Queue, both PutI and GetI will be equal
  
  // write this
	PutI = 0;
	GetI = 0;
}

// To check whether Queue is empty or not
bool Queue::IsEmpty(void){
   // write this
	if (PutI == GetI) return true;
	else return false;
}

  // To check whether Queue is full or not
bool Queue::IsFull(void){
   // write this
	if ((PutI + 1) % FIFOSIZE == GetI) return true;
  else return false;
}

  // Inserts an element in queue at rear end
bool Queue::Put(char x){
   // write this
	if (Queue::IsFull()) return false;
	Buf[PutI] = x;
	PutI = (PutI + 1) % FIFOSIZE;
  return true;
}

  // Removes an element in Queue from front end. 
bool Queue::Get(char *pt){
   // write this
	if (Queue::IsEmpty()) return false;
	*pt = Buf[GetI];
	GetI = (GetI + 1) % FIFOSIZE;
  return true;
}

  /* 
     Printing the elements in queue from front to rear. 
     This function is only to test the code. 
     This is not a standard function for Queue implementation. 
  */
void Queue::Print(void){
	for (int i = GetI; i < PutI; i = (i+1)%FIFOSIZE) {
		SSD1306_OutChar(Buf[i]);
	}
}


