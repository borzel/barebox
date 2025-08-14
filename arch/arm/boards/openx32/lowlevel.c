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

    //##########################################################

    /* AIPS setup - Only setup MPROTx registers. The PACR default values are good.
	 * Set all MPROTx to be non-bufferable, trusted for R/W,
	 * not forced to user-mode.
	 */
	writel(0x77777777, 0x43f00000);
	writel(0x77777777, 0x43f00004);
	writel(0x77777777, 0x53f00000);
	writel(0x77777777, 0x53f00004);

	/* MAX (Multi-Layer AHB Crossbar Switch) setup
	 * MPR - priority for MX25 is (SDHC2/SDMA)>USBOTG>RTIC>IAHB>DAHB
	 */
	writel(0x00043210, 0x43f04000);
	writel(0x00043210, 0x43f04100);
	writel(0x00043210, 0x43f04200);
	writel(0x00043210, 0x43f04300);
	writel(0x00043210, 0x43f04400);
	/* SGPCR - always park on last master */
	writel(0x10, 0x43f04010);
	writel(0x10, 0x43f04110);
	writel(0x10, 0x43f04210);
	writel(0x10, 0x43f04310);
	writel(0x10, 0x43f04410);
	/* MGPCR - restore default values */
	writel(0x0, 0x43f04800);
	writel(0x0, 0x43f04900);
	writel(0x0, 0x43f04a00);
	writel(0x0, 0x43f04b00);
	writel(0x0, 0x43f04c00);

	/* Configure M3IF registers
	 * M3IF Control Register (M3IFCTL) for MX25
	 * MRRP[0] = LCDC           on priority list (1 << 0)  = 0x00000001
	 * MRRP[1] = MAX1       not on priority list (0 << 1)  = 0x00000000
	 * MRRP[2] = MAX0       not on priority list (0 << 2)  = 0x00000000
	 * MRRP[3] = USB HOST   not on priority list (0 << 3)  = 0x00000000
	 * MRRP[4] = SDMA       not on priority list (0 << 4)  = 0x00000000
	 * MRRP[5] = SD/ATA/FEC not on priority list (0 << 5)  = 0x00000000
	 * MRRP[6] = SCMFBC     not on priority list (0 << 6)  = 0x00000000
	 * MRRP[7] = CSI        not on priority list (0 << 7)  = 0x00000000
	 *                                                       ----------
	 *                                                       0x00000001
	 */
	writel(0x1, 0xb8003000);

	/* configure ARM clk */
	writel(0x20034000, MX25_CCM_BASE_ADDR + MX25_CCM_CCTL);

	/* enable all the clocks */
	writel(0x1fffffff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR0);
	writel(0xffffffff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR1);
	writel(0x000fdfff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR2);

    //##########################################################

    setup_uart();

    relocate_to_current_adr();
    setup_c();

	barebox_arm_entry(0x80000000, SZ_16M, __dtb_imx25_openx32_start);
}