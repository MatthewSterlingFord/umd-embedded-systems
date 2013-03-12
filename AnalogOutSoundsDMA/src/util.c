#include "util.h"

uint_fast32_t map(uint_fast32_t val,
                  uint_fast32_t in_min, uint_fast32_t in_max,
                  uint_fast32_t out_min, uint_fast32_t out_max) {
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
