#include "logger.h"
/*
  File: logger.cpp
  ----------------------------------------------------
  Logging implementation.
*/

#include <Arduino.h>

static LogLevel s_level = LOG_INFO;

void logger_setLevel(LogLevel level)
{
    s_level = level;
}

void logger_log(LogLevel level, const char* tag, const char* msg)
{
    if (level < s_level) return;

    switch (level)
    {
        case LOG_DEBUG: Serial.print("[DEBUG]"); break;
        case LOG_INFO:  Serial.print("[INFO] "); break;
        case LOG_WARN:  Serial.print("[WARN] "); break;
        case LOG_ERROR: Serial.print("[ERROR]"); break;
        default:        Serial.print("[?]    "); break;
    }

    Serial.print("[");
    Serial.print(tag);
    Serial.print("] ");
    Serial.println(msg);
}
