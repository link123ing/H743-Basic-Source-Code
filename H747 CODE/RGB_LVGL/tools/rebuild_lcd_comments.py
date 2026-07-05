#!/usr/bin/env python3
"""Rebuild BSP LCD sources with UTF-8 comments from 例程39 reference."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REF_LCD_C = ROOT / "例程39 压力测试" / "Drivers" / "BSP" / "LCD" / "lcd.c"
REF_LCD_H = ROOT / "例程39 压力测试" / "Drivers" / "BSP" / "LCD" / "lcd.h"
REF_LTDC_C = ROOT / "例程39 压力测试" / "Drivers" / "BSP" / "LCD" / "ltdc.c"

OUT_LCD_C = ROOT / "BSP" / "LCD" / "lcd.c"
OUT_LCD_H = ROOT / "BSP" / "LCD" / "lcd.h"
OUT_LTDC_C = ROOT / "BSP" / "LCD" / "lcd_ltdc.c"


def read_ref(path: Path) -> str:
    raw = path.read_bytes()
    for enc in ("utf-8", "gbk", "gb2312"):
        try:
            return raw.decode(enc)
        except UnicodeDecodeError:
            continue
    raise UnicodeDecodeError("ref", b"", 0, 1, path.name)


def build_lcd_h() -> str:
    text = read_ref(REF_LCD_H)
    text = text.replace('#include "./SYSTEM/sys/sys.h"', '#include "main.h"')
    text = text.replace('#include "./BSP/LCD/ltdc.h"', '#include "lcd_ltdc.h"')
    text = text.replace('#include "./BSP/LCD/mipi_lcd.h"\n\n', "\n")
    text = text.replace('#include "./BSP/LCD/mipi_lcd.h"\n', "\n")
    text = text.replace(
        "#if LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB565",
        "#if LTDC_PIXFORMAT == LTDC_PIXEL_FORMAT_RGB565",
    )
    return text


def build_lcd_c() -> str:
    text = read_ref(REF_LCD_C)
    text = text.replace('#include "./BSP/LCD/lcd.h"', '#include "lcd.h"')
    text = text.replace('#include "./BSP/LCD/lcdfont.h"', '#include "lcdfont.h"')
    text = text.replace('#include "./SYSTEM/usart/usart.h"', '#include <stdio.h>')
    text = text.replace('#include "./SYSTEM/delay/delay.h"', '#include "bsp_delay.h"')
    text = text.replace('#include "./BSP/LCD/ltdc.h"', '#include "lcd_ltdc.h"')
    text = text.replace('#include "./BSP/LCD/mipi_lcd.h"\n', "")
    old_mipi = """    else                                /* 是MIPI屏 */
    {
        dsi_init();                     /* 初始化DSI */
    }
    
"""
    text = text.replace(old_mipi, "\n")
    # RGB panel only: drop MIPI branch
    old = """    if (lcddev.id != 0)                 /* ID非零,说明有RGB屏接入 */
    {
        ltdc_init();                    /* 初始化LTDC */      
    }
    else                                /* 是MIPI屏 */
    {
        dsi_init();                     /* 初始化DSI */
    }
    """
    new = """    if (lcddev.id != 0)                 /* ID非零,说明有RGB屏接入 */
    {
        ltdc_init();                    /* 初始化LTDC */
    }
    """
    text = text.replace(old, new)
    text = text.replace("RGB/MIPI屏", "RGB屏")
    return text


def build_ltdc_c() -> str:
    ref = read_ref(REF_LTDC_C)
    start = ref.index("void ltdc_switch")
    end = ref.index("void HAL_LTDC_MspInit")
    body = ref[start:end]

    body = body.replace("g_ltdc_handle", "hltdc")

    # Remove tft_spi hook for 5.5 inch panel
    body = body.replace(
        """    if (lcdid == 0X5571)
    {
        tft_spi_init();
    }
    
