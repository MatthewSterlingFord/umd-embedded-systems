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

uint16_t audio_buffer[AUDIO_SAMPLE_COUNT] = {}

int main(void) {
  // Select 12MHz crystal oscillator
  LPC_SC ->CLKSRCSEL = 1;

  // PLL 0 Setup
  // M(multipler) value is 147
  // N(divider) value is 8
  // F_CCO = (2 * 147 * 12MHz)/8 = 441MHz
  LPC_SC ->PLL0CFG = (147 - 1) | ((8 - 1) << 16);

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
  //  Power ADC (bit 12)
  //  (DAC always powered)
  //  GPDMA (bit 29)
  LPC_SC ->PCONP |= _BV(12) | _BV(29);

  // Choose undivided peripheral clock for:
  //                    ADC (CLK), DAC (CLK)
  LPC_SC ->PCLKSEL0 |= (1 << 22) | (1 << 24);

  // Setup IO pins (not used currently, handy to keep around)
  LPC_GPIO0 ->FIODIR = _BV(22);
  LPC_GPIO0 ->FIOSET = _BV(22);

  // Configure pins
  //   P0.23 as AD0.0 (1 at bit 14)
  //   P0.26 as AOUT  (2 at bit 20)
  LPC_PINCON ->PINSEL1 |= (1 << 14) | (2 << 20);

  // DMA Configuration register
  //   Enable DMA (1 at bit 0)
  //   Little-endian (default, 0 at bit 1)
  LPC_GPDMA->DMACConfig = _BV(0);

  // DAC Control Register
  //  Enable counter (1 at bit 2)
  //  Enable DMA (1 at bit 3)
  LPC_DAC->DACCTRL = _BV(2) | _BV(3);

  // DAC Counter Value
  //  We're running at 4.41MHz, so to update at 44.1khz, request a new value
  //  every 4,410,000 / 44,100 = 100 cycles
  // However, doing halfword -> halfword takes two transfers, so half it
  LPC_DAC->DACCNTVAL = 49;

  // DMA Channel 0, ADC (channel 0) -> Memory
  LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) &(LPC_ADC ->ADDR0);
  LPC_GPDMACH0->DMACCDestAddr = (uint32_t) &(audio_buffer[0]);
  LPC_GPDMACH0->DMACCLLI = 0;

  // Linked List contorol information (Same as DMA channel control register)
  //  Transfer size: 64 (DMA_BUFFER_LEN, bits 11:0)
  //  Source burst size: 1 (0 (default), bits 14:12)
  //  Destination burst size: 1 (0 (default), bits 17:15)
  //  Source transfer width: word (2, bits 20:18)
  //  Destination transfer width: halfword (1, bits 23:21)
  //  Source increment: don't increment (0, bit 26)
  //  Destination increment: increment (1, bit 27)
  //  Terminal count interrupt: disabled (0, bit 31)
  dmaLLNode.dmaControl = 0xfff | (2 << 18) | (1 << 21) | _BV(27);

  // DMA Channel 0 Config
  //  Source peripheral: ADC (4, bits  5:1)
  //  Destination peripheral: none (default, bits  10:6)
  //  Transfer Type: peripheral-to-memory (2, bits 13:11)
  LPC_GPDMACH0->DMACCConfig = (7 << 6) | (2 << 11);

  // DMA Channel 1, memory -> DAC
  LPC_GPDMACH1->DMACCSrcAddr = (uint32_t) &(audio_buffer[0]);
  LPC_GPDMACH1->DMACCDestAddr = &(LPC_DAC->DACR);
  LPC_GPDMACH1->DMACCLLI = 0;

  // Linked List contorol information (Same as DMA channel control register)
  //  Transfer size: 64 (DMA_BUFFER_LEN, bits 11:0)
  //  Source burst size: 1 (0 (default), bits 14:12)
  //  Destination burst size: 1 (0 (default), bits 17:15)
  //  Source transfer width: halfword (1, bits 20:18)
  //  Destination transfer width: halfword (1, bits 23:21)
  //  Source increment: increment (1, bit 26)
  //  Destination increment: don't increment (0, bit 27)
  //  Terminal count interrupt: disabled (0, bit 31)
  LPC_GPDMACH1->DMACCControl = 0xfff | (1 << 18) | (1 << 21) | _BV(26);

  // DMA Channel 1 Config
  //  Source peripheral: none (default, bits  5:1)
  //  Destination peripheral: DAC (7, bits  10:6)
  //  Transfer Type: memory-to-peripheral (1, bits 13:11)
  LPC_GPDMACH1->DMACCConfig = (7 << 6) | (1 << 11);

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
