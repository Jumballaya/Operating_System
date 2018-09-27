/**
 * IDT Header
 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Segment Selectors
#define KERNEL_CS 0x08

// Interrupt Gate definition
/*
 * First byte (always0):
 * Bit 7: Interrupt is present
 * Bits 6 - 5: Privilege level of caller (0 = kernel, ..., 3 = user)
 * Bit 4: Set to 0 for interrupt gates
 * Bits 3 - 0: bits 1110 = dec 14 = "32 bit interrupt gate"
 */
typedef struct {
  uint16_t low_offset;     // Lower 16 bits of the handler function address
  uint16_t sel;            // Kernel Segment Selector
  uint8_t always0;         // Always 0
  uint8_t flags;           // First byte (definition above)
  uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

// Pointer to the array of interrupt handlers. Assembly instruction 'lidt' will read it
typedef struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

// Functions implemented in idt.c
void set_idt_gate(int n, uint32_t handler);
void set_idt();

#endif
