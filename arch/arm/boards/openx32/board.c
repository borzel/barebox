// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
// SPDX-FileCopyrightText: 2025 Alexander Schulz <code@schulzalex.de>, OpenX32 Project

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

static int openx32_init(void)
{
	barebox_set_hostname("openx32");
	armlinux_set_architecture(MACH_TYPE_OPENX32);
	armlinux_set_serial(imx_uid());

	return 0;
}

console_initcall(openx32_init);

// ######################################################################

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

    // set GPIOs within IOMUXC
	static const iomux_v3_cfg_t gpio_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_CONTRAST__PWM4_PWMO, 0),
		NEW_PAD_CTRL(MX25_PAD_PWM__GPIO_1_26, 0),
		NEW_PAD_CTRL(MX25_PAD_D9__GPIO_4_11, 0),
		NEW_PAD_CTRL(MX25_PAD_UART1_RTS__GPIO_4_24, 0),
		NEW_PAD_CTRL(MX25_PAD_UART2_RTS__GPIO_4_28, 0),
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