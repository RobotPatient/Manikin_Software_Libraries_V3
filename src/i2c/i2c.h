#ifndef MANIKIN_I2C_H
#define MANIKIN_I2C_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    /**
     * @brief Initializes the I2C peripheral with default settings for the Manikin System:
     *        - 1 Stop-bit
     *        - 7-bit addressing
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_baud One of the baud-rates defined in the manikin_i2c_speed_t enum, which will be
     * the SCL frequency
     * @return    MANIKIN_STATUS_OK: No error occurred while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: I2C_inst is null
     *            MANIKIN_STATUS_ERR_INVALID_I2C_BAUD: i2c_baud is not one of the specified speeds
     */
    manikin_status_t manikin_i2c_init(manikin_i2c_inst_t i2c_inst, manikin_i2c_speed_t i2c_baud);

    /**
     * @brief Checks if device with i2c_addr is present on i2c-bus
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @return 0 if no device was found, 1 if device is found on bus
     */
    uint8_t manikin_i2c_check_device_address(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr);

    /**
     * @brief Writes a byte to a register of i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param reg      The register address to write
     * @param data     The data to write to the register
     * @return  MANIKIN_STATUS_OK: No error occurred while writing the register
     *          MANIKIN_STATUS_ERR_NULL_PARAM: I2C_inst is null
     */
    manikin_status_t manikin_i2c_write_reg(manikin_i2c_inst_t i2c_inst,
                                           uint8_t            i2c_addr,
                                           uint16_t           reg,
                                           uint8_t            data);

    /**
     * @brief Writes a short (16-bit) to a register of i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param reg      The register address to write
     * @param data     The data to write to the register
     * @return  MANIKIN_STATUS_OK: No error occurred while writing the register
     *          MANIKIN_STATUS_ERR_NULL_PARAM: I2C_inst is null
     */
    manikin_status_t manikin_i2c_write_reg16(manikin_i2c_inst_t i2c_inst,
                                             uint8_t            i2c_addr,
                                             uint16_t           reg,
                                             uint16_t           data);

    /**
     * @brief Reads a byte from a register of i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param reg      The register address to write
     * @return The data which was read from the register. (Register value)
     */
    uint8_t manikin_i2c_read_reg(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg);

    /**
     * @brief Reads a short (16-bit) from a register of i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param reg      The register address to write
     * @return The data which was read from the register. (Register value)
     */
    uint16_t manikin_i2c_read_reg16(manikin_i2c_inst_t i2c_inst, uint8_t i2c_addr, uint16_t reg);

    /**
     * @brief Reads multiple bytes from an i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param data     An pointer to the buffer to save the data to
     * @param len      The number of bytes to read from i2c-device
     * @return The number of bytes read from the i2c-device
     */
    size_t manikin_i2c_read_bytes(manikin_i2c_inst_t i2c_inst,
                                  uint8_t            i2c_addr,
                                  uint8_t           *data,
                                  size_t             len);

    /**
     * @brief Write multiple bytes to an i2c-device
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C device
     * @param data     An pointer to the buffer to read the data from
     * @param len      The number of bytes to send to the i2c-device
     * @return The number of bytes written to the i2c-device
     */
    size_t manikin_i2c_write_bytes(manikin_i2c_inst_t i2c_inst,
                                   uint8_t            i2c_addr,
                                   const uint8_t     *data,
                                   size_t             len);

    /**
     * @brief Denitializes the I2C peripheral of the Manikin System
     * @param i2c_inst The direct (ptr) handle to the i2c peripheral used for the device
     * @return    MANIKIN_STATUS_OK: No error occurred while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: I2C_inst is null
     */
    manikin_status_t manikin_i2c_deinit(manikin_i2c_inst_t i2c_inst);

#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_I2C_H */