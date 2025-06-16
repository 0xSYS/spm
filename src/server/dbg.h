#pragma once





enum log_types
{
  Info = 1,
  Success,
  Warn,
  Err
};


void Log(enum log_types lt, const char * fmt, ...);
