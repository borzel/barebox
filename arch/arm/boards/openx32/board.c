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
#include <mach/imx/imx-nand.h>
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

	// set GPIOs within IOMUXC
	static const iomux_v3_cfg_t gpio_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_CONTRAST__PWM4_PWMO, 0),
		NEW_PAD_CTRL(MX25_PAD_PWM__GPIO_1_26, 0),
		NEW_PAD_CTRL(MX25_PAD_D9__GPIO_4_11, 0),
		NEW_PAD_CTRL(MX25_PAD_UART1_RTS__GPIO_4_24, 0),
		NEW_PAD_CTRL(MX25_PAD_UART2_RTS__GPIO_4_28, 0),
	};
	mxc_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));


	// // disable watchdog
	// asm volatile(
	// "ldr r0, =0x53FDC002\n\t"
	// "ldr r1, =0x00000003\n\t"
	// "str r1, [r0]"
	// );

	// enable display-backlight
	// configure Prescaler but disable PWM (PWM_PWMC)
	asm volatile(
	"ldr r0, =0x53FC8000\n\t"
	"ldr r1, =0x000100A0\n\t"
	"str r1, [r0]"
	);
	// set periode (PWM_PWMPR) and dutycycle (PWM_PWMSAR)
	asm volatile(
	"ldr r0, =0x53FC8010\n\t"
	"ldr r1, =0x00000190\n\t"
	"str r1, [r0]\n\t"
	"ldr r0, =0x53FC800C\n\t"
	"ldr r1, =0x000000C8\n\t"
	"str r1, [r0]"
	);
	// configure Prescaler and enable PWM (PWM_PWMC)
	asm volatile(
	"ldr r0, =0x53FC8000\n\t"
	"ldr r1, =0x000100A1\n\t"
	"str r1, [r0]"
	);

	// enable LAMP (asserted when high)
	gpio_request(LAMP_PWM, "LAMP_PWM");
	gpio_direction_output(LAMP_PWM, 1);
	gpio_set_value(LAMP_PWM, 0);
	
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 1);
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 0);
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 1);
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 0);
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 1);
	// mdelay(200);
	// gpio_set_value(LAMP_PWM, 0);
	// mdelay(200);

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

// ######################################################################




static iomux_v3_cfg_t tx25_lcdc_gpios[] = {
	// MX25_PAD_A18__GPIO_2_4,		/* LCD Reset (active LOW) */
	// MX25_PAD_PWM__GPIO_1_26,	/* LCD Backlight brightness 0: full 1: off */
	// MX25_PAD_A19__GPIO_2_5,		/* LCD Power Enable 0: off 1: on */
	// MX25_PAD_LSCLK__LSCLK,
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
	MX25_PAD_CONTRAST__CONTRAST,
};


#define REFRESH 60
#define XRES 800
#define YRES 480
#define LEFT_M 210
#define RIGHT_M 52
#define UPPER_M 22
#define LOWER_M 22
#define HSYNC_LEN 1
#define VSYNC_LEN 1
#define PIXCLOCK (1e12/((XRES+LEFT_M+RIGHT_M+HSYNC_LEN)*(YRES+UPPER_M+LOWER_M+VSYNC_LEN)*REFRESH))

// static struct fb_videomode video_modes[] = {
//     {
//      "MyVideoMode", REFRESH, XRES, YRES, PIXCLOCK, LEFT_M, RIGHT_M, UPPER_M, LOWER_M, HSYNC_LEN, VSYNC_LEN,
//      0, FB_VMODE_NONINTERLACED, 0,
//     },
// };

static struct fb_videomode stk5_fb_mode = {
	.name = "Innolux-AT070TN",
	.pixclock	= PIXCLOCK,

	.xres		= 800,
	.yres		= 480,

	.hsync_len	= 1,
	.left_margin	= 210,
	.right_margin	= 52,

	.vsync_len	= 1,
	.upper_margin	= 22,
	.lower_margin	= 22,
};

// #define STK5_LCD_BACKLIGHT_GPIO		IMX_GPIO_NR(1, 26)
// #define STK5_LCD_RESET_GPIO		IMX_GPIO_NR(2, 4)
// #define STK5_LCD_POWER_GPIO		IMX_GPIO_NR(2, 5)

static void tx25_fb_enable(int enable)
{
	// if (enable) {
	// 	gpio_direction_output(STK5_LCD_RESET_GPIO, 1);
	// 	gpio_direction_output(STK5_LCD_POWER_GPIO, 1);
	// 	mdelay(300);
	// 	gpio_direction_output(STK5_LCD_BACKLIGHT_GPIO, 0);
	// } else {
	// 	gpio_direction_output(STK5_LCD_BACKLIGHT_GPIO, 1);
	// 	gpio_direction_output(STK5_LCD_RESET_GPIO, 0);
	// 	gpio_direction_output(STK5_LCD_POWER_GPIO, 0);
	// }
}

static struct imx_fb_platform_data tx25_fb_data = {
	.mode		= &stk5_fb_mode,
	.num_modes	= 1,
	.dmacr		= 0x80040060,
	.enable		= tx25_fb_enable,
	.bpp		= 32,
	.pcr		= 0xFC228080,
};

static int openx32_init_fb(void)
{
	//mx25pdk_fec_init();

	if (!IS_ENABLED(CONFIG_DRIVER_VIDEO_IMX)){
		gpio_set_value(LAMP_PWM, 0);
		mdelay(500);
		gpio_set_value(LAMP_PWM, 1);
		mdelay(500);
		gpio_set_value(LAMP_PWM, 0);
		mdelay(500);
		gpio_set_value(LAMP_PWM, 1);
		return 0;
	}

	gpio_set_value(LAMP_PWM, 0);
	mdelay(100);
	gpio_set_value(LAMP_PWM, 1);
	mdelay(100);
	gpio_set_value(LAMP_PWM, 0);
	mdelay(100);
	gpio_set_value(LAMP_PWM, 1);
		
	//tx25_fb_enable(0);

	//mxc_iomux_v3_setup_multiple_pads(tx25_lcdc_gpios,
			//ARRAY_SIZE(tx25_lcdc_gpios));

	add_generic_device(
		"imxfb",
		-1,
		NULL,
		(resource_size_t)MX25_LCDC_BASE_ADDR,
		0x1000,
		IORESOURCE_MEM,
		&tx25_fb_data);
	return 0;
}
device_initcall(openx32_init_fb);


