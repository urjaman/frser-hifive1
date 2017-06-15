#pragma once

#define LED_RED (1<<22)
#define LED_GREEN (1<<19)
#define LED_BLUE (1<<21)

void init_leds(void);
void set_led(uint32_t id);
void clear_led(uint32_t id);
