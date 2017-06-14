#pragma once

/* This will give spi_set_speed if needed. A bit illogically maybe, but anyways... */
#include "frser-flashapi.h"

void spi_init(void);
uint8_t spi_uninit(void);
uint8_t spi_txrx(const uint8_t c);

//#define FRSER_ASYNC_SPI_API
void spi_awrite_fast(uint8_t d);
void spi_awrite(uint8_t d);
uint8_t spi_aread(void);

void spi_select(void);
void spi_deselect(void);

void spi_init_cond(void);
