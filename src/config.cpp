


#include <nlohmann/json.hpp>


#include <sstream>


#include "dbg_log.hpp"
#include "config.hpp"
#include "utils.hpp"






void SPMConfig::Write(cfgStruct cfg_out)
{
  nlohmann::json json_out;
  std::ostringstream out_path;
  
  // Create json structure
  json_out =
  {
    { "general",
      {
        { "deviceStatus",    cfg_out.dev_status_mpack     },
        { "port",            cfg_out.port                 },
        { "serverReply",     cfg_out.server_reply         },
        { "wolPort",         cfg_out.wol_port             },
        { "socketCallbacks", cfg_out.power_opts_callbacks },
        { "userFeedback",    cfg_out.user_feedback        }
      }
    },
    { "debug",
      {
        { "stdoutLog", cfg_out.debug_log  },
        { "msgbox",    cfg_out.msgbox_log }
      }
    },
    { "restrictedSession",
      {
        { "enabled",  cfg_out.restrict_mode      },
        { "timeout",  cfg_out.restrict_timeout   },
        { "timespan", cfg_out.rescrict_time_span }
      }
    },
    {
      "environments",
      {
        { "lastEnvCreated", cfg_out.last_env_index }
      }
    }
  };
  
#if defined (_WIN32) || defined (_WIN64)
  out_path << SPMUtils::GetHomeDir() << "\\.spm\\config.json";
#endif

#ifdef __linux__
  out_path << SPMUtils::GetHomeDir() << "/.spm/config.json";
#endif
  
  
  std::ofstream out_conf(out_path.str());
  
#ifndef SINGLE_LINE_CONFIG
  out_conf << "// File created from libspm\n\n";
  out_conf << json_out.dump(4); // Write a human-readable json config
#else
  out_conf << json_out; // If enabled the entire config is written on a single line
#endif
  
}

SPMConfig::cfgStruct SPMConfig::Read()
{
  cfgStruct cfg_in; // Configuration structure
  std::ostringstream out_path;
  
  #if defined (_WIN32) || defined (_WIN64)
    out_path << SPMUtils::GetHomeDir() << "\\.spm\\config.json";
  #endif
  
  #ifdef __linux__
    out_path << SPMUtils::GetHomeDir() << "/.spm/config.json";
  #endif
  
  
  std::ifstream in_conf(out_path.str());
  
  nlohmann::json json_in = nlohmann::json::parse(in_conf);
  
  // Parse json objects
  nlohmann::json general_obj = json_in["general"];
  
  cfg_in.dev_status_mpack     = general_obj["deviceStatus"];
  cfg_in.port                 = general_obj["port"];
  cfg_in.server_reply         = general_obj["serverReply"];
  cfg_in.wol_port             = general_obj["wolPort"];
  cfg_in.power_opts_callbacks = general_obj["socketCallbacks"];
  cfg_in.user_feedback        = general_obj["userFeedback"];
  
  
  nlohmann::json debug_obj = json_in["debug"];
  
  cfg_in.debug_log  = debug_obj["stdoutLog"];
  cfg_in.msgbox_log = debug_obj["msgbox"];
  
  
  nlohmann::json restrictedSession_obj = json_in["restrictedSession"];
  
  cfg_in.restrict_mode      = restrictedSession_obj["enabled"];
  cfg_in.restrict_timeout   = restrictedSession_obj["timeout"];
  cfg_in.rescrict_time_span = restrictedSession_obj["timespan"];
  
  nlohmann::json environments_obj = json_in["environments"];
  
  cfg_in.last_env_index = environments_obj["lastEnvCreated"];
  
  return cfg_in;
}
