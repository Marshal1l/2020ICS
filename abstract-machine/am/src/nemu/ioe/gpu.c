#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
// #define W 800
// #define H 600
// #define FPS 60
void __am_gpu_init()
{
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg)
{
  int h = inl(VGACTL_ADDR) & 0xffff;
  int w = inl(VGACTL_ADDR) >> 16;
  *cfg = (AM_GPU_CONFIG_T){
      .present = true, .has_accel = false, .width = w, .height = h, .vmemsz = w * h * sizeof(uint32_t)};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl)
{
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (!ctl->sync && (w == 0 || h == 0))
    return;
  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t screen_w = inl(VGACTL_ADDR) >> 16;
  for (int i = y; i < y + h; i++)
  {
    for (int j = x; j < x + w; j++)
    {
      fb[screen_w * i + j] = pixels[w * (i - y) + (j - x)];
    }
  }
  if (ctl->sync)
  {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status)
{
  status->ready = true;
}
