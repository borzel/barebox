// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix

#include <common.h>
#include <init.h>
#include <mach/imx/imx25-regs.h>
#include <asm/barebox-arm.h>

ENTRY_FUNCTION_WITHSTACK(start_imx25_openx32, MX25_IRAM_BASE_ADDR + MX25_IRAM_SIZE, r0, r1, r2)
{
	extern char __dtb_imx25_openx32_start[];
    void *fdt;

    relocate_to_current_adr();
    setup_c();

	// disable watchdog
	writel(0, 0x53FDC008);

	// LAMP ON
	writel(0x04000000, 0x53FCC000);

	// LAMP OFF
	//writel(0x00000000, 0x53FCC000);

	barebox_arm_entry(0x80000000, SZ_256M, __dtb_imx25_openx32_start);
}