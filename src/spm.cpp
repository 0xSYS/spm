/*

Main Source file of SPM library
*/




#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

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
#include "dbg_log.hpp"
#include "config.hpp"
#include "spm_list.hpp"
#include "sckt_io.hpp"





/*
! - - - - - - - - !
! Initialization  !
! - - - - - - - - !
*/


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

  //mainDir << "/lists";

  //SPM_LOG(SPMDebug::Info, "Checking for '.spm/lists' directory...");
  //if(SPMUtils::checkDir(mainDir.str()) == false)
  //{
  //  SPMUtils::makeDir(mainDir.str());
  //}
  //else
  //{
  //  SPM_LOG(SPMDebug::Success, "'.spm/lists' directory found");
  //}
  SPM::CreateDefaultEnv();
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
      defaultConfig.port = DEFAULT_PORT;
      defaultConfig.wol_port = 10;
      defaultConfig.user_feedback = false;
      defaultConfig.msgbox_log = true;
      defaultConfig.power_opts_callbacks = true;
      defaultConfig.last_env_index = 0;
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
	mainDir << SPMUtils::GetHomeDir() << "/.spm/lists/devices.spmls";
#endif

#if defined(_WIN32) || defined(_WIN64)
  mainDir << SPMUtils::GetHomeDir() << "\\.spm\\lists\\devices.spmls";
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

/*
! - - - - - - - - - - - - !
! Environment management  !
! - - - - - - - - - - - - !
*/

void SPM::CreateDefaultEnv()
{
  std::ostringstream env_path;
  nlohmann::json env_info;
  
  
  // Directory creation
  
  /* Breakdown:
  - Checks for .spm/data directory. If not existing it creates the data directory
  This is where all environments are stored (including the default storage environment)
  Example: env_0 (the default), env_1, env_2, env_3, ...
  
  - Checks for .spm/data/env_0 (The default environment). If not existing it creates the env_0 directory
  env_0, env_1, ... holds info.spmenv, devices.spmls, devices.spmls (.spmls = Spm LiSt)
  
  - Creates the info.spmenv file (Very important file when synchronizing the environment from desktop to mobile)
  */
#ifdef __linux__
	env_path << SPMUtils::GetHomeDir() << "/.spm/";
#endif
  
#if defined(_WIN32) || defined(_WIN64)
  env_path << SPMUtils::GetHomeDir() << "\\.spm\\";
#endif


#ifdef __linux__
  env_path << "data/";
#endif

#if defined(_WIN32) || defined(_WIN64)
  env_path << "data\\";
#endif

  SPM_LOG(SPMDebug::Info, "Creating Default storage environment");
  
  if(SPMUtils::checkDir(env_path.str()) == false)
  {
    SPM_LOG(SPMDebug::Info, "'.spm/data' directory created");
    SPMUtils::makeDir(env_path.str());
  }
  else
  {
    SPM_LOG(SPMDebug::Success, "'.spm/data' directory found");
  }
  
#ifdef __linux__  
  env_path << "env_0/";
#endif

#if defined(_WIN32) || defined(_WIN64)
  env_path << "env_0\\";
#endif
  
  if(SPMUtils::checkDir(env_path.str()) == false)
  {
    SPM_LOG(SPMDebug::Info, "'.spm/data/env_0' directory created");
    SPMUtils::makeDir(env_path.str());
  }
  else
  {
    SPM_LOG(SPMDebug::Success, "'.spm/data/env_0' directory found");
  }
  
  // Create a small binary encoded json file containing minimal information about the created storage environment
  env_info =
  {
    { "dateCreated", SPMUtils::GetCurrentDate() },
    { "name", "Default Environment" },
    { "description", "The default storage environment for spm" },
    { "uniqueIdentifier", SPMUtils::genRandomHash(16) /*give it a random hash made out of 16 characters to avoid confusion in between multiple default storage environments*/ },
    { "index", 0 }
  };
  
#if defined(_WIN32) || defined(_WIN64)
  env_path << "\\info.spmenv";
#endif

#ifdef __linux__
  env_path << "/info.spmenv";
#endif
  
  // Serialization to MessagePack
  std::vector<std::uint8_t> msgpack_dat = nlohmann::json::to_msgpack(env_info);
  std::ofstream out_env_info(env_path.str(), std::ios::binary);
  out_env_info.write(reinterpret_cast<const char*>(msgpack_dat.data()), msgpack_dat.size());
  out_env_info.close();
}

void SPM::CheckEnv(int env_index)
{
  std::ostringstream env_path;
  
#ifdef __linux__  
  env_path << SPMUtils::GetHomeDir() << "/.spm/data/";
  env_path << "env_" << env_index << "/info.spmenv";
#endif

#if defined(_WIN32) || defined(_WIN64)
  env_path << SPMUtils::GetHomeDir() << "\\.spm\\data\\";
  env_path << "env_" << env_index << "\\info.spmenv";
#endif

  //env_path << "env" << "info.spmenv";
  std::cout << "env_path: " << env_path.str() << "\n";
  if(SPMUtils::checkFile(env_path.str()) == false)
	{
	  SPM_LOG(SPMDebug::Err, "Missing default env information");
#ifndef NO_MSGBOX
		SPMDebug::MsgBoxLog(SPMDebug::Err, "Missing environment information !!!");
#endif
	}
	else
	{
	  SPM_LOG(SPMDebug::Success, "Environment info found !");
	}
}

