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

uint_fast16_t analog_val;
void ADC_IRQHandler(void) {
  if (LPC_ADC ->ADDR0 & _BV(31)) {
    //analog_val = (LPC_ADC ->ADDR0 >> 4) & 0x0fff;
    //alpha_display(HEX_CHAR_TABLE[analog_val >> 8]);

    // Print result
    analog_val = (LPC_ADC ->ADDR0 >> 4) & 0x0fff;
    if ((CoreDebug ->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
      printf("0: %#X  %d\n", analog_val, analog_val);
    }
  } else if (LPC_ADC ->ADDR1 & _BV(31)) {
    analog_val = (LPC_ADC ->ADDR1 >> 4) & 0x0fff;
    alpha_display(HEX_CHAR_TABLE[analog_val >> 8]);
    if ((CoreDebug ->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
      printf("1: %#X  %d\n", analog_val, analog_val);
    }
  }
}

int main(void) {
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set clock divider to 0+1=1
  LPC_SC ->CCLKCFG = 0;

  // Power PWM, ADC
  LPC_SC ->PCONP |= _BV(6) | _BV(12);

  // Choose undivided peripheral clocks for TIMER0,1
  //LPC_SC ->PCLKSEL0 |= (0 << 24);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = 0;
  LPC_GPIO0 ->FIOSET = 0;
  alpha_init();

  // Configure pins
  //   P0.23 as AD0.0
  //   P0.24 as AD0.1
  LPC_PINCON ->PINSEL1 |= (1 << 14) | (1 << 16);

  // A/D Control Register
  //  1 in bit 0 - Select AD0.0 to be sampled
  //  1 in bit 1 - Select AD0.1 to be sampled
  //  0 in bits 15:8 - Don't divide clock
  //  0 in bit 16 - Disable burst mode
  //  1 in bit 21 - Enabled; not in power-down mode
  //  0 in bits 26:24 - don't start a conversion yet
  LPC_ADC ->ADCR = _BV(0) | _BV(1) | _BV(21);

  // A/D Interrupt Enable Register
  //  1 in bit 0 - Interrupt when conversion on ADC channel 0 completes
  //  1 in bit 1 - Interrupt when conversion on ADC channel 1 completes
  LPC_ADC ->ADINTEN = _BV(0) | _BV(1);

  NVIC_EnableIRQ(ADC_IRQn);

  // Start burst mode
  LPC_ADC ->ADCR |= _BV(16);

  while (1)
    ;
  return 0;
}
