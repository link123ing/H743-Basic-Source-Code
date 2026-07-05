# LVGL 移植说明（例程39 → RGB_LVGL）

## 工程位置

`H:\STM32H7\RGB_LVGL`

LVGL 库使用同目录下 **例程39 压力测试** 中的 `Middlewares/LVGL`（与官方例程一致）。

## 已添加/修改的文件

| 路径 | 说明 |
|------|------|
| `BSP/LVGL/lv_port_disp.c` | 显示移植（`lcd_color_fill` / RGB565） |
| `BSP/LVGL/lv_port_indev.c` | 输入移植占位（触摸可后续接入） |
| `CM7/Core/Src/lvgl_demo.c` | FreeRTOS 任务 + `lv_demo_stress` |
| `tools/gen_lvgl_mk_from_uvprojx.py` | 从 MDK 工程生成 `Makefile/CM7/lvgl.mk` |
| `Makefile/CM7/stm32h747xx_flash_CM7.ld` | 增加 AXI SRAM 512KB 给 LVGL 大缓冲区 |

## 编译步骤

```bat
cd /d H:\STM32H7\RGB_LVGL\Makefile\CM7
make all -j4
```

`Middlewares/LVGL` 需在工程根目录下（与 CubeMX 的 `Middlewares/FreeRTOS` 同级）。若缺失，从例程39 复制 `Middlewares/LVGL` 到 `RGB_LVGL\Middlewares\` 即可，**不必**再跑任何 Python 脚本。

需将 **GNU Arm Embedded Toolchain** 的 `bin` 目录加入 `PATH`（`arm-none-eabi-gcc`）。

## 运行流程

1. SDRAM / MPU / LTDC 初始化（与实验12 BSP 相同）
2. `lcd_init()` → 串口打印 LCD ID
3. `osKernelInitialize()` → `lvgl_app_init()` → `lvgl_start_task()` → `osKernelStart()`
4. LVGL 任务中运行 **压力测试 Demo**，`vApplicationTickHook` 中 `lv_tick_inc(1)`

## 配置要点（与例程39 一致）

- `lv_conf.h`：`LV_COLOR_DEPTH 16`，`LV_USE_DEMO_STRESS 1`
- 帧缓冲：`LTDC_FRAME_BUF_ADDR = 0xD0000000`（SDRAM）
- 分辨率：800×480，竖屏显示 `lcd_display_dir(1)`

## 触摸（可选）

接入 `BSP/TOUCH` 后，在 `lv_port_indev.c` 中参考例程39 的 `lv_port_indev_template.c` 实现即可。
