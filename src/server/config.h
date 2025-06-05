#pragma once


#include <stdbool.h>




typedef struct
{
  bool allow_sys_info;
  bool dbg_log;
  bool write_log_file;
  bool skip_proc_scan;
  bool terminate_processes;
  bool stdout_capture;
  bool socket_response;
  int port;
}config;

extern config default_conf;
extern config current_conf;

config ReadConfig();
void WriteConfig(config cfg);
void PrintConfig(config cfg);
