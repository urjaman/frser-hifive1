/*	Copyright (C) 2017 Urja Rannikko <urjaman@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "traps.h"
#include "frser.h"
#include "delay.h"

#include "uart.h"

#include "platform.h"
#include "encoding.h"

#if F_CPU % 8000000
#error "F_CPU: only multiples of 8Mhz supported (64 - 384Mhz please)"
#endif
#define PLL_F_VAL (F_CPU/8000000)-1

static void init_pll(void) {
	/* Turn off the LFROSC */
	AON_REG(AON_LFROSC) &= ~ROSC_EN(1);

	/* Make sure the HFROSC is running at its default setting */
	PRCI_REG(PRCI_HFROSCCFG) = (ROSC_DIV(4) | ROSC_TRIM(16) | ROSC_EN(1));
	while ((PRCI_REG(PRCI_HFROSCCFG) & ROSC_RDY(1)) == 0) ;
	PRCI_REG(PRCI_PLLCFG) &= ~PLL_SEL(1);

	/* Slow down the QSPI. */
	SPI0_REG(SPI_REG_SCKDIV) = 7;

	/* Final output divide to divide-by-1: */
	PRCI_REG(PRCI_PLLDIV) = (PLL_FINAL_DIV_BY_1(1) | PLL_FINAL_DIV(0));

	/* Configure the PLL. */
	PRCI_REG(PRCI_PLLCFG) = PLL_REFSEL(1) | PLL_BYPASS(1) | PLL_R(1) | PLL_F(PLL_F_VAL) | PLL_Q(1);

	/* Un-Bypass the PLL. */
	PRCI_REG(PRCI_PLLCFG) &= ~PLL_BYPASS(1);

	/* Wait for PLL Lock
	 * Note that the Lock signal can be glitchy, wait 100 us
	 * HFROSC is running at ... uhh, <24Mhz */
	cycle_delay(100 * 24);

	/* Now it is safe to check for PLL Lock */
	while ((PRCI_REG(PRCI_PLLCFG) & PLL_LOCK(1)) == 0) ;

	/* Switch over to PLL Clock source */
	PRCI_REG(PRCI_PLLCFG) |= PLL_SEL(1);
}

void main(void) {
	init_pll();
	init_traps();
	init_uart();
//	uart_send('H');
	frser_main();
	for(;;);
}
