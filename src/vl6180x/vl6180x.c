#include "vl6180x.h"
#include "i2c/i2c.h"
#include "private/vl6180x_regs.h"

/* All of these are undocumented private registers */
const manikin_sensor_reg_t init_regs[]
    = { { 0x0207, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0208, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0096, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0097, 0xfd, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e3, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e4, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e5, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e6, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00e7, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00f5, 0x02, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00d9, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00db, 0xce, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00dc, 0x03, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00dd, 0xf8, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x009f, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00a3, 0x3c, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00b7, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00bb, 0x3c, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00b2, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00ca, 0x09, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0198, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01b0, 0x17, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01ad, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x00ff, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0100, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0199, 0x05, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01a6, 0x1b, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01ac, 0x3e, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x01a7, 0x1f, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { 0x0030, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x46, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_VHV_REPEAT_RATE, 0xff, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTEGRATION_PERIOD, 0x63, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_VHV_RECALIBRATE, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTERMEASUREMENT_PERIOD, 0x00, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_RANGE_CHECK_ENABLES, (0x10 | 0x01), MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7b, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_INTEGRATION_PERIOD, 0x64, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x40, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_FIRMWARE_RESULT_SCALER, 0x01, MANIKIN_SENSOR_REG_TYPE_WRITE },
        { VL6180X_REG_SYSRANGE_START, 0x03 },
        MANIKIN_SENSOR_REG_TYPE_WRITE };

const manikin_sensor_reg_t read_regs[]
    = { { VL6180X_REG_RESULT_RANGE_VAL, 0x00, MANIKIN_SENSOR_REG_TYPE_READ } };
const manikin_sensor_reg_seq_t read_reg_seq
    = { read_regs, (sizeof(read_reg_seq) / sizeof(manikin_sensor_reg_t)) };

manikin_status_t
init_vl6180x_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    if (sensor_ctx->i2c == NULL)
    {
        return MANIKIN_STATUS_ERR_NULL_PARAM;
    }

    uint8_t data = 0;
    data         = manikin_i2c_read_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET);

    if (data != 1)
    {
        return MANIKIN_STATUS_ERR_SENSOR_INIT_FAIL;
    }

    for (uint8_t i = 0; i < sizeof(init_regs) / sizeof(manikin_sensor_reg_t); i++)
    {
        manikin_i2c_write_reg(
            sensor_ctx->i2c, sensor_ctx->i2c_addr, init_regs[i].reg, init_regs[i].val);
    }
    return MANIKIN_STATUS_OK;
}

uint8_t
read_vl6180x_sensor (manikin_sensor_ctx_t *sensor_ctx, uint8_t *read_buf)
{
    read_buf[0]
        = manikin_i2c_read_reg(sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_RESULT_RANGE_VAL);
    if (read_buf[0] == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

manikin_status_t
deinit_vl6180x_sensor (manikin_sensor_ctx_t *sensor_ctx)
{
    manikin_status_t status = manikin_i2c_write_reg(
        sensor_ctx->i2c, sensor_ctx->i2c_addr, VL6180X_REG_SYSRANGE_START, 0x00);
    if (status != MANIKIN_STATUS_OK)
    {
        return MANIKIN_STATUS_ERR_SENSOR_DEINIT_FAIL;
    }
    return MANIKIN_STATUS_OK;
}

const manikin_sensor_reg_seq_t
vl6180x_get_sample_reg_seq ()
{
    return read_reg_seq;
}