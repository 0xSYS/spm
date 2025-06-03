


#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>
#include <random>

#ifdef __linux__
  #include <unistd.h>
  #include <dirent.h>
#endif

#include <sys/stat.h>

#if defined(_WIN32) || defined(_WIN64)
  #define VC_EXTRALEAN
  #include <Windows.h>
  #include <direct.h>

  #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
  #endif
#endif



#include "utils.hpp"
// #include "globals.hpp"
#include "dbg_log.hpp"





std::string SPMUtils::GetCurrentDate()
{
  std::ostringstream temp;
  auto cd = std::chrono::system_clock::now();
  std::time_t current_dt = std::chrono::system_clock::to_time_t(cd);
  std::tm local_tm =*std::localtime(&current_dt);
  temp << std::put_time(&local_tm, "%d.%m.%Y");
  return temp.str();
}

std::string SPMUtils::genRandomHash(size_t len)
{
  std::string out_hash;
  
  const char charset[] =
  "0123456789"
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz";
  
  const size_t max_index = (sizeof(charset) - 1);
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, max_index - 1);
  
  for (size_t i = 0; i < len; ++i)
  {
    out_hash += charset[dis(gen)];
  }
  
  return out_hash;
}


/*
! - - - - - - - - - - - - - - !
! File / Directory management !
! - - - - - - - - - - - - - - !
*/

// The rest of the functions are crucial for managing files and directories
std::string SPMUtils::GetHomeDir()
{
#ifdef __linux
  return std::getenv("HOME");
#endif

#if defined (_WIN32) || defined (_WIN64)
  return std::getenv("USERPROFILE");
#endif 
}

