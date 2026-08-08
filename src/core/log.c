#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>

void log_message(LogLevel level, const char* file, int line, const char* format, ...)
{
    const char* level_string;
    FILE* stream;

    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        level_string = "DEBUG";
        stream = stdout;
        break;
    case LOG_LEVEL_INFO:
        level_string = "INFO";
        stream = stdout;
        break;
    case LOG_LEVEL_WARN:
        level_string = "WARN";
        stream = stdout;
        break;
    case LOG_LEVEL_ERROR:
        level_string = "ERROR";
        stream = stdout;
        break;
    default:
        level_string = "UNKNOWN";
        stream = stderr;
        break;
    }

    fprintf(stream, "[%s] %s:%d | ", level_string, file, line);

    va_list args;
    va_start(args, format);

    vfprintf(stream, format, args);

    va_end(args);

    fputc('\n', stream);
}
