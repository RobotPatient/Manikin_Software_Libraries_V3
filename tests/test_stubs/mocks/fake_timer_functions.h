#ifndef FAKE_TIMER_FUNCTIONS_H
#define FAKE_TIMER_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "fff.h"
#include "manikin_software_conf.h"

    DECLARE_FAKE_VALUE_FUNC2(int, timer_hal_init, manikin_timer_inst_t, uint32_t);
    DECLARE_FAKE_VALUE_FUNC1(int, timer_hal_start, manikin_timer_inst_t);
    DECLARE_FAKE_VALUE_FUNC1(int, timer_hal_stop, manikin_timer_inst_t);
    DECLARE_FAKE_VALUE_FUNC1(int, timer_hal_deinit, manikin_timer_inst_t);

#ifdef __cplusplus
}
#endif
#endif /* FAKE_TIMER_FUNCTIONS_H */