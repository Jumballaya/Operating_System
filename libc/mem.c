/**
 * Memory
 */
#include "mem.h"

/**
 * memory_copy
 *
 * Copies n bytes of data from one char to another
 */
void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
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
void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
  uint8_t *temp = (uint8_t *)dest;
  for ( ; len != 0; len--) *temp++ = val;
}

// @TODO: compute at link time
// For now we use the hardcoded value we defined in the Makefile
uint32_t free_mem_addr = 0x10000;

/**
 * kmalloc
 *
 * Implementation is just a pointer to some free memory which keeps growing
 */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr) {
  /* Pages are aligned to 4K, or 0x1000 */
  if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
    free_mem_addr &= 0xFFFFF000;
    free_mem_addr += 0x1000;
  }
  /* Save also the physical address */
  if (phys_addr) *phys_addr = free_mem_addr;

  uint32_t ret = free_mem_addr;
  free_mem_addr += size;
  return ret;
}
