#pragma once

/* entry.S */
void trap_entry(void);

/* traps.c */
void handle_trap(uint32_t mcause);

void init_traps(void);
