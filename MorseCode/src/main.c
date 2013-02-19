/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pins.h"
#include "lcd44780.h"

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

#define BUFFER_LEN 5
#define MORSE_MAX_LEN 5

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

uint32_t pulse_buffer[2][BUFFER_LEN] = {0};

uint32_t current_buffer = 0;
uint32_t buffer_pos = 0;

int rising_edge = 1;
void TIMER0_IRQHandler(void) {
	LPC_TIM0->IR  = 0xff;  // reset all interrrupts
	LPC_TIM0->TC = 0;      // begin timing next pulse

    uint32_t pulse_len = LPC_TIM0->CR0;
    // 120482 cycles is 5ms at 12mhz
    // This will be the minimum significant pulse
    if (pulse_len > 60241) { /*
    	if (rising_edge) {
    		LPC_GPIO0->FIOPIN |= (1 << 22);
    		LPC_GPIO1->FIOPIN |= (7 << 18);
    	} else {
    		LPC_GPIO0->FIOPIN &= ~(1 << 22);
			LPC_GPIO1->FIOPIN &= ~(7 << 18);
    	} */

    	if (rising_edge) {
			if (pulse_len > 3000000) {
				buffer_pos = 0;
				current_buffer ^= 1;
	    		LPC_GPIO0->FIOPIN ^= _BV(22);
			}
    	} else if (buffer_pos < (BUFFER_LEN - 1) && !rising_edge){
    		pulse_buffer[current_buffer][buffer_pos] = pulse_len;
    		buffer_pos++;

			if (pulse_len < 1000000) {
				LPC_GPIO1->FIOPIN |= _BV(18);
				LPC_GPIO1->FIOPIN &= ~(_BV(19) | _BV(20));
			} else {
				LPC_GPIO1->FIOPIN |= _BV(18) | _BV(19) | _BV(20);
			}
    	}
    }

    rising_edge = !rising_edge;
    LPC_TIM0->CCR ^= 3; // flip the edge that we capture
}

int main(void) {
	LPC_SC->CLKSRCSEL = 1;   // Select main clock source
	LPC_SC->PLL0CON = 0;     // Bypass PLL0, use clock source directly

	// Feed the PLL register so the PLL0CON value goes into effect
	LPC_SC->PLL0FEED = 0xAA; // set to 0xAA
	LPC_SC->PLL0FEED = 0x55; // set to 0x55

	// Set clock divider to 0+1=1
	LPC_SC->CCLKCFG = 0;

	// Choose undivided peripheral clocks for TIMER0
	LPC_SC->PCLKSEL0 |= _BV(2);

	// Power TIMER0
	LPC_SC->PCONP |= 1;

	// Setup IO pins
	LPC_GPIO0->FIODIR = _BV(22);
	LPC_GPIO0->FIOSET = _BV(22);
	LPC_GPIO0->FIOPIN = 0;

	LPC_GPIO1->FIODIR = _BV(18) | _BV(19) | _BV(20);
	LPC_GPIO1->FIOSET = _BV(18) | _BV(19) | _BV(20);
	LPC_GPIO1->FIOPIN = 0;

	// Configure pins
	//   P1.26 as CAP0.0 (TIMER0 capture pin 0)
	LPC_PINCON->PINSEL3 |= (3 << 20);

	// Timer0 interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);

	// Configure timer one so that it constantly counts up, at the undivided
	// clock rate. When CAP0.0 goes high or low, capture value is stored and
	// timer value is reset and interrupt is triggered
	LPC_TIM0->MCR = 1 << 1;
    LPC_TIM0->TCR = 0x02;           // reset timer
    LPC_TIM0->PR  = 1;             // No prescale
    LPC_TIM0->MR0 = 0xffffffff;     // match value (unnecessary)
    LPC_TIM0->IR  = 0xff;           // reset all interrrupts

	// Capture Control Register
	//   bit 0 - Capture on CAP0.0 rising edge
	//   bit 1 - Capture on CAP0.0 falling edge
	//   bit 2 - Interrupt on CAP0.0 event
	LPC_TIM0->CCR = 1 | (0 << 1) | (1 << 2);

	// Enable timer
	LPC_TIM0->TCR = 1;
/*
	LPC_GPIO0->FIOPIN |= _BV(22);
	volatile int i;
    for(i = 0; i < 6000200; i++){
    	__NOP();
    }

    SystemInit();
    init_lcd();
    printf("clock: %d\n", SystemCoreClock);

    lcd_putstring(0,"LPC 1769 Test Msg");
	LPC_GPIO0->FIOPIN &= ~_BV(22);
	*/

	uint32_t i, parse_buf, average, min, max, cnt, cur;
	//char message[sizeof(int)*8 + 1] = {0};
	while (1) {
		parse_buf = current_buffer ^ 1;
		if (pulse_buffer[parse_buf][0]) {
			cnt = 0;
			max = 0;
			min = 0xFFFFFFFF;
			average = 0;
			for (i = 0; (i < MORSE_MAX_LEN) && (cur = pulse_buffer[parse_buf][i]); ++i, ++cnt) {
				printf("%d  ", cur);
				if (cur < min) min = cur;
				if (cur > max) max = cur;
				average += cur;
			}
			average /= cnt;
			printf("\nMin: %d  max: %d  avg: %d  of cnt: %d \n",
					min, max, average, cnt);

			average /= 2;
			for (i = 0; i < cnt; ++i) {
				cur = pulse_buffer[parse_buf][i];
				printf("%c", (cur < average)? '.':'-');
				pulse_buffer[parse_buf][i] = 0;
			}
			printf("\n");

			/*
			if (pulse_buffer[parse_buf][0] < 1000000) {
				LPC_GPIO1->FIOPIN |= _BV(18);
				LPC_GPIO1->FIOPIN &= ~(_BV(19) | _BV(20));
			} else {
				LPC_GPIO1->FIOPIN |= _BV(18) | _BV(19) | _BV(20);
			} */

		//	itoa(pulse_buffer[parse_buf][1], message, 10);
		//    lcd_putstring(1,message);
		}
	}
	return 0;
}
