#include <stdio.h>
#include <stdarg.h>



#include "dbg.h"
#include "sckt_io.h"
#include "config.h"



void Log(enum log_types lt, const char * fmt, ...)
{
	va_list args;
  va_start(args, fmt);
  char buf[450];
  
  if(lt == Info)
  {
    printf("spm-serv: [\033[38;5;123mInfo\033[0m] -> ");
    if(current_conf.socket_response)
    {
      vsnprintf(buf, sizeof(buf), fmt, args);
      SndReply(sckt, repl_info, buf);
    }
  }
  else if(lt == Success)
  {
    printf("spm-serv: [\033[38;5;41mSuccess\033[0m !] -> ");
    if(current_conf.socket_response)
    {
      vsnprintf(buf, sizeof(buf), fmt, args);
      SndReply(sckt, repl_success, buf);
    }
  }
  else if(lt == Warn)
  {
    printf("spm-serv: [\033[38;5;178mWarn\033[0m] -> ");
    if(current_conf.socket_response)
    {
      vsnprintf(buf, sizeof(buf), fmt, args);
      SndReply(sckt, repl_warn, buf);
    }
  }
  else if(lt == Err)
  {
    printf("spm-serv: [\033[38;5;196mErr\033[0m] -> ");
    if(current_conf.socket_response)
    {
      vsnprintf(buf, sizeof(buf), fmt, args);
      SndReply(sckt, repl_failure, buf);
    }
  }
  
  vprintf(fmt, args);

  puts("");
  
  va_end(args);
}
