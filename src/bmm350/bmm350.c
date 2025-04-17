#include <bmm350/bmm350.h>
#include "i2c/i2c.h"
#include "private/bmm350_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_BMM350 0x10C9F1D2

const manikin_sensor_reg_t init_regs[] = {
    /* Magnitude pad drive 0x07? */
    { BMM350_REG_PAD_CTRL, 0x07, MANIKIN_SENSOR_REG_TYPE_WRITE },
    { BMM350_REG_PMU_CMD_AGGR_SET,
      (0x2 & ~(0x30) | (0 << 0x4) & 0x30),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    { BMM350_REG_PMU_CMD, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE },
    { BMM350_REG_PMU_CMD_AXIS_EN, (0x07), MANIKIN_SENSOR_REG_TYPE_WRITE },
    { BMM350_REG_PMU_CMD, (0x01), MANIKIN_SENSOR_REG_TYPE_WRITE },
    { BMM350_REG_PMU_CMD, (0x01), MANIKIN_SENSOR_REG_TYPE_WRITE }
};

manikin_status_t
check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_BMM350, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_BMM350, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bmm350_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    for (size_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(
            sensor_ctx->i2c, sensor_ctx->i2c_addr, init_regs[i].reg, init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
bmm350_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    MANIKIN_ASSERT(HASH_BMM350, (read_buf != NULL), 0);
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);
    uint8_t mag_data[14] = { 0 };

    uint32_t raw_mag_x, raw_mag_y, raw_mag_z, raw_temp;

    status
        = manikin_i2c_write_reg(sensor_ctx->i2c, sensor_ctx->i2c_addr, BMM350_REG_MAG_X_LSB, 0x00);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    size_t bytes_read
        = manikin_i2c_read_bytes(sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, sizeof(mag_data));
    MANIKIN_ASSERT(HASH_BMM350, (bytes_read == sizeof(mag_data)), MANIKIN_STATUS_ERR_READ_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bmm350_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
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