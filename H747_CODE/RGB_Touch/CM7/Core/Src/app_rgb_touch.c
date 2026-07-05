/**
 * @file app_rgb_touch.c
 * @brief RGB 屏 + 触摸 BSP 应用层（本文件不在 CubeMX 生成列表内，可安全保留）
 */
#include "app_rgb_touch.h"

#include "main.h"
#include <stdio.h>

#include "bsp_delay.h"
#include "mpu.h"
#include "sdram.h"
#include "lcd.h"
#include "lcd_ltdc.h"
#include "touch.h"

static uint16_t s_lastpos[10][2];
static uint8_t s_maxp = 5;

static void load_draw_dialog(void)
{
  lcd_clear(WHITE);
  ltdc_framebuffer_sync();
}

static void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint32_t color)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, row, col;

  if (x1 < size || x2 < size || y1 < size || y2 < size)
  {
    return;
  }

  delta_x = x2 - x1;
  delta_y = y2 - y1;
  row = x1;
  col = y1;

  if (delta_x > 0)
  {
    incx = 1;
  }
  else if (delta_x == 0)
  {
    incx = 0;
  }
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }

  if (delta_y > 0)
  {
    incy = 1;
  }
  else if (delta_y == 0)
  {
    incy = 0;
  }
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }

  if (delta_x > delta_y)
  {
    distance = delta_x;
  }
  else
  {
    distance = delta_y;
  }

  for (t = 0; t <= distance + 1; t++)
  {
    lcd_fill_circle(row, col, size, color);
    xerr += delta_x;
    yerr += delta_y;

    if (xerr > distance)
    {
      xerr -= distance;
      row += incx;
    }

    if (yerr > distance)
    {
      yerr -= distance;
      col += incy;
    }
  }
}

void app_rgb_touch_init(void)
{
  uint8_t tp_ok;
  uint8_t t;

  delay_init((uint16_t)(SystemCoreClock / 1000000U));
  mpu_memory_protection();
  sdram_init();
  lcd_init();

  tp_ok = tp_dev.init();

  s_maxp = 5;
  if (lcddev.id == 0x1018U)
  {
    s_maxp = 10;
  }

  for (t = 0; t < 10U; t++)
  {
    s_lastpos[t][0] = 0xFFFFU;
    s_lastpos[t][1] = 0xFFFFU;
  }

  load_draw_dialog();
  if (tp_ok != 0U)
  {
    lcd_show_string(10, 30, 360, 16, 16, "TOUCH INIT FAIL", RED);
  }
  else
  {
    printf("Touch OK, white canvas ready.\r\n");
  }
}

void app_rgb_touch_poll(void)
{
  uint8_t t;

  tp_dev.scan(0);
  for (t = 0; t < s_maxp; t++)
  {
    if ((tp_dev.sta & (1U << t)) != 0U)
    {
      if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)
      {
        if (s_lastpos[t][0] == 0xFFFFU)
        {
          s_lastpos[t][0] = tp_dev.x[t];
          s_lastpos[t][1] = tp_dev.y[t];
        }

        lcd_draw_bline(s_lastpos[t][0], s_lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, RED);
        s_lastpos[t][0] = tp_dev.x[t];
        s_lastpos[t][1] = tp_dev.y[t];
      }
    }
    else
    {
      s_lastpos[t][0] = 0xFFFFU;
    }
  }

  ltdc_framebuffer_sync();
  HAL_Delay(5);
}
