#ifndef  __CWU50_H__
#define  __CWU50_H__

#include "panels.h"

#define panel_rst(v)    (sunxi_lcd_gpio_set_value(0, 0, v))
#define panel_bl_enable(v)    (sunxi_lcd_gpio_set_value(0, 1, v))
extern struct __lcd_panel cwu50_panel;

#endif