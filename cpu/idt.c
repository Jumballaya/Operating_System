/**
 * IDT
 *
 * Interrupt Descriptor Table
 * -  data structure used by the x86 architecture to implement an interrupt vector table.
 *    The IDT is used by the processor to determine the correct response to interrupts and exceptions.
 */
#include "idt.h"
#include "types.h"

void set_idt_gate(int n, uint32_t handler) {
  idt[n].low_offset = low_16(handler);
  idt[n].sel = KERNEL_CS;
  idt[n].always0 = 0;
  idt[n].flags = 0x8e;
  idt[n].high_offset = high_16(handler);
}

void set_idt() {
  idt_reg.base = (uint32_t) &idt;
  idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;

  asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}
