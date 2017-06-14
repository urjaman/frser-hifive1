#include "main.h"
#include "traps.h"
#include "uart.h"

/* Thse are provided by the BSP. */
#include "platform.h"
#include "encoding.h"

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
			//uart_send(0x15);
			for(;;);
	}
	*claim = id; /* complete id */
}

static void handle_interrupt(uint32_t code) {
	switch (code) {
		case 11: /* Machine external interrupt */
			machine_interrupt();
			break;
		default:
			/* TODO: Uhh... */
			//uart_send(0x15);
			for(;;);
	}
}

static void handle_exception(uint32_t code) {
	/* TODO: Do more than just blind-send a NAK. */
	//uart_send(0x15);
	for(;;);
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

	/* Enable Machine Interrupts. */
	clear_csr(mie, MIP_MTIP);
	set_csr(mie, MIP_MEIP);

	/* Enable Interrupts in general. */
	set_csr(mstatus, MSTATUS_MIE);
}
