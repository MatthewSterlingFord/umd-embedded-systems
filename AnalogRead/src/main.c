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
#include <stdio.h>

#include "alphanumeric.h"
#include "main.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

const char *HEX_CHAR_TABLE = "0123456789ABCDEF";

int main(void) {
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set clock divider to 0+1=1
  LPC_SC ->CCLKCFG = 0;

  // Power ADC
  LPC_SC ->PCONP |= _BV(12);

  // Choose undivided peripheral clocks for TIMER0,1
  LPC_SC ->PCLKSEL0 |= (0 << 24);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = 0;
  LPC_GPIO0 ->FIOSET = 0;

  // Configure pins
  //   P0.23 as AD0.0
  LPC_PINCON ->PINSEL1 |= (1 << 14);

  // A/D Control Register
  //  1 in bit 0 - Select AD0.0 to be sampled
  //  0 in bits 15:8 - Don't divide clock
  //  0 in bit 16 - Disable burst mode
  //  1 in bit 21 - Not in power-down mode
  //  0 in bits 26:24 - don't start a conversion yet
  LPC_ADC ->ADCR = _BV(0) | _BV(21);

  // A/D Interrupt Enable Register (all disabled)
  LPC_ADC ->ADINTEN = 0;

  uint_fast16_t analog_val;

  alpha_init();
  while (1) {
    // Start conversion
    LPC_ADC ->ADCR |= _BV(24);

    // Wait for conversion to finish
    // status bit automatically is cleared once it is
    // read as 1
    while (!(LPC_ADC ->ADDR0 & _BV(31)))
      ;

    analog_val = (LPC_ADC ->ADDR0 >> 4) & 0x0fff;
    alpha_display(HEX_CHAR_TABLE[analog_val >> 8]);

    // Print result
    if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
      printf("%#X  %d\n", analog_val, analog_val);
    }
  }

  return 0;
}
