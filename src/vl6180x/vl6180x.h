#ifndef VL6180X_H
#define VL6180X_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "common/manikin_types.h"

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param i2c The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C sensor
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL on unable to set registers (due to lost
     *          connection, e.g.)
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t vl6180x_init_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param i2c The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C sensor
     * @return Number of bytes read from sensor (VL6180x should be 1) if it returns 0, a fault has
     * occured
     */
    uint8_t vl6180x_read_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr, uint8_t *read_buf);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param i2c The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C sensor
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t vl6180x_deinit_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr);

#ifdef __cplusplus
}
#endif
#endif /* VL6180X_H */