#ifndef __MAIN_h_
#define __MAIN_h_

#define _BV(n) (1 << (n))

// Must be power of 2
#define BUFFER_LEN 1024

// Don't emit these symbols in assembly source
#ifndef __ASSEMBLER__

extern volatile uint32_t pulse_buffer[BUFFER_LEN];
extern volatile uint32_t buffer_pos;

#endif /* __ASSEMBLER__ */

#endif
