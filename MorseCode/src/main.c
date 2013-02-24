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

#include "morse.h"

#define BUFFER_LEN 5
#define MORSE_MAX_LEN 5

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP;

uint32_t pulse_buffer[2][MORSE_MAX_LEN] = { { 0 } };

uint32_t current_buffer = 0;
uint32_t buffer_pos = 0;
uint32_t between_chars = 1;

int rising_edge = 1;
void TIMER0_IRQHandler(void) {
  LPC_TIM0 ->TC = 0;      // begin timing next pulse

  if (LPC_TIM0 ->IR & 1) {
    // If this is from the match event, force swap the buffers
    LPC_TIM0 ->IR = 0xff;   // reset all interrrupts
    buffer_pos = 0;
    current_buffer ^= 1;
    between_chars = 1;
    return;
  }
  LPC_TIM0 ->IR = 0xff;   // reset all interrrupts

  uint32_t pulse_len = LPC_TIM0 ->CR0;
  // 120482 cycles is 5ms at 12mhz
  // This will be the minimum significant pulse length
  // (for debouncing)
  if (pulse_len > 30241) {
    if (rising_edge) {
      // Space between characters: (aka a long time)
      /*
       if (pulse_len > 7000000) {
       // flip buffer
       buffer_pos = 0;
       current_buffer ^= 1;
       }
       */
    } else if (buffer_pos < (BUFFER_LEN - 1)) {
      pulse_buffer[current_buffer][buffer_pos] = pulse_len;
      buffer_pos++;
    }
  }

  if (rising_edge)
    LPC_TIM0 ->MCR &= ~(1);
  else
    LPC_TIM0 ->MCR |= 1;

  rising_edge = !rising_edge;

  // Toggle between capturing rising edge and falling edge, such that the
  // rising_edge flag should always be accurate
  // If its possible to miss a pin event, then this will lead to ocassionally
  // registering extremely long pulses. However, if we miss a pin event we'll
  // probably have data corruption anyways...
  // This technique also has the very nice behavior of self-correcting itself
  // if it ever does manage to get out of sync, because the next interrupt
  // will only trigger when the expected condition is met.
  LPC_TIM0 ->CCR ^= _BV(0) | _BV(1);
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
  LPC_PINCON ->PINSEL3 |= (3 << 20);

  // Timer0 interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Configure timer one so that it constantly counts up, at the undivided
  // clock rate. When CAP0.0 goes high or low, capture value is stored,
  // timer value is reset and interrupt is triggered
  LPC_TIM0 ->MCR = _BV(0) | _BV(1); // interrupt and reset timer on match
  LPC_TIM0 ->TCR = 0x02;            // reset timer
  LPC_TIM0 ->PR = 0;               // No prescale
  LPC_TIM0 ->MR0 = 8000000;         // match value
  LPC_TIM0 ->IR = 0xff;            // reset all interrrupts

  // Capture Control Register
  //   bit 0 - Capture on CAP0.0 rising edge
  //   bit 1 - Capture on CAP0.0 falling edge (toggled in interrupt handler)
  //   bit 2 - Interrupt on CAP0.0 event
  LPC_TIM0 ->CCR = 1 | (1 << 2);

  // Enable timer
  LPC_TIM0 ->TCR = 1;

  // experimentally 'ok' guess for an average. Will be refined as we get data
  // from this run.
  uint32_t running_average = 2035614;

  uint32_t i, average, min, max, cur, dot_bound, dash_bound;
  uint32_t *decode_buffer;
  uint_fast8_t code, len, is_dash, all_same_len;
  while (1) {
    decode_buffer = pulse_buffer[current_buffer ^ 1];

    // If the buffer not being written to has a non-zero value in the first
    // entry, then it has been updated since we last checked
    if (decode_buffer[0]) {
      len = 0;
      max = 0;
      min = 0xFFFFFFFF;

      // First pass through the buffer, collecting min and max, which we will
      // will use for decoding the morse code
      for (i = 0; (i < MORSE_MAX_LEN) && (cur = decode_buffer[i]); ++i, ++len) {
        printf("%d  ", cur);
        if (cur < min)
          min = cur;
        if (cur > max)
          max = cur;
      }

      // Decode the buffer into 'code', as binary bits. 1 will be a dash, 0
      // is a dot. For example, '.-.' is 010; '-.--.' is 10110
      code = 0;

      // Morse code specifies dots as the basic unit of time, and a dash as
      // three dots.
      // If a dot is x long, and a dash is 3x long, then the average of the
      // minimum pulse length (a dot) and the maximum (a dash) will be
      // 2x. From this, we'll say pulses < 1.5x long are dots and
      // pulses > 2.5x long are dashes.
      average = (min + max) / 2;

      printf("\nMin: %d  max: %d  avg: %d  of cnt: %d \n", min, max, average,
          len);

      // The edge case for this algorithm is codes which are all dots or
      // all dashes. If this is the case, we can try to detect it. Based on
      // the same proportions, the difference between max and min in a sample
      // with both dots and dashes should be 3x - 1x = 2x; so roughly the same
      // as our average. I'll say that if the average is less than half of the
      // range (max-min), then all the dots are the same.
      all_same_len = average < ((max - min) / 2);
      if (all_same_len) {
        if (average >= running_average) {
          code = 0x1f;
        } // Implicit: else code = 0
      } else {
        dot_bound = average - average / 4;  // 2x - .5x = 1.5x
        dash_bound = average + average / 4; // 2x + .5x = 2.5x

        for (i = 0; i < len; ++i) {
          is_dash = decode_buffer[i] > average;

          printf("%c", is_dash ? '-' : '.');
          if (is_dash) {
            code |= (1 << i);
          }

          // Clear out decode buffer, so next loop can determine length.
          // In retrospect, the interrupt could also swap between two
          // length variables, but this solution works and keeps a little
          // processing out of the interrupt
          decode_buffer[i] = 0;
        }
      }

      char c = morse_decode(code, len);
      if (c) {
        printf("  =  %c \n", c);
      }
    }
  }
  return 0;
}
