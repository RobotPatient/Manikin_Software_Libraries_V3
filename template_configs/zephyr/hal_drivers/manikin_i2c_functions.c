#include "manikin_i2c_functions.h"
#include <zephyr/drivers/i2c.h>
#include <common/manikin_types.h>
#include <errno.h>

int
i2c_hal_init (manikin_i2c_inst_t i2c_inst, uint32_t baud)
{
    if (!device_is_ready(i2c_inst))
    {
        printk("I2C device not ready\n");
        return MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL;
    }

    return 0;
}

uint8_t
i2c_hal_device_acknowledge (manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr)
{
    /*
     * As Zephyr does not have special function for checking connection using i2c_address
     * We implement it manually by reading one-byte, which normally always produces ACK/NACK
     */
    uint8_t buf;
    int     status = i2c_read(i2c_inst, &buf, sizeof(buf), i2c_addr);
    return status == 0 ? MANIKIN_STATUS_OK : MANIKIN_STATUS_ERR_READ_FAIL;
}

int
i2c_hal_error_flag_check (manikin_i2c_inst_t i2c_inst)
{
    /*
     * device_is_ready is a generic function for checking device/peripheral status.
     * It returns 0/-EIO, 0 if okay, -EIO <- With error-code defined in stderr.h header
     */
    int status = device_is_ready(i2c_inst);

    /* We don't care about which error specifically,
     * eventually it will be checked for zero anyways..
     * And as return-type is uint8_t, but status is of type int,
     * we compare and return proper val using ternary */
    return status == 0 ? MANIKIN_STATUS_OK : MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
}

size_t
i2c_hal_read_bytes (manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint8_t *data, size_t len)
{
    /*
     * i2c_read returns 0 or -EIO. This function declaration requires to return the amount of bytes
     * read. We tackle this by doing yet another ternary in the return.
     */
    int status = i2c_read(i2c_inst, data, len, i2c_addr);
    return status == 0 ? len : 0;
}

size_t
i2c_hal_write_bytes (manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, const uint8_t *data, size_t len)
{
    /*
     * i2c_write returns 0 or -EIO. This function declaration requires to return the amount of bytes
     * written. We tackle this by doing yet another ternary in the return.
     */
    int status = i2c_write(i2c_inst, data, len, i2c_addr);
    return status == 0 ? len : 0;
}

int
i2c_hal_deinit (manikin_i2c_inst_t i2c_inst)
{
    /* Zephyr handles this automatically for us. */
    return 0;
}

int
i2c_hal_bus_reset (manikin_i2c_inst_t i2c_inst)
{
    /* Zephyr has a nice function that allows us to reset the bus */
    int status = i2c_recover_bus(i2c_inst);
    return status == 0 ? MANIKIN_STATUS_OK : 1;
}

uint32_t
i2c_hal_get_baud (manikin_i2c_inst_t i2c_inst)
{
    /* As the peripheral is configured in DT,
     *  we need to get the config using zephyr macro's
     *
     *  See:
     * https://docs.zephyrproject.org/latest/doxygen/html/group__i2c__interface.html#ga0eda328bb70285895d09154f9a828040
     */
    uint32_t i2c_config;
    i2c_get_config(i2c_inst, &i2c_config);

    uint32_t i2c_speed_id = I2C_SPEED_GET(i2c_config);
    switch (i2c_speed_id)
    {
        case I2C_SPEED_STANDARD: {
            return 100000;
        }
        case I2C_SPEED_FAST: {
            return 400000;
        }
        case I2C_SPEED_FAST_PLUS: {
            return 1000000;
        }
        case I2C_SPEED_HIGH: {
            return 3400000;
        }
        case I2C_SPEED_ULTRA: {
            return 5000000;
        }
        default: {
            return 0;
        }
    }
    return 0;
}

int
i2c_hal_bus_recover ()
{
    return 0;
}