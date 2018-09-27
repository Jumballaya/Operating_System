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

// @TODO: compute at link time
// For now we use the hardcoded value we defined in the Makefile
u32 free_mem_addr = 0x10000;

/**
 * kmalloc
 *
 * Implementation is just a pointer to some free memory which keeps growing
 */
u32 kmalloc(u32 size, int align, u32 *phys_addr) {
  // Pages are aligned to 4k, or 0x1000
  if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
    free_mem_addr &= 0xFFFFF000;
    free_mem_addr += 0x1000;
  }

  // Save the physical address
  if (phys_addr) *phys_addr = free_mem_addr;

  u32 ret = free_mem_addr;
  free_mem_addr += size;
  return ret;
}
