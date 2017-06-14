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
