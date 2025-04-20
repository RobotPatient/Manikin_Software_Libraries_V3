#include "sample_timer.h"

#include "error_handler/error_handler.h"

#include <manikin_software_conf.h>
#include <manikin_platform.h>

#define HASH_SAMPLE_TIMER 0x33472A3F

enum sample_timer_state
{
    SAMPLE_TIMER_STATE_SAMPLING,
    SAMPLE_TIMER_STATE_BUS_RECOVERY,
    SAMPLE_TIMER_STATE_BUS_RESET
};

manikin_status_t
check_params (sample_timer_ctx_t *timer_inst)
{
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, timer_inst != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);

    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, timer_inst->timer != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER,
                   (timer_inst->frequency <= 1000 && timer_inst->frequency > 1),
                   MANIKIN_STATUS_ERR_INVALID_TIMER_SAMPLE_RATE);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_init (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_INIT(timer_inst->timer, timer_inst->frequency);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    /* Initialize watchdog with window of 2*sample-freq */
    status = MANIKIN_WATCHDOG_HAL_TIMER_INIT(timer_inst->watchdog, timer_inst->frequency / 2);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_start (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_START(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_stop (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_deinit (sample_timer_ctx_t *timer_inst)
{
    manikin_status_t status = check_params(timer_inst);
    MANIKIN_ASSERT(HASH_SAMPLE_TIMER, (status == MANIKIN_STATUS_OK), status);
    status = MANIKIN_TIMER_HAL_DEINIT(timer_inst->timer);
    return status;
}

manikin_status_t
sample_timer_end_cb_handler (sample_timer_ctx_t   *timer_inst,
                             manikin_sensor_ctx_t *sensor,
                             manikin_status_t      read_status)
{
    manikin_status_t status = check_params(timer_inst);
    if (read_status == MANIKIN_STATUS_ERR_READ_FAIL || read_status == MANIKIN_STATUS_ERR_WRITE_FAIL)
    {
        MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
        if (timer_inst->state == SAMPLE_TIMER_STATE_SAMPLING)
        {
            MANIKIN_I2C_HAL_DEINIT(sensor->i2c);
            MANIKIN_I2C_BUS_RESET(sensor->i2c);
            timer_inst->state = SAMPLE_TIMER_STATE_BUS_RESET;
        }
        MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
        MANIKIN_TIMER_HAL_INIT(timer_inst->timer, 1);
        MANIKIN_TIMER_HAL_START(timer_inst->timer);
        return MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
    }
    else
    {
        MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
    }
    return status;
}
manikin_status_t
sample_timer_start_cb_handler (sample_timer_ctx_t *timer_inst, manikin_sensor_ctx_t *sensor)
{
    manikin_status_t status = check_params(timer_inst);
    if (timer_inst->state == SAMPLE_TIMER_STATE_BUS_RESET)
    {
        MANIKIN_I2C_BUS_RECOVER();
        uint32_t baud = MANIKIN_I2C_GET_BAUD(sensor->i2c);
        MANIKIN_I2C_HAL_INIT(sensor->i2c, baud);
        sensor->needs_reinit = 1;
        timer_inst->state    = SAMPLE_TIMER_STATE_BUS_RECOVERY;
        status               = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
    }
    else if (timer_inst->state == SAMPLE_TIMER_STATE_BUS_RECOVERY)
    {
        MANIKIN_TIMER_HAL_STOP(timer_inst->timer);
        MANIKIN_TIMER_HAL_INIT(timer_inst->timer, timer_inst->frequency);
        MANIKIN_TIMER_HAL_START(timer_inst->timer);
        status            = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
        timer_inst->state = SAMPLE_TIMER_STATE_SAMPLING;
    }
    else
    {
        status = MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(sensor->i2c);
        if(status != MANIKIN_STATUS_OK) {
            return MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
        }
        timer_inst->state = SAMPLE_TIMER_STATE_SAMPLING;
        MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
    }
    return status;
}