/**
 * @file            packet_parser.c
 * @brief          Communication packet encapsulator/parser for communication interfaces
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
#include "packet_parser.h"

#include "common/manikin_bit_manipulation.h"

#include <string.h>

#define START_BYTE 0x01
#define COLON      0x3A
#define END_BYTE   0x17
#define CRC16_POLY 0x8D95

static uint16_t
calculate_crc16 (const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ CRC16_POLY;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

manikin_status_t
packet_parser_parse (const uint8_t *recv_bytes, size_t buf_length, packet_parser_cmd_t *cmd)
{
    if (!recv_bytes || !cmd || buf_length < 7)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }

    if (recv_bytes[0] != START_BYTE || recv_bytes[2] != COLON)
    {
        return MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED;
    }

    uint8_t cbor_len  = recv_bytes[1];
    size_t  total_len = 3 + cbor_len + 2 + 1;

    if (buf_length < total_len)
    {
        return MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED;
    }

    const uint8_t *cbor_ptr = &recv_bytes[3];

    uint16_t received_crc
        = CONSTRUCT_SHORT_FROM_BYTES(recv_bytes[4 + cbor_len], recv_bytes[3 + cbor_len]);
    uint16_t calculated_crc = calculate_crc16(cbor_ptr, cbor_len);
    if (received_crc != calculated_crc)
    {
        return MANIKIN_STATUS_ERR_INVALID_PACKET_CRC_FAIL;
    }

    if (recv_bytes[5 + cbor_len] != END_BYTE)
    {
        return MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED;
    }

    cmd->len  = cbor_len;
    cmd->data = cbor_ptr;

    return MANIKIN_STATUS_OK;
}

manikin_status_t
packet_parser_encapsulate (const packet_parser_cmd_t *cmd, uint8_t *out_data, size_t out_max_len)
{
    if (!cmd || !out_data || !cmd->data)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }

    size_t total_len = 3 + cmd->len + 2 + 1;
    if (out_max_len < total_len)
    {
        return MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED;
    }

    size_t idx      = 0;
    out_data[idx++] = START_BYTE;
    out_data[idx++] = (uint8_t)cmd->len;
    out_data[idx++] = COLON;

    memcpy(&out_data[idx], cmd->data, cmd->len);
    idx += cmd->len;

    uint16_t crc    = calculate_crc16(cmd->data, cmd->len);
    out_data[idx++] = GET_LOWER_8_BITS_OF_SHORT(crc);
    out_data[idx++] = GET_UPPER_8_BITS_OF_SHORT(crc);

    out_data[idx] = END_BYTE;

    return MANIKIN_STATUS_OK;
}
