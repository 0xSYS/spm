#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdarg.h>





#ifndef EXIT_FAILURE
  #define EXIT_FAILURE 1
#endif



#include "sckt_io.h"
#include "config.h"
#include "power_funcs.h"
#include "dbg.h"






//void KillServer()
//{
//  // fdsfjdsf
//  // Close socket
//  close(sckt);
//  close(serv_fd);
//}

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

void SndReply(int s, enum reply_type rt, const char * msg_fmt, ...)
{
  va_list args;
  char msg_buf[100];
  char full_buf[350];
  int written_buf;
  
  va_start(args, msg_fmt);
  vsnprintf(msg_buf, sizeof(msg_buf), msg_fmt, args);
  va_end(args);
  
  // Constructing the reply packets depending on the choosen reply type
  if(rt == repl_success)
  {
    written_buf = snprintf(full_buf, sizeof(full_buf), "[spm-serv] -> success: %s", msg_buf);
  }
  else if(rt == repl_warn)
  {
    written_buf = snprintf(full_buf, sizeof(full_buf), "[spm-serv] -> warn: %s", msg_buf);
  }
  else if(rt == repl_failure)
  {
    written_buf = snprintf(full_buf, sizeof(full_buf), "[spm-serv] -> err: %s", msg_buf);
  }
  else if(rt == repl_unknown)
  {
    written_buf = snprintf(full_buf, sizeof(full_buf), "[spm-serv] -> unknown_err: %s", msg_buf);
  }
  
  if(written_buf >= sizeof(full_buf))
  {
    // Output was truncated
    Log(Warn, "Reply message was truncated !!!");
  }
  
  send(s, &full_buf, sizeof(full_buf), 0);
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
  if (serv_fd == 0)
  {
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

  // main loop of the socket listener
  while(1)
  {
    sckt = accept(serv_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    if(sckt < 0)
    {
      Log(Err, "Failed to accept connection !!");
      perror("accept");
      //exit(EXIT_FAILURE);
      continue;
    }

    // Read data
    memset(buffer, 0, sizeof(buffer)); // Clear buffer before reading
    int n = read(sckt, buffer, sizeof(buffer) - 1);
    if(n <= 0)
    {
      close(sckt);
      continue;
    }
    buffer[n] = '\0'; // Null-terminate
  
    Log(Info, "Received socket msg: %s\n", buffer);

    if(strcmp(buffer, "pwroff") == 0)
    {
      // Call Poweroff
      SndReply(sckt, repl_success, "Poweroff succesfully executed");
      // Not The proper way to do that
      // Fisrt requirement is to check for runing procees and if there's any runing process simply deny the following action
      SysAction(Poweroff);
    }
    else if(strcmp(buffer, "fpwroff") == 0)
    {
      SndReply(sckt, repl_success, "Forced Poweroff succesfully executed");
      // Force power off even when there's a process runing (Very dagerous)
    }
    else if(strcmp(buffer, "rbt") == 0)
    {
      // Call Reboot
      SndReply(sckt, repl_success, "Reboot succesfully executed");
      SysAction(Reboot);
    }
    else if(strcmp(buffer, "frbt") == 0)
    {
      SndReply(sckt, repl_success, "Forced Reboot succesfully executed");
      // Force reboot even when there's a process runing (Still very dangerous)
    }
    else if(strcmp(buffer, "stby") == 0)
    {
      SndReply(sckt, repl_success, "Standby executed");
      SysAction(Standby);
    }
    else if(strcmp(buffer, "fstby") == 0)
    {
      SndReply(sckt, repl_success, "Forced Standby succesfully executed");
      // Forced standby mode
    }
    else if(strcmp(buffer, "hiber") == 0)
    {
      SndReply(sckt, repl_success, "Hibernation executed");
      SysAction(Hibernate);
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
      char *copy = strdup(buffer); // Copy the buffer temporarly and then start unpacking the settings
      newCfg = SettingsUnpack(copy);
      Log(Info, "Unpacking settings");
      
      free(copy);
      WriteConfig(newCfg);
    }
    else if(strstr(buffer, "newBlackListProc="))
    {
      // NewBlacklistProcess(const char * procName);
    }
    else if(strcmp(buffer, "killServ") == 0)
    {
      close(sckt);
      close(serv_fd);
      exit(0);
    }
    close(sckt);
  }
  close(serv_fd);
}
