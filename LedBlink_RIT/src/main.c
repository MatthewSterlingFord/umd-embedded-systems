/*
===============================================================================
 Name        : main.c
 Author      : George Pittarelli
 Description : Example of blinking an LED with the Repetitive Interrupt Timer
              (RIT).
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

void RIT_IRQHandler(void) {
	// Toggle LED pin
	LPC_GPIO0->FIOPIN ^= 1 << 22;

	// Clear RI Control register Interrupt
	//   write 1 to bit 0 will clear the interrupt flag
	LPC_RIT->RICTRL |= 1<<0;
}

int main(void) {
	// Setup LED pin (using builtin LED on pin P0.22)
	LPC_GPIO0->FIODIR = (1 << 22);
	LPC_GPIO0->FIOSET = (1 << 22);

	// Power for RIT clock
	LPC_SC->PCONP |= 1 << 16;

	// Choose clock source for RIT
	// Writing `01` to bits 27:26 selects the CPU Clock, undivided
	LPC_SC->PCLKSEL1 |= 1 << 26;

	LPC_RIT->RICOMPVAL = SystemCoreClock / 2;

	// Initialize RIT mask and counter (probably redundant)
	LPC_RIT->RIMASK = 0;
	LPC_RIT->RICOUNTER = 0;

	// RIT control flags:
	//  bit 1, the timer will be cleared to 0 whenever it equals RICOMPVAL
	//  bit 3, enables timer
	LPC_RIT->RICTRL = (1 << 1) | (1 << 3);

	// Enable interrupt
	NVIC_EnableIRQ(RIT_IRQn);

	while(1);
	return 0 ;
}
