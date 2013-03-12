#include "util.h"

uint_fast32_t map(uint_fast32_t val,
                  uint_fast32_t min_in, uint_fast32_t max_in,
                  uint_fast32_t min_out, uint_fast32_t max_out) {
  return (val - min_in) * (max_out - min_out) / (max_in - max_out) + min_out;
}
