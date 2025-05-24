/**
 * Copyright (c) 2025 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file       bhy_parse.h
 * @date       2025-03-28
 * @version    v2.1.0
 *
 */
#ifndef __BHY_PARSE_H__
#define __BHY_PARSE_H__

/* Start of CPP Guard */
#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

#include "bhy.h"

    /**
     * @brief Function to parse FIFO frame data into temperature
     * @param[in] data          : Reference to the data buffer storing data from the FIFO
     * @param[out] temperature  : Reference to the data buffer to store temperature in degree C
     */
    void bhy_parse_temperature_celsius(const uint8_t *data, bhy_float *temperature);

    /**
     * @brief Function to parse FIFO frame data into humidity
     * @param[in] data      : Reference to the data buffer storing data from the FIFO
     * @param[out] humidity : Reference to the data buffer to store humidity in %
     */
    void bhy_parse_humidity(const uint8_t *data, bhy_float *humidity);

    /**
     * @brief Function to parse FIFO frame data into barometric pressure
     * @param[in] data      : Reference to the data buffer storing data from the FIFO
     * @param[out] pressure : Reference to the data buffer to store pressure in Pascals
     */
    void bhy_parse_pressure(const uint8_t *data, bhy_float *pressure);

    /**
     * @brief Function to parse FIFO frame data into altitude
     * @param[in] data      : Reference to the data buffer storing data from the FIFO
     * @param[out] altitude : Reference to the data buffer to store altitude
     */
    void bhy_parse_altitude(const uint8_t *data, bhy_float *altitude);

/* End of CPP Guard */
#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __BHY_PARSE_H__ */
