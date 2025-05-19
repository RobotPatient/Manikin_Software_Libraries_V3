/**
 * @file             test_vl53l4cd.cpp
 * @brief           Test for STM VL53L4CD ranging ToF sensor software module
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
 * Author:          Johan Korten
 */

#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "vl53l4cd/vl53l4cd.h"
#include "fake_i2c_functions.h"
#include "common/manikin_bit_manipulation.h"

// Common mocks and types
static uint8_t       dummy_read_buf[4];
static uint8_t       handle = 1;
manikin_sensor_ctx_t dummy_ctx;

#define VL53L4CD_I2C_ADDR                  0x52u
#define VL53L4CD_SYSTEM_FRESH_OUT_OF_RESET 0x0016u
#define VL53L4CD_IDENTIFICATION_MODEL_ID   0x010Fu
#define VL53L4CD_RESULT_FINAL_RANGE_MM     0x0096u
#define VL53L4CD_SYSTEM_INTERRUPT_CLEAR    0x0086u

// For simulating sensor values
uint16_t cur_reg;
uint8_t  data_ready    = 1;
uint8_t  model_id_high = 0xEA;
uint8_t  model_id_low  = 0xCC;

// Custom read function that simulates a large distance value (over 255mm)
size_t
custom_read_large_distance (manikin_i2c_inst_t handle, uint8_t i2c_addr, uint8_t *bytes, size_t len)
{
    if (len >= sizeof(uint8_t) && cur_reg == VL53L4CD_SYSTEM_FRESH_OUT_OF_RESET)
    {
        cur_reg  = 0;
        bytes[0] = 1u; // Signal that sensor is ready
    }
    else if (cur_reg == VL53L4CD_IDENTIFICATION_MODEL_ID && len >= 2)
    {
        bytes[0] = model_id_high;
        bytes[1] = model_id_low;
        cur_reg  = 0;
    }
    else if (cur_reg == VL53L4CD_SYSTEM_INTERRUPT_CLEAR && len >= 1)
    {
        bytes[0] = data_ready; // Data ready flag
        cur_reg  = 0;
    }
    else if (cur_reg == VL53L4CD_RESULT_FINAL_RANGE_MM && len >= 2)
    {
        // Return simulated distance of 500mm (larger than 255)
        bytes[0] = 0x01; // high byte
        bytes[1] = 0xF4; // low byte (500 decimal = 0x01F4)
        cur_reg  = 0;
    }
    else
    {
        cur_reg = 0u;
        if (len >= 1)
        {
            bytes[0] = 0x01u;
        }
        if (len >= 2)
        {
            bytes[1] = 0x01u;
        }
    }
    return len;
}

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
    if (len >= sizeof(uint8_t) && cur_reg == VL53L4CD_SYSTEM_FRESH_OUT_OF_RESET)
    {
        cur_reg = 0;
        // Signal that sensor is ready
        bytes[0] = 1u;
    }
    else if (cur_reg == VL53L4CD_IDENTIFICATION_MODEL_ID && len >= 2)
    {
        // Return model ID
        bytes[0] = model_id_high;
        bytes[1] = model_id_low;
        cur_reg  = 0;
    }
    else if (cur_reg == VL53L4CD_SYSTEM_INTERRUPT_CLEAR && len >= 1)
    {
        // Return data ready flag
        bytes[0] = data_ready;
        cur_reg  = 0;
    }
    else if (cur_reg == VL53L4CD_RESULT_FINAL_RANGE_MM && len >= 2)
    {
        // Return simulated distance of 150mm
        bytes[0] = 0x00;
        bytes[1] = 0x96; // 150 decimal
        cur_reg  = 0;
    }
    else
    {
        cur_reg = 0u;
        if (len >= 1)
        {
            bytes[0] = 0x01u;
        }
        if (len >= 2)
        {
            bytes[1] = 0x01u;
        }
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
    data_ready = 1;
    cur_reg    = 0;
}

TEST_CASE("vl53l4cd_init_sensor handles null parameter", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    REQUIRE(vl53l4cd_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_init_sensor succeeds with valid context", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl53l4cd_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("vl53l4cd_read_sensor fails with null context", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    REQUIRE(vl53l4cd_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_read_sensor fails with null read buffer", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl53l4cd_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_read_sensor reads successfully", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[1]                  = { 0 };
    REQUIRE(vl53l4cd_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
    REQUIRE(read_buf[0] == 150);
}

TEST_CASE("vl53l4cd_read_sensor clips distances over 255mm to 255", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_large_distance;

    uint8_t read_buf[1] = { 0 };
    REQUIRE(vl53l4cd_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
    REQUIRE(read_buf[0] == 255); // Should be clipped to 255
}

TEST_CASE("vl53l4cd_read_sensor fails when data not ready", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    data_ready                           = 0; // Data not ready
    uint8_t read_buf[1]                  = { 0 };
    REQUIRE(vl53l4cd_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_ERR_READ_FAIL);
}

TEST_CASE("vl53l4cd_deinit_sensor handles null context", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    REQUIRE(vl53l4cd_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_deinit_sensor succeeds with valid context", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(vl53l4cd_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

TEST_CASE("vl53l4cd_read_sensor reinitializes when needed", "[vl53l4cd][REQ-F1]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = VL53L4CD_I2C_ADDR;
    dummy_ctx.needs_reinit               = 1; // Needs reinitialization
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[1]                  = { 0 };
    REQUIRE(vl53l4cd_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
    REQUIRE(dummy_ctx.needs_reinit == 0); // Flag should be cleared
}

TEST_CASE("vl53l4cd_parse_raw_data handles null raw_data", "[vl53l4cd][REQ-F3]")
{
    REQUIRE(vl53l4cd_parse_raw_data(NULL, (vl53l4cd_sample_data_t *)0x1234)
            == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_parse_raw_data handles null data pointer", "[vl53l4cd][REQ-F3]")
{
    uint8_t dummy_raw[1] = { 0x00 }; // Dummy distance data
    REQUIRE(vl53l4cd_parse_raw_data(dummy_raw, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("vl53l4cd_parse_raw_data successfully converts valid raw data", "[vl53l4cd][REQ-F3]")
{
    uint8_t                raw_data[1] = { 0x5A }; // Example: 90 mm
    vl53l4cd_sample_data_t parsed_data;

    REQUIRE(vl53l4cd_parse_raw_data(raw_data, &parsed_data) == MANIKIN_STATUS_OK);
    REQUIRE(parsed_data.distance_mm == 0x5A);
}

int
main (int argc, char *argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}