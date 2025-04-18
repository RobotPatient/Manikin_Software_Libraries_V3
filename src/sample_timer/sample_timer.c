#include "sample_timer.h"

#include "error_handler/error_handler.h"

#include <manikin_software_conf.h>
#include <manikin_platform.h>

#define HASH_SAMPLE_TIMER 0x33472A3F

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
sample_timer_irq_handler (sample_timer_ctx_t   *timer_inst,
                          manikin_sensor_ctx_t *sensor,
                          manikin_status_t      read_status)
{
    manikin_status_t status = check_params(timer_inst);
    if (read_status == MANIKIN_STATUS_ERR_READ_FAIL || read_status == MANIKIN_STATUS_ERR_WRITE_FAIL)
    {
        MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
        sensor->needs_reinit = 1;
        MANIKIN_I2C_BUS_RESET(sensor->i2c);
        if (timer_inst->fault_cnt < 2)
        {
            timer_inst->fault_cnt++;
        }
        else
        {
            MANIKIN_TIMER_HAL_DEINIT(timer_inst->timer);
        }
    }
    else
    {
        timer_inst->fault_cnt = 0;
        MANIKIN_WATCHDOG_HAL_KICK(timer_inst->watchdog);
    }
    return status;
}
