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

  /*initialize_paging(&nframes, frames);*/

  asm("int $2");
  asm("int $3");

  kprint("Type something, it will go through the kernel\n"
    "Type 'end' to halt the CPU or 'page' to request a kmalloc()\n> ");
}

void user_input(char *input) {
  if (strcmp(input, "end") == 0) {
    kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
  } else if (strcmp(input, "clear") == 0) {
		clear_screen();
		kprint("\n> ");
		return;
	}
  kprint("You said: ");
  kprint(input);
  kprint("\n> ");
}
