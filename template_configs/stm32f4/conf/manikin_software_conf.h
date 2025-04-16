#ifndef MANIKIN_CONF_H
#define MANIKIN_CONF_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stm32f4xx.h>
#include <stdint.h>

    typedef I2C_TypeDef *manikin_i2c_inst_t;

    typedef TIM_TypeDef *manikin_timer_inst_t;

#ifdef __cplusplus
}
#endif
#endif /* MANIKIN_CONF_H */