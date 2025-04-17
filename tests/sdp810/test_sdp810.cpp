#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "sdp810/sdp810.h"
#include "fake_i2c_functions.h"

// Mocks or test doubles
static uint8_t dummy_read_buf[4];
uint8_t        handle = 1;
// Fake sensor context
manikin_sensor_ctx_t dummy_ctx;

uint16_t cur_reg;

size_t
custom_write_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, const uint8_t *bytes, size_t len)
{
    if (len >= 2)
    {
        cur_reg = (bytes[0] << 8 | bytes[1]);
    }
    return len;
}

size_t
custom_read_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, uint8_t *bytes, size_t len)
{
    memset(bytes, 0x01, len);
    return len;
}

void
reset_mocks ()
{
    RESET_FAKE(i2c_hal_init);
    RESET_FAKE(i2c_hal_error_flag_check);
    RESET_FAKE(i2c_hal_read_bytes);
    RESET_FAKE(i2c_hal_write_bytes);
    RESET_FAKE(i2c_hal_deinit);
}

// --- sdp810_init_sensor ---
TEST_CASE("sdp810_init_sensor handles null parameter", "[sdp810]")
{
    reset_mocks();
    REQUIRE(sdp810_init_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_init_sensor succeeds with valid context", "[sdp810]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = 0x29;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_init_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

// --- sdp810_read_sensor ---
TEST_CASE("sdp810_read_sensor fails with null context", "[sdp810]")
{
    reset_mocks();
    REQUIRE(sdp810_read_sensor(NULL, dummy_read_buf) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_read_sensor fails with null read buffer", "[sdp810]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_read_sensor(&dummy_ctx, NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_read_sensor reads successfully", "[sdp810]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = 0x29;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    uint8_t read_buf[16]                 = { 0 };
    REQUIRE(sdp810_read_sensor(&dummy_ctx, read_buf) == MANIKIN_STATUS_OK);
}

// --- sdp810_deinit_sensor ---
TEST_CASE("sdp810_deinit_sensor handles null context", "[sdp810]")
{
    reset_mocks();
    REQUIRE(sdp810_deinit_sensor(NULL) == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("sdp810_deinit_sensor succeeds with valid context", "[sdp810]")
{
    reset_mocks();
    dummy_ctx.i2c                        = &handle;
    dummy_ctx.i2c_addr                   = 0x29;
    i2c_hal_write_bytes_fake.custom_fake = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake  = custom_read_func;
    REQUIRE(sdp810_deinit_sensor(&dummy_ctx) == MANIKIN_STATUS_OK);
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}