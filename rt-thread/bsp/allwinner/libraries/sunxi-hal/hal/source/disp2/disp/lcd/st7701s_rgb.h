/*
 * Allwinner SoCs display driver.
 *
 * Copyright (C) 2021 Sipeed.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __ST7701S_RGB_PANEL_H__
#define  __ST7701S_RGB_PANEL_H__

#include "panels.h"
void LCD_panel_init(u32 sel);

extern struct __lcd_panel st7701s_rgb_panel;

#endif
