#pragma once




#include <vector>

#include "config.hpp"
#include "spm_list.hpp"






inline SPMConfig::cfgStruct globalConf;

class SPM
{
	public:
	
  std::vector<SPMList::device> mainList; // Store the main list at first run
  int current_env_index = 0;
  
  
  typedef struct
  {
    std::string name;
    std::string description;
    std::string date_created;
  }envInfo;

  
	static void Init(SPMConfig::cfgStruct* settings_init);
	static void CreateDefaultEnv();
	static void CheckEnv(int env_index);
	static void CreateNewEnv(envInfo* e, int env_index);
	static void RemoveEnv(int env_index); //This Does not remove the default environment
	static void LoadEnv(int env_index);
	static void SaveCustomLogFile(std::string f); // To do
	static void LoadCustomDevList(std::string f);
	static void SaveCustomDevList(std::string f);
	static void ReloadDevList();
	static void Terminate();
};