""",
        "",
    )

    header = '''/**
 ****************************************************************************************************
 * @file        lcd_ltdc.c
 * @version     V1.0
 * @brief       LTDC 驱动代码
 ****************************************************************************************************
 * @attention   Waiken-Smart 慧勤智远
 *
 * 实验平台:    STM32H747XIH6小系统板
 *
 ****************************************************************************************************
 */

#include "lcd_ltdc.h"
#include "lcd.h"
#include "bsp_delay.h"


uint32_t *g_ltdc_framebuf[2];              /* LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域 */
_ltdc_dev lcdltdc;                         /* 管理LCD LTDC的重要参数 */


'''

    # ltdc_init: use BSP frame buffer + CubeMX guard
    init_start = body.index("void ltdc_init")
    init_end = body.index("void HAL_LTDC_MspInit") if "void HAL_LTDC_MspInit" in body else len(body)
    init_ref = ref[ref.index("void ltdc_init"): ref.index("    ltdc_display_dir(1);")]

    # Keep timing tables from ref but patch init tail like BSP
    init_tail_bsp = '''
    lcddev.width = lcdltdc.pwidth;      /* 设置lcddev的宽度参数 */
    lcddev.height = lcdltdc.pheight;    /* 设置lcddev的高度参数 */
    lcdltdc.pixformat = LTDC_PIXFORMAT; /* 颜色像素格式 */

    g_ltdc_framebuf[0] = (uint32_t *)LTDC_FRAME_BUF_ADDR;
    lcdltdc.pixsize = 2;

#if !BSP_USE_CUBEMX_LTDC
    hltdc.Instance = LTDC;

    if (lcdid == 0X8081)
    {
        hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AH;     /* 水平同步极性:高电平有效 */
    }
    else
    {
        hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;     /* 水平同步极性:低电平有效 */
    }

    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;         /* 垂直同步极性:低电平有效 */
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;         /* 数据使能极性:低电平有效 */
    hltdc.State = HAL_LTDC_STATE_RESET;

    if (lcdid == 0X1018 || lcdid == 0X8081)
    {
        hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;   /* 像素时钟极性:高电平有效 */
    }
    else
    {
        hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;    /* 像素时钟极性:低电平有效 */
    }

    hltdc.Init.HorizontalSync = lcdltdc.hsw - 1;
    hltdc.Init.VerticalSync = lcdltdc.vsw - 1;
    hltdc.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1;
    hltdc.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1;
    hltdc.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1;
    hltdc.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1;
    hltdc.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1;
    hltdc.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1;
    hltdc.Init.Backcolor.Red = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Blue = 0;
    HAL_LTDC_Init(&hltdc);
#endif

    ltdc_layer_parameter_config(0, (uint32_t)g_ltdc_framebuf[0], LTDC_PIXFORMAT, 255, 0, 6, 7, 0X000000);
    ltdc_layer_window_config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);

    ltdc_display_dir(1);
    ltdc_select_layer(0);
    LTDC_RST(1);
    delay_ms(10);
    LTDC_RST(0);
    delay_ms(50);
    LTDC_RST(1);
    delay_ms(200);

    LTDC_BL(1);
    ltdc_clear(0XFFFFFFFF);
}
'''

    # body before ltdc_init + patched init + gpio init
    pre_init = body[: body.index("void ltdc_init")]
    post_init_funcs = '''

/**
 * @brief       LTDC底层GPIO初始化(背光/复位/供电)
 * @retval      无
 */
void ltdc_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    LTDC_BL_GPIO_CLK_ENABLE();
    LTDC_RST_GPIO_CLK_ENABLE();
    LCD_PWREN_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = LTDC_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LTDC_RST_GPIO_PIN;
    HAL_GPIO_Init(LTDC_RST_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_PWREN_GPIO_PIN;
    HAL_GPIO_Init(LCD_PWREN_GPIO_PORT, &gpio_init_struct);

    LCD_PWREN(1);
}
'''

    # Patch ltdc_init header to call gpio init first
    init_fn = ref[ref.index("void ltdc_init"): ref.index("    lcddev.width = lcdltdc.pwidth")]
    init_fn = init_fn.replace(
        "void ltdc_init(void)\n{\n    uint16_t lcdid = 0;",
        "void ltdc_init(void)\n{\n    ltdc_gpio_init();\n    uint16_t lcdid = 0;",
    )
    init_fn = init_fn.replace("g_ltdc_handle", "hltdc")

    # Remove old framebuf / HAL_LTDC_Init block from ref init_fn tail - it's only up to lcdid tables
    # init_fn currently ends before lcddev.width - good

    # Find where panel table ends in init_fn (ends at "} \n\n    lcddev" in full ref - we cut early)
    # init_fn is only start through panel id read and timing - need full panel branches
    init_fn = ref[ref.index("void ltdc_init"): ref.index("    lcddev.width = lcdltdc.pwidth")]
    init_fn = init_fn.replace(
        "void ltdc_init(void)\n{\n    uint16_t lcdid = 0;",
        "void ltdc_init(void)\n{\n    ltdc_gpio_init();\n    uint16_t lcdid = 0;",
    )
    init_fn = init_fn.replace("g_ltdc_handle", "hltdc")
    if "tft_spi_init" in init_fn:
        init_fn = init_fn.replace(
            """    if (lcdid == 0X5571)
    {
        tft_spi_init();
    }
    
""",
            "",
        )

    result = header + pre_init + init_fn + init_tail_bsp + post_init_funcs
    return result


def main() -> None:
    OUT_LCD_H.write_text(build_lcd_h(), encoding="utf-8", newline="\n")
    OUT_LCD_C.write_text(build_lcd_c(), encoding="utf-8", newline="\n")
    OUT_LTDC_C.write_text(build_ltdc_c(), encoding="utf-8", newline="\n")
    print("wrote:", OUT_LCD_H, OUT_LCD_C, OUT_LTDC_C)


if __name__ == "__main__":
    main()
