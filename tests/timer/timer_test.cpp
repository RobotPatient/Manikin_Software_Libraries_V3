#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "sample_timer/sample_timer.h"
#include "fake_timer_functions.h"
#include "fake_watchdog_functions.h"
#include "fake_i2c_functions.h"

#define RESET_ALL_FAKES()                       \
    do                                          \
    {                                           \
        RESET_FAKE(timer_hal_init);             \
        RESET_FAKE(timer_hal_start);            \
        RESET_FAKE(timer_hal_stop);             \
        RESET_FAKE(timer_hal_deinit);           \
        RESET_FAKE(watchdog_hal_init);          \
        RESET_FAKE(watchdog_hal_kick);          \
        RESET_FAKE(watchdog_hal_deinit);        \
        RESET_FAKE(i2c_hal_error_flag_check);   \
        RESET_FAKE(i2c_hal_bus_reset);          \
        RESET_FAKE(i2c_hal_device_acknowledge); \
        RESET_FAKE(i2c_hal_bus_recover);        \
    } while (0)

sample_timer_ctx_t
sample_timer_ctx_init_with_valid_param ()
{
    sample_timer_ctx_t ctx {};
    uint8_t            timer_handle;
    uint8_t            watchdog_handle;
    ctx.timer                         = &timer_handle;
    ctx.frequency                     = 100;
    ctx.watchdog                      = &watchdog_handle;
    ctx.state                         = 0;
    timer_hal_init_fake.return_val    = 0;
    watchdog_hal_init_fake.return_val = 0;
    return ctx;
}

