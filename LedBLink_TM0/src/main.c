/*
	===============================================================================
	Name				 : main.c
	Author			 :
	Version		 :
	Copyright	 : Copyright (C)
	Description : main definition
	===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// TODO: insert other include files here

int main(void) {
	// Setup input clock source
	LPC_SC->CLKSRCSEL = 1;   // Select main clock source
	LPC_SC->PLL0CON = 0;     // Bypass PLL0, use clock source directly

	// Feed the PLL register so the PLL0CON value goes into effect
	LPC_SC->PLL0FEED = 0xAA; // set to 0xAA
	LPC_SC->PLL0FEED = 0x55; // set to 0x55

	// Set cpu clock divider to divide by 1
	// (divides by value set here + 1)
	LPC_SC->CCLKCFG = 0;

	// Set bits 2 and 3 of PCLKSEL0 to choose peripheral divider for TIMER0
	// Setting to 1 chooses no divider
	LPC_SC->PCLKSEL0 = (1 << 2);

	// Setup IO pins
	LPC_GPIO0->FIODIR = (1 << 22);
	LPC_GPIO0->FIOSET = (1 << 22);

	// Setup timer
	LPC_SC->PCONP |= 1;		 // power for timer

	LPC_TIM0->PR = 0;

	// Capture 0 and 1 on rising edge, and enable interrupt
	LPC_TIM0->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
	LPC_TIM0->MCR = (0x3<<0);			 // Interrupt and Reset on MR0 and MR1

	LPC_TIM0->TCR = 0x02;						// reset timer
	LPC_TIM0->PR	= 1;
	LPC_TIM0->MR0 = 12000000;				// match value
	LPC_TIM0->IR	= 0xff;						// reset all interrrupts
	LPC_TIM0->MCR = 0x04;						// stop timer on match
	LPC_TIM0->TCR = 1;							// enable timer

	while(1) {
		if (!(LPC_TIM0->TCR & 0x01)) {
			LPC_GPIO0->FIOPIN ^= (1 << 22);
			LPC_TIM0->TCR = 0x02;						// reset timer
			LPC_TIM0->TCR = 1;				// enable timer
		}
	}
	return 0 ;
}
