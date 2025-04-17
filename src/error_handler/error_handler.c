#include "error_handler.h"
#include "manikin_platform.h"

void manikin_critical_assert(int file_crc32, int line)
{
    MANIKIN_CRIT_ERROR_HANDLER(file_crc32, line);
}

void manikin_non_critical_assert(int file_crc32, int line)
{
    MANIKIN_NON_CRIT_ERROR_HANDLER(file_crc32, line);
}