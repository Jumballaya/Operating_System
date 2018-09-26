/**
 * Memory
 */
#include "mem.h"

/**
 * memory_copy
 *
 * Copies n bytes of data from one char to another
 */
void memory_copy(u8 *source, u8 *dest, int nbytes) {
  int i;
  for (i = 0; i < nbytes; i++) {
    *(dest + i) = *(source + i);
  }
}

/**
 * memory_set
 *
 * Set memory address
 */
void memory_set(u8 *dest, u8 val, u32 len) {
  u8 *temp = (u8 *)dest;
  for ( ; len != 0; len--) *temp++ = val;
}
