#ifndef WAIT_HEADER
#define WAIT_HEADER

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>


/**
 * init_wait
 * 
 * reset tick to 0
 */
void init_wait(void);

/**
 * get_tick
 *
 * returns current tick counter
 */
uint64_t get_tick(void);

/**
 * wait_until
 *
 * timestamp timestamp to wait until
 */
void wait_until(uint64_t timestamp);

/**
 * wait
 *
 * ms waitlength in ms
 */
void wait(uint64_t ms);
#endif
