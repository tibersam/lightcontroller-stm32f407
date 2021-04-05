#ifndef ATXCONTROLLER
#define ATXCONTROLLER

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


#include "ws2812b.h"


/**
 * change_system_clock
 *
 * if mode 1 change clock to 8 MHz, if 0 to 164 MHz.
 * Also recalculate all values needed for USART1 Baudrate
 */
void change_system_clock(int mode);

/**
 * setup_gpio_atx
 *
 * Setup the gpio pins needed for controlling atx power supply and also
 * the signal for enabeling and dissabeling parts, that are not 5v standby
 */
void setup_gpio_atx(void);

/**
 * get_atx_status
 *
 * return if power ok signal is provided by atx
 */
int get_atx_status(void);

/**
 * enable_atx
 *
 * Enable the atx power supply. blocks until the atx ready signal is provided
 */
void enable_atx(void);

/**
 * disable_atx
 *
 * Disable sk2812 drivers and then disables atx power supply
 */
void disable_atx(void);
#endif
