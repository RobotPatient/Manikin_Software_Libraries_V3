#ifndef FAKE_WATCHDOG_FUNCTIONS_H
#define FAKE_WATCHDOG_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "fff.h"
#include "manikin_software_conf.h"

    DECLARE_FAKE_VALUE_FUNC2(int, watchdog_hal_init, manikin_watchdog_inst_t, uint32_t);
    DECLARE_FAKE_VALUE_FUNC1(int, watchdog_hal_kick, manikin_watchdog_inst_t);
    DECLARE_FAKE_VALUE_FUNC1(int, watchdog_hal_deinit, manikin_watchdog_inst_t);

#ifdef __cplusplus
}
#endif
#endif /* FAKE_WATCHDOG_FUNCTIONS_H */
