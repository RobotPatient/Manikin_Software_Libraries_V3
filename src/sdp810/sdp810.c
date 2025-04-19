#include "sdp810.h"
#include "i2c/i2c.h"
#include "private/sdp810_regs.h"
#include "error_handler/error_handler.h"

#include <string.h>
#include <common/manikin_bit_manipulation.h>

#define HASH_SDP810 0x6BCF2C37

const manikin_sensor_reg_t init_regs[]
    = { { SDP810_REG_CONT_MASS_FLOW, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE } };

manikin_status_t
sdp810_check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_SDP810, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_SDP810, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = sdp810_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_SDP810, (status == MANIKIN_STATUS_OK), status);
    sensor_ctx->needs_reinit = 0;
    for (size_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(
            sensor_ctx->i2c, sensor_ctx->i2c_addr, init_regs[i].reg, init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    // Validate input
    MANIKIN_ASSERT(
        HASH_SDP810, sensor_ctx != NULL && read_buf != NULL, MANIKIN_STATUS_ERR_NULL_PARAM);
    if (sensor_ctx->needs_reinit)
    {
        sdp810_init_sensor(sensor_ctx);
    }
    // Read sensor data directly into read_buf (reuse as temp buffer)
    uint8_t bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, SDP810_READ_BUFFER_SIZE);

    MANIKIN_ASSERT(
        HASH_SDP810, bytes_read == SDP810_READ_BUFFER_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
sdp810_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = sdp810_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_SDP810, status == MANIKIN_STATUS_OK, MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}
