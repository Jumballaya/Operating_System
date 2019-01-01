/**
 * Kernel
 */
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include <stdint.h>

void kernel_main() {
  isr_install();
  irq_install();

  uint32_t *frames;
  uint32_t nframes;
  page_directory_t* kernel_directory;
  page_directory_t* current_directory;

  /*initialize_paging(nframes, frames, kernel_directory, current_directory);*/

  kprint("Type something, it will go through the kernel\n"
    "Type 'end' to halt the CPU or 'page' to request a kmalloc()\n> ");
}

void request_page() {
	uint32_t phys_addr;
	uint32_t page = kmalloc(1000, 1, &phys_addr);
	char page_str[16] = "";
	hex_to_ascii(page, page_str);
	char phys_str[16] = "";
	hex_to_ascii(phys_addr, phys_str);
	kprint("Page: ");
	kprint(page_str);
	kprint(", physical address: ");
	kprint(phys_str);
	kprint("\n");
}

void user_input(char *input) {
  if (strcmp(input, "end") == 0) {
    kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
  } else if (strcmp(input, "page") == 0) {
		request_page();
  } else if (strcmp(input, "clear") == 0) {
		clear_screen();
		kprint("\n> ");
		return;
	}
  kprint("You said: ");
  kprint(input);
  kprint("\n> ");
}
