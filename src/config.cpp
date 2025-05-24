



#include <sstream>
#include <inifile-cpp/inicpp.h>


#include "dbg_log.hpp"
#include "config.hpp"
#include "utils.hpp"






void SPMConfig::Write(cfgStruct cfg_out)
{
  ini::IniFile config;
  std::ostringstream out_path;

  // Structure filling
  config["Settings"]["pcStat"]                    = cfg_out.pc_status_mpack;
  // config["Settings"]["CLIMode"] = cfg_out.cli_mode; // No longer needed
  config["Settings"]["dbgLog"]                    = cfg_out.debug_log;
  // config["Restricted_session"]["autolink"]        = cfg_out.restrict_autolink; // Still no ideea what that is lmaooo
  config["RestrictedSession"]["restrictMode"]     = cfg_out.restrict_mode;
  config["RestrictedSession"]["timeoutSession"]   = cfg_out.restrict_timeout;
  config["RestrictedSession"]["timeoutTimeSpan"]  = cfg_out.rescrict_time_span;
  config["RemotePowerOptions"]["Port"]            = cfg_out.port;
  config["RemotePowerOptions"]["socketCallbacks"] = cfg_out.power_opts_callbacks;

// String concatenation to form the file path to the configuration file
#if defined (_WIN32) || defined(_WIN64)
  out_path << SPMUtils::GetHomeDir() << "\\.spm\\spm.conf";
#endif

#ifdef __linux__
  out_path << SPMUtils::GetHomeDir() << "/.spm/spm.conf";
#endif
  
  config.save(out_path.str()); // And save it with the file path and name formed earlier.
  SPM_LOG(SPMDebug::Info, "Default config created");
  //Do stuff here
}

SPMConfig::cfgStruct SPMConfig::Read()
{
  cfgStruct cfg_in; // Configuration structure
  ini::IniFile in_config;
  std::ostringstream in_path;
#ifdef __linux__
  in_path << SPMUtils::GetHomeDir() << "/.spm/spm.conf"; // Create the file location of the config file
#endif

#if defined(_WIN32) || defined(_WIN64)
  in_path << SPMUtils::GetHomeDir() << "\\.spm\\spm.conf"; // Create the file location of the config file
#endif

  if(!SPMUtils::checkFile(in_path.str()))
  {
    SPM_LOG(SPMDebug::Err, "spm.conf not found !");
  }
  else
  {
    SPM_LOG(SPMDebug::Info, "Reading config...");
    in_config.setMultiLineValues(true); // Allow the ini parser to load the sections and fields line by line

    SPM_LOG(SPMDebug::noType, "Config path: ", in_path.str());

    in_config.load(in_path.str()); // Then open the config file

    cfg_in.debug_log          = in_config["Settings"]["dbgLog"].as<bool>();
    cfg_in.pc_status_mpack    = in_config["Settings"]["pcStat"].as<bool>();
    cfg_in.rescrict_time_span = in_config["RestrictedSession"]["timeoutTimeSpan"].as<int>();
    cfg_in.restrict_timeout   = in_config["RestrictedSession"]["timeoutSession"].as<bool>();
    cfg_in.restrict_mode      = in_config["RestrictedSession"]["restrictMode"].as<bool>();
    cfg_in.port               = in_config["RemotePowerOptions"]["Port"].as<int>();

  }

  return cfg_in;
}
