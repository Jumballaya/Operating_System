/**
 * Kernel
 */

void dummy_test_entrypoint() {}

void _start() {
  char* video_memory = (char*) 0xb8000;
  *video_memory = 'X';
}
