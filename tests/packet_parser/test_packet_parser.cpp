/**
 * @file             test_packet_parser.cpp
 * @brief            Test for packet encapsulator/parser module (CBOR-based BLE protocol)
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
 * Author: Victor Hogeweij
 */

#include "catch2/catch_all.hpp"
#include "packet_parser/packet_parser.h"
#include "common/manikin_bit_manipulation.h"

// Fake CBOR message: {1: 42} = A1 01 18 2A
static const uint8_t test_cbor_data[] = { 0xA1, 0x01, 0x18, 0x2A }; // CBOR map
static const size_t  test_cbor_len    = sizeof(test_cbor_data);

TEST_CASE("packet_parser_encapsulate produces correct packet", "[packet_parser][REQ-F1]")
{
    packet_parser_cmd_t cmd
        = { .len = static_cast<uint16_t>(test_cbor_len), .data = test_cbor_data };

    uint8_t          out_packet[64] = { 0 };
    manikin_status_t status = packet_parser_encapsulate(&cmd, out_packet, sizeof(out_packet));
    REQUIRE(status == MANIKIN_STATUS_OK);

    // Check structure
    REQUIRE(out_packet[0] == 0x01);          // START_BYTE
    REQUIRE(out_packet[1] == test_cbor_len); // LENGTH
    REQUIRE(out_packet[2] == 0x3A);          // COLON
    REQUIRE(memcmp(&out_packet[3], test_cbor_data, test_cbor_len) == 0);

    // CRC validation
    uint16_t crc_calc = 0xFFFF;
    for (size_t i = 0; i < test_cbor_len; ++i)
    {
        crc_calc ^= test_cbor_data[i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            if (crc_calc & 1)
            {
                crc_calc = (crc_calc >> 1) ^ 0x8D95;
            }
            else
            {
                crc_calc >>= 1;
            }
        }
    }
    size_t crc_idx = 3 + test_cbor_len;
    REQUIRE(out_packet[crc_idx] == GET_LOWER_8_BITS_OF_SHORT(crc_calc));
    REQUIRE(out_packet[crc_idx + 1] == GET_UPPER_8_BITS_OF_SHORT(crc_calc));
    REQUIRE(out_packet[crc_idx + 2] == 0x17); // END_BYTE
}

TEST_CASE("packet_parser_parse returns correct cmd from valid packet", "[packet_parser][REQ-F1]")
{
    // Build valid packet
    uint8_t valid_packet[64] = { 0 };
    valid_packet[0]          = 0x01;          // START
    valid_packet[1]          = test_cbor_len; // LENGTH
    valid_packet[2]          = 0x3A;          // COLON
    memcpy(&valid_packet[3], test_cbor_data, test_cbor_len);

    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < test_cbor_len; ++i)
    {
        crc ^= test_cbor_data[i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0x8D95;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    size_t crc_idx            = 3 + test_cbor_len;
    valid_packet[crc_idx]     = GET_LOWER_8_BITS_OF_SHORT(crc);
    valid_packet[crc_idx + 1] = GET_UPPER_8_BITS_OF_SHORT(crc);
    valid_packet[crc_idx + 2] = 0x17; // END_BYTE

    packet_parser_cmd_t parsed_cmd;
    manikin_status_t    status = packet_parser_parse(valid_packet, crc_idx + 3, &parsed_cmd);
    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(parsed_cmd.len == test_cbor_len);
    REQUIRE(memcmp(parsed_cmd.data, test_cbor_data, test_cbor_len) == 0);
}

TEST_CASE("packet_parser_parse detects CRC failure", "[packet_parser][REQ-F1]")
{
    uint8_t corrupted_packet[64]
        = { 0x01, 0x04, 0x3A, 0xA1, 0x01, 0x18, 0x2B, 0x00, 0x00, 0x17 }; // corrupted CRC

    packet_parser_cmd_t cmd    = { 0 };
    manikin_status_t    status = packet_parser_parse(corrupted_packet, 10, &cmd);
    REQUIRE(status == MANIKIN_STATUS_ERR_INVALID_PACKET_CRC_FAIL);
}

TEST_CASE("packet_parser_parse detects malformed start/end", "[packet_parser][REQ-F1]")
{
    uint8_t malformed_packet[] = { 0xFF, 0x04, 0x3A, 0xA1, 0x01, 0x18, 0x2A, 0x00, 0x00, 0xFF };
    packet_parser_cmd_t cmd;
    manikin_status_t status = packet_parser_parse(malformed_packet, sizeof(malformed_packet), &cmd);
    REQUIRE(status == MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED);
}

TEST_CASE("packet_parser_encapsulate fails on nulls", "[packet_parser][REQ-F1]")
{
    uint8_t out_buf[32] = { 0 };

    packet_parser_cmd_t null_data_cmd = { .len = 4, .data = nullptr };
    REQUIRE(packet_parser_encapsulate(&null_data_cmd, out_buf, sizeof(out_buf))
            == MANIKIN_STATUS_ERR_NULL_PARAM);

    packet_parser_cmd_t valid_cmd = { .len = 4, .data = test_cbor_data };
    REQUIRE(packet_parser_encapsulate(&valid_cmd, nullptr, sizeof(out_buf))
            == MANIKIN_STATUS_ERR_NULL_PARAM);
}

int
main (int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}
