#include "alphanumeric.h"

#define DEFINE_ALPHA_PIN(name, port, pin) \
inline void ALPHA_##name##_ON() { \
  LPC_GPIO##port ->FIOCLR |= (1 << pin); \
} \
inline void ALPHA_##name##_OFF() { \
  LPC_GPIO##port ->FIOSET |= (1 << pin); \
} \
inline void ALPHA_##name##_INIT() { \
  LPC_GPIO##port ->FIODIR |= (1 << pin); \
}

DEFINE_ALPHA_PIN(TOP, 1, 18);
DEFINE_ALPHA_PIN(TOP_LEFT, 1, 19);
DEFINE_ALPHA_PIN(TOP_RIGHT, 1, 20);
DEFINE_ALPHA_PIN(MIDDLE, 1, 21);
DEFINE_ALPHA_PIN(BOTTOM_LEFT, 1, 22);
DEFINE_ALPHA_PIN(BOTTOM_RIGHT, 1, 23);
DEFINE_ALPHA_PIN(BOTTOM, 1, 24);
DEFINE_ALPHA_PIN(DOT, 1, 25);

inline void alpha_init() {
  ALPHA_TOP_INIT();
  ALPHA_TOP_LEFT_INIT();
  ALPHA_TOP_RIGHT_INIT();
  ALPHA_MIDDLE_INIT();
  ALPHA_BOTTOM_LEFT_INIT();
  ALPHA_BOTTOM_RIGHT_INIT();
  ALPHA_BOTTOM_INIT();
  ALPHA_DOT_INIT();
  ALPHA_DOT_OFF();
}

inline void alpha_dot_on() {
  ALPHA_DOT_ON();
}

inline void alpha_dot_off() {
  ALPHA_DOT_OFF();
}

inline void alpha_display(char c) {
  // Characters generated from:
  // http://en.wikipedia.org/wiki/Seven-segment_display_character_representations
  switch (c) {
  case '0':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case '1':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case '2':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case '3':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case '4':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case '5':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case '6':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case '7':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case '8':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case '9':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'a':
  case 'A':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'b':
  case 'B':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'c':
  case 'C':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case 'd':
  case 'D':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case 'e':
  case 'E':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'f':
  case 'F':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'g':
  case 'G':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case 'h':
  case 'H':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'i':
  case 'I':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case 'j':
  case 'J':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case 'k':
  case 'K':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'l':
  case 'L':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case 'm':
  case 'M':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case 'n':
  case 'N':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case 'o':
  case 'O':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case 'p':
  case 'P':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'q':
  case 'Q':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'r':
  case 'R':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;

  case 's':
  case 'S':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 't':
  case 'T':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'u':
  case 'U':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_OFF();
    break;

  case 'v':
  case 'V':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case 'w':
  case 'W':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_OFF();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_OFF();
    break;

  case 'x':
  case 'X':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_OFF();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'y':
  case 'Y':
    ALPHA_TOP_OFF();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_ON();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_OFF();
    ALPHA_TOP_LEFT_ON();
    ALPHA_MIDDLE_ON();
    break;

  case 'z':
  case 'Z':
    ALPHA_TOP_ON();
    ALPHA_TOP_RIGHT_ON();
    ALPHA_BOTTOM_RIGHT_OFF();
    ALPHA_BOTTOM_ON();
    ALPHA_BOTTOM_LEFT_ON();
    ALPHA_TOP_LEFT_OFF();
    ALPHA_MIDDLE_ON();
    break;
  }
}
