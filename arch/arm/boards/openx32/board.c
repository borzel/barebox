// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix

#define pr_fmt(fmt) "openx32: " fmt

#define LAMP_PWM		IMX_GPIO_NR(1, 26)
#define FEC_RESET_B		IMX_GPIO_NR(1, 1)
#define USB_POWER		IMX_GPIO_NR(4, 11)
#define MCU_BUSY		IMX_GPIO_NR(4, 24)
#define SURFACE_RESET	IMX_GPIO_NR(4, 28)

#include <common.h>
#include <init.h>
#include <driver.h>
#include <linux/sizes.h>
#include <gpio.h>
#include <environment.h>
#include <mach/imx/imx25-regs.h>
#include <asm/armlinux.h>
#include <asm/sections.h>
#include <asm/barebox-arm.h>
#include <io.h>
#include <asm/mach-types.h>
#include <mach/imx/iomux-mx25.h>
#include <mach/imx/generic.h>
#include <mach/imx/iim.h>
#include <platform_data/imxfb.h>
#include <linux/err.h>
#include <mach/imx/bbu.h>
#include <asm/mmu.h>





/*
 * Set up input pins with hysteresis and 100-k pull-ups
 */
#define UART5_IN_PAD_CTRL       (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP)
/*
 * FIXME: need to revisit this
 * The original code enabled PUE and 100-k pull-down without PKE, so the right
 * value here is likely:
 *	0 for no pull
 * or:
 *	PAD_CTL_PUS_100K_DOWN for 100-k pull-down
 */
#define UART5_OUT_PAD_CTRL      0

static void mx25pdk_uart_init(void)
{
	static const iomux_v3_cfg_t uart_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_LBA__UART5_RXD_MUX, UART5_IN_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_ECB__UART5_TXD_MUX, UART5_OUT_PAD_CTRL),
	};

	mxc_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));
}

// static int rdb_mem_init(void)
// {
// 	// if (!of_machine_is_compatible("fsl,ls1046a-rdb"))
// 	// 	return 0;

// 	arm_add_mem_device("ram0", 0x80000000, 0x4000000);

// 	return 0;
// }
// mem_initcall(rdb_mem_init);

static int openx32_init(void)
{
// # configure AIPS1 (base Address 0x43F0_xxxx)
// # ==============================
// # AIPS1_OPACR0_7
// 0x43F00040 0x00000000 32
writel(0x00000000, 0x43F00040);
// # AIPS1_OPACR8_15
// 0x43F00044 0x00000000 32
writel(0x00000000, 0x43F00044);
// # AIPS1_OPACR16_23
// 0x43F00048 0x00000000 32
writel(0x00000000, 0x43F00048);
// # AIPS1_OPACR24_31
// 0x43F0004C 0x00000000 32
writel(0x00000000, 0x43F0004C);
// # AIPS1_OPACR32_33
// 0x43F00050 0x00000000 32
writel(0x00000000, 0x43F00050);
// # AIPS1_MPROT0_7
// 0x43F00000 0x77777777 32
writel(0x77777777, 0x43F00000);
// # AIPS1_MPROT8_15
// 0x43F00004 0x77777777 32
writel(0x77777777, 0x43F00004);

// # configure AIPS2  (base Address 0x53F0_xxxx) - keep for now, may need to modify based on MX25
// # ==============================
// # AIPS2_OPACR0_7
// 0x53F00040 0x00000000 32
writel(0x00000000, 0x53F00040);
// # AIPS2_OPACR8_15
// 0x53F00044 0x00000000 32
writel(0x00000000, 0x53F00044);
// # AIPS2_OPACR16_23
// 0x53F00048 0x00000000 32
writel(0x00000000, 0x53F00048);
// # AIPS2_OPACR24_31
// 0x53F0004C 0x00000000 32
writel(0x00000000, 0x53F0004C);
// # AIPS2_OPACR32_33
// 0x53F00050 0x00000000 32
writel(0x00000000, 0x53F00050);
// # AIPS2_MPROT0_7
// 0x53F00000 0x77777777 32
writel(0x77777777, 0x53F00000);
// # AIPS2_MPROT8_15
// 0x53F00004 0x77777777 32
writel(0x77777777, 0x53F00004);

	// configure peripheral enables for CGCR0
	// configure peripheral enables for CGCR1
	// configure peripheral enables for CGCR2
	// Set Peripheral Clock Divider Registers (CCM_PDR0...CCM_PDR3)
	asm volatile(
	"ldr r0, =0x53F8000C \n\t"
	"ldr r1, =0x1FFFFFFF \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80010 \n\t"
	"ldr r1, =0xFFFFFFFF \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80014 \n\t"
	"ldr r1, =0x000FDFFF \n\t"
	"str r1, [r0] \n\t"

	"ldr r0, =0x53F80018 \n\t"
	"ldr r1, =0x23C83403 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F8001C \n\t"
	"ldr r1, =0x03030303 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80020 \n\t"
	"ldr r1, =0x01010103 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80024 \n\t"
	"ldr r1, =0x01010101 \n\t"
	"str r1, [r0]"
	);

	barebox_set_hostname("openx32");
	armlinux_set_architecture(MACH_TYPE_OPENX32);
	armlinux_set_serial(imx_uid());

	return 0;
}

