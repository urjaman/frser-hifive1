#include "main.h"
#include "delay.h"


/* This can be used from weird clocking situations, with appropriate math in caller. */
/*
void cycle_delay(const uint32_t cycles)
{
	uint32_t tmp1, tmp2;
	asm volatile (
		"rdcycle %[tmp1]\n\t"
	        "1: \n\t"
		"rdcycle %[tmp2]\n\t"
		"sub %[tmp2], %[tmp2], %[tmp1]\n\t"
	        "bltu %[tmp2], %[cyc], 1b\n\t"
	        : [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2) : [cyc] "r" (cycles)
	);
}
*/

void cycle_delay(const uint32_t cycles)
{
	uint32_t loops;
	if (cycles < 2) return; /* Safety */
	loops = cycles >> 1; // 2 cycles/loop assumption.
	asm volatile (
	        "1: \n\t"
		"addi %[cyc], %[cyc], -1\n\t"
	        "bnez %[cyc], 1b\n\t"
	        : : [cyc] "r" (loops)
	);
}


/* This is for the runtime fixed frequency use. */
void udelay(uint32_t us)
{
	const uint32_t cycles_per_us = F_CPU/1000000UL;
	cycle_delay(us*cycles_per_us);
}
