/**
 * String
 */
#include "string.h"

/**
 * into_to_ascii
 *
 * K & R
 */
void int_to_ascii(int n, char str[]) {
  int i, sign;
  if ((sign = n) < 0) n = -n;
  i = 0;
  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (sign < 0) str[i++] = '-';
  str[i] = '\0';

  reverse(str);
}

/**
 * hex_to_ascii
 *
 */
void hex_to_ascii(int n, char str[]) {
  append(str, '0');
  append(str, 'x');
  char zeros = 0;

  s32 tmp;
  int i;
  for (i = 28; i > 0; i -= 4) {
    tmp = (n >> i) & 0xF;
    if (tmp == 0 && zeros == 0) continue;
    zeros = 1;
    if (tmp > 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
  }

  tmp = n & 0xF;
  if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
  else append(str, tmp + '0');
}

/**
 * reverse
 * K & R
 */
void reverse(char s[]) {
  int c, i, j;
  for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/**
 * strlen
 * K & R
 */
int strlen(char s[]) {
  int i = 0;
  while (s[i] != '\0') ++i;
  return i;
}

/**
 * append
 */
void append(char s[], char n) {
  int len = strlen(s);
  s[len] = n;
  s[len+1] = '\0';
}

/**
 * pop
 */
char pop (char stack[]) {
  int len = strlen(stack);
  if (len <= 0) {
    return ' ';
  }
  char last = stack[len-1];
  stack[len-1] = 0;
  return last;
}

/**
 * backspace
 */
void backspace(char s[]) {
  int len = strlen(s);
  s[len-1] = '\0';
}

/**
 * K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2
 */
int strcmp(char s1[], char s2[]) {
  int i;
  for (i = 0; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0') return 0;
  }
  return s1[i] - s2[i];
}
