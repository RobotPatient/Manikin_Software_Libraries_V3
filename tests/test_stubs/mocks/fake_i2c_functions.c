#include "fake_i2c_functions.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

DEFINE_FAKE_VALUE_FUNC2(int, i2c_hal_init, manikin_i2c_inst_t, uint32_t);
DEFINE_FAKE_VALUE_FUNC2(uint8_t, i2c_hal_device_acknowledge, manikin_i2c_inst_t, uint8_t);
DEFINE_FAKE_VALUE_FUNC4(
    size_t, i2c_hal_write_bytes, manikin_i2c_inst_t, uint8_t, const uint8_t *, size_t);
DEFINE_FAKE_VALUE_FUNC4(size_t, i2c_hal_read_bytes, manikin_i2c_inst_t, uint8_t, uint8_t *, size_t);
DEFINE_FAKE_VALUE_FUNC1(uint32_t, i2c_hal_error_flag_check, manikin_i2c_inst_t);
DEFINE_FAKE_VALUE_FUNC1(int, i2c_hal_deinit, manikin_i2c_inst_t);
