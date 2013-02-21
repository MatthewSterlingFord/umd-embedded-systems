#ifndef __MORSE_H_
#define __MORSE_H_

#include <stdint.h>

const char *TABLE = "\x00etianmsurwdkgohvf\x00l\x00pjbxcyzq\x00\x0054\x003\x00\x00\x002\x00\x00\x00\x00\x00\x00\x0016\x00\x00\x00\x00\x00\x00\x007\x00\x00\x008\x009\x00";

char morse_decode(uint_fast8_t code, uint_fast8_t len);

#endif
