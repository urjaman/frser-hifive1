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
#include "frser-flashapi.h"
#include "spilib.h"

void flash_set_safe(void) {
	spi_uninit();
}

void flash_select_protocol(uint8_t allowed_protocols) {
	(void)allowed_protocols;
	spi_init();
}

void flash_spiop(uint32_t sbytes, uint32_t rbytes) {
	spi_init_cond();
	spi_spiop(sbytes,rbytes);
}
