#include "main.h"
#include "delay.h"

#include "platform.h"
#include "encoding.h"

void cycle_delay(const uint32_t cycles)
{
	uint32_t start = read_csr(mcycle);
	uint32_t passed;
	do {
		passed = read_csr(mcycle) - start;
	} while (passed < cycles);
}


/* This is for the runtime fixed frequency use. */
void udelay(uint32_t us)
{
	const uint32_t cycles_per_us = F_CPU/1000000UL;
	cycle_delay(us*cycles_per_us);
}
