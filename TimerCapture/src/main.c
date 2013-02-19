/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Some macros to speed bit twiddling
#define BITON(var,bit) var |= (1 << bit)
#define BITOFF(var,bit) var &= ~(1 << bit)
#define BITTOG(var,bit) var ^= (1 << bit)

// MASK(3,4) = 0x78
#define MASK(start,len) (~(~0 << (len)) << start)
#define INVMASK(start,len) (~MASK(start,len))

// SETBITS assumes the variable is r/w capable
#define SETBITS(var,start,len,val) var = (((var) & INVMASK(start,len)) | (((val) << start) & MASK(start,len)))

// Everything not in range is set to 0 (possibly painful if you overwrite lesser bits)
#define SETBITS0(var,start,len,val) var = ((val) << start) & MASK(start,len)

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

int rising_edge = 1;
void TIMER0_IRQHandler(void) {
    LPC_TIM0->TC = 0;

    // 120482 cycles is 10ms at 12mhz
    if (!rising_edge && LPC_TIM0->CR0 > 120482) {
    	LPC_GPIO0->FIOPIN ^= (1 << 22);

    	// Play with Timer1 to reset the out pin
    	LPC_TIM1->MR0 = 0; // match value
    	LPC_TIM1->EMR = 1 << 4;
    	LPC_TIM1->TCR = 1;

		// make sure it has a clock cycle to reset
		__NOP();

		// Reconfigure emr to
		LPC_TIM1->TCR = 0x02;          // reset timer
		LPC_TIM1->EMR = 2 << 4;
		LPC_TIM1->MR0 = LPC_TIM0->CR0; // match value
		LPC_TIM1->TCR = 1;
    }

    rising_edge = !rising_edge;
    LPC_TIM0->IR  = 0xff;           // reset all interrrupts
}

int main(void) {
	LPC_SC->CLKSRCSEL = 1;   // Select main clock source
	LPC_SC->PLL0CON = 0;     // Bypass PLL0, use clock source directly

	// Feed the PLL register so the PLL0CON value goes into effect
	LPC_SC->PLL0FEED = 0xAA; // set to 0xAA
	LPC_SC->PLL0FEED = 0x55; // set to 0x55

	// Set clock divider to 0+1=1
	LPC_SC->CCLKCFG = 0;

	// Choose undivided peripheral clocks for TIMER0,1
	LPC_SC->PCLKSEL0 |= (1 << 2);
	LPC_SC->PCLKSEL0 |= (1 << 4);

	// Power TIMER0, TIMER1
	LPC_SC->PCONP |= 3;

	// Bring MAT1.0 low on timer1 match
	LPC_TIM1->EMR = 2 << 4;

	// Setup IO pins
	LPC_GPIO0->FIODIR = (1 << 22);
	LPC_GPIO0->FIOSET = (1 << 22);

	// Configure pins
	//   P1.26 as CAP0.0 (TIMER0 capture pin 0)
	//   P1.27 as CAP0.1 (TIMER0 capture pin 1)
	//   P1.22 as MAT1.0
	LPC_PINCON->PINSEL3 |= (3 << 20) | (3 << 22)
			             | (3 << 12);

	// Timer0 interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Configure timer one so that it constantly counts up, at the undivided
	// clock rate. When CAP0.0 goes high or low, capture value is stored and
	// timer value is reset and interrupt is triggered
	LPC_TIM0->MCR = 1 << 1;
    LPC_TIM0->TCR = 0x02;           // reset timer
    LPC_TIM0->PR  = 1;             // No prescale
    LPC_TIM0->MR0 = 0xffffffff;     // match value (unnecessary)
    LPC_TIM0->IR  = 0xff;           // reset all interrrupts
	LPC_TIM0->TCR = 1;              // enable timer

	// Capture Control Register
	//   bit 0 - Capture on CAP0.0 rising edge
	//   bit 1 - Capture on CAP0.0 falling edge
	//   bit 2 - Interrupt on CAP0.0 event
	LPC_TIM0->CCR = 1 | (1 << 1) | (1 << 2);

	// Timer 1, which controls led.
    LPC_TIM1->PR  = 1;          // Prescale
    LPC_TIM1->TC  = 0;          //  counter

	while(1);
	return 0;
}
