/*
!!! DO NOT EVER TOUCH THIS FILE OR I'LL HAUNT YOU !!!
*/












#pragma once







#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

#ifdef __linux__
  #include <unistd.h>
  #include <sys/wait.h>
  #include <sys/types.h>
  #include <stdio.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
  #include <Windows.h>
#endif


#include "utils.hpp"
#include "spm.hpp"
// #include "globals.hpp"



//static SPMUtils spmUtils;
//static SPM spm;



#ifdef DEBUG_FN_CALLS
  #if defined(_MSC_VER)
    #define FN_SIGNATURE __FUNCSIG__
  #else
    #define FN_SIGNATURE __PRETTY_FUNCTION__
  #endif
#endif



#define SPM_LOG(logType, ...) SPMDebug::Log(logType, FN_SIGNATURE, __FILE__, __LINE__, __VA_ARGS__)



inline void PrintArgs() {} // Base case for recursion

template <typename T, typename... Args>
inline void PrintArgs(T first, Args... rest)
{
  std::cout << first; // Print the first argument
  PrintArgs(rest...);        // Recurse for remaining arguments
}


inline void AppendToStream(std::ostringstream&) {}

template <typename T, typename... Args>
inline void AppendToStream(std::ostringstream& oss, T first, Args... rest)
{
  oss << first;
  AppendToStream(oss, rest...);
}

#ifdef __linux__
inline std::vector<std::string> parseCommand(const std::string& command)
{
  std::vector<std::string> args;
  std::istringstream stream(command);
  std::string arg;
  bool inQuotes = false;
  std::string temp;

  while (stream)
  {
    char c = stream.get();
    if(stream.eof()) break;

    if(c == '"' || c == '\'')
    {  // Handle both single and double quotes
        inQuotes = !inQuotes;
    }
    else if(c == ' ' && !inQuotes)
    {
      // Space outside quotes
      if (!temp.empty())
      {
        args.push_back(temp);
        temp.clear();
      }
    }
    else
    {
      temp += c;
    }
  }

  if (!temp.empty()) args.push_back(temp);
  return args;
}
#endif

class SPMDebug
{
  public:
  enum logTypes
  {
    Info = 1,
    Success,
    Warn,
    Err,
    noType = 0
  };


