/**
 * @file             test_w25qxx.cpp
 * @brief           Test for Winbond W25QXX 128 Mbit SPI NOR flash chip driver
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
#include <w25qxx128/w25qxx128.h>
#include "fake_spi_functions.h"
#include "fake_timer_functions.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <common/manikin_types.h>

// NOTE: Test buffers used amongst all tests and cleared in reset_mocks function
std::vector<uint8_t> spi_write_buffer;
std::vector<uint8_t> spi_read_buffer;
bool                 busy_reg_called = false;

// NOTE: Timeout flag and invalid data to simulate errors
bool timeout_occurred  = false;
bool invalid_data_read = false;

size_t
custom_spi_write (manikin_spi_inst_t inst, const uint8_t *data, size_t len)
{
    if (timeout_occurred)
    {
        return 0; // Simulate a timeout by returning 0 bytes written
    }
    if (data != NULL)
    {
        if (data[0] == 0x01u)
        {
            busy_reg_called = true;
        }
    }
    std::copy(data, data + len, std::back_inserter(spi_write_buffer));
    return len;
}

size_t
custom_spi_read (manikin_spi_inst_t inst, uint8_t *data, size_t len)
{
    if (timeout_occurred)
    {
        return 0; // Simulate a timeout by returning 0 bytes read
    }

    if (invalid_data_read)
    {
        // Simulate invalid data by returning garbage
        std::fill(data, data + len, 0xFF);
    }
    else
    {
        if (busy_reg_called && len == 1)
        {
            data[0] = 0x00; // Indicate that the chip is busy
        }
        else
        {
            // Ensure we don't copy more than what is available in the buffer
            size_t bytes_to_copy = std::min(len, spi_read_buffer.size());
            std::copy(spi_read_buffer.begin(), spi_read_buffer.end(), data);
            spi_read_buffer.erase(spi_read_buffer.begin(), spi_read_buffer.begin() + bytes_to_copy);

            // If we still have more data in the buffer, move the remaining data to the front
            if (!spi_read_buffer.empty())
            {
                std::move(spi_read_buffer.begin(), spi_read_buffer.end(), spi_read_buffer.begin());
            }
        }
    }
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
    RESET_FAKE(timer_hal_get_tick);
    timer_hal_get_tick_fake.return_val = 0;
    spi_write_buffer.clear();
    spi_write_buffer.reserve(64);
    spi_read_buffer.clear();
    spi_read_buffer.reserve(64);
}

TEST_CASE("w25qxx_init returns correct status", "[init]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000 };

    /* JEDEC ID */
    spi_read_buffer.push_back(0xEF);
    spi_read_buffer.push_back(0x70);
    spi_read_buffer.push_back(0x18);

    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    // Test the initialization function
    REQUIRE(w25qxx_init(&mem_ctx) == MANIKIN_STATUS_OK);

    // Verify that JEDEC ID read occurred
    CHECK(spi_hal_read_bytes_fake.call_count == 1);
    CHECK(spi_hal_write_bytes_fake.call_count == 2);
    CHECK(spi_write_buffer.at(0) == 0x9F);
    CHECK(spi_write_buffer.at(1) == 0x66);
    CHECK(spi_write_buffer.at(2) == 0x99);
}

TEST_CASE("w25qxx_status correctly checks for chip presence", "[status]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000, .fault_cnt = 0 };
    mem_ctx.fault_cnt = 0;

    /* JEDEC ID */
    spi_read_buffer.push_back(0xEF);
    spi_read_buffer.push_back(0x70);
    spi_read_buffer.push_back(0x18);

    // Mock SPI read to return a valid JEDEC ID
    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    // Test the status check function
    REQUIRE(w25qxx_status(&mem_ctx) == MANIKIN_STATUS_OK);

    // Ensure the function interacted with SPI
    CHECK(spi_hal_read_bytes_fake.call_count == 1);
    CHECK(spi_hal_write_bytes_fake.call_count == 1);
    CHECK(spi_write_buffer.at(0) == 0x9F);
}

TEST_CASE("w25qxx_write writes data to the memory chip", "[write]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000, .fault_cnt = 0 };

    uint8_t  test_data[4] = { 1, 2, 3, 4 };
    uint32_t addr         = 0;

    // Simulate successful SPI write
    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;
    auto status = w25qxx_write(&mem_ctx, test_data, addr, sizeof(test_data));
    // Test the write function
    REQUIRE(status == MANIKIN_MEMORY_RESULT_OK);
    // Check if the write function was called with the correct data
    CHECK(spi_hal_write_bytes_fake.call_count
          == 4); // Expecting 4 write calls for write enable, address , data, and end
    CHECK(spi_write_buffer.at(0) == 0x06); // Page program command
    CHECK(spi_write_buffer.at(1) == 0x02); // Page program command
    CHECK(spi_write_buffer.at(2) == 0x00); // Address byte 1
    CHECK(spi_write_buffer.at(3) == 0x00); // Address byte 2
    CHECK(spi_write_buffer.at(4) == 0x00); // Address byte 3
    CHECK(spi_write_buffer.at(5) == 1);    // Data byte 1
    CHECK(spi_write_buffer.at(6) == 2);    // Data byte 2
    CHECK(spi_write_buffer.at(7) == 3);    // Data byte 3
    CHECK(spi_write_buffer.at(8) == 4);    // Data byte 4
}

