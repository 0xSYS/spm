


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#include <procps-ng/readproc.h>
#include <cJSON/cJSON.h>

#include "proc_manager.h"
#include "dbg.h"
#include "sckt_io.h"






bool IsRunnningProc(const char * proc_name)
{
  bool found = 0;
  PROCTAB *pt = openproc(PROC_FILLSTAT | PROC_FILLARG);
  if(!pt)
  {
    Log(Err, "Failed to get process table!!!");
    perror("openproc");
    found = false;
  }
  else
  {
    proc_t proc;
    memset(&proc, 0, sizeof(proc));
    
    while(readproc(pt, &proc) != NULL)
    {
      if(strcmp(proc.cmd, proc_name) == 0)
      {
        found = true;
        Log(Info, "%s is running", proc_name);
        break;
      }
      memset(&proc, 0, sizeof(proc));
    }
  }
  closeproc(pt);
  return found;
}

void KillRunningProc(const char* proc_name)
{
  PROCTAB *pt = openproc(PROC_FILLSTAT | PROC_FILLSTATUS | PROC_FILLARG);
  if (!pt)
  {
    Log(Err, "Failed to get active processes");
    perror("openproc");
    SndReply(sckt, repl_failure, "Failed to get active processes !!!");
  }
  else
  {
    proc_t proc;
    memset(&proc, 0, sizeof(proc));
    int killed = 0;
    while(readproc(pt, &proc) != NULL)
    {
      // proc.cmd is the short name (e.g., "bash")
      if (proc.cmd && strcmp(proc.cmd, proc_name) == 0)
      {
        Log(Info, "Killing PID: %d >> %s", proc.tid, proc.cmd);
        if(kill(proc.tid, SIGKILL) == -1)
        {
          Log(Err, "Failed to kill process !!!");
          perror("kill");
          SndReply(sckt, repl_failure, "Failed to kill process with PID: %d (%s)", proc.tid, proc_name);
        }
        else
        {
          killed++;
        }
      }

      if(proc.cmdline)
        free(proc.cmdline);
      
      if(proc.environ)
        free(proc.environ);
    }
    closeproc(pt);
    
    if (killed == 0)
    {
      Log(Err, "%s is not running", proc_name);
      SndReply(sckt, repl_failure, "%s is not runnning", proc.tid, proc_name);
    }
  }
}