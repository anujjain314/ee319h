// Lab7Main.c
// Runs on TM4C123
// Use I2C to send an 8-bit data/commands to the SSD1306 128 by 64 pixel oLED
// SSD1306 does not simulate, you must debug with real SSD1306
// Jonathan and Daniel Valvano
// Last Modified: 1/17/2021
// Ramesh Yerraballi modified 3/20/2017

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

// Typo on OLED has a 0-ohm resistor selection 
// that says        "IIC ADDRESS SELECT 0x78 0x7A"
// should have said "IIC ADDRESS SELECT 0x3C 0x3D"
// search for I2C SSD1306 on Amazon.com
// https://www.amazon.com/gp/product/B0871KW7BD

// EE319K uses I2C3 
// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

#include <stdio.h>
#include <stdint.h>
#include "PLL.h"
#include "SSD1306.h"
#include "../inc/tm4c123gh6pm.h"
#include "IO.h"
#include "Print.h"
#include "image.h"

#define SIZE 16
uint32_t const TestData[SIZE] ={
  0,7,9,10,99,100,409,654,999,1000,9999,10000,20806,65535,
  123400009,0xFFFFFFFF
};

int main(void){  
  uint32_t i,line;
  PLL_Init(Bus80MHz);    // set system clock to 80 MHz
  IO_Init();
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  while(1){  
    SSD1306_OutClear();       // set screen to black
    SSD1306_DrawBMP(0,60,logo,14,SSD1306_WHITE);
    SSD1306_OutBuffer();
    IO_Touch();	
    SSD1306_OutClear();       // set screen to black
    SSD1306_OutString("Lab 7 Spring 2021\nWelcome to EE319K\n\nHit SW1 to advance");
    IO_Touch();

    SSD1306_OutClear();       // set screen to black
    line = 0;
    for(i=0;i<SIZE;i++){
			if(line == 8){
        IO_Touch(); // wait to see all test cases
        line = 0;   // clear and restart
        SSD1306_OutClear();   // set screen to black
			}
      IO_HeartBeat();
      SSD1306_SetCursor(0,line);
      LCD_OutDec(TestData[i]);
      SSD1306_SetCursor(11,line); line++;
      LCD_OutFix(TestData[i]);
    }
    IO_Touch(); // wait to see all test cases
  }
}

