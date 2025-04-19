#ifndef ADS7138_REGS_H
#define ADS7138_REGS_H
#ifdef __cplusplus
extern "C"
{
#endif
#define ADS7138_REG(reg, opcode)    (reg | opcode << 8)
#define ADS7138_READ_BUF_SIZE       16 /* 2 bytes per channel, 8 channels */
#define ADS7138_REG_SYSTEM_STATUS   0x00
#define ADS7138_REG_GEN_CFG         0x01
#define ADS7138_REG_PIN_CFG         0x05
#define ADS7138_REG_SEQUENCE_CFG    0x10
#define ADS7138_REG_AUTO_SEQ_CH_SEL 0x12

#define ADS7138_REG_ALL_PINS_ANALOG_INP_BIT 0x00
#define ADS7138_REG_CAL_BIT                 0x02
#define ADS7138_REG_SW_RESET_BIT            0x01
#define ADS7138_REG_AUTO_SEQ_ALL_CH_SEL     0xFF
#define ADS7138_REG_SEQ_START_BIT           (1 << 4)
#define ADS7138_REG_ENABLE_AUTO_SEQ         0x01
#define ADS7138_REG_SYSTEM_STATUS_RSVD_BIT  (7)

#define ADS7138_OP_SINGLE_READ      0b00010000
#define ADS7138_OP_SINGLE_WRITE     0b00010000
#define ADS7138_OP_SET_BIT          0b00011000
#define ADS7138_OP_CLEAR_BIT        0b00100000
#define ADS7138_OP_CONTINUOUS_READ  0b00110000
#define ADS7138_OP_CONTINUOUS_WRITE 0b00101000
#ifdef __cplusplus
}
#endif
#endif /* ADS7138_REGS_H */
