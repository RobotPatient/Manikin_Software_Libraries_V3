#ifndef MANIKIN_TIMER_FUNCTIONS_H
#define MANIKIN_TIMER_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "manikin_software_conf.h"
    int timer_hal_init(manikin_timer_inst_t timer_inst, uint32_t freq);

    int timer_hal_start(manikin_timer_inst_t timer_inst);

    int timer_hal_stop(manikin_timer_inst_t timer_inst);

    int timer_hal_deinit(manikin_timer_inst_t timer_inst);
#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_TIMER_FUNCTIONS_H */