/**
 * @file            test_ads7138.cpp
 * @brief           Test for TI ADS7138 8-channel 16-bit ADC driver software module
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
#include "ads7138/ads7138.h"
#include "fake_i2c_functions.h"

// Common mocks and types
static uint8_t       dummy_read_buf[4];
static uint8_t       handle = 1;
manikin_sensor_ctx_t dummy_ctx;

#define EMPTY_FILL_BYTE  0xFFu
#define ADS7138_I2C_ADDR 0x14u

size_t
custom_write_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, const uint8_t *bytes, size_t len)
{
    return len;
}

size_t
custom_read_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, uint8_t *bytes, size_t len)
{
    memset(bytes, EMPTY_FILL_BYTE, len);
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

TEST_CASE("ads7138_init_sensor handles null parameter", "[ads7138][REQ-F3]")
{
    reset_mocks();
    REQUIRE(ads7138_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("ads7138_init_sensor succeeds with valid context", "[ads7138][REQ-F3]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = ADS7138_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(ads7138_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("ads7138_read_sensor fails with null context", "[ads7138][REQ-F3]")
{
    reset_mocks();
    REQUIRE(ads7138_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("ads7138_read_sensor fails with null read buffer", "[ads7138][REQ-F3]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(ads7138_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("ads7138_read_sensor reads successfully", "[ads7138][REQ-F3]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = ADS7138_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[16]                 = { 0 };
    REQUIRE(ads7138_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
}

TEST_CASE("ads7138_deinit_sensor handles null context", "[ads7138][REQ-F3]")
{
    reset_mocks();
    REQUIRE(ads7138_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("ads7138_deinit_sensor succeeds with valid context [REQ-F3]", "[ads7138]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = ADS7138_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(ads7138_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}