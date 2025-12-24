/**
 * @file             manikin_platform.h
 * @brief           Platform bindings configuration for Manikin Libraries V3 library
 *
 * @par
 * Copyright 2025 (C) RobotPatient Simulators
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of the Manikin Software Libraries V3 project
 *
 * Author:          Victor Hogeweij
 */

#ifndef MANIKIN_PLATFORM_H
#define MANIKIN_PLATFORM_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include "manikin_i2c_functions.h"
#include "manikin_tick.h"
#include "manikin_error_functions.h"

/**
 * @brief Manikin I2C HAL init binding
 * @param i2c_inst Platform specific typedef ptr to your memory mapped i2c peripheral (e.g. handle)
 * @param baud The i2c-bus frequency on which the bus shall run
 * @return Integer value 0 on success, 1 on fail
 */
#define MANIKIN_I2C_HAL_INIT(i2c_inst, baud) i2c_hal_init(i2c_inst, baud)

/**
 * @brief Check for device acknowledgment on the I2C bus
 * @param i2c_inst I2C instance
 * @param i2c_addr Address of the I2C device
 * @return 0 if no device acknowledged, 1 if device acknowledged
 */
#define MANIKIN_I2C_HAL_DEVICE_ACKNOWLEDGE(i2c_inst, i2c_addr) \
    i2c_hal_device_acknowledge(i2c_inst, i2c_addr);

/**
 * @brief Check if any error flags are set on the I2C peripheral
 * @param i2c_inst I2C instance
 * @return 0 if no error condition was detected on bus, 1 if there was an error condition
 */
#define MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(i2c_inst) i2c_hal_error_flag_check(i2c_inst);

/**
 * @brief Read multiple bytes from an I2C device
 * @param i2c_inst I2C instance
 * @param i2c_addr Address of the I2C device
 * @param data Pointer to buffer to store read data
 * @param len Number of bytes to read
 * @return Number of bytes read
 */
#define MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr, data, len) \
    i2c_hal_read_bytes(i2c_inst, i2c_addr, data, len)

/**
 * @brief Write multiple bytes to an I2C device
 * @param i2c_inst I2C instance
 * @param i2c_addr Address of the I2C device
 * @param data Pointer to data to write
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
#define MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr, data, len) \
    i2c_hal_write_bytes(i2c_inst, i2c_addr, data, len)

/**
 * @brief Deinitialize the I2C peripheral
 * @param i2c_inst I2C instance
 * @return Integer value 0 on success, 1 on fail
 */
#define MANIKIN_I2C_HAL_DEINIT(i2c_inst) i2c_hal_deinit(i2c_inst)

/**
 * @brief Get the current system tick count
 * @return Tick count (usually in milliseconds)
 */
#define MANIKIN_HAL_GET_TICK() timer_hal_get_tick()

/**
 * @brief Reset the I2C peripheral and sensor by toggling the physical voltage line of sensor
 * @param i2c_inst I2C instance
 * @return Integer value 0 on success, 1 on fail
 */
#define MANIKIN_I2C_BUS_RESET(i2c_inst) i2c_hal_bus_reset(i2c_inst)

/**
 * @brief Attempt to recover the I2C bus from a stuck state
 * @return Integer value 0 on success, 1 on fail
 */
#define MANIKIN_I2C_BUS_RECOVER() i2c_hal_bus_recover()

/**
 * @brief Get the current baud rate of the I2C peripheral
 * @param i2c_inst I2C instance
 * @return Current baud rate in Hz
 */
#define MANIKIN_I2C_GET_BAUD(i2c_inst) i2c_hal_get_baud(i2c_inst)

/**
 * @brief Handle a critical error by printing the file and line number
 * @param hash File identifier or hash
 * @param line Line number where the error occurred
 */
#define MANIKIN_CRIT_ERROR_HANDLER(hash, line) critical_error(hash, line)

/**
 * @brief Handle a non-critical error by printing the file and line number
 * @param hash File identifier or hash
 * @param line Line number where the error occurred
 */
#define MANIKIN_NON_CRIT_ERROR_HANDLER(hash, line) non_critical_error(hash, line)

#define MANIKIN_DELAY_US(period) hal_delay_us(period)

#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_PLATFORM_H */