#pragma once


#include <stdbool.h>
#include <sys/types.h>





bool IsRunnningProc(const char * proc_name);
void KillRunningProc(const char * proc_name);
void AddProtectedProc(const char * proc_name);
void RemoveProtectedName(const char * proc_name);
char **GetProtectedProc();