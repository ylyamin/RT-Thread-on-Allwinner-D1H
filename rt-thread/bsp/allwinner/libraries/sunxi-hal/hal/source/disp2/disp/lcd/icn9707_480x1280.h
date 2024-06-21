#ifndef  __ICN9707_480X1280_H__
#define  __ICN9707_480X1280_H__

#include "panels.h"

#define panel_rst(v)    (sunxi_lcd_gpio_set_value(0, 0, v))
#define panel_bl_enable(v)    (sunxi_lcd_gpio_set_value(0, 1, v))
extern struct __lcd_panel icn9707_480x1280_panel;

#endif