TEST_CASE("sample_timer_init", "[init]")
{
    RESET_ALL_FAKES();

    SECTION("returns error when ctx is null")
    {
        REQUIRE(sample_timer_init(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
    }

    SECTION("initializes timer and watchdog")
    {

        sample_timer_ctx_t ctx    = sample_timer_ctx_init_with_valid_param();
        auto               status = sample_timer_init(&ctx);

        REQUIRE(status == MANIKIN_STATUS_OK);
        REQUIRE(timer_hal_init_fake.call_count == 1);
        REQUIRE(watchdog_hal_init_fake.call_count == 1);
    }
}

TEST_CASE("sample_timer_start", "[start]")
{
    RESET_ALL_FAKES();

    SECTION("returns error when ctx is null")
    {
        REQUIRE(sample_timer_start(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
    }

    SECTION("starts the timer")
    {
        sample_timer_ctx_t ctx    = sample_timer_ctx_init_with_valid_param();
        auto               status = sample_timer_start(&ctx);

        REQUIRE(status == MANIKIN_STATUS_OK);
        REQUIRE(timer_hal_start_fake.call_count == 1);
    }
}

TEST_CASE("sample_timer_stop", "[stop]")
{
    RESET_ALL_FAKES();

    SECTION("returns error when ctx is null")
    {
        REQUIRE(sample_timer_stop(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
    }

    SECTION("stops the timer")
    {
        sample_timer_ctx_t ctx = sample_timer_ctx_init_with_valid_param();

        auto status = sample_timer_stop(&ctx);

        REQUIRE(status == MANIKIN_STATUS_OK);
        REQUIRE(timer_hal_stop_fake.call_count == 1);
    }
}

TEST_CASE("sample_timer_deinit", "[deinit]")
{
    RESET_ALL_FAKES();

    SECTION("returns error when ctx is null")
    {
        REQUIRE(sample_timer_deinit(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
    }

    SECTION("deinitializes timer and watchdog")
    {
        sample_timer_ctx_t ctx              = sample_timer_ctx_init_with_valid_param();
        timer_hal_deinit_fake.return_val    = MANIKIN_STATUS_OK;
        watchdog_hal_deinit_fake.return_val = MANIKIN_STATUS_OK;
        auto status                         = sample_timer_deinit(&ctx);

        REQUIRE(status == MANIKIN_STATUS_OK);
        REQUIRE(timer_hal_deinit_fake.call_count == 1);
        REQUIRE(watchdog_hal_deinit_fake.call_count == 1);
    }
}

TEST_CASE("sample_timer_start_cb_handler", "[cb_start]")
{
    RESET_ALL_FAKES();

    SECTION("returns OK and kicks watchdog and recovers I2C on error")
    {
        sample_timer_ctx_t   ctx = sample_timer_ctx_init_with_valid_param();
        manikin_sensor_ctx_t sensor {};

        i2c_hal_error_flag_check_fake.return_val   = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
        i2c_hal_device_acknowledge_fake.return_val = 0;

        auto status = sample_timer_start_cb_handler(&ctx, &sensor);

        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
        REQUIRE(watchdog_hal_kick_fake.call_count == 1);
    }
}

TEST_CASE("sample_timer_end_cb_handler", "[cb_end]")
{
    RESET_ALL_FAKES();

    SECTION("returns OK and handles error recovery")
    {
        sample_timer_ctx_t   ctx = sample_timer_ctx_init_with_valid_param();
        manikin_sensor_ctx_t sensor {};

        i2c_hal_error_flag_check_fake.return_val = 1;

        auto status
            = sample_timer_end_cb_handler(&ctx, &sensor, MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);

        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
        REQUIRE(watchdog_hal_kick_fake.call_count == 1);
        REQUIRE(i2c_hal_bus_reset_fake.call_count == 1);
    }

    SECTION("returns OK on clean read with watchdog kick")
    {
        sample_timer_ctx_t   ctx = sample_timer_ctx_init_with_valid_param();
        manikin_sensor_ctx_t sensor {};

        i2c_hal_error_flag_check_fake.return_val = 0;

        auto status = sample_timer_end_cb_handler(&ctx, &sensor, MANIKIN_STATUS_OK);

        REQUIRE(status == MANIKIN_STATUS_OK);
        REQUIRE(watchdog_hal_kick_fake.call_count == 1);
        REQUIRE(i2c_hal_bus_reset_fake.call_count == 0);
    }
    SECTION("Perform reset sequence")
    {
        const uint16_t     example_sample_rate = 100;
        const uint16_t     timeout_sample_rate = 1;
        sample_timer_ctx_t ctx                 = sample_timer_ctx_init_with_valid_param();
        ctx.frequency                          = example_sample_rate;
        manikin_sensor_ctx_t sensor {};

        auto status = sample_timer_init(&ctx);
        REQUIRE(status == MANIKIN_STATUS_OK);
        /* 0: Set artificial fault-state using test stub */
        i2c_hal_error_flag_check_fake.return_val   = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;
        i2c_hal_device_acknowledge_fake.return_val = 0;

        /* 1: It should now notice the fault flag and return it, so that the SM can start */
        status = sample_timer_start_cb_handler(&ctx, &sensor);
        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
        REQUIRE(timer_hal_init_fake.arg1_val == example_sample_rate);
        REQUIRE(watchdog_hal_kick_fake.call_count == 1);
        REQUIRE(ctx.state == 0);

        /* 2: Great, it should now set the bus reset in action */
        status = sample_timer_end_cb_handler(&ctx, &sensor, status);
        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
        REQUIRE(timer_hal_init_fake.call_count == 2);
        REQUIRE(timer_hal_init_fake.arg1_val == timeout_sample_rate);
        REQUIRE(watchdog_hal_kick_fake.call_count == 2);
        REQUIRE(i2c_hal_bus_reset_fake.call_count == 1);
        REQUIRE(ctx.state == 2);

        /* 3: Bus is reset, now it should recover the bus */
        status = sample_timer_start_cb_handler(&ctx, &sensor);
        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
        REQUIRE(watchdog_hal_kick_fake.call_count == 3);
        REQUIRE(timer_hal_init_fake.arg1_val == timeout_sample_rate);
        REQUIRE(i2c_hal_bus_recover_fake.call_count == 1);
        REQUIRE(ctx.state == 1);

        /* 4: In this state the end cb will do nothing, as it has timeout of 1 period */
        status = sample_timer_end_cb_handler(&ctx, &sensor, status);
        REQUIRE(watchdog_hal_kick_fake.call_count == 4);
        REQUIRE(timer_hal_init_fake.call_count == 3);
        REQUIRE(timer_hal_init_fake.arg1_val == timeout_sample_rate);
        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);

        /* Reset the fault-state */
        i2c_hal_error_flag_check_fake.return_val = 0;

        /* 5: Bus recovered: It will finish the state-machine now */
        status = sample_timer_start_cb_handler(&ctx, &sensor);
        REQUIRE(ctx.state == 0);
        REQUIRE(watchdog_hal_kick_fake.call_count == 5);
        REQUIRE(timer_hal_init_fake.arg1_val == example_sample_rate);
        REQUIRE(timer_hal_init_fake.call_count == 4);
        REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);

        /* 6: It should now be operational in original state */
        status = sample_timer_end_cb_handler(&ctx, &sensor, status);
        REQUIRE(watchdog_hal_kick_fake.call_count == 6);
        REQUIRE(status == MANIKIN_STATUS_OK);

        /* 7: Start handler decides, whether reset was successful */
        status = sample_timer_start_cb_handler(&ctx, &sensor);
        REQUIRE(ctx.state == 0);
        REQUIRE(watchdog_hal_kick_fake.call_count == 7);
        /* Check it is back to originally set sample-rate */
        REQUIRE(timer_hal_init_fake.arg1_val == example_sample_rate);
        /* It is back to operational state! */
        REQUIRE(status == MANIKIN_STATUS_OK);
    }
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}