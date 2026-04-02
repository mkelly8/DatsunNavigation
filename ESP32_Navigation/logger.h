#ifndef LOGGER_H
#define LOGGER_H
/*
  File: logger.h
  ----------------------------------------------------
  Logging interface.

  Responsibilities:
  - Structured debug output over Serial
  - Log level filtering (DEBUG / INFO / WARN / ERROR)
  - millis() timestamp on every line

  Output format:  [<ms>ms][LEVEL][tag] message

  Thread safety: no internal mutex — concurrent calls from multiple
  FreeRTOS tasks may produce interleaved Serial output in rare cases.
  Acceptable for debug monitoring; Serial writes are short.
*/

#include <stdint.h>
#include <stdarg.h>

typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} LogLevel;

// Set the minimum level that will be printed (default: LOG_DEBUG).
void logger_setLevel(LogLevel level);

// Emit a fixed string log line.
void logger_log(LogLevel level, const char* tag, const char* msg);

// Emit a formatted log line (printf-style).
void logger_logf(LogLevel level, const char* tag, const char* fmt, ...);

#endif // LOGGER_H
