#pragma once




#include <string>
#include <vector>



#define DEFAULT_PORT 8085


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
    bool feedback;                 // Enable / disable feedback after sending a packet
    bool skipProcScan;             // Enable / disable scanning the current process list before executing a power action
    bool debugLog;                 // Enable / disable general debuging (This also includes sending minimal debug information from server to frontend)
    bool writeLogFiles;            // Enable / disable writing to log files
    bool alowSysInfo;              // Enable / disable sending system information trough the socket
    bool stdoutCapture;            // Enable / disable stdout buffer capture to text file when runing headles cli commands
    std::string terminateProceses; // Values: Never, Always, Always first
    int listenPort;                // Use different port for sending / recepting the packets
  }ServerSettings;
  
  enum ActionTypes
  {
    Poweroff = 1,
    ForcePoweroff, // The forced power actions should be avoided from being used as they can cause system corruptions if not handeled carefully
    Reboot,
    ForceReboot
  };
  
    static void SndPowerAction(int actType, std::string target);                  // [*] Send power action to a device (Poweroff / reboot)
    static sysInfo GetSysInfo();                                                  // [ ] Retrieve system information of a specific device
    static std::vector <sysInfo> GetSysInfoArr(std::vector<std::string> devices); // [ ] Retrieve system information from multiple devices into an aray
    static void AddProtectedProc(std::string target, std::string proc);           // [*] Prevents server restart / poweroff if a specific process is runing
    static void RemoveProtectedProc(std::string target, std::string proc);        // [*] Remove protected process
    static void AddUnauthorizedProc(std::string target, std::string proc);        // [*] Add unauthorized process (the server prevents a specific process from being executed trough headless CLI execution)
    static void RemoveUnauthorizedProc(std::string target, std::string proc);     // [*] Remove unauthorized process
    static void SndCustomSettings(std::string target, ServerSettings);            // [*] Send custom configuration for the server via sockets
    static void SndResetSettings(std::string target);                             // [*] Reset server to default settings by removing the configuration file
    static void SndClearLogs(std::string target);                                 // [*] Send clearance of the log files
    static void SndStopServer(std::string target);                                // [*] Stops the server from listening
    static void SndResumeServer(std::string target);                              // [*] Resume the server
    static void SndKillServer(std::string target);                                // [*] Kill the server process
};
