/**
 * @file            test_bmm350.cpp
 * @brief           Test for Bosch BMM350 3-axis magnetometer software module
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
#include "bmm350/bmm350.h"
#include "fake_i2c_functions.h"

// Common mocks and types
static uint8_t       dummy_read_buf[4];
static uint8_t       handle = 1;
manikin_sensor_ctx_t dummy_ctx;

#define EMPTY_FILL_BYTE 0x01u
#define BMM350_I2C_ADDR 0x14u

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

// --- bmm350_init_sensor ---
TEST_CASE("bmm350_init_sensor handles null parameter", "[bmm350][REQ-F4]")
{
    reset_mocks();
    REQUIRE(bmm350_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_init_sensor succeeds with valid context", "[bmm350][REQ-F4]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = BMM350_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(bmm350_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

// --- bmm350_read_sensor ---
TEST_CASE("bmm350_read_sensor fails with null context", "[bmm350][REQ-F4]")
{
    reset_mocks();
    REQUIRE(bmm350_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_read_sensor fails with null read buffer", "[bmm350][REQ-F4]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(bmm350_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_read_sensor reads successfully", "[bmm350][REQ-F4]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = BMM350_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[16]                 = { 0 };
    REQUIRE(bmm350_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
}

// --- bmm350_deinit_sensor ---
TEST_CASE("bmm350_deinit_sensor handles null context", "[bmm350][REQ-F4]")
{
    reset_mocks();
    REQUIRE(bmm350_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_deinit_sensor succeeds with valid context", "[bmm350][REQ-F4]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = BMM350_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(bmm350_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("bmm350_parse_raw_data handles null raw_data", "[bmm350][REQ-F3]")
{
    REQUIRE(bmm350_parse_raw_data(NULL, (bmm350_sample_data_t *)0x1234)
            == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_parse_raw_data handles null data pointer", "[bmm350][REQ-F3]")
{
    uint8_t dummy_raw[15] = { 0 }; // Minimum length for 5 x 3-byte fields
    REQUIRE(bmm350_parse_raw_data(dummy_raw, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("bmm350_parse_raw_data successfully parses valid 24-bit signed data", "[bmm350][REQ-F3]")
{
    // Example values:
    // magneto_x = 0x001234 → 4660
    // magneto_y = 0xFFEEDD → -4369 (signed 24-bit)
    // magneto_z = 0x000001 → 1
    // temperature = 0xFFFFFF → -1 (signed 24-bit)
    // sensor_time = 0x800000 → -8388608 (lowest signed 24-bit)

    uint8_t raw_data[15] = {
        0x34, 0x12, 0x00, // magneto_x
        0xDD, 0xEE, 0xFF, // magneto_y
        0x01, 0x00, 0x00, // magneto_z
        0xFF, 0xFF, 0xFF, // temperature
        0x00, 0x00, 0x80  // sensor_time
    };

    bmm350_sample_data_t parsed_data;

    REQUIRE(bmm350_parse_raw_data(raw_data, &parsed_data) == MANIKIN_STATUS_OK);

    REQUIRE(parsed_data.magneto_x_ut == 0x001234); // 4660
    REQUIRE(parsed_data.magneto_y_ut == -4387);    // 0xFFEEDD
    REQUIRE(parsed_data.magneto_z_ut == 1);
    REQUIRE(parsed_data.temperature_mdeg == -1);     // 0xFFFFFF
    REQUIRE(parsed_data.sensor_time_us == -8388608); // 0x800000
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}