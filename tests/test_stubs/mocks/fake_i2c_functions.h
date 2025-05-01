/**
 * @file             fake_i2c_functions.h
 * @brief            Test stubs used in software tests for testing i2c module
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

#ifndef FAKE_I2C_FUNCTIONS_H
#define FAKE_I2C_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>
#include "fff.h"
#include "manikin_software_conf.h"

    DECLARE_FAKE_VALUE_FUNC2(uint32_t, i2c_hal_init, manikin_i2c_inst_t, uint32_t)

    DECLARE_FAKE_VALUE_FUNC2(uint8_t, i2c_hal_device_acknowledge, manikin_i2c_inst_t, uint8_t)

    DECLARE_FAKE_VALUE_FUNC4(
        size_t, i2c_hal_write_bytes, manikin_i2c_inst_t, const uint8_t, const uint8_t *, size_t)

    DECLARE_FAKE_VALUE_FUNC4(
        size_t, i2c_hal_read_bytes, manikin_i2c_inst_t, uint8_t, uint8_t *, size_t)

    DECLARE_FAKE_VALUE_FUNC1(uint32_t, i2c_hal_error_flag_check, manikin_i2c_inst_t)

    DECLARE_FAKE_VALUE_FUNC1(uint32_t, i2c_hal_deinit, manikin_i2c_inst_t)

    DECLARE_FAKE_VALUE_FUNC1(uint32_t, i2c_hal_bus_reset, manikin_i2c_inst_t)
    DECLARE_FAKE_VALUE_FUNC0(uint32_t, i2c_hal_bus_recover)

    DECLARE_FAKE_VALUE_FUNC1(uint32_t, i2c_hal_get_baud, manikin_i2c_inst_t)
#ifdef __cplusplus
}
#endif
#endif /* FAKE_I2C_FUNCTIONS_H */
