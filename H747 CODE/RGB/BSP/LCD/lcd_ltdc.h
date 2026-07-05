/**
 ****************************************************************************************************
 * @file        lcd_ltdc.h
 * @brief       LTDC RGB LCD driver (BSP layer, works with CubeMX LTDC init)
 ****************************************************************************************************
 */

#ifndef LCD_LTDC_H
#define LCD_LTDC_H

#include "main.h"
#include "ltdc.h"
#include "dma2d.h"

/* 0: 没有使用8寸800X1280 RGB屏; 1: 使用8寸800X1280 RGB屏 */
#define RGB_80_8001280       0

/* CubeMX 已生成 MX_LTDC_Init / MX_DMA2D_Init 时置 1 */
#define BSP_USE_CUBEMX_LTDC  1

typedef struct
{
    uint32_t pwidth;
    uint32_t pheight;
    uint16_t hsw;
    uint16_t vsw;
    uint16_t hbp;
    uint16_t vbp;
    uint16_t hfp;
    uint16_t vfp;
    uint8_t activelayer;
    uint8_t dir;
    uint16_t width;
    uint16_t height;
    uint32_t pixsize;
    uint8_t pixformat;
} _ltdc_dev;

extern _ltdc_dev lcdltdc;

#define LTDC_PIXFORMAT_ARGB8888      0X00
#define LTDC_PIXFORMAT_RGB888        0X01
#define LTDC_PIXFORMAT_RGB565        0X02
#define LTDC_PIXFORMAT_ARGB1555      0X03
#define LTDC_PIXFORMAT_ARGB4444      0X04
#define LTDC_PIXFORMAT_L8            0X05
#define LTDC_PIXFORMAT_AL44          0X06
#define LTDC_PIXFORMAT_AL88          0X07

#define LTDC_DE_GPIO_PORT               GPIOK
#define LTDC_DE_GPIO_PIN                GPIO_PIN_7
#define LTDC_DE_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOK_CLK_ENABLE(); }while(0)

#define LTDC_VSYNC_GPIO_PORT            GPIOI
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_13
#define LTDC_VSYNC_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)

#define LTDC_HSYNC_GPIO_PORT            GPIOI
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_12
#define LTDC_HSYNC_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)

#define LTDC_CLK_GPIO_PORT              GPIOI
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_14
#define LTDC_CLK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)

#define LTDC_BL_GPIO_PORT               GPIOB
#define LTDC_BL_GPIO_PIN                GPIO_PIN_0
#define LTDC_BL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define LTDC_RST_GPIO_PORT              GPIOH
#define LTDC_RST_GPIO_PIN               GPIO_PIN_5
#define LTDC_RST_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)

#define LTDC_PIXFORMAT           LTDC_PIXEL_FORMAT_RGB565
#define LTDC_BACKLAYERCOLOR      0X00000000
#define LTDC_FRAME_BUF_ADDR      0XD0000000U

#define LTDC_BL(x)      do{ (x) ? \
                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET); \
                        }while(0)

#define LTDC_RST(x)     do{ (x) ? \
                            HAL_GPIO_WritePin(LTDC_RST_GPIO_PORT, LTDC_RST_GPIO_PIN, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(LTDC_RST_GPIO_PORT, LTDC_RST_GPIO_PIN, GPIO_PIN_RESET); \
                        }while(0)

void ltdc_switch(uint8_t sw);
void ltdc_layer_switch(uint8_t layerx, uint8_t sw);
void ltdc_select_layer(uint8_t layerx);
void ltdc_display_dir(uint8_t dir);
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color);
uint32_t ltdc_read_point(uint16_t x, uint16_t y);
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void ltdc_clear(uint32_t color);
uint8_t ltdc_clk_set(uint32_t pll3n, uint32_t pll3m, uint32_t pll3r);
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor);
uint16_t ltdc_panelid_read(void);
void ltdc_init(void);
void ltdc_gpio_init(void);

#endif /* LCD_LTDC_H */
