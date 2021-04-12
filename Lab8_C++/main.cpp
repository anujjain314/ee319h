// main.cpp
// Runs on TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/17/2021 

// Specifications:
// Measure distance using slide pot, sample at 10 Hz
// maximum distance can be any value from 1.5 to 2cm
// minimum distance is 0 cm
// Calculate distance in fixed point, 0.01cm
// Analog Input connected to PD2=ADC5
// displays distance on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats (use them in creative ways)
// 

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "SSD1306.h"
#include "TExaS.h"
#include "PLL.h"
#include "SlidePot.h"
#include "print.h"

SlidePot my(150,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(unsigned long period){
  //*** students write this ******
}

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
 //*** students write this ******
}
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.01 cm
int main1(void){      // single step this program and look at Data
  DisableInterrupts();
  TExaS_Init();  // start scope set system clock to 80 MHz
  ADC_Init(SAC_NONE);    // turn on ADC, PD2, set channel to 5
  EnableInterrupts();
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 5, PD2
  }
}


uint32_t startTime,stopTime;
uint32_t ADCtime,OutDectime,ConvertTime,OutFixTime; // in usec
// use main2 to measure execution times
// use main2 to choose the sac value for your system
// use main2 to calibrate position as a function of ADC data
int main2(void){
  TExaS_Init();       // Bus clock is 80 MHz
  NVIC_ST_RELOAD_R = 0x00FFFFFF; // maximum reload value
  NVIC_ST_CURRENT_R = 0;          // any write to current clears it
  NVIC_ST_CTRL_R = 5;
  ADC_Init(SAC_32);  // turn on ADC, set channel to 5
 // 32-point averaging
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear(); 
  while(1){  // use SysTick to measure execution times 
    SSD1306_SetCursor(0,0);
    SSD1306_OutString((char*)"Lab 8, main2\nsac=5"); 
    // ----measure ADC conversion time----
    startTime = NVIC_ST_CURRENT_R;
    Data = ADC_In();  // sample 12-bit channel 5
    stopTime = NVIC_ST_CURRENT_R;
    ADCtime = ((startTime-stopTime)&0x0FFFFFF)/80;    // usec
    SSD1306_SetCursor(0,2);
    // ----measure LCD_OutDec time----
    startTime = NVIC_ST_CURRENT_R;
    LCD_OutDec(Data);
    SSD1306_OutString((char *)"   ");  // spaces cover up characters from last output
    stopTime = NVIC_ST_CURRENT_R;
    OutDectime = ((startTime-stopTime)&0x0FFFFFF)/80; // usec
    // ----measure Convert time----
    startTime = NVIC_ST_CURRENT_R;
    Position = my.Convert(Data);  // your Lab 8 program
    stopTime = NVIC_ST_CURRENT_R;
    ConvertTime = ((startTime-stopTime)&0x0FFFFFF)/80; // usec 
    // ----measure LCD_OutFix time----
    startTime = NVIC_ST_CURRENT_R;
    LCD_OutFix(Position);
    stopTime = NVIC_ST_CURRENT_R;
    OutFixTime = ((startTime-stopTime)&0x0FFFFFF)/80; // usec 
    //------show results
    SSD1306_SetCursor(0,3); SSD1306_OutString((char*)"ADC(us)= ");     LCD_OutDec(ADCtime);
    SSD1306_SetCursor(0,4); SSD1306_OutString((char*)"OutDec(us)= ");  LCD_OutDec(OutDectime);SSD1306_OutString((char*)"  ");
    SSD1306_SetCursor(0,5); SSD1306_OutString((char*)"Convert(us)= "); LCD_OutDec(ConvertTime);
    SSD1306_SetCursor(0,6); SSD1306_OutString((char*)"OutFix(us)= "); LCD_OutDec(OutFixTime);
  }
}
// use main3 to determine 
// See the Lab8_Accuracy  section in chapter 14 of the ebook
// http://users.ece.utexas.edu/%7Evalvano/Volume1/E-Book/C14_ADCdataAcquisition.htm
// Collect five measurements with your distance measurement system.
int main3(void){
  TExaS_Init();     // Bus clock is 80 MHz
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear(); 
  SSD1306_OutString((char*)"Lab 8, main3");  
  ADC_Init(SAC_32);  // turn on ADC, set channel to 5
 // 32-point averaging
  while(1){  
    Data = ADC_In();  // sample 12-bit channel 5
    Position = my.Convert(Data);
    SSD1306_SetCursor(0,1); // second row
    LCD_OutDec(Data); SSD1306_OutString((char*)"   ");
    SSD1306_SetCursor(0,2); // third row
    LCD_OutFix(Position);   // your Lab 7 solution
  }
}

