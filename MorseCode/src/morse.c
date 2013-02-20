#include "morse.h"

char morse_decode(uint_fast8_t code, uint_fast8_t len) {
	return (TABLE + (1 << len))[code];
}
