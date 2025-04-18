#include <bmm350/bmm350.h>
#include "i2c/i2c.h"
#include "private/bmm350_regs.h"
#include "error_handler/error_handler.h"

#define HASH_BMM350 0x10C9F1D2

const manikin_sensor_reg_t init_regs[] = {
    /* Set strong drive on all pins  */
    { BMM350_REG_PAD_CTRL, 0x07, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Clear the sampling settings register first  */
    { BMM350_REG_PMU_CMD_AGGR_SET, 0, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set the sampling rate to 200Hz with 4 AVG samples */
    { BMM350_REG_PMU_CMD_AGGR_SET,
      (BMM350_AVG_SHIFT(BMM350_AVG_4) | BMM350_ODR_200HZ),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Update ODR and AVG parameters */
    { BMM350_REG_PMU_CMD, BMM350_PMU_CMD_UPD_OAE, MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Enable x, y, z axis */
    { BMM350_REG_PMU_CMD_AXIS_EN,
      (BMM350_AXIS_EN_X | BMM350_AXIS_EN_Y | BMM350_AXIS_EN_Z),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Enable normal mode */
    { BMM350_REG_PMU_CMD, (BMM350_PMU_CMD_NM), MANIKIN_SENSOR_REG_TYPE_WRITE },
};

manikin_status_t
check_params (const manikin_sensor_ctx_t *sensor_ctx)
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
    MANIKIN_ASSERT(HASH_BMM350, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    status
        = manikin_i2c_write_reg(sensor_ctx->i2c, sensor_ctx->i2c_addr, BMM350_REG_MAG_X_LSB, 0x00);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);

    size_t bytes_read = manikin_i2c_read_bytes(sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, 14);
    MANIKIN_ASSERT(HASH_BMM350, (bytes_read == 14), MANIKIN_STATUS_ERR_READ_FAIL);

    return MANIKIN_STATUS_OK;
}

manikin_status_t
bmm350_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_BMM350, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, BMM350_REG_PMU_CMD, BMM350_PMU_CMD_SUS);
    MANIKIN_ASSERT(
        HASH_BMM350, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}