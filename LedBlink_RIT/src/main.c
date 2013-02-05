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

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

void RIT_IRQHandler(void) {
	LPC_GPIO0->FIOPIN ^= (1 << 22);
   // Clear RI Control register Interrupt
   LPC_RIT->RICTRL |= 1<<0; //bit 0 = 1 => write 0 to value 1 will clear the interrupt flag
}

int main(void) {
	LPC_GPIO0->FIODIR = (1 << 22);
	LPC_GPIO0->FIOSET = (1 << 22);

	LPC_SC->PCONP |= 1 << 16;  // power
	LPC_SC->PCLKSEL0 &= ~(2 << 26);
	LPC_SC->PCLKSEL1 |= 1 << 26;
	LPC_RIT->RICOMPVAL = SystemCoreClock / 2;
	LPC_RIT->RIMASK = 0;
	LPC_RIT->RICOUNTER = 0;

	//bit 1 = 1 => The timer will be cleared to 0 whenever the counter value equals RICOMPVAL
	//bit 3 = 1 => Timer enable
	LPC_RIT->RICTRL = (1 << 1) | (1 << 3);

	NVIC_EnableIRQ(RIT_IRQn);

	// Enter an infinite loop, just incrementing a counter
	while(1);
	return 0 ;
}
