/*
 * Copyright (c) ylyamin
 */
#ifndef DRV_USBH_H_
#define DRV_USBH_H_

#include <stdio.h>
#include <rtthread.h>

#define BV(x)              (1 << (x))

#define EHCI0_BASE (0x04101000)
#define EHCI1_BASE (0x04200000)
#define OHCI_BASE  (0x04200000+0x400)

#define CCU_BASE ((uintptr_t) 0x02001000)             /*!< CCU Clock Controller Unit (CCU) Base */

/*!< CCU Clock Controller Unit (CCU) */
typedef struct CCU_Type
{
    volatile uint32_t PLL_CPU_CTRL_REG;               /*!< Offset 0x000 PLL_CPU Control Register */
             uint32_t reserved_0x004 [0x0003];
    volatile uint32_t PLL_DDR_CTRL_REG;               /*!< Offset 0x010 PLL_DDR Control Register */
             uint32_t reserved_0x014 [0x0003];
    volatile uint32_t PLL_PERI_CTRL_REG;              /*!< Offset 0x020 PLL_PERI Control Register */
             uint32_t reserved_0x024 [0x0007];
    volatile uint32_t PLL_VIDEO0_CTRL_REG;            /*!< Offset 0x040 PLL_VIDEO0 Control Register */
             uint32_t reserved_0x044;
    volatile uint32_t PLL_VIDEO1_CTRL_REG;            /*!< Offset 0x048 PLL_VIDEO1 Control Register */
             uint32_t reserved_0x04C [0x0003];
    volatile uint32_t PLL_VE_CTRL_REG;                /*!< Offset 0x058 PLL_VE Control Register */
             uint32_t reserved_0x05C [0x0007];
    volatile uint32_t PLL_AUDIO0_CTRL_REG;            /*!< Offset 0x078 PLL_AUDIO0 Control Register */
             uint32_t reserved_0x07C;
    volatile uint32_t PLL_AUDIO1_CTRL_REG;            /*!< Offset 0x080 PLL_AUDIO1 Control Register */
             uint32_t reserved_0x084 [0x0023];
    volatile uint32_t PLL_DDR_PAT0_CTRL_REG;          /*!< Offset 0x110 PLL_DDR Pattern0 Control Register */
    volatile uint32_t PLL_DDR_PAT1_CTRL_REG;          /*!< Offset 0x114 PLL_DDR Pattern1 Control Register */
             uint32_t reserved_0x118 [0x0002];
    volatile uint32_t PLL_PERI_PAT0_CTRL_REG;         /*!< Offset 0x120 PLL_PERI Pattern0 Control Register */
    volatile uint32_t PLL_PERI_PAT1_CTRL_REG;         /*!< Offset 0x124 PLL_PERI Pattern1 Control Register */
             uint32_t reserved_0x128 [0x0006];
    volatile uint32_t PLL_VIDEO0_PAT0_CTRL_REG;       /*!< Offset 0x140 PLL_VIDEO0 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO0_PAT1_CTRL_REG;       /*!< Offset 0x144 PLL_VIDEO0 Pattern1 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT0_CTRL_REG;       /*!< Offset 0x148 PLL_VIDEO1 Pattern0 Control Register */
    volatile uint32_t PLL_VIDEO1_PAT1_CTRL_REG;       /*!< Offset 0x14C PLL_VIDEO1 Pattern1 Control Register */
             uint32_t reserved_0x150 [0x0002];
    volatile uint32_t PLL_VE_PAT0_CTRL_REG;           /*!< Offset 0x158 PLL_VE Pattern0 Control Register */
    volatile uint32_t PLL_VE_PAT1_CTRL_REG;           /*!< Offset 0x15C PLL_VE Pattern1 Control Register */
             uint32_t reserved_0x160 [0x0006];
    volatile uint32_t PLL_AUDIO0_PAT0_CTRL_REG;       /*!< Offset 0x178 PLL_AUDIO0 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO0_PAT1_CTRL_REG;       /*!< Offset 0x17C PLL_AUDIO0 Pattern1 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT0_CTRL_REG;       /*!< Offset 0x180 PLL_AUDIO1 Pattern0 Control Register */
    volatile uint32_t PLL_AUDIO1_PAT1_CTRL_REG;       /*!< Offset 0x184 PLL_AUDIO1 Pattern1 Control Register */
             uint32_t reserved_0x188 [0x005E];
    volatile uint32_t PLL_CPU_BIAS_REG;               /*!< Offset 0x300 PLL_CPU Bias Register */
             uint32_t reserved_0x304 [0x0003];
    volatile uint32_t PLL_DDR_BIAS_REG;               /*!< Offset 0x310 PLL_DDR Bias Register */
             uint32_t reserved_0x314 [0x0003];
    volatile uint32_t PLL_PERI_BIAS_REG;              /*!< Offset 0x320 PLL_PERI Bias Register */
             uint32_t reserved_0x324 [0x0007];
    volatile uint32_t PLL_VIDEO0_BIAS_REG;            /*!< Offset 0x340 PLL_VIDEO0 Bias Register */
             uint32_t reserved_0x344;
    volatile uint32_t PLL_VIDEO1_BIAS_REG;            /*!< Offset 0x348 PLL_VIDEO1 Bias Register */
             uint32_t reserved_0x34C [0x0003];
    volatile uint32_t PLL_VE_BIAS_REG;                /*!< Offset 0x358 PLL_VE Bias Register */
             uint32_t reserved_0x35C [0x0007];
    volatile uint32_t PLL_AUDIO0_BIAS_REG;            /*!< Offset 0x378 PLL_AUDIO0 Bias Register */
             uint32_t reserved_0x37C;
    volatile uint32_t PLL_AUDIO1_BIAS_REG;            /*!< Offset 0x380 PLL_AUDIO1 Bias Register */
             uint32_t reserved_0x384 [0x001F];
    volatile uint32_t PLL_CPU_TUN_REG;                /*!< Offset 0x400 PLL_CPU Tuning Register */
             uint32_t reserved_0x404 [0x003F];
    volatile uint32_t CPU_AXI_CFG_REG;                /*!< Offset 0x500 CPU_AXI Configuration Register */
    volatile uint32_t CPU_GATING_REG;                 /*!< Offset 0x504 CPU_GATING Configuration Register */
             uint32_t reserved_0x508 [0x0002];
    volatile uint32_t PSI_CLK_REG;                    /*!< Offset 0x510 PSI Clock Register */
             uint32_t reserved_0x514 [0x0003];
    volatile uint32_t APB0_CLK_REG;                   /*!< Offset 0x520 APB0 Clock Register */
    volatile uint32_t APB1_CLK_REG;                   /*!< Offset 0x524 APB1 Clock Register */
             uint32_t reserved_0x528 [0x0006];
    volatile uint32_t MBUS_CLK_REG;                   /*!< Offset 0x540 MBUS Clock Register */
             uint32_t reserved_0x544 [0x002F];
    volatile uint32_t DE_CLK_REG;                     /*!< Offset 0x600 DE Clock Register */
             uint32_t reserved_0x604 [0x0002];
    volatile uint32_t DE_BGR_REG;                     /*!< Offset 0x60C DE Bus Gating Reset Register */
             uint32_t reserved_0x610 [0x0004];
    volatile uint32_t DI_CLK_REG;                     /*!< Offset 0x620 DI Clock Register */
             uint32_t reserved_0x624 [0x0002];
    volatile uint32_t DI_BGR_REG;                     /*!< Offset 0x62C DI Bus Gating Reset Register */
    volatile uint32_t G2D_CLK_REG;                    /*!< Offset 0x630 G2D Clock Register */
             uint32_t reserved_0x634 [0x0002];
    volatile uint32_t G2D_BGR_REG;                    /*!< Offset 0x63C G2D Bus Gating Reset Register */
             uint32_t reserved_0x640 [0x0010];
    volatile uint32_t CE_CLK_REG;                     /*!< Offset 0x680 CE Clock Register */
             uint32_t reserved_0x684 [0x0002];
    volatile uint32_t CE_BGR_REG;                     /*!< Offset 0x68C CE Bus Gating Reset Register */
    volatile uint32_t VE_CLK_REG;                     /*!< Offset 0x690 VE Clock Register */
             uint32_t reserved_0x694 [0x0002];
    volatile uint32_t VE_BGR_REG;                     /*!< Offset 0x69C VE Bus Gating Reset Register */
             uint32_t reserved_0x6A0 [0x001B];
    volatile uint32_t DMA_BGR_REG;                    /*!< Offset 0x70C DMA Bus Gating Reset Register */
             uint32_t reserved_0x710 [0x0003];
    volatile uint32_t MSGBOX_BGR_REG;                 /*!< Offset 0x71C MSGBOX Bus Gating Reset Register */
             uint32_t reserved_0x720 [0x0003];
    volatile uint32_t SPINLOCK_BGR_REG;               /*!< Offset 0x72C SPINLOCK Bus Gating Reset Register */
             uint32_t reserved_0x730 [0x0003];
    volatile uint32_t HSTIMER_BGR_REG;                /*!< Offset 0x73C HSTIMER Bus Gating Reset Register */
    volatile uint32_t AVS_CLK_REG;                    /*!< Offset 0x740 AVS Clock Register */
             uint32_t reserved_0x744 [0x0012];
    volatile uint32_t DBGSYS_BGR_REG;                 /*!< Offset 0x78C DBGSYS Bus Gating Reset Register */
             uint32_t reserved_0x790 [0x0007];
    volatile uint32_t PWM_BGR_REG;                    /*!< Offset 0x7AC PWM Bus Gating Reset Register */
             uint32_t reserved_0x7B0 [0x0003];
    volatile uint32_t IOMMU_BGR_REG;                  /*!< Offset 0x7BC IOMMU Bus Gating Reset Register */
             uint32_t reserved_0x7C0 [0x0010];
    volatile uint32_t DRAM_CLK_REG;                   /*!< Offset 0x800 DRAM Clock Register */
    volatile uint32_t MBUS_MAT_CLK_GATING_REG;        /*!< Offset 0x804 MBUS Master Clock Gating Register */
             uint32_t reserved_0x808;
    volatile uint32_t DRAM_BGR_REG;                   /*!< Offset 0x80C DRAM Bus Gating Reset Register */
             uint32_t reserved_0x810 [0x0008];
    volatile uint32_t SMHC0_CLK_REG;                  /*!< Offset 0x830 SMHC0 Clock Register */
    volatile uint32_t SMHC1_CLK_REG;                  /*!< Offset 0x834 SMHC1 Clock Register */
    volatile uint32_t SMHC2_CLK_REG;                  /*!< Offset 0x838 SMHC2 Clock Register */
             uint32_t reserved_0x83C [0x0004];
    volatile uint32_t SMHC_BGR_REG;                   /*!< Offset 0x84C SMHC Bus Gating Reset Register */
             uint32_t reserved_0x850 [0x002F];
    volatile uint32_t UART_BGR_REG;                   /*!< Offset 0x90C UART Bus Gating Reset Register */
             uint32_t reserved_0x910 [0x0003];
    volatile uint32_t TWI_BGR_REG;                    /*!< Offset 0x91C TWI Bus Gating Reset Register */
             uint32_t reserved_0x920 [0x0003];
    volatile uint32_t CAN_BGR_REG;                    /*!< Offset 0x92C CAN Bus Gating Reset Register */
             uint32_t reserved_0x930 [0x0004];
    volatile uint32_t SPI0_CLK_REG;                   /*!< Offset 0x940 SPI0 Clock Register */
    volatile uint32_t SPI1_CLK_REG;                   /*!< Offset 0x944 SPI1 Clock Register */
             uint32_t reserved_0x948 [0x0009];
    volatile uint32_t SPI_BGR_REG;                    /*!< Offset 0x96C SPI Bus Gating Reset Register */
    volatile uint32_t EMAC_25M_CLK_REG;               /*!< Offset 0x970 EMAC_25M Clock Register */
             uint32_t reserved_0x974 [0x0002];
    volatile uint32_t EMAC_BGR_REG;                   /*!< Offset 0x97C EMAC Bus Gating Reset Register */
             uint32_t reserved_0x980 [0x0010];
    volatile uint32_t IRTX_CLK_REG;                   /*!< Offset 0x9C0 IRTX Clock Register */
             uint32_t reserved_0x9C4 [0x0002];
    volatile uint32_t IRTX_BGR_REG;                   /*!< Offset 0x9CC IRTX Bus Gating Reset Register */
             uint32_t reserved_0x9D0 [0x0007];
    volatile uint32_t GPADC_BGR_REG;                  /*!< Offset 0x9EC GPADC Bus Gating Reset Register */
             uint32_t reserved_0x9F0 [0x0003];
    volatile uint32_t THS_BGR_REG;                    /*!< Offset 0x9FC THS Bus Gating Reset Register */
             uint32_t reserved_0xA00 [0x0005];
    volatile uint32_t I2S1_CLK_REG;                   /*!< Offset 0xA14 I2S1 Clock Register */
    volatile uint32_t I2S2_CLK_REG;                   /*!< Offset 0xA18 I2S2 Clock Register */
    volatile uint32_t I2S2_ASRC_CLK_REG;              /*!< Offset 0xA1C I2S2_ASRC Clock Register */
    volatile uint32_t I2S_BGR_REG;                    /*!< Offset 0xA20 I2S Bus Gating Reset Register */
    volatile uint32_t OWA_TX_CLK_REG;                 /*!< Offset 0xA24 OWA_TX Clock Register */
             uint32_t reserved_0xA28;
    volatile uint32_t OWA_BGR_REG;                    /*!< Offset 0xA2C OWA Bus Gating Reset Register */
             uint32_t reserved_0xA30 [0x0004];
    volatile uint32_t DMIC_CLK_REG;                   /*!< Offset 0xA40 DMIC Clock Register */
             uint32_t reserved_0xA44 [0x0002];
    volatile uint32_t DMIC_BGR_REG;                   /*!< Offset 0xA4C DMIC Bus Gating Reset Register */
    volatile uint32_t AUDIO_CODEC_DAC_CLK_REG;        /*!< Offset 0xA50 AUDIO_CODEC_DAC Clock Register */
    volatile uint32_t AUDIO_CODEC_ADC_CLK_REG;        /*!< Offset 0xA54 AUDIO_CODEC_ADC Clock Register */
             uint32_t reserved_0xA58;
    volatile uint32_t AUDIO_CODEC_BGR_REG;            /*!< Offset 0xA5C AUDIO_CODEC Bus Gating Reset Register */
             uint32_t reserved_0xA60 [0x0004];
    volatile uint32_t USB0_CLK_REG;                   /*!< Offset 0xA70 USB0 Clock Register */
    volatile uint32_t USB1_CLK_REG;                   /*!< Offset 0xA74 USB1 Clock Register */
             uint32_t reserved_0xA78 [0x0005];
    volatile uint32_t USB_BGR_REG;                    /*!< Offset 0xA8C USB Bus Gating Reset Register */
             uint32_t reserved_0xA90 [0x000B];
    volatile uint32_t DPSS_TOP_BGR_REG;               /*!< Offset 0xABC DPSS_TOP Bus Gating Reset Register */
             uint32_t reserved_0xAC0 [0x0019];
    volatile uint32_t DSI_CLK_REG;                    /*!< Offset 0xB24 DSI Clock Register */
             uint32_t reserved_0xB28 [0x0009];
    volatile uint32_t DSI_BGR_REG;                    /*!< Offset 0xB4C DSI Bus Gating Reset Register */
             uint32_t reserved_0xB50 [0x0004];
    volatile uint32_t TCONLCD_CLK_REG;                /*!< Offset 0xB60 TCONLCD Clock Register */
             uint32_t reserved_0xB64 [0x0006];
    volatile uint32_t TCONLCD_BGR_REG;                /*!< Offset 0xB7C TCONLCD Bus Gating Reset Register */
    volatile uint32_t TCONTV_CLK_REG;                 /*!< Offset 0xB80 TCONTV Clock Register */
             uint32_t reserved_0xB84 [0x0006];
    volatile uint32_t TCONTV_BGR_REG;                 /*!< Offset 0xB9C TCONTV Bus Gating Reset Register */
             uint32_t reserved_0xBA0 [0x0003];
    volatile uint32_t LVDS_BGR_REG;                   /*!< Offset 0xBAC LVDS Bus Gating Reset Register */
    volatile uint32_t TVE_CLK_REG;                    /*!< Offset 0xBB0 TVE Clock Register */
             uint32_t reserved_0xBB4 [0x0002];
    volatile uint32_t TVE_BGR_REG;                    /*!< Offset 0xBBC TVE Bus Gating Reset Register */
    volatile uint32_t TVD_CLK_REG;                    /*!< Offset 0xBC0 TVD Clock Register */
             uint32_t reserved_0xBC4 [0x0006];
    volatile uint32_t TVD_BGR_REG;                    /*!< Offset 0xBDC TVD Bus Gating Reset Register */
             uint32_t reserved_0xBE0 [0x0004];
    volatile uint32_t LEDC_CLK_REG;                   /*!< Offset 0xBF0 LEDC Clock Register */
             uint32_t reserved_0xBF4 [0x0002];
    volatile uint32_t LEDC_BGR_REG;                   /*!< Offset 0xBFC LEDC Bus Gating Reset Register */
             uint32_t reserved_0xC00;
    volatile uint32_t CSI_CLK_REG;                    /*!< Offset 0xC04 CSI Clock Register */
    volatile uint32_t CSI_MASTER_CLK_REG;             /*!< Offset 0xC08 CSI Master Clock Register */
             uint32_t reserved_0xC0C [0x0004];
    volatile uint32_t CSI_BGR_REG;                    /*!< Offset 0xC1C CSI Bus Gating Reset Register */
             uint32_t reserved_0xC20 [0x000C];
    volatile uint32_t TPADC_CLK_REG;                  /*!< Offset 0xC50 TPADC Clock Register */
             uint32_t reserved_0xC54 [0x0002];
    volatile uint32_t TPADC_BGR_REG;                  /*!< Offset 0xC5C TPADC Bus Gating Reset Register */
             uint32_t reserved_0xC60 [0x0004];
    volatile uint32_t DSP_CLK_REG;                    /*!< Offset 0xC70 DSP Clock Register */
             uint32_t reserved_0xC74 [0x0002];
    volatile uint32_t DSP_BGR_REG;                    /*!< Offset 0xC7C DSP Bus Gating Reset Register */
             uint32_t reserved_0xC80 [0x0020];
    volatile uint32_t RISC_CLK_REG;                   /*!< Offset 0xD00 RISC Clock Register */
    volatile uint32_t RISC_GATING_REG;                /*!< Offset 0xD04 RISC Gating Configuration Register */
             uint32_t reserved_0xD08;
    volatile uint32_t RISC_CFG_BGR_REG;               /*!< Offset 0xD0C RISC_CFG Bus Gating Reset Register */
             uint32_t reserved_0xD10 [0x007D];
    volatile uint32_t PLL_LOCK_DBG_CTRL_REG;          /*!< Offset 0xF04 PLL Lock Debug Control Register */
    volatile uint32_t FRE_DET_CTRL_REG;               /*!< Offset 0xF08 Frequency Detect Control Register */
    volatile uint32_t FRE_UP_LIM_REG;                 /*!< Offset 0xF0C Frequency Up Limit Register */
    volatile uint32_t FRE_DOWN_LIM_REG;               /*!< Offset 0xF10 Frequency Down Limit Register */
             uint32_t reserved_0xF14 [0x0003];
    volatile uint32_t RISC_RST_REG;                   /*!< Offset 0xF20 RISC LOCK RESET Register */
             uint32_t reserved_0xF24 [0x0003];
    volatile uint32_t CCU_FAN_GATE_REG;               /*!< Offset 0xF30 CCU FANOUT CLOCK GATE Register */
    volatile uint32_t CLK27M_FAN_REG;                 /*!< Offset 0xF34 CLK27M FANOUT Register */
    volatile uint32_t PCLK_FAN_REG;                   /*!< Offset 0xF38 PCLK FANOUT Register */
    volatile uint32_t CCU_FAN_REG;                    /*!< Offset 0xF3C CCU FANOUT Register */
             uint32_t reserved_0xF40 [0x0030];
} CCU_TypeDef; /* size of structure = 0x1000 */

#define CCU ((CCU_TypeDef *) CCU_BASE) 

#endif /* DRV_USBH_H_ */