/**
 * @file             test_vl6180x.cpp
 * @brief           Test for STM VL6180x ranging ToF sensor software module
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

#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "vl6180x/vl6180x.h"
#include "fake_i2c_functions.h"
#include "common/manikin_bit_manipulation.h"

// Common mocks and types
static uint8_t       dummy_read_buf[4];
static uint8_t       handle = 1;
manikin_sensor_ctx_t dummy_ctx;

#define VL6180X_I2C_ADDR           0x29u
#define VL6180X_FRESH_OUT_OF_RESET 0x0016u

// NOTE: Used for simulating sensor detection
uint16_t cur_reg;

size_t
custom_write_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, const uint8_t *bytes, size_t len)
{
    if (len >= sizeof(uint16_t))
    {
        cur_reg = CONSTRUCT_SHORT_FROM_BYTES(bytes[0], bytes[1]);
    }
    return len;
}

size_t
custom_read_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, uint8_t *bytes, size_t len)
{
    if (len >= sizeof(uint8_t) && cur_reg == VL6180X_FRESH_OUT_OF_RESET)
    {
        cur_reg = 0;
        // Signal that sensor is ready
        bytes[0] = 1u;
    }
    else
    {
        cur_reg  = 0u;
        bytes[0] = 200u;
    }
    return len;
}

void
reset_mocks ()
{
    RESET_FAKE(i2c_hal_init);
    RESET_FAKE(i2c_hal_error_flag_check);
    RESET_FAKE(i2c_hal_read_bytes);
    RESET_FAKE(i2c_hal_write_bytes);
    RESET_FAKE(i2c_hal_deinit);
}

TEST_CASE("vl6180x_init_sensor handles null parameter", "[vl6180x]")
{
    reset_mocks();
    REQUIRE(vl6180x_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl6180x_init_sensor succeeds with valid context", "[vl6180x]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL6180X_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl6180x_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("vl6180x_read_sensor fails with null context", "[vl6180x]")
{
    reset_mocks();
    REQUIRE(vl6180x_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl6180x_read_sensor fails with null read buffer", "[vl6180x]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl6180x_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl6180x_read_sensor reads successfully", "[vl6180x]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL6180X_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[1]                  = { 0 };
    REQUIRE(vl6180x_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
}

TEST_CASE("vl6180x_deinit_sensor handles null context", "[vl6180x]")
{
    reset_mocks();
    REQUIRE(vl6180x_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl6180x_deinit_sensor succeeds with valid context", "[vl6180x]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL6180X_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl6180x_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

int
main (int argc, char *argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}