#include "panels.h"
#include "NV3051F.h"

#define panel_rst(v)    (sunxi_lcd_gpio_set_value(0, 0, v))
#define panel_bl_enable(v)    (sunxi_lcd_gpio_set_value(0, 1, v))

static void LCD_power_on(u32 sel);
static void LCD_power_off(u32 sel);
static void LCD_bl_open(u32 sel);
static void LCD_bl_close(u32 sel);

static void LCD_panel_init(u32 sel);
static void LCD_panel_exit(u32 sel);

static void LCD_cfg_panel_info(struct panel_extend_para * info)
{

}

static s32 LCD_open_flow(u32 sel)
{
    printk("raoyiming +++ LCD_open_flow\n");
   // LCD_OPEN_FUNC(sel, LCD_power_on, 100);   //open lcd power, and delay 50ms
    LCD_OPEN_FUNC(sel, LCD_panel_init, 0);   //open lcd power, than delay 200ms
    LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 200);     //open lcd controller, and delay 100ms
    LCD_OPEN_FUNC(sel, LCD_bl_open, 0);     //open lcd backlight, and delay 0ms
    printk("raoyiming +++ LCD_open_flow finish\n");
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

extern void _axp_LCD_control(bool on);

static void LCD_panel_init(u32 sel)
{
	
    u32 i;
    printk("<0>raoyiming +++ LCD_panel_init\n");
	sunxi_lcd_power_enable(sel, 0);
    sunxi_lcd_pin_cfg(sel, 1);

    /*all off*/
    _axp_LCD_control(0);
    sunxi_lcd_delay_ms(100);

    /*start*/
    panel_rst(1);
    _axp_LCD_control(1);

    /*T2*/
    sunxi_lcd_delay_ms(10);
    panel_rst(0);

    /*T3*/
    sunxi_lcd_delay_ms(220);
    panel_rst(1);

    sunxi_lcd_dsi_clk_enable(sel);
	/* T6 */
	sunxi_lcd_delay_ms(120);
    printk("<0>raoyiming +++ LCD_panel_init finish\n");
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

struct __lcd_panel NV3051F_panel = {
    /* panel driver name, must mach the name of lcd_drv_name in sys_config.fex */
    .name = "NV3051F",
    .func = {
        .cfg_panel_info = LCD_cfg_panel_info,
        .cfg_open_flow = LCD_open_flow,
        .cfg_close_flow = LCD_close_flow,
        .lcd_user_defined_func = LCD_user_defined_func,
    },
};
