#ifndef MANIKIN_PLATFORM_H
#define MANIKIN_PLATFORM_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include "fake_i2c_functions.h"
#include "fake_timer_functions.h"

#define MANIKIN_I2C_HAL_INIT(i2c_inst, baud) i2c_hal_init(i2c_inst, baud)

#define MANIKIN_I2C_HAL_DEVICE_ACKNOWLEDGE(i2c_inst, i2c_addr) \
    i2c_hal_device_acknowledge(i2c_inst, i2c_addr);

#define MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(i2c_inst) i2c_hal_error_flag_check(i2c_inst);

#define MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr, data, len) \
    i2c_hal_read_bytes(i2c_inst, i2c_addr, data, len)

#define MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr, data, len) \
    i2c_hal_write_bytes(i2c_inst, i2c_addr, data, len)

#define MANIKIN_I2C_HAL_DEINIT(i2c_inst) i2c_hal_deinit(i2c_inst)

#define MANIKIN_TIMER_HAL_INIT(timer_inst, freq) timer_hal_init(timer_inst, freq)

#define MANIKIN_TIMER_HAL_START(timer_inst) timer_hal_start(timer_inst);

#define MANIKIN_TIMER_HAL_STOP(timer_inst) timer_hal_stop(timer_inst);

#define MANIKIN_TIMER_HAL_DEINIT(timer_inst, freq) timer_hal_deinit(timer_inst)

#define MANIKIN_CRIT_ERROR_HANDLER(hash, line) {printf("Critical error occured in file: %d, line_num: %d\n", hash, line);}

#define MANIKIN_NON_CRIT_ERROR_HANDLER(hash, line) {printf("Non_critical error occured in file: %d, line_num: %d\n", hash, line);}
#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_PLATFORM_H */