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

const char *HEX_CHAR_TABLE = "0123456789ABCDEF";

int main(void) {
  LPC_SC ->CLKSRCSEL = 1;   // Select main clock source
  LPC_SC ->PLL0CON = 0;     // Bypass PLL0, use clock source directly

  // Feed the PLL register so the PLL0CON value goes into effect
  LPC_SC ->PLL0FEED = 0xAA; // set to 0xAA
  LPC_SC ->PLL0FEED = 0x55; // set to 0x55

  // Set clock divider to 0 + 1=1
  LPC_SC ->CCLKCFG = 0;

  // Choose undivided peripheral clock for
  LPC_SC ->PCLKSEL0 |= (1 << 24);

  // Setup IO pins
  LPC_GPIO0 ->FIODIR = 0;
  LPC_GPIO0 ->FIOSET = 0;

  // Configure pins
  //   P0.26 as AOUT
  LPC_PINCON ->PINSEL1 |= (2 << 20);

  LPC_DAC ->DACCTRL = 0;
  //LPC_DAC ->DACCNTVAL = 0;

  uint32_t i = 0, j = 0;
  while (1) {
	  for (i = 0; i < 0x3ff; ++i) {
		  LPC_DAC ->DACR = ((LPC_DAC ->DACR) & ~(0x3ff << 6)) | (i << 6);

		  for (j = 0; j < 10000; ++j) {
			  asm ("nop");
		  }
	  }
  }

  return 0;
}
