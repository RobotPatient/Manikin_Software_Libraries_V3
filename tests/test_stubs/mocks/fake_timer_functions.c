#include "fake_timer_functions.h"

DEFINE_FAKE_VALUE_FUNC2(int, timer_hal_init, manikin_timer_inst_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC1(int, timer_hal_start, manikin_timer_inst_t);
DEFINE_FAKE_VALUE_FUNC1(int, timer_hal_stop, manikin_timer_inst_t);
DEFINE_FAKE_VALUE_FUNC1(int, timer_hal_deinit, manikin_timer_inst_t);