void SPM::CreateNewEnv(envInfo* e, int env_index)
{
  std::ostringstream env_path;
  std::ostringstream env_name;
  nlohmann::json env_info;
  bool allow_next_step = true;
  std::vector<bool> existing_env;
  
  SPM_LOG(SPMDebug::Info, "Creating new environment with index: ", env_index);
  
#ifdef __linux
  env_path << SPMUtils::GetHomeDir() << "/.spm/data/";
#endif
   
#if defined(_WIN32) || defined(_WIN64)
  env_path << SPMUtils::GetHomeDir() << "\\.spm\\data\\";
#endif


#ifdef __linux__  
  env_path << "env_" << env_index << "/";
#endif
 
#if defined(_WIN32) || defined(_WIN64)
  env_path << "env_" << env_index << "\\";
#endif
   

  // Check for existing environments
  if(SPMUtils::checkDir(env_path.str()))
  {
    SPM_LOG(SPMDebug::Err, "env_", env_index , " Already exists !!!");
#ifndef NO_MSGBOX
    SPMDebug::MsgBoxLog(SPMDebug::Err, "environment ", env_index, " Already exists !!!");
#endif
    allow_next_step = false;
  }
  else
    allow_next_step = true;
	
  
  // Trying to prevent env creation unless if there's no preexisting env
  if(allow_next_step)
  {
    SPMUtils::makeDir(env_path.str());
    
    // this feels kinda meh
    if(e == nullptr)
    {
      e = new envInfo();
      env_name << "env_" << env_index;
      e->name = env_name.str();
      e->description = "";
    }
    else
    {
      if(e->name.empty())
      {
        env_name.str("");
        env_name.clear();
        env_name << "env_" << env_index;
        e->name = env_name.str();
      }
    }
   
    // Create a small binary encoded json file containing minimal information about the created storage environment
    env_info =
    {
      { "dateCreated", SPMUtils::GetCurrentDate() },
      { "name", e->name },
      { "description", e->description },
      { "uniqueIdentifier", SPMUtils::genRandomHash(16) /*give it a random hash made out of 16 characters to avoid confusion in between multiple default storage environments*/ },
      { "index", env_index }
    };
   
#if defined(_WIN32) || defined(_WIN64)
    env_path << "\\info.spmenv";
#endif
 
#ifdef __linux__
    env_path << "/info.spmenv";
#endif
   
    // Serialization to MessagePack
    std::vector<std::uint8_t> msgpack_dat = nlohmann::json::to_msgpack(env_info);
    std::ofstream out_env_info(env_path.str(), std::ios::binary);
    out_env_info.write(reinterpret_cast<const char*>(msgpack_dat.data()), msgpack_dat.size());
    out_env_info.close();
    SPM_LOG(SPMDebug::Success, "Environment successfully created !");
  }
}

void SPM::RemoveEnv(int env_index)
{
  std::ostringstream env_path;
#ifdef __linux
  env_path << SPMUtils::GetHomeDir() << "/.spm/data/";
#endif
   
#if defined(_WIN32) || defined(_WIN64)
  env_path << SPMUtils::GetHomeDir() << "\\.spm\\data\\";
#endif

  // Prevent removal of the default env
  if(env_index == 0)
    SPM_LOG(SPMDebug::Err, "Cannot remove env_0 !!! aka default environment");
  else
  {
    bool success;

    // Check for these files
    // if one of them exists the env removal action is prevented
    std::ostringstream temp1;
    temp1 << env_path.str() << "devices.spmls";
    
    std::ostringstream temp2;
    temp2 << env_path.str() << "ip_table.spmls";
    
    std::ostringstream temp3;
    temp3 << env_path.str() << "commands.spmls";
    std::ostringstream temp_str;
    if(SPMUtils::checkFile(temp1.str()))
    {
      temp_str << "devices.spmls\n";
      success = false;
    }
    else if(SPMUtils::checkFile(temp2.str()))
    {
      temp_str << "ip_table.spmls\n";
      success = false;
    }
    else if(SPMUtils::checkFile(temp3.str()))
    {
      temp_str << "commands.spmls\n";
      success = false;
    }
    else
    {
      // You can remove the env if none of these files exist
      env_path << "env_" << env_index;
      SPMUtils::removeDir(env_path.str());
      success = true;
    }
    
    if(!success)
    {
      SPM_LOG(SPMDebug::Err, "Failed to remove environment ", env_index, " due to existing files in it:\n", temp_str.str());
#ifndef NO_MSGBOX
      SPMDebug::MsgBoxLog(SPMDebug::Err, "Failed to remove environment ", env_index, " due to existing files in it:\n", temp_str.str());
#endif
    }
  }
}

void SPM::LoadEnv(int env_index)
{
  // Todo
}

void SPM::UnloadEnv(int env_index)
{
  // Todo
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
