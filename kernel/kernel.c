/**
 * Kernel
 */
#include "../drivers/screen.h"
#include "util.h"

void main() {
  clear_screen();
  kprint("Welcome to the operating system\n");
}

