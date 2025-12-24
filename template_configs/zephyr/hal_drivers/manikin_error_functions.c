#include "manikin_error_functions.h"
#include "manikin_software_conf.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MANIKIN_ERR_HANDLER);

void
critical_error (uint32_t hash, uint32_t line)
{
    /* Do something */
    LOG_ERR("Critical erorr occurred in file: %d at line: %d\n", hash, line);
}

void
non_critical_error (uint32_t hash, uint32_t line)
{
    /* Do something */
    LOG_WRN("Fault occurred in file: %d at line: %d\n", hash, line);
}