  template <typename T, typename... Args>
  static inline void Log(int logType, std::string fnCall, std::string file, int line, T mainStr, Args... r)
  {
    if(logType == 1)
    {
#ifdef ANSI_ESCAPES
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | \033[38;5;245m" << fnCall << "\033[0m} [\033[38;5;123mInfo\033[0m] -> " << mainStr;
#else
      std::cout << "libspm: [\033[38;5;123mInfo\033[0m] -> " << mainStr;
#endif
#else
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << fnCall <<  " [Info] -> " << mainStr;
#else
      std::cout << "libspm: [Info] -> " << mainStr;
#endif
#endif
    }
    else if(logType == 2)
    {
#ifdef ANSI_ESCAPES
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | \033[38;5;245m" << fnCall << "\033[0m} [\033[38;5;41mSuccess !\033[0m] -> " << mainStr;
#else
      std::cout << "libspm: [\033[38;5;41mSuccess !\033[0m] -> " << mainStr;
#endif
#else
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << fnCall <<  " [Success !] -> " << mainStr;
#else
      std::cout << "libspm: [Success !] -> " << mainStr;
#endif
#endif
    }
    else if(logType == 3)
    {
#ifdef ANSI_ESCAPES
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | \033[38;5;245m" << fnCall << "\033[0m} [\033[38;5;178mWarn\033[0m] -> " << mainStr;
#else
      std::cout << "libspm: [\033[38;5;178mWarn\033[0m] -> " << mainStr;
#endif
#else
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << fnCall <<  " [Warn] -> " << mainStr;
#else
      std::cout << "libspm: [Warn] -> " << mainStr;
#endif
#endif
    }
    else if(logType == 4)
    {
#ifdef ANSI_ESCAPES
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | \033[38;5;245m" << fnCall << "\033[0m} [\033[38;5;196mErr\033[0m] -> " << mainStr;
#else
      std::cout << "libspm: [\033[38;5;123mErr\033[0m] -> " << mainStr;
#endif
#else
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << fnCall <<  " [Err] -> " << mainStr;
#else
      std::cout << "libspm: [Err] -> " << mainStr;
#endif
#endif
    }
    else if(logType == 0)
    {
#ifdef DEBUG_FN_CALLS
#ifdef ANSI_ESCAPES
      std::cout << "libspm: {" << file << ":" << line << " | \033[38;5;245m" << fnCall << "\033[0m} [ \033[38;5;105mcustom debug\033[0m ] -> " << mainStr;
#else
      std::cout << "libspm: {" << file << ":" << line << " | " << fnCall << "} [ custom debug ] -> " << mainStr;
#endif
#else
      std::cout << "libspm: [ custom debug ] -> " << mainStr;
#endif
    }
    // Forgor to print the rest of the args XD
    
    PrintArgs(r...); // Process remaining arguments
    std::cout << std::endl;
    
    // Setting parsing needs to be fixed to get this to work 
    if(!globalConf.debug_log)
    {
      std::ostringstream logName;

      // std::cout << "DBG LOG\n";

// Create the log filename containing the current date
#ifdef __linux__
      logName << SPMUtils::GetHomeDir() << "/.spm/logs/spm_log - " << SPMUtils::GetCurrentDate();
#endif

#if defined(_WIN32) || defined(_WIN64)
      logName << SPMUtils::GetHomeDir() << "\\.spm\\logs\\spm_log - " << SPMUtils::GetCurrentDate() << ".txt";
#endif 

      std::ofstream out_log_init(logName.str());
      time_t t_stamp_init;
      time(&t_stamp_init);
      std::ostringstream logText;
      //AppendToStream(logText, r...);
      char *t = ctime(&t_stamp_init);
      t[strlen(t)-1] = '\0';
      logText << "[ " << t << " ] -> " << mainStr << "\n";
      AppendToStream(logText, r...);

      out_log_init << logText.str();
      out_log_init.close();
    }
  }
  template<typename T, typename... Args >
  static void MsgBoxLog(int logType, T mainStr, Args... r)
  {
    std::ostringstream text;
#ifdef NO_MSGBOX
    text << "{MsgBoxLog off} | " << mainStr;
#else
    text << mainStr;
#endif
    AppendToStream(text, r...);

#ifdef NO_MSGBOX
    std::cout << text.str() << "\n";
#else
    

// For security reasons there's a lot of code below
// !!! PLS DO NOT EVER TOUCH THIS !!!
// 🡇 🡇 🡇
#ifdef __linux__
    std::ostringstream cmd;
    pid_t p = fork();
    if(logType == 1)
    {
      cmd << "zenity --info --title='Server Power Management - Information' --text='" << text.str() << "'";
    }
    else if(logType == 2)
    {
      cmd << "zenity --info --title='Server Power Management - Success' --text='" << text.str() << "'";
    }
    else if(logType == 3)
    {
      cmd << "zenity --warning --title='Server Power Management - Warning' --text='" << text.str() << "'";
    }
    else if(logType == 4)
    {
      cmd << "zenity --error --title='Server Power Management - Error' --text='" << text.str() << "'";
    }


    std::string command = cmd.str();
    std::vector<std::string> args = parseCommand(command);

    std::vector<char*> c_args;
    for (auto& s : args)
    {
      c_args.push_back(s.data());  // Convert std::string to char*
    }
    c_args.push_back(nullptr);  // Null-terminate

    if(p == 0)
    {
      execvp(c_args[0], c_args.data());
      perror("execlp failed");
      _exit(1);
    }
    else if(p > 0)
    {
      int stat;
      waitpid(p, &stat, 0);
    }
    else
    {
      perror("fork failed");
    }
#endif // Linux defines
// 🡅 🡅 🡅
// !!! DO NOT EVER TOUCH THIS !!!

#if defined(_WIN32) || defined(_WIN64)
    int msgBoxID;
    std::string cppStr = text.str();
    std::wstring wstr(cppStr.begin(), cppStr.end());
    LPCWSTR temp = wstr.c_str();


    if(logType == 1)
    {
      msgBoxID = MessageBoxW(NULL, (LPCWSTR)temp, (LPCWSTR)L"Server Power Management - Info", MB_ICONINFORMATION | MB_OK);
    }
    else if(logType == 2)
    {
      msgBoxID = MessageBoxW(NULL, (LPCWSTR)temp, (LPCWSTR)L"Server Power Management - Sucess", MB_ICONINFORMATION | MB_OK);
    }
    else if(logType == 3)
    {
      msgBoxID = MessageBoxW(NULL, (LPCWSTR)temp, (LPCWSTR)L"Server Power Management - Warning", MB_ICONEXCLAMATION | MB_OK);
    }
    else if(logType == 4)
    {
      msgBoxID = MessageBoxW(NULL, (LPCWSTR)temp, (LPCWSTR)L"Server Power Management - Error", MB_ICONERROR | MB_OK);
    }
#endif // Win32 defines

#endif // NO_MSGBOX
  }
};
