/*

Main Source file of SPM library
*/




#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>

#include <sys/stat.h>
#include <type_traits>

#ifdef __linux
  #include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
  #include <Windows.h>
  #include <direct.h>
#endif



#include "spm.hpp"
#include "utils.hpp"
// #include "globals.hpp"
#include "dbg_log.hpp"
#include "config.hpp"
#include "spm_list.hpp"


// SPMUtils spmUtils; // Moved to globals.hpp
//SPMConfig spmConf;
SPMList spmLst;


void SPM::Init(SPMConfig::cfgStruct* settings_init)
{
  /*
  Steps:
  If runing on windows Set ansi escapes on windows terminal
  1) Check for .spm directory (linux / windows). If none of them exist create them
  2) Check for configuration file. If not existing create it. (Here the default configuration structure is assigned)
  3) Check for availavble lists of computers or / servers
  4) Parse the lists
  */

  SPMConfig::cfgStruct defaultConfig;
  SPMConfig::cfgStruct customSettings;

#if defined(_WIN32) || defined(_WIN64)
  SPMUtils::SetWinTerm();
#endif

  // Basic Logging test to stdout
#ifndef SKIP_INITIAL_LOG_TEST
  SPM_LOG(SPMDebug::Info, "Info Test");
  SPM_LOG(SPMDebug::Success, "Success Test");
  SPM_LOG(SPMDebug::Warn, "Warning Test");
  SPM_LOG(SPMDebug::Err, "Error test");
  std::cout << "\n\n\n\n";
#endif

  std::ostringstream mainDir;

  mainDir << SPMUtils::GetHomeDir() << "/.spm";

  SPM_LOG(SPMDebug::Info, "Checking for '.spm' directory");
  if(SPMUtils::checkDir(mainDir.str()) == false)
  {
    SPMUtils::makeDir(mainDir.str());
    SPM_LOG(SPMDebug::Info, "'.spm' directory created");
  }
  else
  {
    SPM_LOG(SPMDebug::Success, "'.spm' directory found");
  }

  mainDir << "/lists";

  SPM_LOG(SPMDebug::Info, "Checking for '.spm/lists' directory...");
  if(SPMUtils::checkDir(mainDir.str()) == false)
  {
    SPMUtils::makeDir(mainDir.str());
  }
  else
  {
    SPM_LOG(SPMDebug::Success, "'.spm/lists' directory found");
  }
  mainDir.str(""); // Reset the string content so it dosen't retain the previous directories
  mainDir.clear(); // And clear any lefrover errors

  
  // Reassign the home directory with the main directory + logs directory
  // Todo: This should be done based on the settings
  mainDir << SPMUtils::GetHomeDir() << "/.spm/logs";

  SPM_LOG(SPMDebug::Info, "Checking for '.spm/logs' directory");
  if(SPMUtils::checkDir(mainDir.str()) == false)
  {
    SPM_LOG(SPMDebug::Info, "'.spm/logs' directory created");
    SPMUtils::makeDir(mainDir.str());
  }
  else
  {
    SPM_LOG(SPMDebug::Success, "'.spm/logs' directory found");
  }

  mainDir.str("");
  mainDir.clear();
#ifdef __linux__
/*
What an idiot.
I spent almost 4 fucking hours trying to find out why my fronted wasn't working at all and I js foudn out I forgot to update the fucking file extensions (Down below)
GOD DAMN
*/
  mainDir << SPMUtils::GetHomeDir() << "/.spm/config.json";
#endif

  // Check for custom settings
  if(settings_init == nullptr)
  {
#if defined(_WIN32) || defined(_WIN64)
    mainDir << SPMUtils::GetHomeDir() << "\\.spm\\config.json";
#endif
    SPM_LOG(SPMDebug::noType, "Main Directory: ", mainDir.str());
    if(!SPMUtils::checkFile(mainDir.str()))
    {
      //defaultConfig.config_storage = true; // Just stupid. Simply use the nullptr instead
      defaultConfig.restrict_mode = true;
      defaultConfig.restrict_timeout = true;
      defaultConfig.dev_status_mpack = true;
      defaultConfig.debug_log = false;
      defaultConfig.rescrict_time_span = 8;
      defaultConfig.port = 8080;
      defaultConfig.wol_port = 10;
      defaultConfig.user_feedback = false;
      defaultConfig.msgbox_log = true;
      defaultConfig.power_opts_callbacks = true;
      globalConf = defaultConfig;
      SPMConfig::Write(defaultConfig);
    }
	  else if(SPMUtils::checkFile(mainDir.str()) == true)
	  {
		  SPM_LOG(SPMDebug::Success, "'.spm/config.json' found");
		  // Once found read its settings and store them into the config structure
		  globalConf = SPMConfig::Read();
		  SPMUtils::printConfig(globalConf);
	  }
  }
  else
  {
    // Get custom settings and sync to the global settings
    globalConf =* settings_init;
    SPMUtils::printConfig(globalConf);
  }

	mainDir.str("");
	mainDir.clear();


#ifdef __linux__
	mainDir << SPMUtils::GetHomeDir() << "/.spm/lists/main_list.sls";
#endif

#if defined(_WIN32) || defined(_WIN64)
  mainDir << SPMUtils::GetHomeDir() << "\\.spm\\lists\\main_list.sls";
#endif

	SPM_LOG(SPMDebug::noType, "File path to list: ", mainDir.str());
	if(SPMUtils::checkFile(mainDir.str()) == false)
	{
	  SPM_LOG(SPMDebug::Warn, "No main list has beed found");
	}
	else
	{
	  // Parse the main list
	}
}

void SPM::Terminate()
{
  SPM_LOG(SPMDebug::Info, "Safe exiting SPM");
  // This is where variables are freed, files closed and exit processes.
}





/*
Todo:
- Creating single include headers for use in custom projects



*/
