#ifndef TIMEOUT_MODULE_H
#define TIMEOUT_MODULE_H

#include <stdint.h>

void init_timeout_module(void);

void reset_timeout(void);

void set_timeouttime(uint64_t timeout);

void check_timeout(void);

#endif


