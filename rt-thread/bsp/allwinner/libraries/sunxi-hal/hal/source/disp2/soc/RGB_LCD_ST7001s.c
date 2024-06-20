/*
 * disp_board_config.c
 *
 * Copyright (c) 2007-2020 Allwinnertech Co., Ltd.
 * Author: zhengxiaobin <zhengxiaobin@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hal_clk.h>
#include <hal_gpio.h>
#include "../disp/disp_sys_intf.h"
#include "disp_board_config.h"


struct property_t g_lcd0_config_soc[] = {
    {
        .name = "lcd_used",
        .type = PROPERTY_INTGER,
        .v.value = 1,
    },
    {
        .name = "lcd_driver_name",
        .type = PROPERTY_STRING,
        .v.str = "st7701s_rgb",
    },
    {
        .name = "lcd_if",
        .type = PROPERTY_INTGER,
        .v.value = 0, /* LCD_IF_HV */
    },
    {
        .name = "lcd_hv_if",
        .type = PROPERTY_INTGER,
        .v.value = 0, /* LCD_HV_IF_PRGB_1CYC */
    },
    {
        .name = "lcd_x",
        .type = PROPERTY_INTGER,
        .v.value = 480, //
    },
    {
        .name = "lcd_y",
        .type = PROPERTY_INTGER,
        .v.value = 272,
    },
    {
        .name = "lcd_width",
        .type = PROPERTY_INTGER,
        .v.value = 80,
    },
    {
        .name = "lcd_height",
        .type = PROPERTY_INTGER,
        .v.value = 72,
    },
    {
        .name = "lcd_dclk_freq",
        .type = PROPERTY_INTGER,
        .v.value = 12,
    },
    /* lcd_ht = lcd_x + lcd_hspw + lcd_hbp + lcd_hfp */
    {
        .name = "lcd_ht",
        .type = PROPERTY_INTGER,
        .v.value = 612,
    },
    {
        .name = "lcd_hbp",
        .type = PROPERTY_INTGER,
        .v.value = 60,
    },
    {
        .name = "lcd_hspw",
        .type = PROPERTY_INTGER,
        .v.value = 12,
    },
    /* lcd_vt = lcd_y + lcd_vspw + lcd_vbp + lcd_vfp */
    {
        .name = "lcd_vt",
        .type = PROPERTY_INTGER,
        .v.value = 520,
    },
    {
        .name = "lcd_vspw",
        .type = PROPERTY_INTGER,
        .v.value = 4,
    },
    {
        .name = "lcd_vbp",
        .type = PROPERTY_INTGER,
        .v.value = 18,
    },
    {
        .name = "lcd_frm",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "lcd_io_phase",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "lcd_gamma_en",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "lcd_bright_curve_en",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "lcd_cmap_en",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "deu_mode",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "lcdgamma4iep",
        .type = PROPERTY_INTGER,
        .v.value = 22,
    },
    {
        .name = "smart_color",
        .type = PROPERTY_INTGER,
        .v.value = 90,
    },
    {
        .name = "lcd_pwm_used",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    // from drv_lcd
    {
        .name = "bl_pin",
        .type = PROPERTY_INTGER,
        .v.value = GPIO_PD22,
    },
    {
        .name = "bl_level",
        .type = PROPERTY_INTGER,
        .v.value = 1,
    },
    // backlight enable
    /*{
        .name = "lcd_gpio_1",
        .type = PROPERTY_GPIO,
        .v.gpio_list = {
            .gpio = GPIOD(22),
            .mul_sel = GPIO_DIRECTION_OUTPUT,
            .pull = 0,
            .drv_level = 3,
            .data = 1,
        },
    },*/
    /* POWER-vcc */
    {
        .name = "lcd_gpio_0", //RST
        .type = PROPERTY_GPIO,
        .v.gpio_list = {
            .gpio = GPIOG(13),
            .mul_sel = GPIO_DIRECTION_OUTPUT,
            .pull = 1,
            .drv_level = 3,
            .data = 0,
        },
    },

    {
        .name = "lcd_gpio_1", //CS - 16?
        .type = PROPERTY_GPIO,
        .v.gpio_list = {
            .gpio = GPIOE(14),//14
            .mul_sel = GPIO_DIRECTION_OUTPUT,
            .pull = 1,
            .drv_level = 3,
            .data = 1,
        },
    },

    {
        .name = "lcd_gpio_2", //SDI
        .type = PROPERTY_GPIO,
        .v.gpio_list = {
            .gpio = GPIOE(12),//12
            .mul_sel = GPIO_DIRECTION_OUTPUT,
            .pull = 1,
            .drv_level = 3,
            .data = 1,
        },
    },
    {
        .name = "lcd_gpio_3", //SCL
        .type = PROPERTY_GPIO,
        .v.gpio_list = {
            .gpio = GPIOE(15),//15
            .mul_sel = GPIO_DIRECTION_OUTPUT,
            .pull = 1,
            .drv_level = 3,
            .data = 1,
        },
    },


    // gpio
    {
        .name = "LCD0_D3",     // 1 - 5 (5)
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(1),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D4",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(2),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D5",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(3),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D6",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(4),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D7",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(5),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D10",     //6 - 11 (6)
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(6),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D11",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(7),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D12",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(8),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D13",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(9),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D14",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(10),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D15",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(11),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D19",     //13 - 17 (5)
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(13),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D20",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(14),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D21",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(15),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D22",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(16),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_D23",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(17),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_CLK",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(18),      //18 - 21 (4)
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_DE",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(19),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_HSYNC",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(20),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    },
    {
        .name = "LCD0_VSYNC",
        .type = PROPERTY_PIN,
        .v.gpio_list = {
            .gpio = GPIOD(21),
            .mul_sel = 2,
            .pull = 0,
            .drv_level = 3,
        },
    }};

