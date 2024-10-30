#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd)
{
  uint8_t rkeycode = -1;
  if ((rkeycode = inl(KBD_ADDR)) != -1)
  {
    kbd->keydown = 1;
    kbd->keycode = rkeycode;
  }
  else
  {
    kbd->keydown = 0;
    kbd->keycode = AM_KEY_NONE;
  }
}
