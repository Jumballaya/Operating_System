/**
 * Memory
 */
#include "mem.h"
#include "function.h"
#include "../drivers/screen.h"

// @TODO: compute at link time
// For now we use the hardcoded value we defined in the Makefile
uint32_t free_mem_addr = 0x10000;

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

// Set Frame sets a bit in the frames bitset
void set_frame(uint32_t frame_addr, uint32_t* frames) {
  uint32_t frame = frame_addr / 0x1000;
  uint32_t id = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  frames[id] |= (0x1 << offset);
};

// Clear Frame clears a bit on the frames bitset
void clear_frame(uint32_t frame_addr, uint32_t* frames) {
  uint32_t frame = frame_addr / 0x1000;
  uint32_t id = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  frames[id] &= ~(0x1 << offset);
};

// Test Frame tests if a bit is set on the frame bitset
uint32_t test_frame(uint32_t frame_addr, uint32_t* frames) {
  uint32_t frame = frame_addr / 0x1000;
  uint32_t id = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  return (frames[id] & (0x1 << offset));
};

// First Frame finds the first free frame
uint32_t first_frame(uint32_t* nframes, uint32_t* frames) {
  uint32_t i,j;
  for (i = 0; i < INDEX_FROM_BIT(*nframes); i++) {
    if (frames[i] != 0xFFFFFFFF) {
      for (j = 0; j < 32; j++) {
        uint32_t toTest = 0x1 << j;
        if (!(frames[i] & toTest)) {
          return i * 4 * 8 + j;
        }
      }
    }
  }
};

/**
 * alloc_frame
 *
 * Allocate a new memory frame
 */
void alloc_frame(page_t *page, int is_kernel, int is_writeable, uint32_t* nframes, uint32_t* frames) {
  // Frame was already allocated
  if (page->frame != 0) {
    return;
  }

  uint32_t id = first_frame(nframes, frames);
  if (id  == (uint32_t)-1) {
    // Kernel panic using the PANIC macro
    panic("No free frames!");
  }

  set_frame(id * 0x1000, frames); // Set the frame
  page->present = 1;
  page->rw = is_writeable ? 1 : 0;
  page->user = is_kernel ? 0 : 1;
  page->frame = id;
}

/**
 * free_frame
 *
 * Frees an allocated frame
 */
void free_frame(page_t *page, uint32_t* frames) {
  uint32_t frame;
  if (!(frame = page->frame)) {
    // The page didn't have an allocated frame
    return;
  }

  clear_frame(frame, frames);
  page->frame = 0x0; // Remove the frame from the page
};

// Initialize Paging
// @TODO: nframes, frames, kernel_directory and current_directory are global variables
//        most likely shouldn't be (as this is the libc directory). I need to find a better
//        place to put paging/frame/memory functions that ARE NOT part of the libc.
void initialize_paging(uint32_t nframes, uint32_t* frames, page_directory_t* kernel_directory, page_directory_t* current_directory) {
  // Size of the physical memory.. for now we assume 16MB
  uint32_t mem_end_page = 0x1000000;

  nframes = mem_end_page / 0x1000;
  frames = (uint32_t*) kmalloc(INDEX_FROM_BIT(nframes), 0, 0);
  memory_set(frames, 0, INDEX_FROM_BIT(nframes));

  // Make a page directory
  kernel_directory = (page_directory_t*)kmalloc(sizeof(page_directory_t), 1, 0);
  memory_set(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;


  // We need to identity map (phys addr = virt addr) from
  // 0x0 to the end of used memory, so we can access this
  // transparently, as if paging wasn't enabled.
  // NOTE that we use a while loop here deliberately.
  // inside the loop body we actually change placement_address
  // by calling kmalloc(). A while loop causes this to be
  // computed on-the-fly rather than once at the start.
  uint32_t i = 0;
  while (i < free_mem_addr) {
    alloc_frame(get_page(i, 1, kernel_directory), 0, 0, nframes, frames);
    i += 0x1000;
  }

  // Register the fault handler
  register_interrupt_handler(14, page_fault);

  // Enable paging
  switch_page_directory(kernel_directory, current_directory);
};

// Switch Page loads the specified page directory
void switch_page_directory(page_directory_t *dir, page_directory_t* current_directory) {
  *current_directory = *dir;
  asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
  uint32_t cr0;
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; // enable paging
  asm volatile("mov %0, %%cr0":: "r"(cr0));
};

// Get Page retrieves a pointer to the page required. If make is set to 1 the page should be created
// if not found
page_t *get_page(uint32_t address, int make, page_directory_t *dir) {
  // Turn address into an index
  address /= 0x1000;

  uint32_t table_id = address / 1024;
  if (dir->tables[table_id]) {
    // if the table is already assigned
    return &dir->tables[table_id]->pages[address%1024];
  } else if (make) {
    uint32_t tmp;
    dir->tables[table_id] = (page_table_t*)kmalloc(sizeof(page_table_t), &tmp, 0);
    memory_set(dir->tables[table_id], 0, 0x1000);
    dir->tablesPhysical[table_id] = tmp | 0x7; // PRESENT, RW, US.
    return &dir->tables[table_id]->pages[address%1024];
  }

  return 0;
};

// Page Fault handle page faults
void page_fault(registers_t regs) {
  uint32_t fault_addr;

  asm volatile("mov %%cr2, %0": "=r"(fault_addr));

  char addr[8];
  hex_to_ascii(fault_addr, addr);

  int present = !(regs.err_code & 0x1);   // Page not present
  int rw = regs.err_code & 0x2;           // Write operation?
  int user = regs.err_code & 0x4;         // Processor was in user mode?
  int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
  int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

  clear_screen();
  kprint("Page Fault (");
  if (present) kprint(" present");
  if (rw) kprint(" read-only");
  if (user) kprint(" user-mode");
  if (reserved) kprint(" reserved");
  if (id) kprint(" instruction fetch");
  kprint(" ) at 0x");
  kprint(addr);
  kprint("\n");
};
