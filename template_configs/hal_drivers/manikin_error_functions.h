#ifndef MANIKIN_ERROR_FUNCTIONS_H
#define MANIKIN_ERROR_FUNCTIONS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stddef.h>

    void critical_error(uint32_t hash, uint32_t line);

    void non_critical_error(uint32_t hash, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* MANIKIN_ERROR_FUNCTIONS_H */