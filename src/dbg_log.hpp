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



#ifdef ANSI_ESCAPES
  #define ESC_ORANGE3       "\033[38;5;214m"
  #define ESC_BRIGHT_RED    "\033[38;5;196m"
  #define ESC_GRAY62        "\033[38;5;245m"
  #define ESC_MEDIUM_PURPLE "\033[38;5;105m"
  #define ESC_KHAKI         "\033[38;5;178m"
  #define ESC_EMERALD       "\033[38;5;41m"
  #define ESC_MINT          "\033[38;5;123m"
  #define ESC_ORANGE_RED    "\033[38;5;202m"
  #define ESC_RST           "\033[0m"
#else
  #define ESC_ORANGE3       ""
  #define ESC_BRIGHT_RED    ""
  #define ESC_GRAY62        ""
  #define ESC_MEDIUM_PURPLE ""
  #define ESC_KHAKI         ""
  #define ESC_EMERALD       ""
  #define ESC_MINT          ""
  #define ESC_RST           ""
#endif

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
  enum log_types
  {
    Info,
    Success,
    Warn,
    Err,
    custom
  };


  template <typename T, typename... Args>
  static inline void Log(log_types lt, std::string fnCall, std::string file, int line, T mainStr, Args... r)
  {
    if(lt == Info)
    {
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << ESC_GRAY62 << fnCall << ESC_RST << "}" << "[" << ESC_MINT << "Info" << ESC_RST << " -> " << mainStr;
#else
      std::cout << "libpm: [" << ESC_MINT << "Info" << ESC_RST << " -> " << mainStr;
#endif
    }
    else if(lt == Success)
    {
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << ESC_GRAY62 << fnCall << ESC_RST << "}" << "[" << ESC_EMERALD << "Success !" << ESC_RST << " -> " << mainStr;
#else
      std::cout << "libpm: [" << ESC_EMERALD << "Success !" << ESC_RST << " -> " << mainStr;
#endif
    }
    else if(lt == Warn)
    {
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << ESC_GRAY62 << fnCall << ESC_RST << "}" << "[" << ESC_KHAKI << "Warn" << ESC_RST << " -> " << mainStr;
#else
      std::cout << "libpm: [" << ESC_KHAKI << "Warn" << ESC_RST << " -> " << mainStr;
#endif
    }
    else if(lt == Err)
    {
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << ESC_GRAY62 << fnCall << ESC_RST << "}" << "[" << ESC_BRIGHT_RED << "Err" << ESC_RST << " -> " << mainStr;
#else
      std::cout << "libpm: [" << ESC_BRIGHT_RED << "Err" << ESC_RST << " -> " << mainStr;
#endif
    }
    else if(lt == custom)
    {
#ifdef DEBUG_FN_CALLS
      std::cout << "libspm: {" << file << ":" << line << " | " << ESC_GRAY62 << fnCall << ESC_RST << "}" << "[" << ESC_MEDIUM_PURPLE << "custom" << ESC_RST << " -> " << mainStr;
#else
      std::cout << "libpm: [" << ESC_MEDIUM_PURPLE << "custom" << ESC_RST << " -> " << mainStr;
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
  template<typename T, typename... Args>
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
