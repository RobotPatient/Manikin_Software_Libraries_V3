#include "i2c.h"
#include "common/manikin_bit_manipulation.h"
#include "manikin_platform.h"

manikin_status_t
validate_baud (const manikin_i2c_speed_t baud)
{
    manikin_status_t status;
    switch (baud)
    {
        case MANIKIN_I2C_SPEED_100KHz:
        case MANIKIN_I2C_SPEED_400KHz:
        case MANIKIN_I2C_SPEED_1MHz:
            status = MANIKIN_STATUS_OK;
            break;

        default:
            status = MANIKIN_STATUS_ERR_INVALID_I2C_BAUD;
            break;
    }
    return status;
}

manikin_status_t
manikin_i2c_init (manikin_i2c_inst_t i2c_inst, const manikin_i2c_speed_t i2c_baud)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }

    manikin_status_t status = validate_baud(i2c_baud);
    if (status != MANIKIN_STATUS_OK)
    {
        return status;
    }

    status = MANIKIN_I2C_HAL_INIT(i2c_inst, i2c_baud);
    if (status != MANIKIN_STATUS_OK)
    {
        return status;
    }

    status = MANIKIN_I2C_HAL_ERROR_FLAG_CHECK(i2c_inst);
    if (status != MANIKIN_STATUS_OK)
    {
        return status;
    }

    return MANIKIN_STATUS_OK;
}

uint8_t
manikin_i2c_check_device_address (manikin_i2c_inst_t i2c_inst, const uint8_t i2c_addr)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    return MANIKIN_I2C_HAL_DEVICE_ACKNOWLEDGE(i2c_inst, i2c_addr << 1);
}

manikin_status_t
manikin_i2c_write_reg (manikin_i2c_inst_t i2c_inst,
                       const uint8_t      i2c_addr,
                       const uint16_t     reg,
                       const uint8_t      data)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    uint8_t bytes[3];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    bytes[2] = data;
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_WRITE_FAIL;
    }
    else
    {
        return MANIKIN_STATUS_OK;
    }
}

manikin_status_t
manikin_i2c_write_reg16 (manikin_i2c_inst_t i2c_inst,
                         const uint8_t      i2c_addr,
                         const uint16_t     reg,
                         const uint16_t     data)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    uint8_t bytes[4];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    bytes[2] = GET_UPPER_8_BITS_OF_SHORT(data);
    bytes[3] = GET_LOWER_8_BITS_OF_SHORT(data);

    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return MANIKIN_STATUS_ERR_WRITE_FAIL;
    }
    else
    {
        return MANIKIN_STATUS_OK;
    }
}

uint8_t
manikin_i2c_read_reg (manikin_i2c_inst_t i2c_inst, const uint8_t i2c_addr, const uint16_t reg)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    uint8_t bytes[2];
    uint8_t res;
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return 0;
    }
    if (MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, &res, sizeof(res)) != sizeof(res))
    {
        return 0;
    }
    return res;
}

uint16_t
manikin_i2c_read_reg16 (manikin_i2c_inst_t i2c_inst, const uint8_t i2c_addr, const uint16_t reg)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    uint8_t bytes[2];
    uint8_t res[2];
    bytes[0] = GET_UPPER_8_BITS_OF_SHORT(reg);
    bytes[1] = GET_LOWER_8_BITS_OF_SHORT(reg);
    if (MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, bytes, sizeof(bytes)) != sizeof(bytes))
    {
        return 0;
    }
    if (MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, res, sizeof(res)) != sizeof(res))
    {
        return 0;
    }
    return CONSTRUCT_SHORT_FROM_BYTES(res[0], res[1]);
}

size_t
manikin_i2c_read_bytes (manikin_i2c_inst_t i2c_inst,
                        const uint8_t      i2c_addr,
                        uint8_t           *data,
                        const size_t       len)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    return MANIKIN_I2C_HAL_READ_BYTES(i2c_inst, i2c_addr << 1, data, len);
}

size_t
manikin_i2c_write_bytes (manikin_i2c_inst_t i2c_inst,
                         const uint8_t      i2c_addr,
                         const uint8_t     *data,
                         const size_t       len)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    return MANIKIN_I2C_HAL_WRITE_BYTES(i2c_inst, i2c_addr << 1, data, len);
}

manikin_status_t
manikin_i2c_deinit (manikin_i2c_inst_t i2c_inst)
{
    if (i2c_inst == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }
    return MANIKIN_I2C_HAL_DEINIT(i2c_inst);
}
