


#include <stdbool.h>
#include <procps-ng/readproc.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "proc_manager.h"
#include "dbg.h"






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