int MailStatus;
uint32_t MailValue;
#define MS 80000
void SysTick_Handler(void){ // every 100 ms
  PF1 ^= 0x02;     // Heartbeat
  //*** students write this ******
// should call ADC_In() and Sensor.Save
}
// final main program to create distance meter
// this is the minimal complete solution to Lab8
int main(void){  
  DisableInterrupts();
  TExaS_Init();    // bus clock at 80 MHz
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  ADC_Init(SAC_32);  // turn on ADC, set channel to 5
 // 32-point averaging  
  PortF_Init();
  // more initializations
  EnableInterrupts();
  while(1){
    // call your Sensor.Sync(); // wait for semaphore
    // can call Sensor.ADCsample, Sensor.Distance, Sensor.Convert as needed    
  }
}

uint32_t Histogram[64]; // probability mass function
uint32_t Center;
// use main4 program to demonstrate CLT
int main4(void){ uint32_t i,d,sum,sac; 
  DisableInterrupts();
  TExaS_Init();    // bus clock at 80 MHz
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  ADC_Init(SAC_NONE);  // turn on ADC, set channel to 5
 // 32-point averaging  
  PortF_Init();
  while(1){
    for(sac=0; sac<7; sac++){
      ADC0_SAC_R = sac;               // 1 to 64 hardware averaging
      SSD1306_OutClear(); 
      sum = 0;
      for(i=0; i<100; i++){
        sum += ADC_In(); 
      }
      Center = sum/100; // centering allows us to zoom in
      LCD_OutDec(sac);
      for(i=0; i<128; i++) Histogram[i] = 0; // clear
      for(i=0; i<400; i++){
        Data = ADC_In();
        PF3 ^= 0x08;       // Heartbeat
        if(Data < (Center-32)){
          Histogram[0]++;
        }else if(Data >= (Center+32)){
          Histogram[63]++;
        }else{
          d = Data-Center+32;
          Histogram[d]++;
        }
        MailStatus = 0;    // mailbox is empty
      }
      SSD1306_SetPlot(0,127,0,149,SSD1306_WHITE);
      SSD1306_ClearBuffer();
      for(i=0; i<64; i++){
        if(Histogram[i] > 149) Histogram[i]=149;
        for(int j=0; j<Histogram[i];j++){
          SSD1306_DrawPoint(2*i,j);
          SSD1306_DrawPoint(2*i+1,j);
        }
      }
      SSD1306_OutBuffer();   // 30ms, update entire screen
      SSD1306_SetCursor(0,0);
      SSD1306_OutString((char*)"CLT sac="); LCD_OutDec(sac);
      while(PF4){}; // wait for touch
      SSD1306_OutClear();        
      while(PF4==0){}; // wait for release      
    }
  }
}
// main5 is graphing version
// sampling rate fs is about 4 Hz
// use main5 to study the Nyquist Theorem
// wiggle the slide pot f oscillations/sec
// how fast can you wiggle it and still see oscillations?
int main5(void){ uint32_t time; 
  DisableInterrupts();
  TExaS_Init();    // bus clock at 80 MHz
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  ADC_Init(SAC_32);  // turn on ADC, set channel to 5
 // 32-point averaging  
  PortF_Init();
  SSD1306_SetPlot(0,127,0,200,SSD1306_WHITE);
  SSD1306_ClearBuffer();
  time = 0;
  EnableInterrupts();
  while(1){
    Delay1ms(227);     // approximate 4 Hz sampling rate
    PF3 ^= 0x08;       // Heartbeat
    Data = ADC_In();
    Position = my.Convert(Data); 
    SSD1306_DrawPoint(time,Position);
    SSD1306_OutBuffer();   // 30ms, update entire screen
    time = time+1;
    if(time == 128){
      SSD1306_ClearBuffer();
      time = 0;
    }    
  }
}



