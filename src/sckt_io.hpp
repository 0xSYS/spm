#pragma once




#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "spm.hpp"



#define DEFAULT_PORT 8124

#if defined(_WIN32) || defined(_WIN64)
  #define SPM_SOCKET SOCKET
#endif

#ifdef __linux__
  #define SPM_SOCKET int
#endif

inline char repl_buf[1024];

class SPM_SocketIO
{
  public:
  /*
  System information structure
  */
  typedef struct
  {
    std::string usrName;
    std::string ip;
    std::string broadcastIP;
    std::string macAddr;
    std::string batteryInfo;
  }sysInfo;

  typedef struct
  {
    bool socket_response;    // Enable / disable feedback after sending a packet
    bool skip_proc_scan;     // Enable / disable scanning the current process list before executing a power action
    bool debug_log;          // Enable / disable general debuging (This also includes sending minimal debug information from server to frontend)
    bool write_log_files;    // Enable / disable writing to log files
    bool alow_sys_info;      // Enable / disable sending system information trough the socket
    bool stdout_capture;     // Enable / disable stdout buffer capture to text file when runing headles cli commands
    bool terminate_proceses; // Values: Never, Always, Always first
    int listen_port;         // Use different port for sending / recepting the packets
  }ServerSettings;
  
  //enum action_type
  //{
  //  Poweroff = 1,
  //  ForcePoweroff, // The forced power actions should be avoided from being used as they can cause system corruptions if not handeled carefully
  //  Reboot,
  //  ForceReboot,
  //  Standby,
  //  ForceStandby,
  //  Sleep,
  //  ForceSleep,
  //  Hibernate
  //};
  
    static bool ping(int count, int delay, std::string ip);                       // [*] The basic ping
    static bool InternalPing(int count, int delay, std::string ip);               // [ ] Custom SPM ping. Returns true only if the SPM server is runing
    static void SndPowerAction(SPM::server_actions at, std::string target);       // [*] Send power action to a device (Poweroff / reboot)
    static sysInfo GetSysInfo();                                                  // [ ] Retrieve system information of a specific device
    static std::string GetServerReplyStr(int s);                                       // [ ] Get the curret replied message from the server
    static std::vector <sysInfo> GetSysInfoArr(std::vector<std::string> devices); // [ ] Retrieve system information from multiple devices into an aray
    static bool IsSSH_Ready(std::string target);                                  // [ ] Checks if SSH daemon runs (which means the host computer / server is ready for ssh connections)
    static void EnableSSHDaemon(std::string target);                              // [ ] Enable / disable the SSH daemon
    static void CheckRuningService(std::string target, std::string serv_name);    // [ ] Check for any custom runing service
    static void AddProtectedProc(std::string target, std::string proc);           // [*] Prevents server restart / poweroff if a specific process is runing
    static void RemoveProtectedProc(std::string target, std::string proc);        // [*] Remove protected process
    static void AddUnauthorizedProc(std::string target, std::string proc);        // [*] Add unauthorized process (the server prevents a specific process from being executed trough headless CLI execution)
    static void RemoveUnauthorizedProc(std::string target, std::string proc);     // [*] Remove unauthorized process
    static void SndCustomSettings(std::string target, ServerSettings ss);         // [*] Send custom configuration for the server via sockets
    static void SndResetSettings(std::string target);                             // [*] Reset server to default settings by removing the configuration file
    static void SndClearLogs(std::string target);                                 // [*] Send clearance of the log files
    static void SndStopServer(std::string target);                                // [*] Stops the server from listening
    static void SndResumeServer(std::string target);                              // [*] Resume the server
    static void SndKillServer(std::string target);                                // [*] Kill the server process
};
