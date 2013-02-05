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
void TIMER0_interrupt(void) {
	LPC_GPIO0->FIOPIN ^= (1 << 22);
}

int main(void) {
	LPC_GPIO0->FIODIR = (1 << 22);
	LPC_GPIO0->FIOSET = (1 << 22);


	// Setup timer
	LPC_SC->PCONP |= 1;  // power
    LPC_PINCON->PINSEL3 &= ~((0x3<<20)|(0x3<<22));
    LPC_PINCON->PINSEL3 |= ((0x3<<20)|(0x3<<22));
	// LPC_SC->PCLKSEL0 = 2; // select clock (default 0)

    uint8_t pclk;
    uint32_t pclkdiv = (LPC_SC->PCLKSEL1 >> 14) & 0x03;

    switch ( pclkdiv )
    {
      case 0x00:
      default:
            pclk = 4;
            break;
      case 0x01:
            pclk = 1;
            break;
      case 0x02:
            pclk = 2;
            break;
      case 0x03:
            pclk = 8;
            break;
    }

    LPC_TIM0->PR  = 0;

    LPC_TIM0->MR0 = 1/4;

    /* Capture 0 and 1 on rising edge, interrupt enable. */
    LPC_TIM0->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
    LPC_TIM0->MCR = (0x3<<0);      /* Interrupt and Reset on MR0 and MR1 */

    NVIC_EnableIRQ(TIMER0_IRQn);

    LPC_TIM0->TCR = 0x02;           /* reset timer */
    LPC_TIM0->PR  = (SystemCoreClock / (pclk * 1000));
    LPC_TIM0->MR0 = 100;  //enter delay time
    LPC_TIM0->IR  = 0xff;           /* reset all interrrupts */
    LPC_TIM0->MCR = 0x04;           /* stop timer on match */
	LPC_TIM0->TCR = 1;  // enable
	// Enter an infinite loop, just incrementing a counter
	volatile static int i = 0 ;
	while(1) {
		i++;
		if (i >= 300000) {
			i = 0;
		}
		if (!(LPC_TIM0->TCR & 0x01)) {
			LPC_GPIO0->FIOPIN ^= (1 << 22);
		    LPC_TIM0->TCR = 0x02;           /* reset timer */
		    LPC_TIM0->IR  = 0xff;           /* reset all interrrupts */
			LPC_TIM0->TCR = 1;  // enable
		}
	}
	return 0 ;
}
