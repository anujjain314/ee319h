// UART1.cpp
// Runs on TM4C123

// EE319H tasks
// 1) Initialize UART1 on PC4 PC5
// 2) Implement the FIFO as a class
// 3) Run transmitter with busy-wait synchronization
// 4) Run receiver with 1/2 full FIFO interrupt
// Daniel and Jonathan Valvano
// 1/12/2021

// EE319K Lab 9, use U1Rx connected to PC4 interrupt
// EE319K Lab 9, use U1Tx connected to PC5 busy wait
// EE319K Lab 9 hardware
// System 1        System 2
//   PC4 ----<<----- PC5
//   PC5 ---->>----- PC4
//   Gnd ----------- Gnd
// When running on one board connect PC4 to PC5

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "FIFO.h"
#include "UART1.h"

#define PF1  (*((volatile uint32_t *)0x40025008))

extern "C" void DisableInterrupts(void); // Disable interrupts
extern "C" long StartCritical (void);    // previous I bit, disable interrupts
extern "C" void EndCritical(long sr);    // restore I bit to previous value
extern "C" void WaitForInterrupt(void);  // low power mode
extern "C" void EnableInterrupts(void);

extern "C" void UART1_Handler(void);
#define NVIC_EN0_INT6   0x00000040  // Interrupt 6 enable
Queue RxFifo;
// Initialize UART1
// Baud rate is 1000 bits/sec
// Lab 9
void UART1_Init(void){
  SYSCTL_RCGCUART_R |= 0x00000002;  // activate UART1
  SYSCTL_RCGCGPIO_R |= 0x00000004;  // activate port C
  UART1_CTL_R &= ~0x00000001;    		// disable UART
  UART1_IBRD_R = 5000;     					// IBRD = int(80,000,000/(16*1000)) = int(5000)
  UART1_FBRD_R = 0;     						// FBRD = round(0 * 64) = 0
  UART1_LCRH_R = 0x00000070;  			// 8 bit, no parity bits, one stop, FIFOs
	
	UART1_IFLS_R &= ~0x3F;
	UART1_IFLS_R += UART_IFLS_RX4_8;  // set interrupt to occur when FIFO 1/2 full
	UART1_IM_R = UART_IM_RXIM;				// enable receiver interrupt
	
  UART1_CTL_R |= 0x00000001;     		// enable UART
  GPIO_PORTC_AFSEL_R |= 0x30;    		// enable alt funct on PC5-4
  GPIO_PORTC_DEN_R |= 0x30;      		// configure PC5-4 as UART1
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;   		// disable analog on PC5-4
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF00FF) | 0x00004000;
	NVIC_EN0_R = NVIC_EN0_INT6;
	EnableInterrupts();
}


// input ASCII character from UART
// spin if software RxFifo is empty
// Lab 9
char UART1_InChar(void){
	while((UART1_FR_R&0x0010) != 0);      // wait until RXFE is 0
  return((char)(UART1_DR_R&0xFF));
}
// Lab 9
// check software RxFifo
bool UART1_InStatus(void){
  // write this
	if (RxFifo.IsEmpty()) return false;
	return true;
}

// output ASCII character to UART
// busy wait
// spin if hardware not ready
// Lab 9
// in Lab 9 this will never wait
void UART1_OutChar(char data){
  while((UART1_FR_R&0x0020) != 0);      // wait until TXFF is 0
  UART1_DR_R = data;
}

// one of two things has happened:
// 1) hardware RX FIFO goes from 7 to 8 or more items
// 2) receiver timeout (data in hardware Rx but no activity)
// Lab 9
uint32_t errorCount = 0;
void UART1_Handler(void){
	static uint32_t RxCounter = 0;
  PF1  ^= 0x02; // single toggle debugging
    // write this
	if (RxFifo.IsFull()) errorCount++;
	while ((UART1_FR_R&0x0010) == 0) {
		char data = UART1_InChar();
		RxFifo.Put(data);
		errorCount++;
	}
	RxCounter++;
	UART1_ICR_R = 0x10; // this clears bit 4 (RXRIS) in the RIS register
}

//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until ETX is typed
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
// removes STX CR ETX
void UART1_InMessage(char *bufPt){
uint8_t index = 0;
char data = UART1_InChar();
        while(data != 0x03 && index < 8){            // keep getting data until string is full or ETX is received
            if(data != 0x02 && data != 0x0D){        // filter out STX and CR
                bufPt[index] = data;                            // add character to string
                index++;
            }
            data = UART1_InChar();
    }
}
