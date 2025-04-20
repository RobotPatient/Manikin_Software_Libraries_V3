#include <bhi360/bhi360.h>
#include "i2c/i2c.h"
#include "private/bhi360_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_BHI360 0xC3A556E3

const manikin_sensor_reg_t init_regs[] = { { 0x00, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE } };

manikin_status_t
bhi360_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    sensor_ctx->needs_reinit = 0;

    for (size_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg16(
            sensor_ctx->i2c, sensor_ctx->i2c_addr, init_regs[i].reg, init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

    if (sensor_ctx->needs_reinit)
    {
        bhi360_init_sensor(sensor_ctx);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = bhi360_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);
    return MANIKIN_STATUS_OK;
}