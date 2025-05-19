/**
 * @file            vl6180x.c
 * @brief           Driver module for STM VL6180x ranging ToF sensor
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

#ifndef VL6180X_H
#define VL6180X_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "common/manikin_types.h"
    /**
     * @brief This struct contains the structure of samples for vl6180x ToF sensor
     *        The units are millimeters
     */
    typedef struct
    {
        uint32_t distance_mm;
    } vl6180x_sample_data_t;

    /**
     * @brief Initialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Successful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL on unable to set registers (due to lost
     *          connection, e.g.)
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t vl6180x_init_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Read the sensor, which should read 1-byte of data (distance in mm)
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @param read_buf Ptr to read-buffer which is used for storing the samples
     * @return MANIKIN_STATUS_OK on success,
     *         MANIKIN_STATUS_READ_FAIL on failure while reading,
     *         MANIKIN_STATUS_WRITE_FAIL on failure while writing.
     */
    manikin_status_t vl6180x_read_sensor(manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t vl6180x_deinit_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Parse the raw sensor data to formatted output-data with correct unit (mm)
     * @param raw_data Ptr to read-buffer which contains the raw samples
     * @param data     Ptr to struct to save the processed samples to
     * @return         MANIKIN_STATUS_OK on Successful conversion
     *                 MANIKIN_STATUS_ERR_NULL_PARAM on invalid raw_data or data param
     *                 MANIKIN_STATUS_ERR_CONVERSION_FAILED on error while converting samples
     */
    manikin_status_t vl6180x_parse_raw_data(const uint8_t *raw_data, vl6180x_sample_data_t *data);
#ifdef __cplusplus
}
#endif
#endif // VL6180X_H