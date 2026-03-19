#ifndef LOGGER_H
#define LOGGER_H
/*
  File: logger.h
  ----------------------------------------------------
  Logging interface.

  Responsibilities:
  - Structured debug output over Serial
  - Log level filtering (DEBUG / INFO / WARN / ERROR)
*/

#include <stdint.h>

typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3
} LogLevel;

// Set the minimum level that will be printed (default: LOG_INFO).
void logger_setLevel(LogLevel level);

// Emit a log line: "[LEVEL][tag] msg"
void logger_log(LogLevel level, const char* tag, const char* msg);

#endif
