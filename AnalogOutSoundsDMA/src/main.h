#ifndef __MAIN_h_
#define __MAIN_h_

#define _BV(n) (1 << (n))

#define DMA_BUFFER_LEN 64

// Each field is one word (32 bits) long
typedef struct {
  uint32_t sourceAddr;
  uint32_t destAddr;
  uint32_t nextNode;
  uint32_t dmaControl;
} DMALinkedListNode;

// A pool of 64 nodes takes up 4 words * 64 = 256 words = 1kb of space, but
// can transfer a maximum of 64 * ~4k = ~256k of RAM
#define DMA_LL_POOL_SIZE 64

#endif
