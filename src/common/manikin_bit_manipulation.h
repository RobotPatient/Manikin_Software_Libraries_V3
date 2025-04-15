#ifndef MANIKIN_BIT_MANIPULATION_H
#define MANIKIN_BIT_MANIPULATION_H
/* Extern c for compiling with c++*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define BITMASK_COMPARE(NUM, MASK) (NUM & MASK)
#define BIT_IS_SET(VAL, BIT_NUM)   ((VAL >> BIT_NUM) & 1)
#define SHIFT_ONE_LEFT_BY_N(N)     (1 << N)

#define GET_LOWER_4_BITS_OF_BYTE(x) BITMASK_COMPARE(x, 0xF)
#define GET_UPPER_4_BITS_OF_BYTE(x) BITMASK_COMPARE((x >> 4), 0xF)

#define GET_LOWER_8_BITS_OF_SHORT(x) BITMASK_COMPARE(x, 0xFF)
#define GET_UPPER_8_BITS_OF_SHORT(x) BITMASK_COMPARE((x >> 8), 0xFF)

#define CONSTRUCT_SHORT_FROM_BYTES(upper, lower) ((upper << 8) | lower); 

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MANIKIN_BIT_MANIPULATION_H */