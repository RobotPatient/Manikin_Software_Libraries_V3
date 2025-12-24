#ifndef MANIKIN_TICK_H
#define MANIKIN_TICK_H
#include <stddef.h>

void hal_delay_us(size_t period);

size_t timer_hal_get_tick();

#endif /* MANIKIN_TICK_H */