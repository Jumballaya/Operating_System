/**
 * IO Ports
 */
#include "io.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "../libc/string.h"
#include "./screen.h"
#include "../libc/function.h"

static void io_callback(registers_t *regs) {
  /* The PIC leaves us the scancode in port 0x60 */
  uint8_t scancode = port_byte_in(0x60);

  char out[10];
  int_to_ascii(scancode, out);
  kprint(out);
  UNUSED(regs);
}

void init_io() {
  register_interrupt_handler(IRQ9, io_callback);
  register_interrupt_handler(IRQ10, io_callback);
  register_interrupt_handler(IRQ11, io_callback);
  register_interrupt_handler(IRQ12, io_callback);
}
