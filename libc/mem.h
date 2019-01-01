#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>
#include "../cpu/isr.h"
#include "string.h"

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

/* At this stage there is no 'free' implemented. */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

// Page struct holds the page information
typedef struct {
  uint32_t present  : 1;  // page present in memory
  uint32_t rw       : 1;  // read-only if clear, readwrite if set
  uint32_t user     : 1;  // root only if clear
  uint32_t accessed : 1;  // has page been accessed since last refresh?
  uint32_t dirty    : 1;  // has page been written to since last refresh?
  uint32_t unused   : 7;  // amalgamation of unused and reserved bits
  uint32_t frame    : 20; // frame address (shifted right 12 bits)
} page_t;

// Page Table
typedef struct page_table {
  page_t pages[1025];
} page_table_t;

// Page Directory
typedef struct page_directoy {
  page_table_t *tables[1024];  // array of pointers to pagetables
  uint32_t tablesPhysical[1024]; // as above but gives the physical location, for loading CR3 register
  uint32_t physicalAddr[1024];   // physical address of tablesPhysical.
} page_directory_t;

// Initialize Paging
void initialize_paging(uint32_t nframes, uint32_t* frames, page_directory_t* kernel_directory, page_directory_t* current_directory);

// Switch Page loads the specified page directory
void switch_page_directory(page_directory_t *dir, page_directory_t *current_directory);

// Get Page retrieves a pointer to the page required. If make is set to 1 the page should be created
// if not found
page_t *get_page(uint32_t address, int make, page_directory_t *dir);

// Frame
void alloc_frame(page_t *page, int is_kernel, int is_writeable, uint32_t* nframes, uint32_t* frames);
void free_frame(page_t *page, uint32_t* frames);

// Page Fault handle page faults
void page_fault(registers_t regs);

// Set Frame sets a bit in the frames bitset
void set_frame(uint32_t frame_addr, uint32_t* frames);

// Clear Frame clears a bit on the frames bitset
void clear_frame(uint32_t frame_addr, uint32_t* frames);

// Test Frame tests if a bit is set on the frame bitset
uint32_t test_frame(uint32_t frame_addr, uint32_t* frames);

// First Frame finds the first free frame
uint32_t first_frame(uint32_t* nframes, uint32_t* frames);

#endif
