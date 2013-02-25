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

#include "main.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

int rising_edge = 1;
void TIMER0_IRQHandler(void) {
  LPC_GPIO0 ->FIOPIN ^= (1 << 22);
  LPC_TIM0 ->IR = 0xff;           // reset all interrrupts
}

int main(void) {
  // Select 12MHz crystal oscillator
  LPC_SC ->CLKSRCSEL = 1;

  // PLL 0 Setup
  // M(multipler) value is 96 (so we write 95)
  // N(divider) value is 6 (so we write 5)
  // This will yield: F_CCO = (2 * 96 * 12MHz)/6 = 384MHz
  LPC_SC ->PLL0CFG = (96 - 1) | ((6 - 1) << 16);

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
  // (384 / 12) = 32MHz
  LPC_SC ->CCLKCFG = (12 - 1);

  // PLL1 Setup (for USB) at 48MHz
  // Enable PLL1
  LPC_SC ->PLL1CFG = (3) /* M val; 48/12 = 4, 4-1 = 3 */
                   | (1) /* P val; */;

  LPC_SC ->PLL1CON = _BV(0);

  // Feed PLL1
  LPC_SC ->PLL1FEED = 0xAA;
  LPC_SC ->PLL1FEED = 0x55;

  // Wait for PLL1 Lock
  while (!(LPC_SC ->PLL1STAT & _BV(10)))
    ;

  // Connect PLL1
  LPC_SC ->PLL1CON = _BV(0) | _BV(1);

  // Feed PLL1
  LPC_SC ->PLL1FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL1FEED = 0x55; // set to 0x55

  // Power USB, TIMER0
  LPC_SC ->PCONP |= _BV(1) | _BV(31);

  // USB Setup


  // Choose undivided peripheral clocks for TIMER0
  LPC_SC ->PCLKSEL0 |= (1 << 2);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = (1 << 22);
  LPC_GPIO0 ->FIOSET = (1 << 22);

  // Timer0 interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Configure timer one so that it constantly counts up, at the undivided
  // clock rate. When CAP0.0 goes high or low, capture value is stored and
  // timer value is reset and interrupt is triggered
  LPC_TIM0 ->TCR = 0x02;           // reset timer
  LPC_TIM0 ->MCR = _BV(1) | _BV(0);
  LPC_TIM0 ->TC = 0;
  LPC_TIM0 ->PR = 0;             // No prescale
  LPC_TIM0 ->MR0 = 1000000;     // match value (unnecessary)
  LPC_TIM0 ->IR = 0xff;           // reset all interrrupts
  LPC_TIM0 ->TCR = 1;              // enable timer

  while (1)
    ;
  return 0;
}
