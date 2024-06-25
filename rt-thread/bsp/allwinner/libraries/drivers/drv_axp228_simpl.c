/*
 * Copyright (c) ylyamin
 */
#include <rthw.h>
#include <rtdevice.h>

#include "sunxi_hal_twi.h"

#define DBG_LVL DBG_WARNING
#define DBG_TAG "drv/axp"
#include <rtdbg.h>

struct dcdc_reg
{
    char name[20];
    uint8_t addr;
    uint8_t value;
};

static void _axp_voltage(void)
{
    struct rt_i2c_bus_device *i2c_bus;
    struct rt_i2c_msg msg[2];
    uint8_t one_buf[1] = {0x00};

    struct dcdc_reg dcdc_regs[17] = {
            {"DCDC1-5 Enable Set",0x10,0x00},
            {"DLDO1  Voltage Set",0x15,0x00},
            {"DLDO2  Voltage Set",0x16,0x00},
            {"DLDO3  Voltage Set",0x17,0x00},
            {"DLDO4  Voltage Set",0x18,0x00},
            {"ELDO1  Voltage Set",0x19,0x00},
            {"ELDO2  Voltage Set",0x1A,0x00},
            {"ELDO3  Voltage Set",0x1B,0x00},
            {"DC5LDO Voltage Set",0x1C,0x00},
            {"DCDC1  Voltage Set",0x21,0x00},
            {"DCDC2  Voltage Set",0x22,0x00},
            {"DCDC3  Voltage Set",0x23,0x00},
            {"DCDC4  Voltage Set",0x24,0x00},
            {"DCDC5  Voltage Set",0x25,0x00},
            {"ALDO1  Voltage Set",0x28,0x00},
            {"ALDO2  Voltage Set",0x29,0x00},        
            {"ALDO3  Voltage Set",0x2A,0x00},
    };   

    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");

    if (!i2c_bus)
    {
        rt_kprintf("i2c0 not found\n");
        return;
    }

    msg[0].addr = (0x69 >> 1);
    msg[0].flags = 0;
    msg[0].buf = one_buf;
    msg[0].len = sizeof(one_buf);

    msg[1].addr = (0x69 >> 1);
    msg[1].flags = RT_I2C_RD;
    msg[1].buf = one_buf;
    msg[1].len = sizeof(one_buf);

    for (int i =0;i < 17;i++)
    {
        one_buf[0] = dcdc_regs[i].addr;
        rt_i2c_transfer(i2c_bus, &msg[0], 2);
        rt_hw_us_delay(1000);
        dcdc_regs[i].value = msg[1].buf[0];
    }

    for (int i =0;i < 17;i++)
    {
        rt_kprintf("Reg name:'%s' addr: %x value: %d\n",dcdc_regs[i].name, dcdc_regs[i].addr, dcdc_regs[i].value);
    }
}
MSH_CMD_EXPORT_ALIAS(_axp_voltage, axp_voltage, axp_voltage);

void _axp_ALDO2_DCDC3_control(int on)
{
    struct rt_i2c_bus_device *i2c_bus;
    struct rt_i2c_msg msg[2];

    uint8_t reg_addr_buf[2];
    
    if (on)
    {
        reg_addr_buf[0] = 0x10;
        reg_addr_buf[1] = 0x8B; //ALDO2 and DCDC3 Enable
    }else
    {
        reg_addr_buf[0] = 0x10;
        reg_addr_buf[1] = 0x3;  //ALDO2 and DCDC3 Disble
    }
    
    i2c_bus = (struct rt_i2c_bus_device*)rt_device_find("i2c0");

    if (!i2c_bus)
    {
        rt_kprintf("i2c0 not found\n");
        return;
    }

    msg[0].addr = (0x69 >> 1);
    msg[0].flags = 0;
    msg[0].buf = reg_addr_buf;
    msg[0].len = sizeof(reg_addr_buf);

    rt_i2c_transfer(i2c_bus, &msg[0], 1);
}

static void _axp_ALDO2_DCDC3_control_cmd(int argc, char *args[])
{
    if (argc < 2) return;
    int on = atoi(args[1]);
    rt_kprintf("ALDO2_DCDC3 On/Off %d\n",on);
    _axp_ALDO2_DCDC3_control(on);
}
MSH_CMD_EXPORT_ALIAS(_axp_ALDO2_DCDC3_control_cmd, axp_ALDO2_DCDC3_control_cmd, axp_ALDO2_DCDC3_control_cmd);


