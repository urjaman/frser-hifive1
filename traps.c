#include "main.h"
#include "traps.h"

/* Thse are provided by the BSP. */
#include "platform.h"
#include "encoding.h"


static void machine_interrupt(void) {

}

static void handle_interrupt(uint32_t code) {
	switch (code) {
		case 11: /* Machine external interrupt */
			machine_interrupt();
			break;
		default:
			/* TODO: Uhh... */
			for(;;);
	}
}

static void handle_exception(uint32_t code) {
	/* TODO: Hey, do something about it... */
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
  write_csr(mtvec, &trap_entry);
}
