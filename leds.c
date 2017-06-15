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
#include "leds.h"

#include "platform.h"

void init_leds(void) {
	GPIO_REG(GPIO_OUTPUT_VAL) |= LED_RED|LED_GREEN|LED_BLUE;
	GPIO_REG(GPIO_OUTPUT_EN) |= LED_RED|LED_GREEN|LED_BLUE;
}

void clear_led(uint32_t id) {
	GPIO_REG(GPIO_OUTPUT_VAL) |= id;
}

void set_led(uint32_t id) {
	GPIO_REG(GPIO_OUTPUT_VAL) &= ~id;
}
