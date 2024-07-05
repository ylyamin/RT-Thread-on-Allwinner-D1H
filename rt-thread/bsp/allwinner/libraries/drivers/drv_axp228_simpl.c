/*
 * Copyright (c) ylyamin
 */
#include <rthw.h>
#include <rtdevice.h>

#include "sunxi_hal_twi.h"

#define DBG_LVL DBG_WARNING
#define DBG_TAG "drv/axp"
#include <rtdbg.h>

#define AXP228_ADDR (0x69 >> 1)

#define DCDC_CNTRL_REG 0x10
#define ELDO_CNTRL_REG 0x12
#define ALDO_CNTRL_REG 0x13

#define DLDO1_Voltage_REG 0x15
#define DLDO2_Voltage_REG 0x16
#define DLDO3_Voltage_REG 0x17
#define DLDO4_Voltage_REG 0x18
#define ELDO1_Voltage_REG 0x19
#define ELDO2_Voltage_REG 0x1A
#define ELDO3_Voltage_REG 0x1B
#define DC5LD_Voltage_REG 0x1C
#define DCDC1_Voltage_REG 0x21
#define DCDC2_Voltage_REG 0x22
#define DCDC3_Voltage_REG 0x23
#define DCDC4_Voltage_REG 0x24
#define DCDC5_Voltage_REG 0x25
#define ALDO1_Voltage_REG 0x28
#define ALDO2_Voltage_REG 0x29
#define ALDO3_Voltage_REG 0x2A

//DCDC_CNTRL_REG
#define ALDO2_Enable_mask BIT(7)
#define ALDO1_Enable_mask BIT(6)
#define DCDC5_Enable_mask BIT(5)
#define DCDC4_Enable_mask BIT(4)
#define DCDC3_Enable_mask BIT(3)
#define DCDC2_Enable_mask BIT(2)
#define DCDC1_Enable_mask BIT(1)
#define DC5LD_Enable_mask BIT(0)

//ELDO_CNTRL_REG
#define DC1SW_Enable_mask BIT(7)
#define DLDO4_Enable_mask BIT(6)
#define DLDO3_Enable_mask BIT(5)
#define DLDO2_Enable_mask BIT(4)
#define DLDO1_Enable_mask BIT(3)
#define ELDO3_Enable_mask BIT(2)
#define ELDO2_Enable_mask BIT(1)
#define ELDO1_Enable_mask BIT(0)

//ALDO_CNTRL_REG
#define ALDO3_Enable_mask BIT(7)
struct dcdc_reg
{
    char name[40];
    uint8_t addr;
    uint8_t value;
};

#define ENABLE_REGISTERS {\
            {"DCDC1-5 ALDO1-2 DC5LDO Control",DCDC_CNTRL_REG,0},\
            {"ELDO1-3 DLDO1-4 DC1SW Control ",ELDO_CNTRL_REG,0},\
            {"ADLDO3 Control                ",ALDO_CNTRL_REG,0},\
                         }  

#define VOLTAGE_REGISTERS {\
            {"DLDO1  Voltage Set",DLDO1_Voltage_REG,0},\
            {"DLDO2  Voltage Set",DLDO2_Voltage_REG,0},\
            {"DLDO3  Voltage Set",DLDO3_Voltage_REG,0},\
            {"DLDO4  Voltage Set",DLDO4_Voltage_REG,0},\
            {"ELDO1  Voltage Set",ELDO1_Voltage_REG,0},\
            {"ELDO2  Voltage Set",ELDO2_Voltage_REG,0},\
            {"ELDO3  Voltage Set",ELDO3_Voltage_REG,0},\
            {"DC5LDO Voltage Set",DC5LD_Voltage_REG,0},\
            {"DCDC1  Voltage Set",DCDC1_Voltage_REG,0},\
            {"DCDC2  Voltage Set",DCDC2_Voltage_REG,0},\
            {"DCDC3  Voltage Set",DCDC3_Voltage_REG,0},\
            {"DCDC4  Voltage Set",DCDC4_Voltage_REG,0},\
            {"DCDC5  Voltage Set",DCDC5_Voltage_REG,0},\
            {"ALDO1  Voltage Set",ALDO1_Voltage_REG,0},\
            {"ALDO2  Voltage Set",ALDO2_Voltage_REG,0},\
            {"ALDO3  Voltage Set",ALDO3_Voltage_REG,0},\
    };  

void _axp_set_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t reg_value)
{
    struct rt_i2c_msg msg[1];
    uint8_t buf[2] = {reg_addr,reg_value};

    msg[0].addr = AXP228_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf = buf;
    msg[0].len = sizeof(buf);
    rt_i2c_transfer(i2c_bus, &msg[0], 1);
}

void _axp_get_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t *reg_value)
{
    struct rt_i2c_msg msg[2];
    uint8_t buf[1] = {reg_addr};

    msg[0].addr = AXP228_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].buf = buf;
    msg[0].len = sizeof(buf);

    msg[1].addr = AXP228_ADDR;
    msg[1].flags = RT_I2C_RD;
    msg[1].buf = buf;
    msg[1].len = sizeof(buf);

    rt_i2c_transfer(i2c_bus, &msg[0], 2);
    *reg_value = msg[1].buf[0];
}

void _axp_update_reg(struct rt_i2c_bus_device *i2c_bus, uint8_t reg_addr, uint8_t val, uint8_t mask)
{
    uint8_t reg_val = 0;
    _axp_get_reg(i2c_bus, reg_addr, &reg_val);

    if ((reg_val & mask) != val) {
        reg_val = (reg_val & ~mask) | (val & mask);
        _axp_set_reg(i2c_bus, reg_addr, reg_val);
    }
}

