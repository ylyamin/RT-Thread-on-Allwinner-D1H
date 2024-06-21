#include "panels.h"
#include "icn9707_480x1280.h"

static void LCD_power_on(u32 sel);
static void LCD_power_off(u32 sel);
static void LCD_bl_open(u32 sel);
static void LCD_bl_close(u32 sel);

static void LCD_panel_init(u32 sel);
static void LCD_panel_exit(u32 sel);

static void LCD_cfg_panel_info(struct panel_extend_para * info)
{

    printk("raoyiming +++LCD_cfg_panel_info\n");
    return;
    u32 i = 0, j=0;
    u32 items;
    u8 lcd_gamma_tbl[][2] =
    {
        //{input value, corrected value}
        {0, 0},
        {15, 15},
        {30, 30},
        {45, 45},
        {60, 60},
        {75, 75},
        {90, 90},
        {105, 105},
        {120, 120},
        {135, 135},
        {150, 150},
        {165, 165},
        {180, 180},
        {195, 195},
        {210, 210},
        {225, 225},
        {240, 240},
        {255, 255},
    };

    u8 lcd_bright_curve_tbl[][2] =
    {
        //{input value, corrected value}
        {0    ,0  },//0
        {15   ,3  },//0
        {30   ,6  },//0
        {45   ,9  },// 1
        {60   ,12  },// 2
        {75   ,16  },// 5
        {90   ,22  },//9
        {105   ,28 }, //15
        {120  ,36 },//23
        {135  ,44 },//33
        {150  ,54 },
        {165  ,67 },
        {180  ,84 },
        {195  ,108},
        {210  ,137},
        {225 ,171},
        {240 ,210},
        {255 ,255},
    };

    u32 lcd_cmap_tbl[2][3][4] = {
    {
        {LCD_CMAP_G0,LCD_CMAP_B1,LCD_CMAP_G2,LCD_CMAP_B3},
        {LCD_CMAP_B0,LCD_CMAP_R1,LCD_CMAP_B2,LCD_CMAP_R3},
        {LCD_CMAP_R0,LCD_CMAP_G1,LCD_CMAP_R2,LCD_CMAP_G3},
        },
        {
        {LCD_CMAP_B3,LCD_CMAP_G2,LCD_CMAP_B1,LCD_CMAP_G0},
        {LCD_CMAP_R3,LCD_CMAP_B2,LCD_CMAP_R1,LCD_CMAP_B0},
        {LCD_CMAP_G3,LCD_CMAP_R2,LCD_CMAP_G1,LCD_CMAP_R0},
        },
    };

    //memset(info,0,sizeof(panel_extend_para));

    items = sizeof(lcd_gamma_tbl)/2;
    for(i=0; i<items-1; i++) {
        u32 num = lcd_gamma_tbl[i+1][0] - lcd_gamma_tbl[i][0];

        for(j=0; j<num; j++) {
            u32 value = 0;

            value = lcd_gamma_tbl[i][1] + ((lcd_gamma_tbl[i+1][1] - lcd_gamma_tbl[i][1]) * j)/num;
            info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] = (value<<16) + (value<<8) + value;
        }
    }
    info->lcd_gamma_tbl[255] = (lcd_gamma_tbl[items-1][1]<<16) + (lcd_gamma_tbl[items-1][1]<<8) + lcd_gamma_tbl[items-1][1];

    items = sizeof(lcd_bright_curve_tbl)/2;
    for(i=0; i<items-1; i++) {
        u32 num = lcd_bright_curve_tbl[i+1][0] - lcd_bright_curve_tbl[i][0];

        for(j=0; j<num; j++) {
            u32 value = 0;

            value = lcd_bright_curve_tbl[i][1] + ((lcd_bright_curve_tbl[i+1][1] - lcd_bright_curve_tbl[i][1]) * j)/num;
            info->lcd_bright_curve_tbl[lcd_bright_curve_tbl[i][0] + j] = value;
        }
    }
    info->lcd_bright_curve_tbl[255] = lcd_bright_curve_tbl[items-1][1];

    memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));
}

static s32 LCD_open_flow(u32 sel)
{
    printk("raoyiming +++ LCD_open_flow\n");
    LCD_OPEN_FUNC(sel, LCD_power_on, 100);   //open lcd power, and delay 50ms
    LCD_OPEN_FUNC(sel, LCD_panel_init, 200);   //open lcd power, than delay 200ms
    LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 200);     //open lcd controller, and delay 100ms
    LCD_OPEN_FUNC(sel, LCD_bl_open, 0);     //open lcd backlight, and delay 0ms

    return 0;
}

static s32 LCD_close_flow(u32 sel)
{
    LCD_CLOSE_FUNC(sel, LCD_bl_close, 0);       //close lcd backlight, and delay 0ms
    LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 0);         //close lcd controller, and delay 0ms
    LCD_CLOSE_FUNC(sel, LCD_panel_exit, 200);   //open lcd power, than delay 200ms
    printk("raoyiming +++ LCD_close_flow\n");
    LCD_CLOSE_FUNC(sel, LCD_power_off, 500);   //close lcd power, and delay 500ms

    return 0;
}

