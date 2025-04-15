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
    manikin_status_t vl6180x_init_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param i2c The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C sensor
     * @param read_buf Ptr to read-buffer which is used for storing the samples
     * @return Number of bytes read from sensor (VL6180x should be 1) if it returns 0, a fault has
     * occured
     */
    uint8_t vl6180x_read_sensor(manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param i2c The direct (ptr) handle to the i2c peripheral used for the device
     * @param i2c_addr The address of the I2C sensor
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t vl6180x_deinit_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Get read sequence needed for sensor read-out
     * @return Returns pointer to internal register sequence that describes which registers to read
     * for sensor samples
     */
    const manikin_sensor_reg_seq_t vl6180x_get_sample_reg_seq();

#ifdef __cplusplus
}
#endif
#endif /* VL6180X_H */