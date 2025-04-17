#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H
#ifdef __cplusplus
extern "C"
{
#endif

#define MANIKIN_ASSERT(FILE_HASH, cond, retval)           \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            manikin_critical_assert(FILE_HASH, __LINE__); \
            return retval;                                \
        }                                                 \
    } while (0)
#define MANIKIN_NON_CRIT_ASSERT(FILE_HASH, cond, retval)      \
    do                                                        \
    {                                                         \
        if (!(cond))                                          \
        {                                                     \
            manikin_non_critical_assert(FILE_HASH, __LINE__); \
            return retval;                                    \
        }                                                     \
    } while (0)

    void manikin_critical_assert(int file_crc32, int line);

    void manikin_non_critical_assert(int file_crc32, int line);
#ifdef __cplusplus
}
#endif
#endif /* ERROR_HANDLER_H */
