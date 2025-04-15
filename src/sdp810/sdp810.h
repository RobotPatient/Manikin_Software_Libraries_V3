#ifndef SDP810_H
#define SDP810_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "manikin_types.h"

    manikin_status_t sdp810_init_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr);

    uint8_t sdp810_read_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr);

    manikin_status_t sdp810_deinit_sensor(manikin_i2c_inst_t i2c, uint8_t i2c_addr);

#ifdef __cplusplus
}
#endif
#endif /* SDP810_H */