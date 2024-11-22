/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <stdio.h>

int main(void)
{
    printf("Hello RISC-V\n");
    return 0;
}


void tcon_dump_regs(void)
{

rt_kprintf("\n\rStep 1 Select HV interface type\n\n\r");

rt_kprintf("	TCON_LCD0->LCD_CTL_REG      0x040 		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x040)	);
rt_kprintf("	TCON_LCD0->LCD_HV_IF_REG    0x058		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x058)	);

rt_kprintf("\n\rStep 2 Clock configuration\n\n\r");

rt_kprintf("	CCU->PLL_PERI_CTRL_REG      0x020       %08x\n\r",  *(uint32_t *)( 0x02001000 + 0x020)	);
rt_kprintf("	CCU->PLL_VIDEO0_CTRL_REG    0x040       %08x\n\r",  *(uint32_t *)( 0x02001000 + 0x040)	);
rt_kprintf("	CCU->TCONLCD_CLK_REG        0xB60		%08x\n\r",  *(uint32_t *)( 0x02001000 + 0xB60)	);
rt_kprintf("	CCU->TCONLCD_BGR_REG        0xB7C		%08x\n\r",  *(uint32_t *)( 0x02001000 + 0xB7C)	);
rt_kprintf("	TCON_LCD0->LCD_DCLK_REG     0x044		%08x\n\r",  *(uint32_t *)( 0x05461000 + 0x044)	);
rt_kprintf("	CCU->DSI_CLK_REG            0xB24		%08x\n\r",  *(uint32_t *)( 0x02001000 + 0xB24)	);
rt_kprintf("	CCU->DSI_BGR_REG            0xB4C		%08x\n\r",  *(uint32_t *)( 0x02001000 + 0xB4C)	);
rt_kprintf("	CCU->LVDS_BGR_REG           0xBAC		%08x\n\r",  *(uint32_t *)( 0x02001000 + 0xBAC)	);

rt_kprintf("\n\rStep 3 Set sequence parameters\n\n\r");

rt_kprintf("	TCON_LCD0->LCD_BASIC0_REG   0x048		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x048)	);	
rt_kprintf("	TCON_LCD0->LCD_BASIC1_REG   0x04C		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x04C)	);		
rt_kprintf("	TCON_LCD0->LCD_BASIC2_REG   0x050		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x050)	);		
rt_kprintf("	TCON_LCD0->LCD_BASIC3_REG   0x054		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x054)	);	

rt_kprintf("\n\rStep 4 Open IO output\n\n\r");		

rt_kprintf("	TCON_LCD0->LCD_IO_TRI_REG   0x08C		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x08C) 	);			
rt_kprintf("	TCON_LCD0->LCD_IO_POL_REG   0x088		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x088) 	);		

rt_kprintf("\n\rStep 5 LVDS digital logic configuration\n\n\r");	

rt_kprintf("	TCON_LCD0->LCD_LVDS_IF_REG  0x084		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x084)	);				
rt_kprintf("	TCON_LCD0->LVDS1_IF_REG     0x244		%08x\n\r", *(uint32_t *)( 0x05461000 + 0x244)	); 		

rt_kprintf("\n\rStep 6 LVDS controller configuration\n\n\r");

rt_kprintf("PHY0 COMBO_PHY_REG \n\n\r");

rt_kprintf("	DSI0_PHY->combo_phy_reg1    0x114		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x114)	); 			
rt_kprintf("	DSI0_PHY->combo_phy_reg0    0x110		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x110)	); 			
rt_kprintf("	DSI0_PHY->dphy_ana4         0x05c		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x05c)	); 			
rt_kprintf("	DSI0_PHY->dphy_ana3         0x058		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x058)	); 			
rt_kprintf("	DSI0_PHY->dphy_ana2         0x054		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x054)	); 		
rt_kprintf("	DSI0_PHY->dphy_ana1         0x050		%08x\n\r", *(uint32_t *)( 0x05451000 + 0x050)	); 		

rt_kprintf("\n\rPHY1 LCD_LVDS0_ANA_REG \n\n\r");					

rt_kprintf("	TCON_LCD0->LCD_LVDS_ANA_REG[0]  0x220           %08x\n\r", *(uint32_t *)( 0x05461000 + 0x220) );		

rt_kprintf("\n\rStep 5-7 Set and open interrupt function\n\n\r");

rt_kprintf("	TCON_LCD0->LCD_GINT0_REG    0x004           %08x\n\r", *(uint32_t *)( 0x05461000 + 0x004) 	);		
rt_kprintf("	TCON_LCD0->LCD_GINT1_REG    0x008           %08x\n\r", *(uint32_t *)( 0x05461000 + 0x008) 	);	

}

MSH_CMD_EXPORT_ALIAS(tcon_dump_regs, tcon_dump_regs, tcon_dump_regs);

