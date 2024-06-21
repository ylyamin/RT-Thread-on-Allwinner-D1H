#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <hal_clk.h>
#include <hal_gpio.h>
#include "disp_sys_intf.h"
#include "disp_board_config.h"

#include <rtthread.h>

#include "lcd_cfg.h"

struct property_t *g_lcd0_config = NULL;
static struct lcd_cfg_panel_info _panel_info = DEFAULT_LCD_CONFIG;

extern struct property_t g_lcd0_config_soc[];
extern struct property_t g_lcd1_config[];
extern struct property_t g_disp_config[];
extern u32 g_lcd0_config_len;
extern u32 g_lcd1_config_len;
extern u32 g_disp_config_len;

static struct property_t *_lcd_property_find(const char *name, struct property_t *_config, const u32 config_len)
{
    u32 i;

    for (i = 0; i < config_len; i++)
    {
        if (_config[i].type != PROPERTY_INTGER)
        {
            continue;
        }

        if (!strcmp(name, _config[i].name))
        {
            return _config + i;
        }
    }

    return NULL;
}

#ifndef LCD_XML_CONFIG_FILE
#define LCD_CONFIG_FILE "/etc/lcd_config.xml"
#else
#define LCD_CONFIG_FILE LCD_XML_CONFIG_FILE
#endif
const struct lcd_cfg_panel_info *load_lcd_config_from_xml(void)
{
    struct property_t *_config_item;
    static rt_uint8_t init_state = 0;

    rt_enter_critical();
    if (init_state == 0)
    {
        init_state = 1;
        rt_exit_critical();
    }
    else
    {
        rt_exit_critical();

        while (init_state != 2)
        {
            rt_thread_mdelay(10);
        }
        goto _RET;
    }

    g_lcd0_config = g_lcd0_config_soc;

_RET:

    init_state = 2;
    _config_item = _lcd_property_find("lcd_x", g_lcd0_config, g_lcd0_config_len);
    if(_config_item != NULL)
    {
        _panel_info.width = _config_item->v.value;
    }    
    _config_item = _lcd_property_find("lcd_y", g_lcd0_config, g_lcd0_config_len);
    if(_config_item != NULL)
    {
        _panel_info.height = _config_item->v.value;
    }
    _config_item = _lcd_property_find("bl_pin", g_lcd0_config, g_lcd0_config_len);
    if(_config_item != NULL)
    {
        _panel_info.bl_pin = _config_item->v.value;
        _panel_info.bl_gpio_pin = _config_item->v.value;
    }
    _config_item = _lcd_property_find("bl_level", g_lcd0_config, g_lcd0_config_len);
    if(_config_item != NULL)
    { 
        _panel_info.bl_level = _config_item->v.value;
        _panel_info.bl_gpio_level = _config_item->v.value;
    }
    _panel_info.bl_mode = 0;
    _panel_info.pwr_pin = -1;

    return &_panel_info;
}
