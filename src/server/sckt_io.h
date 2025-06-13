#pragma once




#define DEFAULT_PORT 8124


enum reply_type
{
  repl_success,
  repl_warn,
  repl_failure,
  repl_unknown
};

typedef struct
{
  enum reply_type rt;
  const char * msg;
  const char * descript;
  
}reply_packet;


void KillServer();
void SndReply(int s, enum reply_type rt, const char * msg_fmt, ...);
void SndSuccessCallback();
void SndWarnCallback();
void SndFailureCallback();
void StartScktReception();
