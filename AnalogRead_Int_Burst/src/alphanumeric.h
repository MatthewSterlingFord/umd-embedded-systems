#ifndef __ALPHANUMERIC_h_
#define __ALPHANUMERIC_h_

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

void alpha_init();
void alpha_display(char c);
void alpha_dot_on();
void alpha_dot_off();

#endif
