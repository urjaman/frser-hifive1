#include "main.h"
#include "spilib.h"
#include "frser-cfg.h"

static uint8_t spi_initialized = 0;

uint32_t spi_set_speed(uint32_t hz) {
	return hz;
}

void spi_init_cond(void) {
	if (!spi_initialized) spi_init();
}

uint8_t spi_txrx(const uint8_t c) {
	(void)c;
	return 0;
/*
	spi_awrite(c);
	return spi_aread(); */
}

void spi_init(void) {
	spi_initialized = 1;
}

uint8_t spi_uninit(void) {
	if (spi_initialized) {
		spi_initialized = 0;
		return 1;
	}
	return 0;
}

void spi_select(void) {
// TODO
}

void spi_deselect(void) {
// TODO
}

// TODO: async api, and hook txrx
