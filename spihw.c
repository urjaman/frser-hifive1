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
#include "spilib.h"
#include "frser-cfg.h"

#include "platform.h"
#include "hifive1.h"

#include "delay.h"

/* This is for my serprog shield. */
#define SPI_CS (1 << PIN_8_OFFSET)

/* These are fixed by HW. */
#define SPI_SCK  (1 << 5)
#define SPI_MOSI (1 << 3)
#define SPI_MISO (1 << 4)

#define SPI_HW_PINS (SPI_SCK|SPI_MOSI|SPI_MISO)

static uint8_t spi_initialized = 0;
static uint32_t spi_div = 15; // 256 / 2*(15+1) = 8Mhz

uint32_t spi_set_speed(uint32_t hz) {
	if (hz > F_CPU/2) { /* I mean, really? well, whatever. */
		spi_div = 0;
	} else {
		uint32_t div = (((F_CPU/2)+(hz-1)) / hz)-1;
		if (div > 4095) div = 4095;
		spi_div = div;
	}
	if (spi_initialized) spi_init(); /* Apply speed change */
	return F_CPU / (2*(spi_div+1));
}

void spi_init_cond(void) {
	if (!spi_initialized) spi_init();
}

uint8_t spi_txrx(const uint8_t c) {
	spi_awrite_fast(c);
	return spi_aread();
}

void spi_init(void) {
	GPIO_REG(GPIO_OUTPUT_VAL) |= SPI_CS;
	GPIO_REG(GPIO_OUTPUT_EN) |= SPI_CS;

	/* Oh wow, reset defaults are basically fine for us. */
	SPI1_REG(SPI_REG_SCKDIV) = spi_div;

	GPIO_REG(GPIO_IOF_SEL) &= ~SPI_HW_PINS;
	GPIO_REG(GPIO_IOF_EN) |= SPI_HW_PINS;

	spi_initialized = 1;
}

uint8_t spi_uninit(void) {
	if (spi_initialized) {
		GPIO_REG(GPIO_IOF_EN) &= ~SPI_HW_PINS;
		GPIO_REG(GPIO_OUTPUT_EN) &= ~SPI_CS;
		spi_initialized = 0;
		return 1;
	}
	return 0;
}

void spi_select(void) {
	GPIO_REG(GPIO_OUTPUT_VAL) &= ~SPI_CS;
	udelay(1);
}

void spi_deselect(void) {
	udelay(1);
	GPIO_REG(GPIO_OUTPUT_VAL) |= SPI_CS;
	udelay(1);
}

static int flight_bytes = 0;

void spi_awrite_fast(uint8_t d) {
	// this be fast.
	SPI1_REG(SPI_REG_TXFIFO) = d;
	flight_bytes++;
}

void spi_awrite(uint8_t d) {
	// we flush out previous bytes from the rxfifo...
	for(;;) {
		uint32_t rx = SPI1_REG(SPI_REG_RXFIFO);
		if (rx & SPI_RXFIFO_EMPTY) break;
		flight_bytes--;
	}
	// then push the byte to the FIFO
	while (SPI1_REG(SPI_REG_TXFIFO) & SPI_TXFIFO_FULL) ;
	SPI1_REG(SPI_REG_TXFIFO) = d;
	flight_bytes++;
}

uint8_t spi_aread(void) {
	/* Flush the oldest bytes, and return the last one pushed. */
	uint32_t rx;
	while (flight_bytes) {
		rx = SPI1_REG(SPI_REG_RXFIFO);
		if (rx & SPI_RXFIFO_EMPTY) continue;
		flight_bytes--;
	}
	return rx;
}
