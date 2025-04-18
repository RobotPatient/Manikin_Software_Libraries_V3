#include <ads7138/ads7138.h>
#include "i2c/i2c.h"
#include "private/ads7138_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_ADS7138 0x157F34F4

const manikin_sensor_reg_t init_regs[] = {
    /* All channels are configured as analog input*/
    { ADS7138_REG(ADS7138_REG_PIN_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_ALL_PINS_ANALOG_INP_BIT,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set cal bit */
    { ADS7138_REG(ADS7138_REG_GEN_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_CAL_BIT,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set all adc channels as inputs. enabled in scanning sequence. */
    { ADS7138_REG(ADS7138_REG_AUTO_SEQ_CH_SEL, ADS7138_OP_SET_BIT),
      ADS7138_REG_AUTO_SEQ_ALL_CH_SEL,
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set Auto sequence mode on = 1. And 4th for sequence start. */
    { ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_SET_BIT),
      ADS7138_REG_ENABLE_AUTO_SEQ,
      MANIKIN_SENSOR_REG_TYPE_WRITE }
};

manikin_status_t
ads7138_check_params (const manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    const uint8_t data
        = manikin_i2c_read_reg(sensor_ctx->i2c,
                               sensor_ctx->i2c_addr,
                               ADS7138_REG(ADS7138_REG_SYSTEM_STATUS, ADS7138_OP_SINGLE_READ));

    MANIKIN_NON_CRIT_ASSERT(HASH_ADS7138,
                            BIT_IS_SET(data, ADS7138_REG_SYSTEM_STATUS_RSVD_BIT),
                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);
    sensor_ctx->needs_reinit = 0;
    for (size_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(
            sensor_ctx->i2c, sensor_ctx->i2c_addr, init_regs[i].reg, init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_read_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    MANIKIN_ASSERT(HASH_ADS7138, (read_buf != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    if (sensor_ctx->needs_reinit)
    {
        ads7138_init_sensor(sensor_ctx);
    }
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_SET_BIT),
                                   ADS7138_REG_SEQ_START_BIT);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    uint8_t raw_buf[ADS7138_READ_BUF_SIZE];
    size_t  bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, raw_buf, ADS7138_READ_BUF_SIZE);
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_SEQUENCE_CFG, ADS7138_OP_CLEAR_BIT),
                                   ADS7138_REG_SEQ_START_BIT);
    MANIKIN_ASSERT(HASH_ADS7138, bytes_read == ADS7138_READ_BUF_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);

    // Convert raw buffer into 12-bit values, store as 2-byte values into read_buf
    for (int i = 0; i < 8; i++)
    {
        uint16_t val        = ((uint16_t)raw_buf[2 * i] << 4) | (raw_buf[2 * i + 1] >> 4);
        read_buf[2 * i]     = (val >> 8) & 0xFF;
        read_buf[2 * i + 1] = val & 0xFF;
    }
    return status;
}

manikin_status_t
ads7138_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = ads7138_check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG(ADS7138_REG_GEN_CFG, ADS7138_OP_SET_BIT),
                                   ADS7138_REG_SW_RESET_BIT);
    MANIKIN_ASSERT(
        HASH_ADS7138, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}