console_initcall(openx32_init);

// ######################################################################

/*
 * FIXME: need to revisit this
 * The original code enabled PUE and 100-k pull-down without PKE, so the right
 * value here is likely:
 *	0 for no pull
 * or:
 *	PAD_CTL_PUS_100K_DOWN for 100-k pull-down
 */
#define FEC_OUT_PAD_CTRL	0

#define I2C_PAD_CTRL		(PAD_CTL_HYS | PAD_CTL_PUS_100K_UP | \
				 PAD_CTL_ODE)

static void mx25pdk_fec_init(void)
{
	static const iomux_v3_cfg_t fec_pads[] = {
		MX25_PAD_FEC_TX_CLK__FEC_TX_CLK,
		MX25_PAD_FEC_RX_DV__FEC_RX_DV,
		MX25_PAD_FEC_RDATA0__FEC_RDATA0,
		NEW_PAD_CTRL(MX25_PAD_FEC_TDATA0__FEC_TDATA0, FEC_OUT_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_FEC_TX_EN__FEC_TX_EN, FEC_OUT_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_FEC_MDC__FEC_MDC, FEC_OUT_PAD_CTRL),
		MX25_PAD_FEC_MDIO__FEC_MDIO,
		MX25_PAD_FEC_RDATA1__FEC_RDATA1,
		NEW_PAD_CTRL(MX25_PAD_FEC_TDATA1__FEC_TDATA1, FEC_OUT_PAD_CTRL),

		NEW_PAD_CTRL(MX25_PAD_GPIO_B__GPIO_B, 0), /* FEC_RESET_B */
	};

	static const iomux_v3_cfg_t i2c_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_I2C1_CLK__I2C1_CLK, I2C_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_I2C1_DAT__I2C1_DAT, I2C_PAD_CTRL),
	};

	mxc_iomux_v3_setup_multiple_pads(fec_pads, ARRAY_SIZE(fec_pads));

	/* configure FEC_RESET as output */
	gpio_request(FEC_RESET_B, "FEC_RESET");
	gpio_direction_output(FEC_RESET_B, 1);

	/* Assert RESET */
	gpio_set_value(FEC_RESET_B, 0);

	udelay(10);

	/* Deassert RESET */
	gpio_set_value(FEC_RESET_B, 1);

	/* Setup I2C pins */
	mxc_iomux_v3_setup_multiple_pads(i2c_pads, ARRAY_SIZE(i2c_pads));
}

