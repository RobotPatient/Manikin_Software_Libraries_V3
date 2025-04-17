#include "sample_timer.h"
#include <manikin_software_conf.h>
#include <manikin_platform.h>

#define HASH_SAMPLE_TIMER 0x33472A3F

manikin_status_t
sample_timer_init (sample_timer_ctx_t *timer_inst)
{
    if (timer_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    if (timer_inst->timer == NULL || timer_inst->dest_buffer == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    if (timer_inst->frequency > 1000 || timer_inst->frequency < 1)
    {
        return MANIKIN_STATUS_ERR_INVALID_I2C_BAUD;
    }
    if (timer_inst->dest_buffer_max_size < 1)
    {
        return MANIKIN_STATUS_ERR_INVALID_BUFFER_SIZE;
    }

    manikin_status_t status = MANIKIN_TIMER_HAL_INIT(timer_inst->timer, timer_inst->frequency);
    if (status != MANIKIN_STATUS_OK)
    {
        return status;
    }

    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_start (sample_timer_ctx_t *timer_inst)
{
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_stop (sample_timer_ctx_t *timer_inst)
{
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sample_timer_deinit (sample_timer_ctx_t *timer_inst)
{
    return MANIKIN_STATUS_OK;
}