void SPMUtils::makeDir(std::string d)
{
#ifdef __linux__
  if(mkdir(d.c_str(), 0777))
  {
    SPM_LOG(SPMDebug::Err, "mkdir() failed !!");
  }
#endif

#if defined (_WIN32) || defined (_WIN64)
  std::wstring wstr(d.begin(), d.end());
  LPCWSTR temp = wstr.c_str();
  if(CreateDirectoryW(temp, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
  {
    SPM_LOG(SPMDebug::Err, "CreateDirectoryW() failed !! Err Code: ", GetLastError());
  }
#endif
}

int SPMUtils::removeDir(std::string d)
{
#ifdef __linux__
  int r = -1;
  DIR *dir = opendir(d.c_str());
  size_t path_len = strlen(d.c_str());

  if(dir)
  {
    struct dirent *p;
    r = 0;
    while(!r && (p = readdir(dir)))
    {
      int r2 = -1;
      char *buf;
      size_t len;
      
      // Skip "." and ".."
      if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
        continue;
      
      len = path_len + strlen(p->d_name) + 2;
      buf = (char *)malloc(len);
      
      if(buf)
      {
        struct stat statbuf;
        snprintf(buf, len, "%s/%s", d.c_str(), p->d_name);
        if(!stat(buf, &statbuf))
        {
          if(S_ISDIR(statbuf.st_mode))
          r2 = SPMUtils::removeDir(buf);
          else
          r2 = unlink(buf);
        }
        free(buf);
      }
      r = r2;
    }
    closedir(dir);
  }
  
  if (!r)
  r = rmdir(d.c_str());
  
#endif

#if defined(_WIN32) || defined(_WIN64)
  std::string searchPath = d + "\\*";
  WIN32_FIND_DATA findData;
  HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);

  if(hFind == INVALID_HANDLE_VALUE)
    return -1;

    int r = 0;
    do
    {
      const char* name = findData.cFileName;
      if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        continue;

      std::string fullPath = d + "\\" + name;

      if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        // It's a directory, recurse
        if(removeDirWin(fullPath) != 0)
          r = -1;
      }
      else
      {
        // It's a file or symlink, delete it
        if(!DeleteFile(fullPath.c_str()))
          r = -1;
      }
    }
    while(FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

    // Remove the now-empty directory
    if(!RemoveDirectory(d.c_str()))
      r = -1;
#endif

return r;
}

bool SPMUtils::checkDir(std::string d)
{
#ifdef __linux__
  struct stat s;

  if(stat(d.c_str(), &s) != 0)
    return false;
  else
    return true;
#endif

#if defined(_WIN32) || defined(_WIN64)
  std::wstring wstr(d.begin(), d.end());
  LPCWSTR temp = wstr.c_str();
  DWORD attributes = GetFileAttributesW(temp);
  return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
#endif
}

bool SPMUtils::checkFile(std::string f)
{
#ifdef __linux__
  struct stat buf;

  if(stat(f.c_str(), &buf) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
  
#endif
#if defined(_WIN32) || defined(_WIN64)
  std::wstring wstr(f.begin(), f.end());
  LPCWSTR temp = wstr.c_str();
  DWORD fileAttr = GetFileAttributesW(temp);
  return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
#endif
}



/*
! - - - !
! Misc  !
! - - - !
*/
 
 
void SPMUtils::printConfig(SPMConfig::cfgStruct c)
{
  std::cout << "Config structure:\n"
  //<< "[BOOL] -> config_storage       = " << c.config_storage       << "\n"
  << "[BOOL] -> dev_status_mpack     = " << c.dev_status_mpack     << "\n"
  << "[BOOL] -> msgbox_log           = " << c.msgbox_log           << "\n"
  << "[BOOL] -> debug_log            = " << c.debug_log            << "\n"
  << "[BOOL] -> restrict_mode        = " << c.restrict_mode        << "\n"
  << "[BOOL] -> restrict_timeout     = " << c.restrict_timeout     << "\n"
  << "[BOOL] -> power_opts_callbacks = " << c.power_opts_callbacks << "\n"
  << "[BOOL] -> user_feedback        = " << c.user_feedback        << "\n"
  << "[INT]  -> rescrict_time_span   = " << c.rescrict_time_span   << "\n"
  << "[INT]  -> port                 = " << c.port                 << "\n"
  << "[INT]  -> wolPort              = " << c.wol_port             << "\n"
  << "[INT]  -> last_env_index       = " << c.last_env_index       << "\n";
}

void SPMUtils::printDevArray(std::vector<SPMList::device> d)
{
  int loop_index = 0;
  for(const auto &dev : d)
  {
    loop_index++;
    std::cout << "[Loop Index]: " << loop_index
    << "\n{\n"
    << "    name: " << dev.name                     << ",\n"
    << "    interface_type: " << dev.interface_type << ",\n"
    << "    hw_address: " << dev.hw_addr            << ",\n"
    << "    broadcast_ip: " << dev.broadcast_ip     << ",\n"
    << "    os_ip: " << dev.os_ip                   << ",\n"
    << "    notes: " << dev.notes                   << ",\n"
    << "},\n";
    
  }
}

char * SPMUtils::getErr()
{
  char buf[256];
  char* msg;
#if((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
  strerror_r(errno, temp_msrgerr, sizeof(temp_msrgerr));
  msg = buf;
#else
  msg = strerror_r(errno, buf, sizeof(buf));
#endif
  return msg;
}

#if defined(_WIN32) || defined(_WIN64)
  void SPMUtils::SetWinTerm()
  {
    HANDLE stdoutHandle, stdinHandle;
    DWORD outModeInit, inModeInit;
    DWORD outMode = 0, inMode = 0;
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    stdinHandle = GetStdHandle(STD_INPUT_HANDLE);

    if(stdoutHandle == INVALID_HANDLE_VALUE || stdinHandle == INVALID_HANDLE_VALUE) 
    {
      exit(GetLastError());
    }
    
    if(!GetConsoleMode(stdoutHandle, &outMode) || !GetConsoleMode(stdinHandle, &inMode)) 
    {
      exit(GetLastError());
    }

    outModeInit = outMode;
    inModeInit = inMode;
    
    // Enable ANSI escape codes
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    // Set stdin as no echo and unbuffered
    inMode = (ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);

    if(!SetConsoleMode(stdoutHandle, outMode) || !SetConsoleMode(stdinHandle, inMode)) 
    {
      exit(GetLastError());
    }
    SetConsoleOutputCP(CP_UTF8); //Enabling unicode charset on windows console
  }
#endif
