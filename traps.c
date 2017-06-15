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
#include "uart.h"

#include "leds.h"

/* These are provided by the BSP. */
#include "platform.h"
#include "encoding.h"

static void panic(uint32_t info) {
	set_led(LED_RED);
	/* Require the compiler to keep info in a register here. */
	asm volatile ("" :: "r" (info) );
	for(;;);
}

static void machine_interrupt(void) {
	volatile uint32_t* claim = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_CLAIM_OFFSET);
	uint32_t id = *claim;
	switch (id) {
		case 0: /* Nothing */
			return;
		case 3: /* UART0 */
			uart_isr();
			break;
		default: /* Whoops */
			panic(id|0x1000);
	}
	*claim = id; /* complete id */
}

static void handle_interrupt(uint32_t code) {
	switch (code) {
		case 11: /* Machine external interrupt */
			machine_interrupt();
			break;
		default:
			panic(code);
	}
}

static void handle_exception(uint32_t code) {
	panic(code | 0x2000);
}

void handle_trap(uint32_t mcause) {
	uint32_t code = mcause&MCAUSE_CAUSE;
	if (mcause&MCAUSE_INT) {
		handle_interrupt(code);
	} else {
		handle_exception(code);
	}
}

void init_traps(void) {
	/* Minimum clear of the PLIC. */
	volatile uint32_t* plic_enables = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_ENABLE_OFFSET);
	for (int i=0;i<8;i++) plic_enables[i] = 0;
	volatile uint32_t* plic_threshold = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_THRESHOLD_OFFSET);
	*plic_threshold = 0;
	/* Clear any claimables. */
	volatile uint32_t* claim = (uint32_t*)(PLIC_CTRL_ADDR + PLIC_CLAIM_OFFSET);
	uint32_t id;
	do {
		id = *claim;
		if (id) *claim = id;
	} while (id);

	/* Set the vector. */
	write_csr(mtvec, &trap_entry);

	/* Disable timer interrupts... */
	clear_csr(mie, MIP_MTIP);
	/* Enable Machine Interrupts. */
	set_csr(mie, MIP_MEIP);

	/* Enable Interrupts */
	set_csr(mstatus, MSTATUS_MIE);
}
