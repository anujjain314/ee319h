// Lab6.c
// Runs on TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: George Koussa & Anuj Jain
// Date Created: 3/6/17 
// Last Modified: 3/24/21  
// Lab number: 6
// Hardware connections
// PB0-PB4 : 4-bit dac, PE0-PE4: four synthesizer keys


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Sound.h"
#include "Key.h"
#include "Music.h"
#include "TExaS.h"

void DAC_Init(void);         // your lab 6 solution
void DAC_Out(uint8_t data);  // your lab 6 solution
uint8_t Testdata;

// lab video Lab6_voltmeter
int voltmetermain(void){ //voltmetermain(void){     
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  DAC_Init(); // your lab 6 solution
  Testdata = 15;
  EnableInterrupts();
  while(1){                
    Testdata = (Testdata+1)&0x0F;
    DAC_Out(Testdata);  // your lab 6 solution
  }
}

// lab video Lab6_static
int staticmain(void){   uint32_t last,now;  
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  LaunchPad_Init();
  DAC_Init(); // your lab 6 solution
  Testdata = 15;
  EnableInterrupts();
  last = LaunchPad_Input();
  while(1){                
    now = LaunchPad_Input();
    if((last != now)&&now){
       Testdata = (Testdata+1)&0x0F;
       DAC_Out(Testdata); // your lab 6 solution
    }
    last = now;
    Clock_Delay1ms(25);   // debounces switch
  }
}



//**************Lab 6 solution below*******************

Note ode_to_joy[30] = {{E, 3, 500}, {E, 4, 500}, {F, 5, 500}, {G, 7, 500}, 
											 {G, 9, 500}, {F, 11, 500}, {E, 13, 500}, {D, 14, 500}, 
                       {C0, 15, 500}, {C0, 15, 500}, {D, 13, 500}, {E, 11, 500}, 
                       {E, 7, 750}, {D, 5, 250}, {D, 3, 1000},
											  {E, 3, 500}, {E, 4, 500}, {F, 5, 500}, {G, 7, 500}, 
											 {G, 9, 500}, {F, 11, 500}, {E, 13, 500}, {D, 14, 500}, 
                       {C0, 15, 500}, {C0, 15, 500}, {D, 13, 500}, {E, 11, 500}, 
                       {D, 7, 750}, {C0, 5, 250}, {C0, 3, 1000}};


int main(void){   	
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Key_Init();
  LaunchPad_Init();
  Music_Init();
  // other initialization
  EnableInterrupts();
	uint32_t heartbeat = 1000000;
	uint8_t led = 0;
	
  while(1){
		//Periodically toggle LED
		heartbeat++;
		if (heartbeat > 100000) {
			LaunchPad_Output(led);
			led = !led;
			heartbeat = 0;
		}
		
		// Play different frequencies when each button is pressed, turns sound off if music is not playing
		uint32_t keys = Key_In();
		if((keys&0x01) == 0x01){
				Sound_Start(A);
		} else if((keys&0x02) == 0x02){
				Sound_Start(B);
		} else if((keys&0x04) == 0x04){
				Sound_Start(C);
		} else if((keys&0x08) == 0x08){
				Sound_Start(D1);
		} else if(!playingMusic){
				Sound_Off();
		}
		
		// Play music if SW2 on launchpad is pressed, Stop music if SW1 is pressed
		uint8_t switches = LaunchPad_Input();
		if(switches == 0x01){
				Music_PlaySong(ode_to_joy, 30);
		} else if(switches == 0x02){
				Music_StopSong();
		}
		Clock_Delay(25); //debounce switch
  }          
}


