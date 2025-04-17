#include <ads7138/ads7138.h>
#include "i2c/i2c.h"
#include "private/ads7138_regs.h"
#include "error_handler/error_handler.h"

#include <common/manikin_bit_manipulation.h>

#define HASH_ADS7138 0x157F34F4

static inline uint16_t
assemble_register (uint8_t opcode, uint8_t regAddr)
{
    uint16_t asmb_register = regAddr | (opcode << 8);
    return asmb_register;
}

const manikin_sensor_reg_t init_regs[] = {
    /* All channels are configured as analog input*/
    { ADS7138_REG_PIN_CFG | ADS7138_OPCODE_SET_BIT << 8, (0x0), MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set cal bit */
    { ADS7138_REG_GENERAL_CFG | ADS7138_OPCODE_SET_BIT << 8,
      (0x02),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set all adc channels as inputs. enabled in scanning sequence. */
    { ADS7138_REG_AUTO_SEQUENCE_CH_SEL | ADS7138_OPCODE_SET_BIT << 8,
      (0xFF),
      MANIKIN_SENSOR_REG_TYPE_WRITE },
    /* Set Auto sequence mode on = 1. And 4th for sequence start. */
    { ADS7138_REG_SEQUENCE_CFG | ADS7138_OPCODE_SET_BIT << 8,
      (0x01),
      MANIKIN_SENSOR_REG_TYPE_WRITE }
};

manikin_status_t
check_params (manikin_sensor_ctx_t *sensor_ctx)
{
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    MANIKIN_ASSERT(HASH_ADS7138, (sensor_ctx->i2c != NULL), MANIKIN_STATUS_ERR_NULL_PARAM);
    return MANIKIN_STATUS_OK;
}

manikin_status_t
ads7138_init_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);

    uint8_t data = 0;
    data         = manikin_i2c_read_reg(sensor_ctx->i2c,
                                sensor_ctx->i2c_addr,
                                ADS7138_REG_SYSTEM_STATUS | ADS7138_OPCODE_SINGLE_READ << 8);

    MANIKIN_NON_CRIT_ASSERT(HASH_ADS7138,
                            BIT_IS_SET(data, ADS7138_REG_SYSTEM_STATUS_RSVD_BIT),
                            MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL);

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
    MANIKIN_ASSERT(HASH_ADS7138, (read_buf != NULL), 0);
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    status
        = manikin_i2c_write_reg(sensor_ctx->i2c,
                                sensor_ctx->i2c_addr,
                                assemble_register(ADS7138_OPCODE_SET_BIT, ADS7138_REG_SEQUENCE_CFG),
                                1 << 4);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    size_t bytes_read = manikin_i2c_read_bytes(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, read_buf, ADS7138_READ_BUF_SIZE);
    status = manikin_i2c_write_reg(
        sensor_ctx->i2c,
        sensor_ctx->i2c_addr,
        assemble_register(ADS7138_OPCODE_CLEAR_BIT, ADS7138_REG_SEQUENCE_CFG),
        1 << 4);
    MANIKIN_ASSERT(HASH_ADS7138, bytes_read == ADS7138_READ_BUF_SIZE, MANIKIN_STATUS_ERR_READ_FAIL);
    for (uint8_t i = 0; i < 16; i++)
    {
        read_buf[i] = read_buf[i] << 4;
        i++;
        read_buf[i] = read_buf[i] >> 4;
    }

    return status;
}

manikin_status_t
ads7138_deinit_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = check_params(sensor_ctx);
    MANIKIN_ASSERT(HASH_ADS7138, (status == MANIKIN_STATUS_OK), status);
    status = manikin_i2c_write_reg(sensor_ctx->i2c,
                                   sensor_ctx->i2c_addr,
                                   ADS7138_REG_GENERAL_CFG | ADS7138_OPCODE_SET_BIT << 8,
                                   (0x01));
    MANIKIN_ASSERT(
        HASH_ADS7138, (status == MANIKIN_STATUS_OK), MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL);
    return MANIKIN_STATUS_OK;
}