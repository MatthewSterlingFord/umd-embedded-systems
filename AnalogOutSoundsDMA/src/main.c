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

// DMA Linked List Nodes, in AHB SRAM
__attribute__ ((section(".ahb_ram")))
static DMALinkedListNode dmaLLNodes[DMA_LL_POOL_SIZE];

int main(void) {
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
  LPC_DAC->DACCNTVAL = 49;

  // Setup DMA linked list
  // Sound sample can be more than 1 page (2^12, maximum transfer per LL node)
  uint32_t sound_buf_pos = (uint32_t) SOUND_CYMBAL;
  int sound_buf_bytes = (sizeof(SOUND_CYMBAL));
  uint_fast16_t ll_idx = 0;
  uint_fast16_t bytes_this_node;
  while ((sound_buf_bytes > 0) && (ll_idx < 64)) {
    dmaLLNodes[ll_idx].sourceAddr = (uint32_t) sound_buf_pos;
    dmaLLNodes[ll_idx].destAddr = (uint32_t) &(LPC_DAC->DACR);
    // This value should equal (itself & 0xFFFFFFFC)
    dmaLLNodes[ll_idx].nextNode = (uint32_t) (&(dmaLLNodes[ll_idx+1]));

    bytes_this_node = MIN(0xffe, sound_buf_bytes);

    // Linked List control information (Same as DMA channel control register)
    //  Transfer size: 64 (DMA_BUFFER_LEN, bits 11:0)
    //  Source burst size: 1 (0 (default), bits 14:12)
    //  Destination burst size: 1 (0 (default), bits 17:15)
    //  Source transfer width: halfword (1, bits 20:18)
    //  Destination transfer width: halfword (1, bits 23:21)
    //  Source increment: increment (1, bit 26)
    //  Destination increment: don't increment (0, bit 27)
    //  Terminal count interrupt: disabled (0, bit 31)
    dmaLLNodes[ll_idx].dmaControl = bytes_this_node
                                        | (1 << 18) | (1 << 21) | _BV(26);

    sound_buf_pos += bytes_this_node;
    sound_buf_bytes -= bytes_this_node;
    ll_idx++;
  }

  // Set last node to loop around to first node
  dmaLLNodes[ll_idx - 1].nextNode = (uint32_t) (&(dmaLLNodes[0]));

  // DMA Channel 0, initialize to linked list for initial DMA
  LPC_GPDMACH0->DMACCSrcAddr = dmaLLNodes[0].sourceAddr;
  LPC_GPDMACH0->DMACCDestAddr = dmaLLNodes[0].destAddr;
  LPC_GPDMACH0->DMACCControl = dmaLLNodes[0].dmaControl;
  LPC_GPDMACH0->DMACCLLI = dmaLLNodes[0].nextNode;

  // DMA Channel 0 Config
  //  Enable channel (1 at bit 0)
  //  Source peripheral: 0 (default, bits  5:1)
  //  Destination peripheral: DAC (7, bits  10:6)
  //  Transfer Type: memory-to-peripheral (1, bits 13:11)
  LPC_GPDMACH0->DMACCConfig = (7 << 6) | (1 << 11);
  LPC_GPDMACH0->DMACCConfig |= _BV(0);

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
