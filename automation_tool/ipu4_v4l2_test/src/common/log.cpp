#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

int Log::gLogLevel = 0xff;

Log::Log()
{
}

Log::~Log()
{
}

void Log::setDebugLevel(void)
{
    char *dbgLevel;
    dbgLevel = getenv("V4L2_TEST_DBG");

    if (dbgLevel) {
        gLogLevel = strtoul(dbgLevel, NULL, 0);
    }
}

void Log::print_log(const char *module, const int level, const char *format, ...)
{
    FILE *log_file = stdout;

    if (gLogLevel & level) {
        const char *level_str = NULL;
        va_list arg;
        va_start(arg, format);

        switch(level) {
            case LOG_DBG:
                level_str = "D";
            break;
            case LOG_INFO:
                level_str = "I";
            break;
            case LOG_ERR:
                level_str = "E";
            break;
            default:
                level_str = "V";
            break;
        }

        fprintf(log_file, "[++++++++++] [%s] %s: ", level_str, module);
        vfprintf(log_file, format, arg);
        fprintf(log_file, "\n");

        va_end(arg);
    }
}
