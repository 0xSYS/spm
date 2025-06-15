#pragma once




#include <vector>

#include "config.hpp"
#include "spm_list.hpp"





inline SPMConfig::cfgStruct globalConf;
inline bool is_spm_init = false;

class SPM
{
	public:
	
  std::vector<SPMList::device> mainList; // Store the main list at first run
  int current_env_index = 0;
  
  enum server_actions
  {
    Wake = 1,
    Shutdown,
    ForceShutdown,
    Restart,
    ForceRestart,
    Sleep,
    ForceSleep,
    Standby,
    ForceStandby,
    Hibernate,     // Dosent require forced action as it dosent cause any data loss or corruption (Last state of the os is saved to local storage)
    IPMI_Wake       = 0x01,
    IPMI_HardReset  = 0x03,
    IPMI_PowerCycle = 0x02,
    IPMI_PowerOff   = 0x00
  };
  
  
  typedef struct
  {
    std::string name;
    std::string description;
    std::string date_created;
  }envInfo;

  
	static void Init(SPMConfig::cfgStruct* settings_init);     // [*] Initialize SPM with custom settings instead of reading its configuration file
	static void CreateDefaultEnv();                            // [*] Checks default environment validity (Kinda stupid tbh)
	static void CheckEnv(int env_index);                       // [*] Checks if a storage environment is valid
	static void CreateNewEnv(envInfo* e, int env_index);       // [*] Creates a simple storage environment
	static void RemoveEnv(int env_index);                      // [*] Remove a storage environment (This Does not remove the default environment)
	static void LoadEnv(int env_index);                        // [ ] Load storage environment
	static void UnloadEnv(int env_index);                      // [ ] Unload a storage environment
	static void SaveCustomLogFile(std::string f);              // [ ] Save a custom path for the text file logging (To do)
	static void LoadCustomDevList(std::string f);              // [ ] Accepts .spmls or simply .json
	static void SaveCustomDevList(std::string f);              // [ ] Save custom device list (can be saved as: .spmls, .json | in the future: .xls / .xsls)
	static void ReloadDevList();                               // [ ] Reload a device list
	static void CheckPowerStat(SPMList::device d);             // [ ] Checks for power status to the given server
	static void ServerAction(int act_type, SPMList::device d); // [ ] Send a power action to a server (Poweroff, Reboot, Standby, Forced Poweroff, Forced Reboot)
	static void SetIPMI_User(std::string usr, std::string pw); // [ ] Set user and password on IPMI in order to perform an action
	static void Terminate();                                   // [ ] Safe exit SPM (unloading stuff freeing variables, stopping processes, etc)
	static unsigned int GetVersion();                          // [ ] Ensuring compatibility
};


inline std::vector<SPM::envInfo> loaded_envs;
inline std::vector<SPMList::device> loaded_dev_list;