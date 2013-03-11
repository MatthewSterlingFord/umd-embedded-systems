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

#include "main.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

uint_fast16_t analog_val;
void ADC_IRQHandler(void) {
  /*
  analog_val = (LPC_ADC ->ADDR0 >> 4) & 0x0fff;

  // ADC is 12 bits, but DAC is only 10, so chop off the bottom 2 bits
  analog_val >>= 2;

  // Write analog_val to DAC
  LPC_DAC ->DACR = ((LPC_DAC ->DACR) & ~(0x3ff << 6)) | (analog_val << 6);
  */
  // Equivalent to above:
  analog_val = (LPC_ADC ->ADDR0 >> 6) & 0x03ff;

  //asm volatile("bfc.w r2, #6, #10");

  //LPC_DAC ->DACR &= ~(0x3ff << 6);
  LPC_DAC ->DACR = (analog_val << 6);

  // Start next conversion
  LPC_ADC ->ADCR |= _BV(24);
}

int main(void) {
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set clock divider to 0 + 1=1
  LPC_SC ->CCLKCFG = 0;

  // Power ADC (DAC always powered)
  LPC_SC ->PCONP |= _BV(12);

  // Choose undivided peripheral clock for ADC, DAC
  LPC_SC ->PCLKSEL0 |= (1 << 22) | (1 << 24);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = 0;
  LPC_GPIO0 ->FIOSET = 0;

  // Configure pins
  //   P0.23 as AD0.0 (1 at bit 14)
  //   P0.26 as AOUT  (2 at bit 20)
  LPC_PINCON ->PINSEL1 |= (1 << 14) | (2 << 20);

  // A/D Control Register
  //  1 in bit 0 - Select AD0.0 to be sampled
  //  0 in bits 15:8 - Don't divide clock
  //  0 in bit 16 - Disable burst mode
  //  1 in bit 21 - Not in power-down mode
  //  0 in bits 26:24 - don't start a conversion yet
  LPC_ADC ->ADCR = _BV(0) | _BV(21);

  // A/D Interrupt Enable Register
  //  1 in bit 0 - Interrupt when conversion on ADC channel 0 completes
  LPC_ADC ->ADINTEN = _BV(0);

  LPC_DAC ->DACCTRL = 0;
  //LPC_DAC ->DACCNTVAL = 0;

  NVIC_EnableIRQ(ADC_IRQn);

  // Start conversion
  LPC_ADC ->ADCR |= _BV(24);

  while (1)
    ;
  return 0;
}
