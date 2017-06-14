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

	/* Slow down the QSPI. Not that we need it. */
	SPI0_REG(SPI_REG_SCKDIV) = 8;

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
	uart_send('H');
	frser_main();
	for(;;);
}
