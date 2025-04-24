/**
 * @file            sample_timer.h
 * @brief           A platform-agnostic timer module with sampling error handling
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

#ifndef SAMPLE_TIMER_H
#define SAMPLE_TIMER_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "common/manikin_types.h"

    typedef struct
    {
        manikin_timer_inst_t    timer;
        manikin_watchdog_inst_t watchdog;
        uint16_t                frequency;
        uint8_t                 state;
    } sample_timer_ctx_t;

    /**
     * @brief Initializes the timer peripheral, so that it can automatically sample an sensor:
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_init(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Start the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_start(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Stop the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_stop(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Deinitializes the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_deinit(sample_timer_ctx_t *timer_inst);

    /**
     * @brief This method should be called before a sample process effort (outside ISR-context!)
     *        It does the following things:
     *        - Reset the watchdog
     *        - Handle Error Conditions (by resetting the I2C-bus or device)
     * @param timer_inst Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @param sensor The sensor instance which should be sampled
     * @return MANIKIN_STATUS_OK: No error occured when resetting peripheral
     */
    manikin_status_t sample_timer_start_cb_handler(sample_timer_ctx_t   *timer_inst,
                                                   manikin_sensor_ctx_t *sensor);

    /**
     * @brief This method should be called after a sample process effort (outside ISR-context!)
     *        It does the following things:
     *        - Reset the watchdog
     *        - Handle Error Conditions (by resetting the I2C-bus or device)
     * @param timer_inst Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @param sensor The sensor instance which has been sampled
     * @param read_status The status received from the sensor_read() function
     * @return MANIKIN_STATUS_OK: No error occured when resetting peripheral
     */
    manikin_status_t sample_timer_end_cb_handler(sample_timer_ctx_t   *timer_inst,
                                                 manikin_sensor_ctx_t *sensor,
                                                 manikin_status_t      read_status);
#ifdef __cplusplus
}
#endif
#endif // SAMPLE_TIMER_H