#include <bhi360/bhi360.h>
#include "i2c/i2c.h"
#include "private/bhi360_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_BHI360 0xC3A556E3

const manikin_sensor_reg_t init_regs[] = {
    { 0x00, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE }
};

manikin_status_t
check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BHI360, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);

//    uint8_t data = 0;
//    data = manikin_i2c_read_reg(sensor_ctx->i2c, sensor_ctx->i2c_addr, ADS7138_REG_SYSTEM_STATUS);
//
//    MANIKIN_NON_CRIT_ASSERT(HASH_BMM350,
//                            BIT_IS_SET(data, ADS7138_REG_SYSTEM_STATUS_RSVD_BIT),
//                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

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
    MANIKIN_ASSERT(HASH_BHI360, (read_buf != NULL), 0);
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);
//    status = manikin_i2c_write_reg16(sensor_ctx->i2c,
//                                     sensor_ctx->i2c_addr,
//                                     ADS7138_REG_SEQUENCE_CFG,
//                                     (1 << 4 | ADS7138_OPCODE_SET_BIT));
    MANIKIN_ASSERT(HASH_BHI360, (status == MANIKIN_STATUS_OK), status);
//    size_t bytes_read = manikin_i2c_read_bytes(
//        sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, ADS7138_READ_BUF_SIZE);
//    MANIKIN_ASSERT(HASH_BMM350, bytes_read == ADS7138_READ_BUF_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);
//    status = manikin_i2c_write_reg16(sensor_ctx->i2c,
//                                     sensor_ctx->i2c_addr,
//                                     ADS7138_REG_SEQUENCE_CFG,
//                                     (1 << 4 | ADS7138_OPCODE_CLEAR_BIT));

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bhi360_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
//    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
//    status = manikin_i2c_write_reg(sensor_ctx->i2c,
//                                   sensor_ctx->i2c_addr,
//                                   ADS7138_REG_GENERAL_CFG,
//                                   (0x01 | ADS7138_OPCODE_SET_BIT));
//    MANIKIN_ASSERT(
//        HASH_ADS7138, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}