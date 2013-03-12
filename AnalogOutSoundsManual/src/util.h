#ifndef __UTIL_h_
#define __UTIL_h_

#include <stdint.h>

#define MAX(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

#define MIN(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

#define ARRAY_LEN(a) (sizeof((a))/sizeof((a)[0]))

uint_fast32_t map(uint_fast32_t val,
                  uint_fast32_t min_in, uint_fast32_t max_in,
                  uint_fast32_t min_out, uint_fast32_t max_out);

#endif
