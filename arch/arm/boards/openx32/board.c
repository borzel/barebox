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

static int openx32_init(void)
{
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

	barebox_set_hostname("openx32");
	armlinux_set_architecture(MACH_TYPE_OPENX32);

	return 0;
}

console_initcall(openx32_init);

// ######################################################################


static void openx32_fb_enable(int enable)
{
	if (enable) {
		writel(0x000100A1, 0x53FC8000);
	} else {
		writel(0x000100A0, 0x53FC8000);
	}
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

/*
	From DTS

	hactive = <800>;
	hfront-porch = <210>;
	hback-porch = <52>;
	hsync-len = <1>;
	vactive = <480>;
	vfront-porch = <22>;
	vback-porch = <22>;
	vsync-len = <1>;
	clock-frequency = <40000000>;
	hsync-active = <0>; // asserted when LOW
	vsync-active = <0>; // asserted when LOW
	de-active = <1>; // asserted when HIGH
	pixelclk-active = <1>; // 1 = falling edge
 */

static struct fb_videomode openx32_fb_mode = {
	.name = "Innolux-AT070TN83",
	//.refresh	= 60,
	.pixclock.ps = 33333,
	.xres		= 800,
	.yres		= 480,
	
	.right_margin	= 210, // hfront porch
	.left_margin	= 45,  // hback porch / blanking
	.hsync_len	= 1,
	
	.upper_margin	= 22,  // vback porch
	.lower_margin	= 22,  // vfront porch
	.vsync_len	= 1,

	//  .display_flags = 	DISPLAY_FLAGS_HSYNC_LOW |
	//  					DISPLAY_FLAGS_VSYNC_LOW |
	//  					DISPLAY_FLAGS_DE_HIGH |
	//  					DISPLAY_FLAGS_PIXDATA_NEGEDGE,

	//.sync = FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,

	//.vmode = FB_VMODE_NONINTERLACED,

	// // //https://github.com/apc-io/apc-8750/blob/master/kernel/drivers/video/wmt/devices/lcd-INNOLUX-AT070TN83.c
	// .right_margin	= 210, // hfront porch
	// .left_margin	= 45,  // hback porch / blanking
	// .hsync_len	= 1,
	
	// .upper_margin	= 22,  // vback porch
	// .lower_margin	= 22,  // vfront porch
	// .vsync_len	= 1,

};

static struct imx_fb_platform_data openx32_fb_data = {
	.mode		= &openx32_fb_mode,
	.num_modes	= 1,
	.lscr1		= 0x00120300,
	.dmacr		= 0x00020010,
	.pcr		= 0xFC228080,
	.bpp		= 32,	
	.enable		= openx32_fb_enable,
};

static int openx32_init_fb(void)
{
	if (!IS_ENABLED(CONFIG_DRIVER_VIDEO_IMX)){
		return 0;
	}

	// set LPCR-Register
	//writel(0xFC228080, 0x53FBC018);

	// // // set GPIOs within IOMUXC
	// static const iomux_v3_cfg_t gpio_pads[] = {
	// 	NEW_PAD_CTRL(MX25_PAD_CONTRAST__PWM4_PWMO, 0),
	// // 	NEW_PAD_CTRL(MX25_PAD_PWM__GPIO_1_26, 0),
	// // 	NEW_PAD_CTRL(MX25_PAD_D9__GPIO_4_11, 0),
	// // 	NEW_PAD_CTRL(MX25_PAD_UART1_RTS__GPIO_4_24, 0),
	// // 	NEW_PAD_CTRL(MX25_PAD_UART2_RTS__GPIO_4_28, 0),
	// };
	// mxc_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));

	// ###############################################################
	// enable display-backlight
		
	// // configure Prescaler but disable PWM (PWM_PWMC)
	// writel(0x000100A0, 0x53FC8000);
	// set periode (PWM_PWMPR) and dutycycle (PWM_PWMSAR)
	writel(0x00000190, 0x53FC8010);
	writel(0x000000C8, 0x53FC800C);
	// // configure Prescaler but enable PWM (PWM_PWMC)
	// writel(0x000100A0, 0x53FC8001);
	
	// ###############################################################

	openx32_fb_enable(0);

	// mxc_iomux_v3_setup_multiple_pads(openx32_lcdc_gpios,
	// 		ARRAY_SIZE(openx32_lcdc_gpios));

	//fb_videomode_set_pixclock_hz(&openx32_fb_data, 40000000);  // 40MHz
	fb_videomode_set_pixclock_hz(&openx32_fb_data, 33333333);  // 33.3MHz
	add_generic_device("imxfb",	-1,	NULL, (resource_size_t)MX25_LCDC_BASE_ADDR, 0x2000, IORESOURCE_MEM, &openx32_fb_data);

	return 0;
}
device_initcall(openx32_init_fb);


