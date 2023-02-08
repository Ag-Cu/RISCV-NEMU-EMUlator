#include <am.h>
#include <nemu.h>
#include "../../../riscv/riscv.h"

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t key = inl(AM_INPUT_KEYBRD);
  kbd->keydown = key & KEYDOWN_MASK;
  kbd->keycode = key & ~KEYDOWN_MASK;
}
