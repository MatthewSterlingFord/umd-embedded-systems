/*
 ===============================================================================
 Name        : main.c
 Author      : George Pittarelli
 Version     :
 Description : Uses the capture feature of TIMER0 (using P1.26) to decode morse
               code, which is printed to the debug console.
               Requires semi-hosted library to print output.
 ===============================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define _BV(n) (1 << (n))

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Must be power of two
#define BUFFER_LEN 1024

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

uint32_t pulse_buffer[BUFFER_LEN] = {0};
uint32_t buffer_pos = 0;

int rising_edge = 1;
void TIMER0_IRQHandler(void) {
  pulse_buffer[buffer_pos] = LPC_TIM0 ->IR;
  LPC_TIM0 ->IR = 0xff;
  buffer_pos++;

  // truncate buffer_pos to 10bits
  buffer_pos &= (BUFFER_LEN - 1);
}

int main(void) {
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set clock divider to 0+1=1
  LPC_SC ->CCLKCFG = 0;

  // Choose undivided peripheral clocks for TIMER0
  LPC_SC ->PCLKSEL0 |= _BV(2);

  // Power TIMER0
  LPC_SC ->PCONP |= 1;

  // Configure pins
  //   P1.26 as CAP0.0 (TIMER0 capture pin 0)
  LPC_PINCON ->PINSEL3 |= (3 << 20) | (3 << 22);

  // Timer0 interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Configure timer one so that it constantly counts up, at the undivided
  // clock rate. When CAP0.0 goes high or low, capture value is stored,
  // timer value is reset and interrupt is triggered
  LPC_TIM0 ->TCR = 0x02;            // reset timer
  LPC_TIM0 ->PR  = 0;               // No prescale
  LPC_TIM0 ->MR0 = 0xffffffff;      // match value
  LPC_TIM0 ->IR  = 0xff;            // reset all interrrupts

  // Capture Control Register
  //   bit 0 - Capture on CAP0.0 rising edge
  //   bit 2 - Interrupt on CAP0.0 event
  //   bit 4 - Capture on CAP0.1 falling edge
  //   bit 5 - Interrupt on CAP0.1 event
  LPC_TIM0 ->CCR = _BV(0) | _BV(2) | _BV(4) | _BV(5);

  // Enable timer
  LPC_TIM0 ->TCR = 1;

  uint32_t have_read_upto = 0, buffer_pos_snapshot,
           will_read_upto = 0, wrapped = 0;
  while (1) {
    // Treat the buffer as two regions, [0, BUFFER_LEN/2) and
    // [BUFFER_LEN/2, BUFFER_LEN). (All this to avoid an extra if statement
    // in the interrupt handler...)
    buffer_pos_snapshot = buffer_pos;

    wrapped = 0;
    if (have_read_upto < buffer_pos_snapshot) {
      will_read_upto = buffer_pos_snapshot;
    } else if (have_read_upto > buffer_pos_snapshot) {
      // buffer_pos wrapped around
      will_read_upto = BUFFER_LEN - 1;
      wrapped = 1;
    }

    while (have_read_upto < will_read_upto) {
      // pulse buffer is full of the interrupt register values. If bit 4 was
      // set, it was an event on CAP0.0 (so rising edge), else it was an
      // event on CAP0.1 (so a falling edge)
      // Output 1 for rising edge, 0 for falling
      printf("%d", (pulse_buffer[have_read_upto] & _BV(4))? 1:0);
      have_read_upto++;
    }

    if (wrapped) {
      have_read_upto = 0;
    }
  }
  return 0;
}
