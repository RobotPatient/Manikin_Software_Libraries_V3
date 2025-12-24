#include "manikin_tick.h"
#include <zephyr/sys/clock.h>

size_t
timer_hal_get_tick ()
{
    return sys_clock_tick_get();
}

void
hal_delay_us (size_t period)
{
    k_usleep(period);
}
