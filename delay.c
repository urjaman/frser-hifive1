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
