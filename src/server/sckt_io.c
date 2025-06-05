#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>





#ifndef EXIT_FAILURE
  #define EXIT_FAILURE 1
#endif



#include "sckt_io.h"
#include "config.h"
#include "power_funcs.h"
#include "dbg.h"





config SettingsUnpack(char s[])
{
  config cfg = {0}; // Zero-initialize all fields
  char *str_ptr;
  char *tok;

  int packetSz = strlen(s);
  if(packetSz == 0)
  {
    Log(Info, "No settings to unpack");
  }
  else
  {
    tok = strtok_r(s, " ", &str_ptr);

    while(tok != NULL)
    {
      if(strncmp(tok, "dbgLog=", 7) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 7, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.dbg_log = (bool)tmp;
        }
      }
      else if(strncmp(tok, "scktResp=", 9) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 9, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.socket_response = (bool)tmp;
        }
      }
      else if(strncmp(tok, "allowSysInfo=", 13) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 13, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.allow_sys_info = (bool)tmp;
        }
      }
      else if(strncmp(tok, "skipProcScan=", 13) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 13, &end_ptr, 10);
        if(errno == 0 && * end_ptr == '\0')
        {
          cfg.skip_proc_scan = (bool)tmp;
        }
      }
      else if(strncmp(tok , "terminateProcesses=", 18) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 18, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.terminate_processes = (bool)tmp;
        }
      }
      else if(strncmp(tok , "stdoutCapture=", 14) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 14, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.stdout_capture = (bool)tmp;
        }
      }
      else if(strncmp(tok, "writeLogFiles=", 14) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 14, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.write_log_file = (bool)tmp;
        }
      }
      else if(strncmp(tok, "port=", 5) == 0)
      {
        errno = 0;
        char * end_ptr;
        long tmp = strtol(tok + 5, &end_ptr, 10);
        if(errno == 0 && *end_ptr == '\0')
        {
          cfg.port = (int)tmp;
        }
      }
      // Advance to next token!
      tok = strtok_r(NULL, " ", &str_ptr);
    }
  }
  return cfg;
}

void TestCall()
{
  Log(Info, "Testing some call...");
}

void StartScktReception()
{
  int serv_fd, sckt;
  struct sockaddr_in addr;


  config newCfg;


  int opt = 1;
  int addrlen = sizeof(addr);
  char buffer[1024] = {0};

  // Create socket
  serv_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_fd == 0) {
    Log(Err, "Failed to create socket !!");
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to the port
  setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(DEFAULT_PORT);

  // Bind
  if(bind(serv_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    Log(Err, "Failed to bind server with address !!");
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen
  if(listen(serv_fd, 3) < 0)
  {
    Log(Err, "Faied to listen to socket !!");
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Accept a connection
  sckt = accept(serv_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
  if(sckt < 0)
  {
    Log(Err, "Failed to accept connection !!");
    perror("accept");
    exit(EXIT_FAILURE);
  }

  // Read data
  read(sckt, buffer, 1024);
  Log(Info, "Recevied socket msg: %s\n", buffer);

  if(strcmp(buffer, "pwroff") == 0)
  {
    // Call Poweroff
    SysPowerOff();
  }
  else if(strcmp(buffer, "fpwroff") == 0)
  {
    // Force power off even when there's a process runing (Very dagerous)
  }
  else if(strcmp(buffer, "rbt") == 0)
  {
    // Call Reboot
    SysReboot();
  }
  else if(strcmp(buffer, "frbt") == 0)
  {
    // Force reboot even when there's a process runing (Still very dangerous)
  }
  else if(strcmp(buffer, "RstSettings") == 0)
  {
    // ResetSettings();
  }
  else if(strcmp(buffer, "clrLogs") == 0)
  {
    // ClearLogs();
  }
  else if(strstr(buffer, "customSettings:"))
  {
    TestCall();
    newCfg = SettingsUnpack(buffer);
    PrintConfig(newCfg);
    WriteConfig(newCfg);
    /*
    Todo:
    Make WriteConfig() to work...
    */
  }
  else if(strstr(buffer, "newBlackListProc="))
  {
    // NewBlacklistProcess(const char * procName);
  }

  // Close socket
  close(sckt);
  close(serv_fd);
}
