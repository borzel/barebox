// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix

#include <common.h>
#include <init.h>
#include <mach/imx/imx25-regs.h>
#include <asm/barebox-arm.h>
#include <debug_ll.h>

extern char __dtb_imx25_openx32_start[];

static inline void setup_uart(void)
{
	void __iomem *uartbase = (void *)MX25_UART5_BASE_ADDR;
	void __iomem *iomuxbase = (void *)MX25_IOMUXC_BASE_ADDR;

	writel(0x0, iomuxbase + 0x174);

	writel(0x00000000, uartbase + 0x80);
	writel(0x00004027, uartbase + 0x84);
	writel(0x00000704, uartbase + 0x88);
	writel(0x00000a81, uartbase + 0x90);
	writel(0x0000002b, uartbase + 0x9c);
	writel(0x00013880, uartbase + 0xb0);
	writel(0x0000047f, uartbase + 0xa4);
	writel(0x0000a259, uartbase + 0xa8);
	writel(0x00000001, uartbase + 0x80);

	putc_ll('>');
}



ENTRY_FUNCTION_WITHSTACK(start_imx25_openx32, MX25_IRAM_BASE_ADDR + MX25_IRAM_SIZE, r0, r1, r2)
{
	arm_cpu_lowlevel_init();
	
	// Disable ESDCTL1 - not connected SDRAM controller
	writel(0x0, 0xB8001008);
 
    setup_uart();

    relocate_to_current_adr();
    setup_c();

	barebox_arm_entry(0x80000000, SZ_16M, __dtb_imx25_openx32_start);
}