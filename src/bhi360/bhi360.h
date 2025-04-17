#ifndef BHI360_H
#define BHI360_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "common/manikin_types.h"

    /**
     * @brief Initialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Successful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL on unable to set registers (due to lost
     *          connection, e.g.)
     *         MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t bhi360_init_sensor(manikin_sensor_ctx_t *sensor_ctx);

    /**
     * @brief Read the sensor, which should read 16-bytes of data (8-channels, 2 bytes each)
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @param read_buf Ptr to read-buffer which is used for storing the samples
     * @return MANIKIN_STATUS_OK on success,
     *         MANIKIN_STATUS_READ_FAIL on failure while reading,
     *         MANIKIN_STATUS_WRITE_FAIL on failure while writing.
     */
    manikin_status_t bhi360_read_sensor(manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf);

    /**
     * @brief Deinitialize the sensor, which disables continuous sampling mode.
     * @param sensor_ctx Ptr to struct containing all settings for sensor, such as i2c instance &
     * address
     * @return MANIKIN_STATUS_OK on Succesful initialization,
     *         MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL on unable to set registers (due to lost
     * connection, e.g.) MANIKIN_STATUS_ERR_NULL_PARAM on invalid i2c handle
     */
    manikin_status_t bhi360_deinit_sensor(manikin_sensor_ctx_t *sensor_ctx);

#ifdef __cplusplus
}
#endif
#endif /* BHI360_H */
