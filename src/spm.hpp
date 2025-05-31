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

  
	static void Init(SPMConfig::cfgStruct* settings_init);
	static void CreateDefaultEnv();
	static void CheckDefaultEnv();
	static void CreateNewEnv(int env_index);
	static void RemoveEnv(int env_index); //This Does not remove the default environment
	static void SwitchEnv(int env_index);
	static void CheckEnv(int env_index);
	static void SaveCustomLogFile(std::string f); // To do
	static void LoadCustomDevList(std::string f);
	static void SaveCustomDevList(std::string f);
	static void ReloadDevList();
	static void Terminate();
};
