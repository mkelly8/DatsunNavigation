#include "logger.h"
/*
  File: logger.cpp
  ----------------------------------------------------
  Logging implementation.

  Every line is prefixed with a millis() timestamp so timing
  relationships between events are visible on the serial monitor.
*/

#include <Arduino.h>
#include <stdio.h>

static LogLevel s_level = LOG_DEBUG;

// ---- Private helper ------------------------------------------

static void printPrefix(LogLevel level, const char* tag)
{
    Serial.print("[");
    Serial.print(millis());
    Serial.print("ms][");
    switch (level)
    {
        case LOG_DEBUG: Serial.print("DEBUG"); break;
        case LOG_INFO:  Serial.print("INFO "); break;
        case LOG_WARN:  Serial.print("WARN "); break;
        case LOG_ERROR: Serial.print("ERROR"); break;
        default:        Serial.print("?    "); break;
    }
    Serial.print("][");
    Serial.print(tag);
    Serial.print("] ");
}

// ---- Public API ----------------------------------------------

void logger_setLevel(LogLevel level)
{
    s_level = level;
}

void logger_log(LogLevel level, const char* tag, const char* msg)
{
    if (level < s_level) return;
    printPrefix(level, tag);
    Serial.println(msg);
}

void logger_logf(LogLevel level, const char* tag, const char* fmt, ...)
{
    if (level < s_level) return;

    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printPrefix(level, tag);
    Serial.println(buf);
}
