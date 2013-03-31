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
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

void TIMER0_IRQHandler(void) {
  // Flip LED pin
  LPC_GPIO0 ->FIOPIN ^= (1 << 22);

  // Clear interrupt flags, or else this interrupt will loop over and over
  LPC_TIM0->IR = 0xff;
}

int main(void) {
  // Setup input clock source
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set cpu clock divider to divide by 1
  // (divides by value set here + 1)
  LPC_SC ->CCLKCFG = 0;

  // Set bits 2 and 3 of PCLKSEL0 to choose peripheral divider for TIMER0
  // Setting to 1 chooses no divider
  LPC_SC ->PCLKSEL0 = (1 << 2);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = (1 << 22);
  LPC_GPIO0 ->FIOSET = (1 << 22);

  // Setup timer
  LPC_SC ->PCONP |= 1;		 // power for timer

  LPC_TIM0 ->PR = 0;

  // Capture 0 and 1 on rising edge, and enable interrupt
  
// This is for input capture. It is not need for this projects LED blink 
  // LPC_TIM0 ->CCR = (0x1 << 0) | (0x1 << 2) | (0x1 << 3) | (0x1 << 5);
// This is done below as well in cleaner way. Not needed twice.  
  // LPC_TIM0 ->MCR = (0x3 << 0);			 // Interrupt and Reset on MR0 and MR1

  // Disable timer
  LPC_TIM0 ->TCR = 0x02;

  // No prescaling
  LPC_TIM0 ->PR = 0;

  // Match value
  LPC_TIM0 ->MR0 = 12000000;

  // Reset all interrupts (though we aren't using them in this example)
  LPC_TIM0 ->IR = 0xff;

  // Match Control Register
  //   1 in bit 0 - timer will trigger the interrupt when it matches
  //   1 in bit 1 - timer will automatically reset to 0 when it matches
  LPC_TIM0 ->MCR = (1 << 0) | (1 << 1);

  // Start timer
  LPC_TIM0 ->TCR = 1;

  NVIC_EnableIRQ(TIMER0_IRQn);

  while (1)
    ;
  return 0;
}
