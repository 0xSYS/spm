#pragma once




#include <vector>

#include "config.hpp"
#include "spm_list.hpp"






inline SPMConfig::cfgStruct globalConf;

class SPM
{
	public:
	
  std::vector<SPMList::computer> mainList; // Store the main list at first run

  
	static void Init();
	static void SaveCustomLogFile(std::string f); // To do
	static void LoadCustomDevList(std::string f);
	static void SaveCustomDevList(std::string f);
	static void ReloadDevList();
	static void Terminate();
};
