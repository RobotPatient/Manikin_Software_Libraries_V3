/**
 * @file            sample_timer.c
 * @brief           A platform-agnostic timer implementation with sampling error handling
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

#include "sample_timer.h"
#include "error_handler/error_handler.h"
#include <manikin_software_conf.h>
#include <manikin_platform.h>

#define HASH_SAMPLE_TIMER 0x33472A3Fu

#define SAMPLE_TIMER_I2C_BUS_RESET_FREQ_HZ 1u
#define SAMPLE_TIMER_I2C_BUS_MAX_FREQ_HZ   1000u

enum sample_timer_state
{
    SAMPLE_TIMER_STATE_SAMPLING,
    SAMPLE_TIMER_STATE_BUS_RECOVERY,
    SAMPLE_TIMER_STATE_BUS_RESET
};

/**
 * @brief Internal function to check the parameters entered into function
 * @param timer_inst The timer settings ptr which should contain timer settings
 * @return - MANIKIN_STATUS_OK if all parameters are valid
 *         - MANIKIN_STATUS_ERR_NULL_PARAM if invalid
 */
static manikin_status_t
sample_timer_check_params (sample_timer_ctx_t *timer_inst)
{
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, timer_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, timer_inst->timer != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER,
                   (timer_inst->frequency <= SAMPLE_TIMER_I2C_BUS_MAX_FREQ_HZ
                    && timer_inst->frequency >= SAMPLE_TIMER_I2C_BUS_RESET_FREQ_HZ),
                   MANIKIN_STATUS_ERR_INVALID_TIMER_SAMPLE_RATE);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_init (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status            = MANIKIN_TIMER_HAL_INIT(timer_inst->timer, timer_inst->frequency);
    timer_inst->state = SAMPLE_TIMER_STATE_SAMPLING;
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    // NOTE: Initialize watchdog with window of 2*sample-freq
    status = MANIKIN_WATCHDOG_HAL_TIMER_INIT(timer_inst->watchdog, timer_inst->frequency / 2);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_start (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_START(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_stop (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_deinit (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_DEINIT(timer_inst->timer);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_WATCHDOG_HAL_DEINIT(timer_inst->watchdog);
    return status;
}
static manikin_status_t
sample_timer_disable_i2c (manikin_sensor_ctx_t *sensor)
{
    manikin_status_t status = MANIKIN_I2C_HAL_DEINIT(sensor->i2c);
    status                  = MANIKIN_I2C_BUS_RESET(sensor->i2c);
    return status;
}
static manikin_status_t
sample_timer_recover_i2c (manikin_sensor_ctx_t *sensor)
{
    manikin_status_t status = MANIKIN_I2C_BUS_RECOVER();
    const uint32_t   baud   = MANIKIN_I2C_GET_BAUD(sensor->i2c);
    status                  = MANIKIN_I2C_HAL_INIT(sensor->i2c, baud);
    return status;
}
static manikin_status_t
sample_timer_set_timeout_freq (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status;
    status = MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
    status = MANIKIN_TIMER_HAL_INIT(timer_inst->timer, SAMPLE_TIMER_I2C_BUS_RESET_FREQ_HZ);
    status = MANIKIN_TIMER_HAL_START(timer_inst->timer);
    return status;
}
static manikin_status_t
sample_timer_reset_timeout (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status;
    status = MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
    status = MANIKIN_TIMER_HAL_INIT(timer_inst->timer, timer_inst->frequency);
    status = MANIKIN_TIMER_HAL_START(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_end_cb_handler (sample_timer_ctx_t   *timer_inst,
                             manikin_sensor_ctx_t *sensor,
                             manikin_status_t      read_status)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
    switch (read_status)
    {
        case MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG: {
            //
            // Let's check once again if it is still blocked and we can fix it easily...
            //
            status = MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(sensor->i2c);
            if (status != MANIKIN_STATUS_OK)
            {
                //
                // Welp, it is still blocked...
                //
                if (timer_inst->state == SAMPLE_TIMER_STATE_SAMPLING)
                {
                    //
                    // First state of statemachine, reset the voltage on sensor vcc and disable i2c
                    //
                    status            = sample_timer_disable_i2c(sensor);
                    timer_inst->state = SAMPLE_TIMER_STATE_BUS_RESET;
                    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, status == MANIKIN_STATUS_OK, status);
                }
                //
                // Set timer to timeout-state, waiting for bus and sensor to recover
                //
                status = sample_timer_set_timeout_freq(timer_inst);
                MANIKIN_ASSERT(HASH_SAMPLE_TIMER, status == MANIKIN_STATUS_OK, status);
                // NOTE: Return error state to make sure the statemachine fully finishes
                status = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
            }
            break;
        }
        case MANIKIN_STATUS_OK: {
            break;
        }
        default: {
            MANIKIN_NON_CRIT_ASSERT(HASH_SAMPLE_TIMER, status != MANIKIN_STATUS_OK, status);
        }
    }
    return status;
}

manikin_status_t
sample_timer_start_cb_handler (sample_timer_ctx_t *timer_inst, manikin_sensor_ctx_t *sensor)
{
    manikin_status_t status = sample_timer_check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
    switch (timer_inst->state)
    {
        case SAMPLE_TIMER_STATE_BUS_RESET: {
            //
            // Second state of statemachine, this recovers the voltage on sensor vcc and inits i2c
            // back to original state.
            //
            status = sample_timer_recover_i2c(sensor);
            MANIKIN_ASSERT(HASH_SAMPLE_TIMER, status == MANIKIN_STATUS_OK, status);

            // NOTE: Make sure the sensor driver inits after hard-reset
            sensor->needs_reinit = 1;
            timer_inst->state    = SAMPLE_TIMER_STATE_BUS_RECOVERY;

            // NOTE: It must return an error-state, otherwise the sensor sampling will continue
            status = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
            break;
        }
        case SAMPLE_TIMER_STATE_BUS_RECOVERY: {
            //
            // Last state of statemachine, this state is after 2 second reset wait.
            // It resets the timer back to sample-rate. So sampling of sensor can start once-again.
            //
            status = sample_timer_reset_timeout(timer_inst);
            MANIKIN_ASSERT(HASH_SAMPLE_TIMER, status == MANIKIN_STATUS_OK, status);
            timer_inst->state = SAMPLE_TIMER_STATE_SAMPLING;

            // NOTE: It must return an error-state, otherwise the sensor sampling will continue
            status = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
            break;
        }
        default: {
            //
            // This function checks if the scl & sda lines are not blocked and busy flag is not set
            // If this is the case, it tries a simple recover mechanism. If it fails it returns an
            // error-state.
            //
            status = MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(sensor->i2c);
            //
            // If this asserts, something is seriously wrong with the i2c-bus or sensor.
            // Wrong beyond being easily fixed with a few clock-cycles on i2c-scl. So we need to
            // hard-reset the sensor, let's start the statemachine.
            //
            MANIKIN_ASSERT(HASH_SAMPLE_TIMER,
                           status == MANIKIN_STATUS_OK,
                           MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
            //
            // Everything seems to be fine, let's reset the statemachine, making sure it does not
            // trigger sporadically
            //
            timer_inst->state = SAMPLE_TIMER_STATE_SAMPLING;
            break;
        }
    }
    return status;
}