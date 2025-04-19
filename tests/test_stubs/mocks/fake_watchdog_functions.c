#include "fake_watchdog_functions.h"

DEFINE_FAKE_VALUE_FUNC2(int, watchdog_hal_init, manikin_watchdog_inst_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC1(int, watchdog_hal_kick, manikin_watchdog_inst_t);
DEFINE_FAKE_VALUE_FUNC1(int, watchdog_hal_deinit, manikin_watchdog_inst_t);
