/**
 * @file             test_sdp810.cpp
 * @brief           Test for Sensirion SDP810 Differential pressure sensor software module
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
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "sdp810/sdp810.h"
#include "fake_i2c_functions.h"

// Common mocks and types
static uint8_t       dummy_read_buf[4];
static uint8_t       handle = 1;
manikin_sensor_ctx_t dummy_ctx;

#define EMPTY_FILL_BYTE 0x01u
#define SDP810_I2C_ADDR 0x10u

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

// --- sdp810_init_sensor ---
TEST_CASE("sdp810_init_sensor handles null parameter", "[sdp810][REQ-F2]")
{
    reset_mocks();
    REQUIRE(sdp810_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_init_sensor succeeds with valid context", "[sdp810][REQ-F2]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = SDP810_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

// --- sdp810_read_sensor ---
TEST_CASE("sdp810_read_sensor fails with null context", "[sdp810][REQ-F2]")
{
    reset_mocks();
    REQUIRE(sdp810_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_read_sensor fails with null read buffer", "[sdp810][REQ-F2]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_read_sensor reads successfully", "[sdp810][REQ-F2]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = SDP810_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[16]                 = { 0 };
    REQUIRE(sdp810_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
}

// --- sdp810_deinit_sensor ---
TEST_CASE("sdp810_deinit_sensor handles null context", "[sdp810][REQ-F2]")
{
    reset_mocks();
    REQUIRE(sdp810_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_deinit_sensor succeeds with valid context", "[sdp810][REQ-F2]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = SDP810_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("sdp810_parse_raw_data handles null raw_data", "[sdp810][REQ-F3]")
{
    REQUIRE(sdp810_parse_raw_data(NULL, (sdp810_sample_data_t *)0x1234)
            == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_parse_raw_data handles null data pointer", "[sdp810][REQ-F3]")
{
    uint8_t dummy_raw[8] = { 0 }; // Minimum required length
    REQUIRE(sdp810_parse_raw_data(dummy_raw, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_parse_raw_data successfully converts valid raw data", "[sdp810][REQ-F3]")
{
    // Example: raw pressure = 0x07D0 (2000), temp = 0x03E8 (1000), scale = 0x03E8 (1000)
    uint8_t raw_data[8] = {
        0x07, 0xD0, // raw_pressure = 2000
        0x00,       // dummy CRC or unused
        0x03, 0xE8, // temp = 1000
        0x00,       // dummy CRC or unused
        0x03, 0xE8  // scale factor = 1000
    };

    sdp810_sample_data_t parsed_data;

    REQUIRE(sdp810_parse_raw_data(raw_data, &parsed_data) == MANIKIN_STATUS_OK);

    // Pressure = 2000 / 1000 = 2.0 mbar
    // Temp     = 1000 / 1000 = 1.0 mbar (note: 'mbar' may be a mislabel for temperature?)
    REQUIRE(parsed_data.air_pressure_mbar == Catch::Approx(2.0f));
    REQUIRE(parsed_data.air_temp_mbar == Catch::Approx(1.0f));
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}