TEST_CASE("w25qxx_read reads data from the memory chip", "[read]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000, .fault_cnt = 0 };

    /* Simulate successful SPI write */
    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    uint8_t  test_data[4] = { 1, 2, 3, 4 };
    uint32_t addr         = 0;

    /* JEDEC ID */
    spi_read_buffer.push_back(0xEF);
    spi_read_buffer.push_back(0x70);
    spi_read_buffer.push_back(0x18);

    /* Simulate successful SPI read */
    std::copy(test_data, test_data + sizeof(test_data), std::back_inserter(spi_read_buffer));

    /* Clear the test_data array for re-use */
    memset(test_data, 0, sizeof(test_data));

    auto status = w25qxx_read(&mem_ctx, test_data, addr, sizeof(test_data));
    /* Test the read function */
    REQUIRE(status == MANIKIN_MEMORY_RESULT_OK);

    /* Write should be called twice, one presence check and one address set operation */
    REQUIRE(spi_hal_write_bytes_fake.call_count == 2);
    REQUIRE(spi_write_buffer.at(0) == 0x9F);
    REQUIRE(spi_write_buffer.at(1) == 0x03);
    REQUIRE(spi_write_buffer.at(2) == 0x00);
    REQUIRE(spi_write_buffer.at(3) == 0x00);
    REQUIRE(spi_write_buffer.at(4) == 0x00);

    /*  Verify that the data read matches expected values */
    REQUIRE(spi_hal_read_bytes_fake.call_count == 2);
    REQUIRE(test_data[0] == 1);
    REQUIRE(test_data[1] == 2);
    REQUIRE(test_data[2] == 3);
    REQUIRE(test_data[3] == 4);
}

TEST_CASE("w25qxx_init handles SPI timeout correctly", "[init][timeout]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000, .fault_cnt = 0 };

    // Set timeout to occur during SPI operation
    timeout_occurred = true;

    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    // Test initialization with SPI timeout
    REQUIRE(w25qxx_init(&mem_ctx)
            == MANIKIN_STATUS_ERR_READ_FAIL); // Expecting an error due to timeout

    // Verify that the write and read operations were called but returned no data
    CHECK(spi_hal_read_bytes_fake.call_count == 1);
    CHECK(spi_hal_write_bytes_fake.call_count == 1);
}
TEST_CASE("w25qxx_status handles invalid JEDEC ID correctly", "[status][invalid]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5u, .pin = 1u };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000u, .fault_cnt = 0u };

    // Simulate an invalid JEDEC ID
    spi_read_buffer.push_back(0xFF); // Invalid response
    spi_read_buffer.push_back(0xFF);
    spi_read_buffer.push_back(0xFF);

    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    // Test the status check with invalid JEDEC ID
    REQUIRE(w25qxx_status(&mem_ctx)
            == MANIKIN_STATUS_ERR_READ_FAIL); // Expecting an error due to invalid ID

    CHECK(spi_hal_read_bytes_fake.call_count == 1u);
    CHECK(spi_hal_write_bytes_fake.call_count == 1u);
}

TEST_CASE("w25qxx_write handles SPI write timeout", "[write][timeout]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5, .pin = 1 };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000, .fault_cnt = 0 };

    uint8_t  test_data[4] = { 1u, 2u, 3u, 4u };
    uint32_t addr         = 0;

    // Simulate a timeout during the SPI write operation
    timeout_occurred = true;

    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;
    auto status = w25qxx_write(&mem_ctx, test_data, addr, sizeof(test_data));
    // Test the write function with timeout
    REQUIRE(status == MANIKIN_MEMORY_RESULT_ERROR); // Expecting error

    // Check that the write operation was attempted but failed
    CHECK(spi_hal_write_bytes_fake.call_count == 2); // Expecting 2 write calls
}

TEST_CASE("w25qxx_write handles invalid data during write", "[write][invalid_data]")
{
    reset_spi_mocks();

    uint8_t                  spi_handle;
    manikin_spi_cs_t         cs_handle = { .port = 5u, .pin = 1u };
    manikin_spi_memory_ctx_t mem_ctx
        = { .spi = &spi_handle, .spi_cs = cs_handle, .mem_size = 16000000u, .fault_cnt = 0u };

    uint8_t *test_data = NULL; // Simulating invalid (NULL) data
    uint32_t addr      = 0;

    spi_hal_read_bytes_fake.custom_fake  = &custom_spi_read;
    spi_hal_write_bytes_fake.custom_fake = &custom_spi_write;

    auto status = w25qxx_write(&mem_ctx, test_data, addr, sizeof(test_data));

    // Test the write function with invalid data (NULL pointer)
    REQUIRE(status == MANIKIN_MEMORY_RESULT_PARERR); // Expecting error

    // No write operations should have been performed
    CHECK(spi_hal_write_bytes_fake.call_count == 0);
}
int
main (int argc, char *argv[])
{
    int result = Catch::Session().run(argc, argv);

    return result;
}