/**
 * @file            bhi360.h
 * @brief           Driver module for a BHI360 programmable IMU
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

#ifndef BHI360_H
#define BHI360_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    /**
     * @brief This struct contains the structure of samples for bhi360 IMU
     *        Which consists of 3-axis 16-bit integers. The unit is g for acc and dps for gyro.
     */
    typedef struct
    {
        uint16_t acc_x_g;
        uint16_t acc_y_g;
        uint16_t acc_z_g;
        uint16_t gyro_x_dps;
        uint16_t gyro_y_dps;
        uint16_t gyro_z_dps;
    } bhi360_sample_data_t;

    /**
     * @brief Initialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Successful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL on unable to set registers (due to lost
     *          connection, e.g.)
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t bhi360_init_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Read the sensor, which should read 16-bytes of data (8-channels, 2 bytes each)
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @param read_buf Ptr to read-buffer which is used for storing the samples
     * @return MANIKIN_STATUS_OK on success,
     *         MANIKIN_STATUS_READ_FAIL on failure while reading,
     *         MANIKIN_STATUS_WRITE_FAIL on failure while writing.
     */
    manikin_status_t bhi360_read_sensor(manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t bhi360_deinit_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Parse the raw sensor data to formatted output-data with correct unit (g and dps)
     * @param raw_data Ptr to read-buffer which contains the raw samples
     * @param data     Ptr to struct to save the processed samples to
     * @return         MANIKIN_STATUS_OK on Successful conversion
     *                 MANIKIN_STATUS_ERR_NULL_PARAM on invalid raw_data or data param
     *                 MANIKIN_STATUS_ERR_CONVERSION_FAILED on error while converting samples
     */
    manikin_status_t bhi360_parse_raw_data(const uint8_t *raw_data, bhi360_sample_data_t *data);
#ifdef __cplusplus
}
#endif
#endif /* BHI360_H */
