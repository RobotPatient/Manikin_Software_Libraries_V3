/**
 * @file           packet_parser.h
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

#ifndef SDP810_H
#define SDP810_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "common/manikin_types.h"
    typedef struct
    {
        uint16_t       len;
        const uint8_t *data;
    } packet_parser_cmd_t;

    /**
     * @brief Parse received packet from byte array to cmd struct
     * @param recv_bytes
     * @param buf_length
     * @param cmd
     * @return MANIKIN_STATUS_OK on success,
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid recv_bytes, buf_length or cmd (eq NULL)
     *         MANIKIN_STATUS_ERR_INVALID_PACKET_CRC_FAIL when failing to validate CRC of packet
     *         MANIKIN_STATUS_ERR_INVALID_PACKET_MALFORMED when structure of packet is malformed
     */
    manikin_status_t packet_parser_parse(const uint8_t       *recv_bytes,
                                         size_t               buf_length,
                                         packet_parser_cmd_t *cmd);

    /**
     * @brief Encapsulate cmd struct to byte array
     * @param cmd Ptr to struct containing cmd to encapsulate
     * @param out_data Ptr to buffer to contain the encapsulated cmd packet
     * @param out_max_len Max size of buffer holding packet (match max communication packet size)
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid out_data, out_max_len or cmd (eq NULL)
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t packet_parser_encapsulate(const packet_parser_cmd_t *cmd,
                                               uint8_t                   *out_data,
                                               size_t                     out_max_len);

#ifdef __cplusplus
}
#endif
#endif // SDP810_H