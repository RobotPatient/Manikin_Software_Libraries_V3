#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "i2c/i2c.h"
#include "fake_i2c_functions.h"
#include "common/manikin_bit_manipulation.h"

uint8_t write_buffer[16] {};
uint8_t read_buffer[16] {};

size_t
custom_write_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, const uint8_t *bytes, size_t len)
{
    REQUIRE(len < sizeof(write_buffer));
    memcpy(write_buffer, bytes, len);
    return len;
}

size_t
custom_read_func (manikin_i2c_inst_t handle, uint8_t i2c_addr, uint8_t *bytes, size_t len)
{
    REQUIRE(len < sizeof(read_buffer));
    memcpy(bytes, read_buffer, len);
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
    memset(write_buffer, 0x00, sizeof(write_buffer));
    memset(read_buffer, 0x00, sizeof(read_buffer));
}

TEST_CASE("manikin_i2c_init returns error on NULL handle", "[init]")
{
    reset_mocks();
    manikin_status_t status = manikin_i2c_init(NULL, MANIKIN_I2C_SPEED_100KHz);
    REQUIRE(status == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("manikin_i2c_init returns error on invalid baudrate", "[init]")
{
    reset_mocks();
    // Assume this value is not defined in manikin_i2c_speed_t enum
    const manikin_i2c_speed_t invalid_baud = (manikin_i2c_speed_t)999;

    // HAL should not even be called if baud is invalid
    i2c_hal_init_fake.call_count             = 0;
    i2c_hal_error_flag_check_fake.call_count = 0;

    uint8_t          handle = 1;
    manikin_status_t status = manikin_i2c_init(&handle, invalid_baud);

    REQUIRE(status == MANIKIN_STATUS_ERR_INVALID_I2C_BAUD);
    REQUIRE(i2c_hal_init_fake.call_count == 0);
    REQUIRE(i2c_hal_error_flag_check_fake.call_count == 0);
}

TEST_CASE("manikin_i2c_init returns error when HAL init fails", "[init]")
{
    reset_mocks();
    i2c_hal_init_fake.return_val             = MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;

    uint8_t          handle = 1;
    manikin_status_t status = manikin_i2c_init(&handle, MANIKIN_I2C_SPEED_100KHz);

    REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_INIT_FAIL);
    REQUIRE(i2c_hal_init_fake.call_count == 1);
    REQUIRE(i2c_hal_error_flag_check_fake.call_count == 0);
}

TEST_CASE("manikin_i2c_init returns error when error flag check fails", "[init]")
{
    reset_mocks();
    i2c_hal_init_fake.return_val             = MANIKIN_STATUS_OK;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG;

    uint8_t          handle = 1;
    manikin_status_t status = manikin_i2c_init(&handle, MANIKIN_I2C_SPEED_100KHz);

    REQUIRE(status == MANIKIN_STATUS_ERR_PERIPHERAL_FAULT_FLAG);
    REQUIRE(i2c_hal_init_fake.call_count == 1);
    REQUIRE(i2c_hal_error_flag_check_fake.call_count == 1);
}

TEST_CASE("manikin_i2c_init succeeds on valid input", "[init]")
{
    reset_mocks();
    i2c_hal_init_fake.return_val             = MANIKIN_STATUS_OK;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;

    uint8_t          handle = 1;
    manikin_status_t status = manikin_i2c_init(&handle, MANIKIN_I2C_SPEED_100KHz);

    REQUIRE(status == MANIKIN_STATUS_OK);
    REQUIRE(i2c_hal_init_fake.call_count == 1);
    REQUIRE(i2c_hal_error_flag_check_fake.call_count == 1);
}

TEST_CASE("manikin_i2c_read_reg reads register correctly", "[read_reg]")
{
    reset_mocks();
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;
    i2c_hal_read_bytes_fake.return_val       = 1;
    i2c_hal_write_bytes_fake.custom_fake     = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake      = custom_read_func;

    read_buffer[0] = 100;

    uint8_t       handle   = 1;
    const uint8_t i2c_addr = 0x10;
    const uint8_t test_reg = 0x56;

    uint8_t reg_val = manikin_i2c_read_reg(&handle, i2c_addr, test_reg);

    REQUIRE(i2c_hal_write_bytes_fake.arg3_val == 2);
    REQUIRE(write_buffer[0] == GET_UPPER_8_BITS_OF_SHORT(test_reg));
    REQUIRE(write_buffer[1] == GET_LOWER_8_BITS_OF_SHORT(test_reg));
    REQUIRE(reg_val == read_buffer[0]);

    memset(write_buffer, 0x00, sizeof(write_buffer));
}

TEST_CASE("manikin_i2c_write_reg returns error on NULL handle", "[write_reg]")
{
    reset_mocks();
    const uint8_t  i2c_addr = 0x42;
    const uint16_t reg      = 0x1234;
    const uint8_t  data     = 0xAB;

    manikin_status_t status = manikin_i2c_write_reg(NULL, i2c_addr, reg, data);
    REQUIRE(status == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("manikin_i2c_write_reg sends correct bytes to I2C", "[write_reg]")
{
    reset_mocks();
    // Set up fake function behavior
    i2c_hal_write_bytes_fake.custom_fake     = custom_write_func;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;
    i2c_hal_write_bytes_fake.return_val      = 3;

    uint8_t        handle   = 1;
    const uint8_t  i2c_addr = 0x42;
    const uint16_t reg      = 0x1234;
    const uint8_t  data     = 0xAB;

    manikin_status_t status = manikin_i2c_write_reg(&handle, i2c_addr, reg, data);
    REQUIRE(status == MANIKIN_STATUS_OK);

    // Validate written bytes (should be: reg high byte, reg low byte, data)
    REQUIRE(i2c_hal_write_bytes_fake.arg3_val == 3);
    REQUIRE(write_buffer[0] == GET_UPPER_8_BITS_OF_SHORT(reg));
    REQUIRE(write_buffer[1] == GET_LOWER_8_BITS_OF_SHORT(reg));
    REQUIRE(write_buffer[2] == data);
}

TEST_CASE("manikin_i2c_write_reg16 returns error on NULL handle", "[write_reg16]")
{
    reset_mocks();
    const uint8_t  i2c_addr = 0x42;
    const uint16_t reg      = 0x1234;
    const uint16_t data     = 0xBEEF;

    manikin_status_t status = manikin_i2c_write_reg16(NULL, i2c_addr, reg, data);
    REQUIRE(status == MANIKIN_STATUS_ERR_NULL_PARAM);
}

TEST_CASE("manikin_i2c_write_reg16 sends correct bytes to I2C", "[write_reg16]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake     = custom_write_func;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;
    i2c_hal_write_bytes_fake.return_val      = 4;

    uint8_t        handle   = 1;
    const uint8_t  i2c_addr = 0x42;
    const uint16_t reg      = 0x1234;
    const uint16_t data     = 0xBEEF;

    manikin_status_t status = manikin_i2c_write_reg16(&handle, i2c_addr, reg, data);
    REQUIRE(status == MANIKIN_STATUS_OK);

    REQUIRE(i2c_hal_write_bytes_fake.arg3_val == 4);
    REQUIRE(write_buffer[0] == GET_UPPER_8_BITS_OF_SHORT(reg));
    REQUIRE(write_buffer[1] == GET_LOWER_8_BITS_OF_SHORT(reg));
    REQUIRE(write_buffer[2] == GET_UPPER_8_BITS_OF_SHORT(data));
    REQUIRE(write_buffer[3] == GET_LOWER_8_BITS_OF_SHORT(data));
}

TEST_CASE("manikin_i2c_read_reg16 returns correct value from I2C", "[read_reg16]")
{
    reset_mocks();
    i2c_hal_write_bytes_fake.custom_fake     = custom_write_func;
    i2c_hal_read_bytes_fake.custom_fake      = custom_read_func;
    i2c_hal_error_flag_check_fake.return_val = MANIKIN_STATUS_OK;
    i2c_hal_read_bytes_fake.return_val       = 2;

    // Simulate read result = 0xCAFE
    read_buffer[0] = 0xCA;
    read_buffer[1] = 0xFE;

    uint8_t        handle   = 1;
    const uint8_t  i2c_addr = 0x42;
    const uint16_t reg      = 0x1234;

    uint16_t val = manikin_i2c_read_reg16(&handle, i2c_addr, reg);

    REQUIRE(i2c_hal_write_bytes_fake.arg3_val == 2);
    REQUIRE(write_buffer[0] == GET_UPPER_8_BITS_OF_SHORT(reg));
    REQUIRE(write_buffer[1] == GET_LOWER_8_BITS_OF_SHORT(reg));
    REQUIRE(val == 0xCAFE);
}

int
main (int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}