static void LCD_power_on(u32 sel)
{
    sunxi_lcd_power_enable(sel, 0);//config lcd_power pin to open lcd power0
    sunxi_lcd_pin_cfg(sel, 1);

	sunxi_lcd_delay_us(100);
//	sunxi_lcd_gpio_set_value(0,1,1);//stby
	sunxi_lcd_delay_ms(1);
	printk("<0>raoyiming +++ sunxi_lcd_gpio_set_value\n");
	//sunxi_lcd_gpio_set_value(0,0,1);//reset
}

static void LCD_power_off(u32 sel)
{
    sunxi_lcd_pin_cfg(sel, 0);
    sunxi_lcd_power_disable(sel, 0);//config lcd_power pin to close lcd power0
}

static void LCD_bl_open(u32 sel)
{
    printk("raoyiming +++ LCD_bl_open\n");
    sunxi_lcd_pwm_enable(sel);//open pwm module
    panel_bl_enable(1);//config lcd_bl_en pin to open lcd backlight
    //sunxi_lcd_backlight_disable(sel);//config lcd_bl_en pin to close lcd backlight
    //sunxi_lcd_pwm_disable(sel);//close pwm module
}

static void LCD_bl_close(u32 sel)
{
    panel_bl_enable(0);//config lcd_bl_en pin to close lcd backlight
    sunxi_lcd_pwm_disable(sel);//close pwm module
}

#define REGFLAG_END_OF_TABLE     0x102
#define REGFLAG_DELAY            0x101

struct lcd_setting_table {
    u16 cmd;
    u32 count;
    u8 para_list[64];
};

static struct lcd_setting_table lcd_init_setting[] = {

	{0xF0, 2, {0x5A, 0x59} },
	{0xF1, 2, {0xA5, 0xA6} },
	{0xB0, 14, {0x54,0x32,0x23,0x45,0x44,0x44,0x44,0x44,0x9F,0x00,0x01,0x9F,0x00,0x01} },
	{0xB1, 10, {0x32,0x84,0x02,0x83,0x29,0x06,0x06,0x72,0x06,0x06} },
	{0xB2, 1, {0x73} },
	{0xB3, 20, {0x0B,0x09,0x13,0x11,0x0F,0x0D,0x00,0x00,0x00,0x03,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x05,0x07} },
	{0xB4, 20, {0x0A,0x08,0x12,0x10,0x0E,0x0C,0x00,0x00,0x00,0x03,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x04,0x06} },
	{0xB6, 2, {0x13,0x13} },
	{0xB8, 4, {0xB4,0x43,0x02,0xCC} },
	{0xB9, 4, {0xA5,0x20,0xFF,0xC8} },
	{0xBA, 2, {0x88,0x23} },
	{0xBD, 10, {0x43,0x0E,0x0E,0x50,0x50,0x29,0x10,0x03,0x44,0x03} },
	{0xC1, 8, {0x00,0x0C,0x16,0x04,0x00,0x30,0x10,0x04} },
	{0xC2, 2, {0x21,0x81} },
	{0xC3, 2, {0x02,0x30} },
	{0xC7, 2, {0x25,0x6A} },
	{0xC8, 38, {0x7C,0x68,0x59,0x4E,0x4B,0x3C,0x41,0x2B,0x44,0x43,0x43,0x60,0x4E,0x55,0x47,0x44,0x38,0x27,0x06,0x7C,0x68,0x59,0x4E,0x4B,0x3C,0x41,0x2B,0x44,0x43,0x43,0x60,0x4E,0x55,0x47,0x44,0x38,0x27,0x06} },
	{0xD4, 6, {0x00,0x00,0x00,0x32,0x04,0x51} },
	{0xF1, 2, {0x5A,0x59} },
	{0xF0, 2, {0xA5,0xA6} },
	{0x36, 1, {0x14} },
	{0x35, 1, {0x00} },
	
	{0x11, 1, {0x00} },
	{REGFLAG_DELAY, 120, {} },
	{0x29, 1, {0x00} },	
	{REGFLAG_DELAY, 20, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }

};

static void LCD_panel_init(u32 sel)
{
	
    u32 i;
    printk("<0>raoyiming +++ LCD_panel_init\n");
	
    /**/
    panel_rst(1);
    sunxi_lcd_delay_ms(10);
    panel_rst(0);
    sunxi_lcd_delay_ms(50);
    panel_rst(1);
    sunxi_lcd_delay_ms(200);

    for (i = 0; ; i++) {
        if(lcd_init_setting[i].cmd == REGFLAG_END_OF_TABLE) {
            break;
        } else if (lcd_init_setting[i].cmd == REGFLAG_DELAY) {
            sunxi_lcd_delay_ms(lcd_init_setting[i].count);
        } else {
            dsi_dcs_wr(sel, (u8)lcd_init_setting[i].cmd, lcd_init_setting[i].para_list, lcd_init_setting[i].count);
        }
    }

   sunxi_lcd_dsi_clk_enable(sel);

    return;
}

static void LCD_panel_exit(u32 sel)
{
    sunxi_lcd_dsi_clk_disable(sel);
    panel_rst(0);
    return ;
}

//sel: 0:lcd0; 1:lcd1
static s32 LCD_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
    return 0;
}

struct __lcd_panel icn9707_480x1280_panel = {
    /* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
    .name = "icn9707_480x1280",
    .func = {
        .cfg_panel_info = LCD_cfg_panel_info,
        .cfg_open_flow = LCD_open_flow,
        .cfg_close_flow = LCD_close_flow,
        .lcd_user_defined_func = LCD_user_defined_func,
    },
};