static void _axp_get_regsisters(struct dcdc_reg *dcdc_regs, uint8_t dcdc_regs_len)
{
    uint8_t reg_value = 0;
    struct rt_i2c_bus_device *i2c_bus;
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");

    if (!i2c_bus)
    {
        rt_kprintf("i2c0 not found\n");
        return;
    }

    for (int i =0;i < dcdc_regs_len;i++)
    {
        _axp_get_reg(i2c_bus, dcdc_regs[i].addr, &reg_value);
        dcdc_regs[i].value = reg_value;
        //rt_hw_us_delay(100);
    }
}

void _axp_LCD_control(int on)
{
    uint8_t value = 0;
    struct rt_i2c_bus_device *i2c_bus;
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");

    if (!i2c_bus)
    {
        rt_kprintf("i2c0 not found\n");
        return;
    }

    if (on) value = DCDC3_Enable_mask | ALDO2_Enable_mask;
    _axp_update_reg(i2c_bus, DCDC_CNTRL_REG, value, DCDC3_Enable_mask | ALDO2_Enable_mask);
}

static void _axp_LCD_control_cmd(int argc, char *args[])
{
    if (argc < 2) return;
    int on = atoi(args[1]);
    _axp_LCD_control(on);
}
MSH_CMD_EXPORT_ALIAS(_axp_LCD_control_cmd, axp_LCD_control_cmd, axp_LCD_control_cmd);

void _axp_USB_control(int on)
{
    uint8_t value_1 = 0;
    uint8_t value_2 = 0;
    uint8_t value_3 = 0;

    struct rt_i2c_bus_device *i2c_bus;
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");

    if (!i2c_bus)
    {
        rt_kprintf("i2c0 not found\n");
        return;
    }

    if (on) {
        value_1 = DCDC1_Enable_mask;
        value_2 = DLDO2_Enable_mask;
        value_3 = 0xff;
    }
    _axp_update_reg(i2c_bus, DCDC_CNTRL_REG, value_1, DCDC1_Enable_mask);
    _axp_update_reg(i2c_bus, DLDO2_Voltage_REG, value_3, 0xff);
    _axp_update_reg(i2c_bus, ELDO_CNTRL_REG, value_2, DLDO2_Enable_mask);
}

static void _axp_USB_control_cmd(int argc, char *args[])
{
    if (argc < 2) return;
    int on = atoi(args[1]);
    _axp_USB_control(on);
}
MSH_CMD_EXPORT_ALIAS(_axp_USB_control_cmd, axp_USB_control_cmd, axp_USB_control_cmd);


static void _axp_get_voltage(void)
{
    struct dcdc_reg dcdc_regs[] = VOLTAGE_REGISTERS;
    uint8_t dcdc_regs_len = sizeof(dcdc_regs) / sizeof(struct dcdc_reg);

    _axp_get_regsisters(dcdc_regs, dcdc_regs_len);

    for (int i =0;i < dcdc_regs_len;i++)
    {
        rt_kprintf("Reg name:'%s' addr: %x value: %d\n",
        dcdc_regs[i].name, 
        dcdc_regs[i].addr, 
        dcdc_regs[i].value);
    }
}
MSH_CMD_EXPORT_ALIAS(_axp_get_voltage, axp_get_voltage, axp_get_voltage);

static void _axp_get_enable(void)
{
    struct dcdc_reg dcdc_regs[] = ENABLE_REGISTERS;
    uint8_t dcdc_regs_len = sizeof(dcdc_regs) / sizeof(struct dcdc_reg);

    _axp_get_regsisters(dcdc_regs, dcdc_regs_len);

    for (int i =0;i < dcdc_regs_len;i++)
    {
        rt_kprintf("Reg name:'%s' addr: %x value: %x\n",
        dcdc_regs[i].name, 
        dcdc_regs[i].addr, 
        dcdc_regs[i].value);

        if (dcdc_regs[i].addr == DCDC_CNTRL_REG)
        {
            rt_kprintf("    ALDO2_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO2_Enable_mask));
            rt_kprintf("    ALDO1_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO1_Enable_mask));
            rt_kprintf("    DCDC5_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC5_Enable_mask));
            rt_kprintf("    DCDC4_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC4_Enable_mask));
            rt_kprintf("    DCDC3_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC3_Enable_mask));
            rt_kprintf("    DCDC2_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC2_Enable_mask));
            rt_kprintf("    DCDC1_Enable %d\n",(bool)(dcdc_regs[i].value & DCDC1_Enable_mask));
            rt_kprintf("    DC5LD_Enable %d\n",(bool)(dcdc_regs[i].value & DC5LD_Enable_mask));
        }
        //
        if (dcdc_regs[i].addr == ELDO_CNTRL_REG)
        {
            rt_kprintf("    DC1SW_Enable %d\n",(bool)(dcdc_regs[i].value & DC1SW_Enable_mask));
            rt_kprintf("    DLDO4_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO4_Enable_mask));
            rt_kprintf("    DLDO3_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO3_Enable_mask));
            rt_kprintf("    DLDO2_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO2_Enable_mask));
            rt_kprintf("    DLDO1_Enable %d\n",(bool)(dcdc_regs[i].value & DLDO1_Enable_mask));
            rt_kprintf("    ELDO3_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO3_Enable_mask));
            rt_kprintf("    ELDO2_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO2_Enable_mask));
            rt_kprintf("    ELDO1_Enable %d\n",(bool)(dcdc_regs[i].value & ELDO1_Enable_mask));
        }
        if (dcdc_regs[i].addr == ALDO_CNTRL_REG)
        {
            rt_kprintf("    ALDO3_Enable %d\n",(bool)(dcdc_regs[i].value & ALDO3_Enable_mask));
        }
    }
}
MSH_CMD_EXPORT_ALIAS(_axp_get_enable, axp_get_enable, axp_get_enable);