struct property_t g_lcd1_config[] = {
    {
        .name = "lcd_used",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
};

struct property_t g_disp_config[] = {
    {
        .name = "disp_init_enable",
        .type = PROPERTY_INTGER,
        .v.value = 1,
    },
    {
        .name = "disp_mode",
        .type = PROPERTY_INTGER,
        .v.value = 0, /* DISP_INIT_MODE_SCREEN0 */
    },
    {
        .name = "screen0_output_type",
        .type = PROPERTY_INTGER,
        .v.value = 1, /* DISP_OUTPUT_TYPE_LCD */
    },
    {
        .name = "screen0_output_mode",
        .type = PROPERTY_INTGER,
        .v.value = 4,
    },
    {
        .name = "screen0_output_format",
        .type = PROPERTY_INTGER,
        .v.value = 1,
    },
    {
        .name = "screen0_output_bits",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "screen0_output_eotf",
        .type = PROPERTY_INTGER,
        .v.value = 4,
    },
    {
        .name = "screen0_output_cs",
        .type = PROPERTY_INTGER,
        .v.value = 257,
    },
    {
        .name = "screen0_output_dvi_hdmi",
        .type = PROPERTY_INTGER,
        .v.value = 2,
    },
    {
        .name = "screen0_output_range",
        .type = PROPERTY_INTGER,
        .v.value = 2,
    },
    {
        .name = "screen0_output_scan",
        .type = PROPERTY_INTGER,
        .v.value = 0,
    },
    {
        .name = "screen0_output_aspect_ratio",
        .type = PROPERTY_INTGER,
        .v.value = 8,
    },
    {
        .name = "screen1_output_type",
        .type = PROPERTY_INTGER,
        .v.value = 0, // NONE
    },
};


u32 g_lcd0_config_len = sizeof(g_lcd0_config_soc) / sizeof(struct property_t);
u32 g_lcd1_config_len = sizeof(g_lcd1_config) / sizeof(struct property_t);
u32 g_disp_config_len = sizeof(g_disp_config) / sizeof(struct property_t);


/*
LCD             RV_Dock_EXT_3517    GPIO Function

1   LEDK    -   pd22_rgn_bl (5v)
2   LEDA
3   GND
4   VCC     -   out 3.3V
5   R0      -       (gnd)
6   R1      -       (gnd)
7   R2      -   pd12        -   LCD0-D18
8   R3          .
9   R4          .
10  R5          .
11  R6          .
12  R7      -   pd17        -   LCD0-D23
13  G0      -       (gnd)
14  G1      -       (gnd)
15  G2      -   pd6         -   LCD0-D10
16  G3          .
17  G4          .
18  G5          .
19  G6          .
20  G7      -   pd11        -   LCD0-D15
21  B0      -       (gnd)
22  B1      -       (gnd)
23  B2      -   pd0         -   LCD0-D2
24  B3          .
25  B4          .
26  B5          .
27  B6          .
28  B7      -   pd5         -   LCD0-D7
29  GND
30  CLK     -   pd18        -   LCD0-CLK
31  DISP    -   (NC)
32  Hsync   -   pd20        -   LCD0-HSYNK
33  Vsync   -   pd21        -   LCD0-VSYNK
34  DEN     -   pd19        -   LCD0-DE
35  NC      -   (NC)
36  GND
37  XR      -   pg13        -   Reset ?
38  YD      -   pe12
39  XL      -   pe15
40  YU      -   pe16

*/