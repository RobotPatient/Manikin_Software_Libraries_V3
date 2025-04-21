/**
 * @file             test_spi.cpp
 * @brief           Test for platform-agnostic spi wrapper module
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
#include "spi/spi.h"
#include "manikin_software_conf.h"
#include "fake_spi_functions.h"

uint8_t spi_write_buffer[64] = {};
uint8_t spi_read_buffer[64]  = {};

size_t
custom_spi_write (manikin_spi_inst_t inst, const uint8_t *data, size_t len)
{
    REQUIRE(len <= sizeof(spi_write_buffer));
    memcpy(spi_write_buffer, data, len);
    return len;
}

size_t
custom_spi_read (manikin_spi_inst_t inst, uint8_t *data, size_t len)
{
    REQUIRE(len <= sizeof(spi_read_buffer));
    memcpy(data, spi_read_buffer, len);
    return len;
}

void
reset_spi_mocks ()
{
    RESET_FAKE(spi_hal_init);
    RESET_FAKE(spi_hal_deinit);
    RESET_FAKE(spi_hal_write_bytes);
    RESET_FAKE(spi_hal_read_bytes);
    RESET_FAKE(spi_hal_set_cs);

    memset(spi_write_buffer, 0, sizeof(spi_write_buffer));
    memset(spi_read_buffer, 0, sizeof(spi_read_buffer));
}

TEST_CASE("spi_init returns error on HAL failure", "[init]")
{
    reset_spi_mocks();
    spi_hal_init_fake.return_val = MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL;

    uint8_t inst = 1u;

    manikin_status_t status = manikin_spi_init(&inst, MANIKIN_SOFTWARE_MIN_SPI_SPEED);
    REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL);
    REQUIRE(spi_hal_init_fake.call_count == 1u);
}

TEST_CASE("spi_init succeeds", "[init]")
{
    reset_spi_mocks();
    spi_hal_init_fake.return_val = MANIKIN_STATUS_OK;
    uint8_t inst                 = 1u;

    manikin_status_t status = manikin_spi_init(&inst, MANIKIN_SOFTWARE_MIN_SPI_SPEED);
    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(spi_hal_init_fake.call_count == 1u);
}

TEST_CASE("spi_write writes correct data", "[write]")
{
    reset_spi_mocks();

    spi_hal_write_bytes_fake.custom_fake = custom_spi_write;
    spi_hal_write_bytes_fake.return_val  = 3u;
    uint8_t inst                         = 1u;

    uint8_t test_data[] = { 0xAA, 0xBB, 0xCC };
    size_t  written     = manikin_spi_write(&inst, test_data, sizeof(test_data));

    REQUIRE(written == 3u);
    REQUIRE(spi_write_buffer[0] == 0xAA);
    REQUIRE(spi_write_buffer[1] == 0xBB);
    REQUIRE(spi_write_buffer[2] == 0xCC);
}

TEST_CASE("spi_read reads correct data", "[read]")
{
    reset_spi_mocks();

    spi_hal_read_bytes_fake.custom_fake = custom_spi_read;
    spi_hal_read_bytes_fake.return_val  = 3;

    // Simulate device returning data
    spi_read_buffer[0] = 0x11;
    spi_read_buffer[1] = 0x22;
    spi_read_buffer[2] = 0x33;
    uint8_t inst       = 1;
    uint8_t output[3]  = {};
    size_t  read       = manikin_spi_read(&inst, output, sizeof(output));

    REQUIRE(read == 3);
    REQUIRE(output[0] == 0x11);
    REQUIRE(output[1] == 0x22);
    REQUIRE(output[2] == 0x33);
}

TEST_CASE("spi_start_transaction sets CS low", "[cs]")
{
    reset_spi_mocks();
    spi_hal_set_cs_fake.return_val = MANIKIN_STATUS_OK;

    manikin_spi_cs_t spi_cs = { 5, 1 };

    manikin_status_t status = manikin_spi_start_transaction(spi_cs);
    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(spi_hal_set_cs_fake.arg1_val == 0); // CS LOW
}

TEST_CASE("spi_end_transaction sets CS high", "[cs]")
{
    reset_spi_mocks();
    spi_hal_set_cs_fake.return_val = MANIKIN_STATUS_OK;
    manikin_spi_cs_t spi_cs        = { 5, 1 };

    manikin_status_t status = manikin_spi_end_transaction(spi_cs);
    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(spi_hal_set_cs_fake.arg1_val == 1); // CS HIGH
}

TEST_CASE("spi_deinit returns correct status", "[deinit]")
{
    reset_spi_mocks();
    spi_hal_deinit_fake.return_val = MANIKIN_STATUS_OK;

    uint8_t inst = 1;

    manikin_status_t status = manikin_spi_deinit(&inst);
    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(spi_hal_deinit_fake.call_count == 1);
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}