static iomux_v3_cfg_t openx32_lcdc_gpios[] = {
	MX25_PAD_LSCLK__LSCLK,
	MX25_PAD_LD0__LD0,
	MX25_PAD_LD1__LD1,
	MX25_PAD_LD2__LD2,
	MX25_PAD_LD3__LD3,
	MX25_PAD_LD4__LD4,
	MX25_PAD_LD5__LD5,
	MX25_PAD_LD6__LD6,
	MX25_PAD_LD7__LD7,
	MX25_PAD_LD8__LD8,
	MX25_PAD_LD9__LD9,
	MX25_PAD_LD10__LD10,
	MX25_PAD_LD11__LD11,
	MX25_PAD_LD12__LD12,
	MX25_PAD_LD13__LD13,
	MX25_PAD_LD14__LD14,
	MX25_PAD_LD15__LD15,
	MX25_PAD_D15__LD16,
	MX25_PAD_D14__LD17,
	MX25_PAD_HSYNC__HSYNC,
	MX25_PAD_VSYNC__VSYNC,
	MX25_PAD_OE_ACD__OE_ACD,
};


static void openx32_fb_enable(int enable)
{
	if (enable) {
		writel(0x000100A1, 0x53FC8000);
	} else {
		writel(0x000100A0, 0x53FC8000);
	}
}

static struct fb_videomode openx32_fb_mode = {
	.name = "Innolux-AT070TN83",
	.refresh	= 60,
	.pixclock.ps = 25000,  // 40 MHz
	.xres		= 800,
	.yres		= 480,
	
	.right_margin	= 208, // hfront porch
	.left_margin	= 47,  // hback porch / blanking
	.hsync_len	= 1,
	
	.upper_margin	= 24,  // vback porch
	.lower_margin	= 130,  // vfront porch
	.vsync_len	= 1,
};

static struct imx_fb_platform_data openx32_fb_data = {
	.mode		= &openx32_fb_mode,
	.num_modes	= 1,
	//.lscr1		= 0x00120300,
	.dmacr		= 0x00020010,
	// results in .pcr = 0xFC228082, 
	.pcr		= PCR_TFT | PCR_COLOR | PCR_PBSIZ_8 | PCR_BPIX_18 | PCR_CLKPOL | PCR_END_BYTE_SWAP | PCR_ACD_SEL | PCR_SCLK_SEL | PCR_PCD(2),
	.bpp		= 32,	
	.enable		= openx32_fb_enable,
};

static int openx32_init_fb(void)
{
	if (!IS_ENABLED(CONFIG_DRIVER_VIDEO_IMX)){
		return 0;
	}

	//writel(0xFC228082, 0x53FBC018);

	// enable LAMP (asserted when high)
	gpio_request(LAMP_PWM, "LAMP_PWM");
	gpio_direction_output(LAMP_PWM, 1);
	gpio_set_value(LAMP_PWM, 0);

	// enable USB_POWER (asserted when high)
	gpio_request(USB_POWER, "USB_POWER");
	gpio_direction_output(USB_POWER, 1);
	gpio_set_value(USB_POWER, 1);

	// enable MCU_BUSY LED (asserted when high)
	gpio_request(MCU_BUSY, "MCU_BUSY");
	gpio_direction_output(MCU_BUSY, 1);
	gpio_set_value(MCU_BUSY, 1);

	// enable SURFACE_RESET pin (asserted when zero)
	gpio_request(SURFACE_RESET, "SURFACE_RESET");
	gpio_direction_output(SURFACE_RESET, 1);
	gpio_set_value(SURFACE_RESET, 1);

	mx25pdk_fec_init();

	// set GPIOs within IOMUXC
	static const iomux_v3_cfg_t gpio_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_CONTRAST__PWM4_PWMO, 0),
	};
	mxc_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));

	// enable display-backlight
	// set periode (PWM_PWMPR) and dutycycle (PWM_PWMSAR)
	writel(0x00000190, 0x53FC8010);
	writel(0x000000C8, 0x53FC800C);

	mxc_iomux_v3_setup_multiple_pads(openx32_lcdc_gpios,
	  		ARRAY_SIZE(openx32_lcdc_gpios));

	openx32_fb_enable(0);
	add_generic_device("imxfb",	-1,	NULL, (resource_size_t)MX25_LCDC_BASE_ADDR, 0x1000, IORESOURCE_MEM, &openx32_fb_data);

	return 0;
}
device_initcall(openx32_init_fb);


