// SlidePot.cpp
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 1/17/2021 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "SlidePot.h"
#include "../inc/tm4c123gh6pm.h"
// feel free to redesign this, as long as there is a class

// ADC initialization function 
// Input: sac sets hardware averaging
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(uint32_t sac){ 
  SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x08;    // 2) activate clock for Port D
	__nop();
	__nop();
//  while((SYSCTL_PRGPIO_R&0x10) == 0);  // 3 for stabilization
  GPIO_PORTD_DIR_R &= ~0x04;    // 4) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04;   // 5) enable alternate function on PD2
  GPIO_PORTD_DEN_R &= ~0x04;    // 6) disable digital I/O on PD2
  GPIO_PORTD_AMSEL_R |= 0x04;   // 7) enable analog functionality on PD2
  ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 9;           // 12) set channel
  ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
	ADC0_SAC_R = sac;
}

//------------ADCIn------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
	uint32_t result;
  ADC0_PSSI_R = 0x0008;
    while((ADC0_RIS_R&0x08) == 0);
    result = ADC0_SSFIFO3_R&0xFFF;
    ADC0_ISC_R = 0x0008;
    return result;
}

// constructor, invoked on creation of class
// m and b are linear calibration coefficents 
SlidePot::SlidePot(uint32_t m, uint32_t b){
//*** students write this ******
// initialize all private variables
// make slope equal to m and offset equal to b
	slope = m;
	offset = b;
}

void SlidePot::Save(uint32_t n){
//*** students write this ******
// 1) save ADC sample into private variable
// 2) calculate distance from ADC, save into private variable
// 3) set semaphore flag = 1
	data = n;
	distance = Convert(data);
	flag = 1;
}
//**********place your calibration data here*************
// distance PD2       ADC  fixed point
// 0.00cm   0.000V     0        0
// 0.50cm   0.825V  1024      500
// 1.00cm   1.650V  2048     1000
// 1.50cm   2.475V  3072     1500  
// 2.00cm   3.300V  4095     2000 
uint32_t SlidePot::Convert(uint32_t n){
  //*** students write this ******
  // use calibration data to convert ADC sample to distance
  return (slope * data + offset)/4096;
}

void SlidePot::Sync(void){
//*** students write this ******
		while(!flag){}			// 1) wait for semaphore flag to be nonzero
		flag = 0;						// 2) set semaphore flag to 0
}

uint32_t SlidePot::ADCsample(void){ // return ADC sample value (0 to 4095)
  //*** students write this ******
  // return last calculated ADC sample
  return data;
}

uint32_t SlidePot::Distance(void){  // return distance value (0 to 200), 0.01cm
  //*** students write this ******
  // return last calculated distance in 0.01cm
  return distance;
}


