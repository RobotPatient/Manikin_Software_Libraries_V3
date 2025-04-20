#ifndef SAMPLE_TIMER_H
#define SAMPLE_TIMER_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "common/manikin_types.h"

    typedef struct
    {
        manikin_timer_inst_t    timer;
        manikin_watchdog_inst_t watchdog;
        uint16_t                frequency;
        uint8_t                 state;
    } sample_timer_ctx_t;

    /**
     * @brief Initializes the timer peripheral, so that it can automatically sample an sensor:
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_init(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Start the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_start(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Stop the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_stop(sample_timer_ctx_t *timer_inst);

    /**
     * @brief Deinitializes the timer peripheral
     * @param timer_ctx_t Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @return    MANIKIN_STATUS_OK: No error occured while configuring peripheral
     *            MANIKIN_STATUS_ERR_NULL_PARAM: sample_timer_ctx_t is null
     */
    manikin_status_t sample_timer_deinit(sample_timer_ctx_t *timer_inst);

    /**
     * @brief This method should be called when a sample was processed (outside ISR-context!)
     *        It does the following things:
     *        - Reset the watchdog
     *        - Handle Error Conditions (by resetting the I2C-bus or device)
     * @param timer_inst Struct handle to sample_timer_ctx_t which contains the settings for the
     * timer peripheral
     * @param read_status The status received from the sensor_read() function
     * @return MANIKIN_STATUS_OK: No error occured when resetting peripheral
     */
    manikin_status_t sample_timer_start_cb_handler(sample_timer_ctx_t   *timer_inst,
                                                   manikin_sensor_ctx_t *sensor);

    manikin_status_t sample_timer_end_cb_handler(sample_timer_ctx_t   *timer_inst,
                                                   manikin_sensor_ctx_t *sensor,
                                                   manikin_status_t      read_status);
#ifdef __cplusplus
}
#endif
#endif /* SAMPLE_TIMER_H */