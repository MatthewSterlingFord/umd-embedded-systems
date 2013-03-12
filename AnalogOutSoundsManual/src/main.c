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
#include "samples.h"
#include "util.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

volatile uint_fast16_t sound_pos = 0;
void TIMER0_IRQHandler(void) {
  // Clear interrupt flags, or else this interrupt will loop over and over
  LPC_TIM0->IR = 0xff;

  LPC_DAC->DACR = SOUND_CYMBAL[sound_pos];
  sound_pos++;
  if (sound_pos >= ARRAY_LEN(SOUND_CYMBAL)) {
    sound_pos = 0;
  }
}

int main(void) {
  // Clear interrupt flags, or else this interrupt will loop over and over
  LPC_TIM0->IR = 0xff;

  // Select 12MHz crystal oscillator
  LPC_SC ->CLKSRCSEL = 1;

  // PLL 0 Setup
  // M(multipler) value is 147
  // N(divider) value is 8
  // F_CCO = (2 * 147 * 12MHz)/8 = 441MHz
  LPC_SC ->PLL0CFG = (147 - 1) | ((8 - 1) << 16);

  // F_CCO = (2 * 96 * 12MHz)/6 = 384MHz
  //LPC_SC ->PLL0CFG = (96 - 1) | ((6 - 1) << 16);

  // Enable PLL0
  LPC_SC ->PLL0CON = 0x01;

  LPC_SC ->PLL0FEED = 0xAA;
  LPC_SC ->PLL0FEED = 0x55;

  // Wait for PLL0 Lock
  while (!(LPC_SC ->PLL0STAT & (1 << 26)))
    ;

  // Connect PLL0
  LPC_SC ->PLL0CON = 0x03;

  LPC_SC ->PLL0FEED = 0xAA;
  LPC_SC ->PLL0FEED = 0x55;

  // Set clock divider to 12, so final running clock speed will be:
  // (441 / 100) = 4.41MHz
  LPC_SC ->CCLKCFG = (100 - 1);

  // Peripheral power
  //   bit 0: TIMER0
  //   (DAC always powered)
  LPC_SC ->PCONP |= _BV(0);

  // Choose undivided peripheral clock for:
  //    TIMER 0 (bits 3:2)
  //    DAC (Bits 23:22)
  LPC_SC ->PCLKSEL0 |= (1 << 2) | (1 << 24);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = _BV(22);
  LPC_GPIO0 ->FIOSET = _BV(22);

  // Configure pins
  //   P0.26 as AOUT  (2 at bit 20)
  LPC_PINCON ->PINSEL1 |= (2 << 20);

  // Disable timer
  LPC_TIM0 ->TCR = 0x02;

  // No prescaling
  LPC_TIM0 ->PR = 0;

  // Match value
  //  4.41MHz / 44.1khz = 100
  LPC_TIM0 ->MR0 = 99;

  // Reset all interrupts (though we aren't using them in this example)
  LPC_TIM0 ->IR = 0xff;

  // Match Control Register
  //   1 in bit 0 - timer will trigger the interrupt when it matches
  //   1 in bit 1 - timer will automatically reset to 0 when it matches
  LPC_TIM0 ->MCR = (1 << 0) | (1 << 1);

  // Start timer
  LPC_TIM0 ->TCR = 1;

  NVIC_EnableIRQ(TIMER0_IRQn);

  volatile int i = 0;
  for (;;) {
    if (i >= 100000) {
      LPC_GPIO0 ->FIOPIN ^= (1 << 22);
      i = 0;
    }
    ++i;
  }
  return 0;
}
