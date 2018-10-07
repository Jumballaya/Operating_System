/**
 * General Functions
 */
#include "string.h"
#include "../drivers/screen.h"

void panic(char msg[]) {
  clear_screen();
  kprint(msg);
}
