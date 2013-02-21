#ifndef __MORSE_H_
#define __MORSE_H_

#include <stdint.h>

extern const char *TABLE;

char morse_decode(uint_fast8_t code, uint_fast8_t len);

#endif
