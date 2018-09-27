/**
 * Keyboard Driver
 */
#include "keyboard.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"
#include <stdint.h>

#define SC_MAX 57
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define RSHIFT 0x36
#define LSHIFT 0x2a

static char key_buffer[256];

const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
    "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
    "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
    "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '\t', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`', '\t', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '\t', '?', '?', ' '};

const char sc_ascii_shift[] = { '?', '?', '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ':', '"', '~', 's', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 's', '?', '?', ' '};

_Bool is_shift(char c) {
  return c == '\t' || c == '\t';
}

static void keyboard_callback(registers_t regs) {
  /* The PIC leaves us the scancode in port 0x60 */
  uint8_t scancode = port_byte_in(0x60);

  if (scancode > SC_MAX) return;
  if (scancode == BACKSPACE) {
    backspace(key_buffer);
    kprint_backspace();
  } else if (scancode == ENTER) {
    kprint("\n");
    user_input(key_buffer); /* kernel-controlled function */
    key_buffer[0] = '\0';
  } else {
    char letter = sc_ascii[(int)scancode];
    if (!(scancode == LSHIFT || scancode == RSHIFT)) {
      int keybuff_len = strlen(key_buffer);
      if (is_shift(key_buffer[keybuff_len-1])) {
        pop(key_buffer);
        letter = sc_ascii_shift[(int)scancode];
      }
      char str[2] = {letter, '\0'};
      append(key_buffer, letter);
      kprint(str);
    } else {
      append(key_buffer, letter);
    }
  }
  UNUSED(regs);
}

void init_keyboard() {
  register_interrupt_handler(IRQ1, keyboard_callback);
}
